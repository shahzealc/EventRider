#include "event.h"
#include "../Database/database.h"

bool Event::insertEvent(sqlite3* db) {
	std::string query = "INSERT INTO Event (name, location, type, description, fees, creator_id, date, time) VALUES ('"
		+ name + "','"
		+ location + "','"
		+ type + "','"
		+ description + "',"
		+ std::to_string(fees) + ","
		+ creator_id + ",'"
		+ date + "','"
		+ time + "');";

	CROW_LOG_INFO<<query;

	if (!Database::executeQuery(&db, query)) {
		std::cerr << "Failed to insert user: " << Database::SQLiteError << std::endl;
		return false;
	}
	return true;
}

crow::json::wvalue Event::GetEventsByQuery(sqlite3* db,std::string query)
{
	sqlite3_stmt* stmt;
	crow::json::wvalue jsonResponse;
	std::vector<crow::json::wvalue> rows;

	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			crow::json::wvalue row;
			row["eventid"] = sqlite3_column_int(stmt, 0);
			row["name"] = (const char*)sqlite3_column_text(stmt, 1);
			row["location"] = (const char*)sqlite3_column_text(stmt, 2);
			row["type"] = (const char*)sqlite3_column_text(stmt, 3);
			row["description"] = (const char*)sqlite3_column_text(stmt, 4);
			row["fees"] = (const char*)sqlite3_column_text(stmt, 5);
			row["creator_id"] = (const char*)sqlite3_column_text(stmt, 6);
			row["date"] = (const char*)sqlite3_column_text(stmt, 7);
			row["time"] = (const char*)sqlite3_column_text(stmt, 8);
			rows.push_back(std::move(row));
		}
		sqlite3_finalize(stmt);
	}

	jsonResponse = std::move(rows);
	return jsonResponse;
}

crow::json::wvalue Event::GetEvents(sqlite3* db, std::string field)
{
	if (field == "location") {
		std::string query = "SELECT * FROM Event WHERE LOWER(location) like LOWER('%" + location + "%') AND date >= date('now') order by date, time;";
		return GetEventsByQuery(db, query);
	}
	else if (field == "type") {
		std::string query = "SELECT * FROM Event WHERE LOWER(type) like LOWER('%" + type + "%') AND date >= date('now') order by date, time;";
		return GetEventsByQuery(db, query);
	}
	else if (field == "name") {
		std::string query = "SELECT * FROM Event WHERE LOWER(name) like LOWER('%" + name + "%') AND date >= date('now') order by date, time;";
		return GetEventsByQuery(db, query);
	}
	else if (field == "creator") {
		std::string query = "SELECT * FROM Event WHERE LOWER(creator_id) like LOWER('%" + creator_id + "%') AND date >= date('now') order by date, time;";
		return GetEventsByQuery(db, query);
	}
	else if (field == "search") {
		std::string query = "SELECT * FROM Event WHERE (LOWER(location) like LOWER('%" + name + "%') or LOWER(type) like LOWER('%" + name + "%') or LOWER(name) like LOWER('%" + name + "%') or LOWER(description) like LOWER('%" + name + "%')) AND date >= date('now') order by date, time;";
		CROW_LOG_INFO<<query;
		return GetEventsByQuery(db, query);
	}
	else {
		std::string query = "SELECT * FROM Event WHERE date >= date('now') ORDER BY date, time;";
		CROW_LOG_INFO<<query;
		return GetEventsByQuery(db, query);
	}
	return crow::json::wvalue();
}

bool Event::DeleteEvent(sqlite3* db) {
	std::string query = "DELETE FROM Event WHERE id = '" + id + "';";

	if (!Database::executeQuery(&db, query)) {
		CROW_LOG_WARNING << "Failed to Delete user: " << Database::SQLiteError;
		return false;
	}
	return true;
}

bool Event::UpdateEvent(sqlite3* db)
{
	std::string query = "UPDATE Event SET "
		"name = '" + name + "', "
		"location = '" + location + "', "
		"type = '" + type + "', "
		"fees = " + std::to_string(fees) + ", "
		"date = '" + date + "', "
		"time = '" + time + "', "
		"description = '" + description + "' "
		"WHERE id = " + id + ";";
	CROW_LOG_WARNING<<query;

	if (!Database::executeQuery(&db, query)) {
		CROW_LOG_WARNING << "Failed to Update user: " << Database::SQLiteError;
		return false;
	}
	return true;
}




