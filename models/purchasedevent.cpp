#include "PurchasedEvent.h"
#include "../Database/database.h"

bool PurchasedEvent::insertPurchasedEvent(sqlite3* db) {
	std::string query = "INSERT INTO PurchasedEvent (event_id, user_id) VALUES ('"
		+ event_id + "', '"
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
		"user_id = '" + user_id + "' "
		"WHERE id = " + id + ";";

	CROW_LOG_WARNING << query;

	if (!Database::executeQuery(&db, query)) {
		CROW_LOG_WARNING << "Failed to update purchased event: " << Database::SQLiteError;
		return false;
	}
	return true;
}
