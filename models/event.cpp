#include "event.h"
#include "../Database/database.h"

#include <sqlite3.h>
#include <iostream>
#include <string>

bool Event::insertEvent(sqlite3* db) {
	std::string query = "INSERT INTO Event (name, city, address, type, description, fees, max_seat, enrolled_count, creator_id, date, image, time) "
		"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, city.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, address.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, description.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 6, fees);
	sqlite3_bind_int(stmt, 7, max_seat);
	sqlite3_bind_int(stmt, 8, enrolled_count);
	sqlite3_bind_text(stmt, 9, creator_id.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 10, date.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 11, image.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 12, time.c_str(), -1, SQLITE_TRANSIENT);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_finalize(stmt);
	return true;
}

crow::json::wvalue Event::GetEventsByQuery(sqlite3* db, std::string query)
{
	sqlite3_stmt* stmt;
	crow::json::wvalue jsonResponse;
	std::vector<crow::json::wvalue> rows;

	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			crow::json::wvalue row;
			row["eventid"] = sqlite3_column_int(stmt, 0);
			row["name"] = (const char*)sqlite3_column_text(stmt, 1);
			row["address"] = (const char*)sqlite3_column_text(stmt, 2);
			row["city"] = (const char*)sqlite3_column_text(stmt, 3);
			row["type"] = (const char*)sqlite3_column_text(stmt, 4);
			row["description"] = (const char*)sqlite3_column_text(stmt, 5);
			row["fees"] = (const char*)sqlite3_column_text(stmt, 6);
			row["max_seat"] = (const char*)sqlite3_column_text(stmt, 7);
			row["enrolled_count"] = (const char*)sqlite3_column_text(stmt, 8);
			row["creator_id"] = (const char*)sqlite3_column_text(stmt, 9);
			row["date"] = (const char*)sqlite3_column_text(stmt, 10);
			row["time"] = (const char*)sqlite3_column_text(stmt, 11);
			row["image"] = (const char*)sqlite3_column_text(stmt, 12);
			rows.push_back(std::move(row));
		}
		sqlite3_finalize(stmt);
	}

	jsonResponse = std::move(rows);
	return jsonResponse;
}

crow::json::wvalue Event::GetEvents(sqlite3* db, std::string field)
{
	if (field == "city") {
		std::string query = "SELECT * FROM Event WHERE LOWER(city) like LOWER('%" + city + "%') AND date >= date('now') order by date, time;";
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
		CROW_LOG_INFO << query;
		return GetEventsByQuery(db, query);
	}
	else if (field == "search") {
		std::string query = "SELECT * FROM Event WHERE (LOWER(city) like LOWER('%" + name + "%') or LOWER(type) like LOWER('%" + name + "%') or LOWER(name) like LOWER('%" + name + "%') or LOWER(description) like LOWER('%" + name + "%')) AND date >= date('now') order by date, time;";
		CROW_LOG_INFO << query;
		return GetEventsByQuery(db, query);
	}
	else if (field == "id") {
		std::string query = "SELECT * FROM Event WHERE id = " + id + ";";
		CROW_LOG_INFO << query;
		return GetEventsByQuery(db, query);
	}
	else {
		std::string query = "SELECT * FROM Event WHERE date >= date('now') ORDER BY date, time;";
		CROW_LOG_INFO << query;
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

bool Event::UpdateEvent(sqlite3* db) {
	std::string query = "UPDATE Event SET "
		"name = ?, "
		"address = ?, "
		"city = ?, "
		"type = ?, "
		"fees = ?, "
		"max_seat = ?, "
		"date = ?, "
		"time = ?, "
		"description = ? "
		"WHERE id = ?";

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, address.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, city.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, fees);
	sqlite3_bind_int(stmt, 6, max_seat);
	sqlite3_bind_text(stmt, 7, date.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 8, time.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 9, description.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 10, id.c_str(), -1, SQLITE_TRANSIENT);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_finalize(stmt);
	return true;
}

bool Event::CheckAvailablity(sqlite3* db, int seats)
{
	std::string query = "SELECT max_seat,enrolled_count FROM EVENT WHERE id =" + id + ";";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	int step = sqlite3_step(stmt);
	if (step == SQLITE_ROW) {
		max_seat = sqlite3_column_int(stmt, 0);
		enrolled_count = sqlite3_column_int(stmt, 1);
		if ((max_seat - enrolled_count) >= seats) {
			return true;
		}
	}

	sqlite3_finalize(stmt);
	return false;
}

bool Event::CheckAvailablityAndInsert(sqlite3* db, int seats)
{
	if (CheckAvailablity(db, seats)) {
		std::string query = "UPDATE Event SET enrolled_count = enrolled_count + " + std::to_string(seats) + " WHERE id = " + id + ";";
		if (!Database::executeQuery(&db, query)) {
			CROW_LOG_WARNING << "Failed to Update event: " << Database::SQLiteError;
			return false;
		}
		return true;
	}
	return false;
}





