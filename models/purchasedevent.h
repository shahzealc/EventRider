#ifndef PurchasedEventModel
#define PurchasedEventModel

#include <string>
#include "../sqlite/sqlite3.h"
#include "../crow_all.h"

class PurchasedEvent {
private:
	std::string id{};
	std::string event_id{};
	std::string user_id{};
	int seats{};

public:
	PurchasedEvent() {}

	std::string getId() const { return id; }
	std::string getEventId() const { return event_id; }
	std::string getUserId() const { return user_id; }
	int getSeats() const { return seats; }

	void setId(std::string newId) { id = newId; }
	void setEventId(const std::string& newEventId) { event_id = newEventId; }
	void setUserId(const std::string& newUserId) { user_id = newUserId; }
	void setSeats(const int& newSeats) { seats = newSeats; }

	bool insertPurchasedEvent(sqlite3* db);
	crow::json::wvalue GetPurchasedEventsByQuery(sqlite3* db, std::string query);
	crow::json::wvalue GetPurchasedEvents(sqlite3* db, std::string field = "*");
	bool DeletePurchasedEvent(sqlite3* db);
	bool UpdatePurchasedEvent(sqlite3* db);
	bool checkUserWithEvent(sqlite3* db);
	bool AddPurchasedEvent(sqlite3* db);
	crow::json::wvalue GetBookings(sqlite3* db);
};

#endif
