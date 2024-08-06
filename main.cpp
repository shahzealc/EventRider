#include <iostream>
#include "models/user.h"
#include "models/event.h"
#include "jwt-cpp/jwt.h"
#include "crow_all.h"
#include "Database/database.h"


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

			if (U1.validateUser(db)) {
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
				res["message"] = "Email or Password incorrect";
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
				return crow::response(200, res);
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

	CROW_ROUTE(app, "/addevent").methods(crow::HTTPMethod::Post)([db](const crow::request& request) {
		auto body = crow::json::load(request.body);

		Event E1;
		E1.setName(body["name"].s());
		E1.setLocation(body["city"].s());
		E1.setType(body["type"].s());
		E1.setDescription(body["description"].s());
		E1.setFees(std::stod(body["fees"].s()));
		E1.setCreatorId(UserInfo.UserId);
		E1.setDate(body["date"].s());
		E1.setTime(body["time"].s());

		crow::json::wvalue res;

		if (E1.insertEvent(db)) {
			res["message"] = "Inserted Successfully";
			return crow::response(200, res);
		}
		else {
			res["message"] = "Something Went Wrong";
			return crow::response(400, res);
		}
		});

	CROW_ROUTE(app, "/events").methods(crow::HTTPMethod::Get)([db](const crow::request& request) {

		if (request.url_params.get("location") != nullptr) {
			auto location = request.url_params.get("location");

			Event E1;
			E1.setLocation(location);

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
			E1.setLocation(body["city"].s());
			E1.setType(body["type"].s());
			E1.setDescription(body["description"].s());
			E1.setFees(std::stod(body["fees"].s()));
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
