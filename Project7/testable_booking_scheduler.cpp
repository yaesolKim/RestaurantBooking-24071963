#include "booking_scheduler.cpp"
class TestableBookingScheduler : public BookingScheduler {
public:
	TestableBookingScheduler(int capacityPerHour, tm dateTime) :
		BookingScheduler{ capacityPerHour },
		dateTime{ dateTime } {
	}
	time_t getNow() override {
		return mktime(&dateTime);
	}

	void setSunday() {
		dateTime = { 0, 0, 17, 28, 3 - 1, 2021 - 1900, 0, 0, -1 };
	}
	void setMonday() {
		dateTime = { 0, 0, 17, 3, 6 - 1, 2024 - 1900, 0, 0, -1 };
	}
private:
	tm dateTime;
};