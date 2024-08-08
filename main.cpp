#include <iostream>
#include "models/user.h"
#include "models/event.h"
#include "models/purchasedevent.h"
#include "jwt-cpp/jwt.h"
#include "crow_all.h"
#include "Database/database.h"
#include <fstream>
#include <filesystem>
#include "Base64/base64.h"

struct SUserInfo {
	std::string UserId{};
	std::string Location{};
};

SUserInfo UserInfo;

std::string JWT_SECRET = "EventRiderJWTToken";

// Middleware to verify JWT
struct VerifyJWT {
	struct context {};

	void before_handle(crow::request& req, crow::response& res, context& /*ctx*/) {

		if (req.url == "/login" || req.url == "/register") {
			return;
		}
		if (req.method == "OPTIONS"_method) {
			return;
		}

		auto auth_header = req.get_header_value("Authorization");

		if (auth_header.empty() || auth_header.substr(0, 7) != "Bearer ") {
			res.code = 401;
			res.end("Unauthorized: No token provided");
			return;
		}

		std::string token = auth_header.substr(7);

		try {
			auto decoded = jwt::decode(token);
			auto verifier = jwt::verify()
				.allow_algorithm(jwt::algorithm::hs256{ JWT_SECRET })
				.with_issuer("auth0");

			verifier.verify(decoded);

			UserInfo.UserId = decoded.get_payload_claim("id").as_string();
			UserInfo.Location = decoded.get_payload_claim("location").as_string();

		}
		catch (const std::exception& e) {
			res.code = 401;
			res.end("Unauthorized: Invalid token");
			return;
		}
	}

	void after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/) {
		// No-op
	}

};

bool saveFile(const std::string& fileName, const std::string& base64Content) {
	std::ofstream file(fileName, std::ios::binary);
	if (file.is_open()) {
		std::string decodedData = base64_decode(base64Content);
		file.write(decodedData.c_str(), decodedData.size());
		file.close();
		return true;
	}
	return false;
}

int main() {

	crow::App<crow::CORSHandler, crow::CookieParser, VerifyJWT> app;

	auto& cors = app.get_middleware<crow::CORSHandler>();
	cors.global().origin("*");
	cors.global().allow_credentials();
	cors.global().headers("Content-Type", "Authorization");
	cors.global().methods("GET"_method, "POST"_method, "DELETE"_method, "PATCH"_method, "PUT"_method, "OPTIONS"_method);


	sqlite3* db = nullptr;

	if (!Database::open(&db, "EventDB.db")) {
		std::cerr << "Failed to open database: " << Database::SQLiteError << std::endl;
		return 1;
	}

	CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::Post)([db](const crow::request& request) {
		try {
			auto body = crow::json::load(request.body);

			User U1;
			U1.setEmail(body["email"].s());
			U1.setPassword(body["password"].s());

			crow::json::wvalue res;

			std::string message = U1.validateUser(db);

			if (message == "true") {
				auto token = jwt::create()
					.set_issuer("auth0")
					.set_type("JWS")
					.set_payload_claim("id", jwt::claim(U1.getId()))
					.set_payload_claim("location", jwt::claim(U1.getLocation()))
					.set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes{ 30 })
					.sign(jwt::algorithm::hs256{ JWT_SECRET });

				res["message"] = "Logged in successfully";
				res["token"] = token;
				res["id"] = U1.getId();
				return crow::response(200, res);
			}
			else {
				res["message"] = message;
				return crow::response(401, res);
			}
		}
		catch (const std::exception& e) {
			crow::json::wvalue res;
			res["message"] = std::string("Error processing request: ") + e.what();
			return crow::response(400, res);
		}
		});

	CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::Post)([db](const crow::request& request) {
		try {
			auto body = crow::json::load(request.body);
			User U1;
			U1.setName(body["name"].s());
			U1.setEmail(body["email"].s());
			U1.setPassword(body["password"].s());
			U1.setLocation(body["city"].s());

			crow::json::wvalue res;

			if (U1.insertUser(db)) {
				res["message"] = "Inserted Successfully";
				return crow::response(201, res);
			}
			else {
				res["message"] = "Failed to insert user: " + Database::SQLiteError;
				return crow::response(500, res);
			}
		}
		catch (const std::exception& e) {
			crow::json::wvalue res;
			res["message"] = std::string("Error processing request: ") + e.what();
			return crow::response(400, res);
		}
		});

	CROW_ROUTE(app, "/user").methods(crow::HTTPMethod::Get)([db](const crow::request& request) {

		if (request.url_params.get("id") != nullptr) {
			auto id = request.url_params.get("id");

			User u1;
			u1.setId(id);
			std::string message = u1.GetUser(db);

			crow::json::wvalue res;
			res["message"] = message;
			if (message == "Success") {
				res["name"] = u1.getName();
				res["email"] = u1.getEmail();
				return crow::response(200, res);
			}
			else {
				return crow::response(400, res);
			}

		}

		return crow::response(400, "Missing parameter");

		});

	CROW_ROUTE(app, "/addevent").methods(crow::HTTPMethod::Post)([db](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body) {
			return crow::response(400, "Invalid JSON");
		}

		Event E1;
		E1.setName(body["name"].s());
		E1.setAddress(body["address"].s());
		E1.setCity(body["city"].s());
		E1.setType(body["type"].s());
		E1.setDescription(body["description"].s());
		E1.setFees(std::stod(body["fees"].s()));
		E1.setMaxSeat(std::stoi(body["max_seat"].s()));
		E1.setEnrolledCount(0);
		E1.setDate(body["date"].s());
		E1.setTime(body["time"].s());
		E1.setCreatorId(UserInfo.UserId);
		// Handle the image
		std::string imageContent = body["image"].s();
		std::string fileName = std::string("Images/").append(body["filename"].s());

		if (!saveFile(fileName, imageContent)) {
			return crow::response(500, "Failed to save file");
		}
		E1.setImage(fileName);

		if (E1.insertEvent(db)) {
			return crow::response(201, "Event added successfully");
		}
		else {
			return crow::response(400, "Failed to add event");
		}
		});

	CROW_ROUTE(app, "/events").methods(crow::HTTPMethod::Get)([db](const crow::request& request) {

		if (request.url_params.get("location") != nullptr) {
			auto location = request.url_params.get("location");

			Event E1;
			E1.setCity(location);

			auto res = E1.GetEvents(db, "location");
			return crow::response(200, res);
		}
		else if (request.url_params.get("name") != nullptr) {
			auto name = request.url_params.get("name");

			Event E1;
			E1.setName(name);

			auto res = E1.GetEvents(db, "name");
			return crow::response(200, res);
		}
		else if (request.url_params.get("creator") != nullptr) {
			auto creator = request.url_params.get("creator");

			Event E1;
			E1.setCreatorId(creator);
			auto res = E1.GetEvents(db, "creator");
			return crow::response(200, res);
		}
		else if (request.url_params.get("type") != nullptr) {
			auto type = request.url_params.get("type");

			Event E1;
			E1.setType(type);

			auto res = E1.GetEvents(db, "type");
			return crow::response(200, res);
		}
		else if (request.url_params.get("search") != nullptr) {
			auto search = request.url_params.get("search");

			Event E1;
			E1.setName(search);

			auto res = E1.GetEvents(db, "search");
			return crow::response(200, res);
		}
		else if (request.url_params.get("eventid") != nullptr) {
			auto id = request.url_params.get("eventid");

			Event E1;
			E1.setId(id);

			auto res = E1.GetEvents(db, "id");
			return crow::response(200, res);
		}
		else {
			Event E1;
			auto res = E1.GetEvents(db);
			return crow::response(200, res);
		}
		});

	CROW_ROUTE(app, "/events").methods(crow::HTTPMethod::Delete)([db](const crow::request& request) {

		if (request.url_params.get("id") != nullptr) {
			auto id = request.url_params.get("id");
			Event E1;
			E1.setId(id);

			if (E1.DeleteEvent(db))
				return crow::response(200);
		}

		return crow::response(400);

		});

	CROW_ROUTE(app, "/events").methods(crow::HTTPMethod::Put)([db](const crow::request& request) {
		auto body = crow::json::load(request.body);
		if (!body) {
			return crow::response(400, "Invalid JSON");
		}

		try {
			Event E1;
			E1.setId(body["eventid"].s());
			E1.setName(body["name"].s());
			E1.setAddress(body["address"].s());
			E1.setCity(body["city"].s());
			E1.setType(body["type"].s());
			E1.setDescription(body["description"].s());
			E1.setFees(std::stod(body["fees"].s()));
			E1.setMaxSeat(std::stoi(body["max_seat"].s()));
			E1.setDate(body["date"].s());
			E1.setTime(body["time"].s());

			crow::json::wvalue res;

			if (E1.UpdateEvent(db)) {
				res["message"] = "Updated Successfully";
				return crow::response(200, res);
			}
			else {
				res["message"] = "Something Went Wrong";
				return crow::response(400, res);
			}
		}
		catch (const std::exception& e) {
			CROW_LOG_ERROR << "Exception: " << e.what();
			return crow::response(500, "Server error");
		}
		});

	CROW_ROUTE(app, "/bookevent").methods(crow::HTTPMethod::Post)([db](const crow::request& request) {

		auto body = crow::json::load(request.body);
		CROW_LOG_WARNING << body;

		Event e1;
		e1.setId(std::to_string(body["eventid"].i()));

		crow::json::wvalue res;

		if (e1.CheckAvailablityAndInsert(db, body["seats"].i())) {

			PurchasedEvent pe1;
			pe1.setEventId(std::to_string(body["eventid"].i()));
			pe1.setUserId(body["userid"].s());
			pe1.setSeats(body["seats"].i());

			if (pe1.AddPurchasedEvent(db)) {
				res["message"] = "Event Booked Successfully";
				return crow::response(200, res);
			}
			else {
				res["message"] = "Something went wrong";
				return crow::response(501, res);
			}
		}
		else {
			res["message"] = "Sorry, the seats are full";
			return crow::response(200, res);
		}


		});

	CROW_ROUTE(app, "/alreadybooked").methods(crow::HTTPMethod::Get)([db](const crow::request& request) {

		PurchasedEvent pe1;
		pe1.setEventId(request.url_params.get("eventid"));
		pe1.setUserId(request.url_params.get("userid"));
		crow::json::wvalue res;

		if (pe1.checkUserWithEvent(db)) {
			res["booked"] = true;
		}
		else {
			res["booked"] = false;
		}
		return crow::response(200, res);

		});

	CROW_ROUTE(app, "/checktoken")([]() {

		return UserInfo.UserId + " authenticated";

		});

	CROW_CATCHALL_ROUTE(app)([]() {
		return crow::response(404, "Wrong Route");
		});

	app.port(8080).run();

	Database::close(&db);

	return 0;
}
