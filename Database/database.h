#ifndef Database
#define Database

#include <filesystem>
#include "../sqlite/sqlite3.h"
#include <string>
#include "../crow_all.h"

namespace Database {
	static std::string SQLiteError{};

	inline bool executeQuery(sqlite3** db, const std::string& query);
	inline bool createTables(sqlite3** db);

	inline bool open(sqlite3** db, std::filesystem::path dbPath) {
		if (sqlite3_open(dbPath.string().c_str(), db) == SQLITE_OK) {
			if (!createTables(db)) {
				return false;
			}
			std::string pragmaQuery = { "PRAGMA foreign_keys = ON;" };
			executeQuery(db, pragmaQuery);
			return true;
		}
		else {
			SQLiteError = { "Failed to open database" };
			return false;
		}
	}

	inline bool createTables(sqlite3** db) {
		const char* User = "CREATE TABLE IF NOT EXISTS User ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"name VARCHAR,"
			"email VARCHAR UNIQUE,"
			"password VARCHAR,"
			"location VARCHAR);";


		if (!executeQuery(db, User)) {
			return false;
		}

		const char* Event = "CREATE TABLE IF NOT EXISTS Event ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"name VARCHAR,"
			"address VARCHAR,"
			"city VARCHAR,"
			"type VARCHAR,"
			"description VARCHAR,"
			"fees REAL,"
			"max_seat INTEGER,"
			"enrolled_count INTEGER,"
			"creator_id INTEGER,"
			"date date,"
			"time time,"
			"image VARCHAR,"
			"FOREIGN KEY(creator_id) REFERENCES User(id));";


		if (!executeQuery(db, Event)) {
			return false;
		}

		const char* PurchasedEvent = "CREATE TABLE IF NOT EXISTS PurchasedEvent ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"event_id INTEGER,"
			"user_id INTEGER,"
			"seats INTEGER,"
			"FOREIGN KEY(user_id) REFERENCES User(id),"
			"FOREIGN KEY(event_id) REFERENCES Event(id));";


		if (!executeQuery(db, PurchasedEvent)) {
			return false;
		}

		return true;
	}

	inline void close(sqlite3** db) {
		if (db) {
			sqlite3_close(*db);
		}
	}

	inline bool executeQuery(sqlite3** db, const std::string& query) {

		char* errMsg = nullptr;

		int rc = sqlite3_exec(*db, query.c_str(), NULL, 0, &errMsg);

		if (rc != SQLITE_OK) {
			SQLiteError = { errMsg };
			sqlite3_free(errMsg);
			return false;
		}
		return true;

	}

	inline std::string find(sqlite3** db, const std::string& tablename, const std::string& field, const std::string& fieldvalue, const std::string& getfield) {

		sqlite3_stmt* stmt;
		std::string selectQuery = "SELECT " + getfield + " FROM " + tablename + " WHERE " + field + " = '" + fieldvalue + "'; ";

		if (sqlite3_prepare_v2(*db, selectQuery.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
			return std::string("");
		}

		int step = sqlite3_step(stmt);
		if (step == SQLITE_ROW) {
			return reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		}

	}

}

#endif