#ifndef EventModel
#define EventModel

#include <string>
#include "../sqlite/sqlite3.h"
#include "../crow_all.h"

class Event {
private:
	std::string id{};
	std::string name{};
	std::string location{};
	std::string type{};
	std::string description{};
	double fees{};
	std::string creator_id{};
	std::string date{};
	std::string time{};

public:
	Event() {}

	std::string getId() const { return id; }
	std::string getName() const { return name; }
	std::string getLocation() const { return location; }
	std::string getType() const { return type; }
	std::string getDescription() const { return description; }
	double getFees() const { return fees; }
	std::string getCreatorId() const { return creator_id; }
	std::string getDate() const { return date; }
	std::string getTime() const { return time; }

	void setId(std::string newId) { id = newId; }
	void setName(const std::string& newName) { name = newName; }
	void setLocation(const std::string& newLocation) { location = newLocation; }
	void setType(const std::string& newType) { type = newType; }
	void setDescription(const std::string& newDescription) { description = newDescription; }
	void setFees(double newFees) { fees = newFees; }
	void setCreatorId(std::string newCreatorId) { creator_id = newCreatorId; }
	void setDate(const std::string& newDate) { date = newDate; }
	void setTime(const std::string& newTime) { time = newTime; }

	bool insertEvent(sqlite3* db);
	crow::json::wvalue GetEventsByQuery(sqlite3* db,std::string query);
	crow::json::wvalue GetEvents(sqlite3* db,std::string field="*");
	bool DeleteEvent(sqlite3* db);

};

#endif