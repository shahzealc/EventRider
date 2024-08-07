#ifndef UserModel
#define UserModel

#include <string>
#include "../Bcrypt/bcrypt.h"
#include "../sqlite/sqlite3.h"

class User {

private:
	std::string id{};
	std::string name{};
	std::string email{};
	std::string password{};
	std::string location{};


public:

	User() {}

	std::string getId() { return id; }
	std::string getName() { return name; }
	std::string getEmail() { return email; }
	std::string getPassword() { return password; }
	std::string getLocation() { return location; }

	void setId(std::string newId) { id = newId; }
	void setName(const std::string& newName) { name = newName; }
	void setEmail(const std::string& newEmail) { email = newEmail; }

	void setPassword(const std::string& newPassword) {
		password = newPassword;
	}

	void setLocation(const std::string& newLocation) { location = newLocation; }

	bool insertUser(sqlite3* db);

	std::string validateUser(sqlite3* db);

};

#endif

