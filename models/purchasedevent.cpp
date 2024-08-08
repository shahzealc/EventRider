#include "PurchasedEvent.h"
#include "../Database/database.h"

bool PurchasedEvent::insertPurchasedEvent(sqlite3* db) {
	std::string query = "INSERT INTO PurchasedEvent (event_id, seats,user_id) VALUES ('"
		+ event_id + "', "
		+ std::to_string(seats) + ", '"
		+ user_id + "');";

	CROW_LOG_INFO << query;

	if (!Database::executeQuery(&db, query)) {
		std::cerr << "Failed to insert purchased event: " << Database::SQLiteError << std::endl;
		return false;
	}
	return true;
}

crow::json::wvalue PurchasedEvent::GetPurchasedEventsByQuery(sqlite3* db, std::string query) {
	sqlite3_stmt* stmt;
	crow::json::wvalue jsonResponse;
	std::vector<crow::json::wvalue> rows;

	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			crow::json::wvalue row;
			row["id"] = sqlite3_column_int(stmt, 0);
			row["event_id"] = (const char*)sqlite3_column_text(stmt, 1);
			row["user_id"] = (const char*)sqlite3_column_text(stmt, 2);
			row["seats"] = (const char*)sqlite3_column_text(stmt, 3);
			rows.push_back(std::move(row));
		}
		sqlite3_finalize(stmt);
	}

	jsonResponse = std::move(rows);
	return jsonResponse;
}

crow::json::wvalue PurchasedEvent::GetPurchasedEvents(sqlite3* db, std::string field) {
	std::string query = "SELECT * FROM PurchasedEvent";
	if (field != "*") {
		query += " WHERE " + field + " IS NOT NULL";
	}
	query += ";";
	return GetPurchasedEventsByQuery(db, query);
}

bool PurchasedEvent::DeletePurchasedEvent(sqlite3* db) {
	std::string query = "DELETE FROM PurchasedEvent WHERE id = '" + id + "';";

	if (!Database::executeQuery(&db, query)) {
		CROW_LOG_WARNING << "Failed to delete purchased event: " << Database::SQLiteError;
		return false;
	}
	return true;
}

bool PurchasedEvent::UpdatePurchasedEvent(sqlite3* db) {
	std::string query = "UPDATE PurchasedEvent SET "
		"event_id = '" + event_id + "', "
		"user_id = '" + user_id + "', "
		"seats = '" + std::to_string(seats) + "' "
		"WHERE id = " + id + ";";

	CROW_LOG_WARNING << query;

	if (!Database::executeQuery(&db, query)) {
		CROW_LOG_WARNING << "Failed to update purchased event: " << Database::SQLiteError;
		return false;
	}
	return true;
}

bool PurchasedEvent::checkUserWithEvent(sqlite3* db) {
	std::string query = "SELECT * FROM PurchasedEvent WHERE user_id = '" + user_id + "' and event_id = '" + event_id + "'; ";
	CROW_LOG_WARNING << query;
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	int step = sqlite3_step(stmt);
	bool exists = (step == SQLITE_ROW);
	if (exists) {
		id = (const char*)sqlite3_column_text(stmt, 0);
		CROW_LOG_WARNING << id;
		seats += sqlite3_column_int(stmt, 3);
	}
	sqlite3_finalize(stmt);
	return exists;
}

bool PurchasedEvent::AddPurchasedEvent(sqlite3* db) {
	if (checkUserWithEvent(db)) {
		return UpdatePurchasedEvent(db);
	}
	else {
		return insertPurchasedEvent(db);
	}
}

crow::json::wvalue PurchasedEvent::GetBookings(sqlite3* db)
{
	crow::json::wvalue result;

	std::string query = "SELECT Event.id, Event.name, Event.date, Event.time, Event.fees, Event.city, PurchasedEvent.event_id, PurchasedEvent.seats "
		"FROM Event "
		"INNER JOIN PurchasedEvent ON Event.id = PurchasedEvent.event_id "
		"WHERE PurchasedEvent.user_id = " + user_id + ";";

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return result;
	}

	std::vector<crow::json::wvalue> bookings;

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		crow::json::wvalue booking;
		booking["event_id"] = sqlite3_column_int(stmt, 0);
		booking["eventName"] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
		booking["date"] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
		booking["time"] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
		booking["fees"] = sqlite3_column_double(stmt, 4);
		booking["location"] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
		booking["seats"] = sqlite3_column_int(stmt, 7);

		bookings.push_back(booking);
	}

	sqlite3_finalize(stmt);

	return bookings;
}
