#include "user.h"
#include "../Database/database.h"

bool User::insertUser(sqlite3* db) {

	std::string query = "INSERT INTO User (name, email,password, location) VALUES ('" + name + "','" + email + "','" + password + "','" + location + "');";

	if (!Database::executeQuery(&db, query)) {
		std::cerr << "Failed to insert user: " << Database::SQLiteError << std::endl;
		return false;
	}
	return true;
}

bool User::validateUser(sqlite3* db) {

	std::string query = "SELECT password,id,location FROM User WHERE email = '" + email + "';";

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	bool isValid = false;
	int step = sqlite3_step(stmt);
	if (step == SQLITE_ROW) {
			if(password==reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)))
				isValid=true;
	}
	id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
	location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
	sqlite3_finalize(stmt);
	return isValid;

}
