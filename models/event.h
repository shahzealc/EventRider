#ifndef EventModel
#define EventModel

#include <string>
#include "../sqlite/sqlite3.h"
#include "../crow_all.h"

class Event {
private:
	std::string id{};
	std::string name{};
	std::string address{};
	std::string city{};
	std::string type{};
	std::string description{};
	double fees{};
	int max_seat{};
	int enrolled_count{};
	std::string creator_id{};
	std::string date{};
	std::string time{};
	std::string image{};

public:
	Event() {}

	std::string getId() const { return id; }
	std::string getName() const { return name; }
	std::string getAddress() const { return address; }
	std::string getCity() const { return city; }
	std::string getType() const { return type; }
	std::string getDescription() const { return description; }
	double getFees() const { return fees; }
	int getMaxSeat() const { return max_seat; }
	int getEnrolledCount() const { return enrolled_count; }
	std::string getCreatorId() const { return creator_id; }
	std::string getDate() const { return date; }
	std::string getTime() const { return time; }
	std::string getImage() const { return image; }

	void setId(std::string newId) { id = newId; }
	void setName(const std::string& newName) { name = newName; }
	void setAddress(const std::string& newAddress) { address = newAddress; }
	void setCity(const std::string& newCity) { city = newCity; }
	void setType(const std::string& newType) { type = newType; }
	void setDescription(const std::string& newDescription) { description = newDescription; }
	void setFees(double newFees) { fees = newFees; }
	void setMaxSeat(int newMaxSeat) { max_seat = newMaxSeat; }
	void setEnrolledCount(int newEnrolledCount) { enrolled_count = newEnrolledCount; }
	void setCreatorId(std::string newCreatorId) { creator_id = newCreatorId; }
	void setDate(const std::string& newDate) { date = newDate; }
	void setTime(const std::string& newTime) { time = newTime; }
	void setImage(const std::string& newImage) { image = newImage; }

	bool insertEvent(sqlite3* db);
	crow::json::wvalue GetEventsByQuery(sqlite3* db,std::string query);
	crow::json::wvalue GetEvents(sqlite3* db,std::string field="*");
	bool DeleteEvent(sqlite3* db);
	bool UpdateEvent(sqlite3* db);
	bool CheckAvailablity(sqlite3* db,int seats);
	bool CheckAvailablityAndInsert(sqlite3* db,int seats);

};

#endif