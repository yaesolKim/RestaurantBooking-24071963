#include "gmock/gmock.h"
#include "booking_scheduler.cpp"
#include "testable_sms_sender.cpp"
#include "testable_mail_sender.cpp"
#include "testable_booking_scheduler.cpp"

using namespace testing;

class BookingSchedulerTest : public Test {
protected:
	void SetUp() override {
		NOT_ON_THE_HOUR = getTime(2021, 3, 26, 9, 5);
		ON_THE_HOUR = getTime(2021, 3, 26, 9, 0);

		bookingScheduler.setSmsSender(&testableSmsSender);
		bookingScheduler.setMailSender(&testableMailSender);
	}
public:
	tm getTime(int year, int month, int day, int hour, int min) {
		tm result = { 0, min, hour, day, month - 1, year - 1900, 0, 1, -1 };
		mktime(&result);
		return result;
	}

	tm addHour(tm base, int hour) {
		base.tm_hour += hour;
		mktime(&base);
		return base;
	}

	tm NOT_ON_THE_HOUR, ON_THE_HOUR;
	const int UNDER_CAPACITY = 1;
	const int CAPACITY_PER_HOUR = 3;
	Customer CUSTOMER_WITHOUT_EMAIL{ "Fake name", "010-1234-5678" };
	Customer CUSTOMER_WITH_EMAIL{ "Fake name", "010-1234-5678", "ys@samsung.com"};

	tm SUNDAY_DATETIME = getTime(2021, 3, 28, 17, 0);
	tm MONDAY_DATETIME = getTime(2024, 6, 3, 17, 0);
	BookingScheduler bookingScheduler{ CAPACITY_PER_HOUR };

	NiceMock<TestableSmsSender> testableSmsSender;
	NiceMock<TestableMailSender> testableMailSender;
};

TEST_F(BookingSchedulerTest, 예약은정시에만가능하다정시가아닌경우예약불가) {
	
	Schedule* schedule = new Schedule{ NOT_ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITHOUT_EMAIL };

	EXPECT_THROW({
		bookingScheduler.addSchedule(schedule);
		}, std::runtime_error);

}

TEST_F(BookingSchedulerTest, 예약은정시에만가능하다정시인경우예약가능) {
	
	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITHOUT_EMAIL };

	bookingScheduler.addSchedule(schedule);

	EXPECT_EQ(true, bookingScheduler.hasSchedule(schedule));

}

TEST_F(BookingSchedulerTest, 시간대별인원제한이있다같은시간대에Capacity초과할경우예외발생) {

	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER_WITHOUT_EMAIL };

	bookingScheduler.addSchedule(schedule);

	try {
		Schedule* newSchedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITHOUT_EMAIL };
		bookingScheduler.addSchedule(newSchedule);
		FAIL();
	}
	catch (std::runtime_error& e) {

		EXPECT_EQ(string{ e.what() }, string{ "Number of people is over restaurant capacity per hour" });
	}

}

TEST_F(BookingSchedulerTest, 시간대별인원제한이있다같은시간대가다르면Capacity차있어도스케쥴추가성공) {

	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER_WITHOUT_EMAIL };
	bookingScheduler.addSchedule(schedule);

	tm diffHour = addHour(ON_THE_HOUR, 1);
	Schedule* new_schedule = new Schedule{ diffHour, UNDER_CAPACITY, CUSTOMER_WITHOUT_EMAIL };
	bookingScheduler.addSchedule(new_schedule);
	EXPECT_EQ(true, bookingScheduler.hasSchedule(schedule));
}

TEST_F(BookingSchedulerTest, 예약완료시SMS는무조건발송) {

	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER_WITHOUT_EMAIL };
	EXPECT_CALL(testableSmsSender, send(schedule))
		.Times(1);

	bookingScheduler.addSchedule(schedule);
}

TEST_F(BookingSchedulerTest, 이메일이없는경우에는이메일미발송) {
		
	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITHOUT_EMAIL };
	EXPECT_CALL(testableMailSender, sendMail(schedule))
		.Times(0);
	
	bookingScheduler.addSchedule(schedule);
}

TEST_F(BookingSchedulerTest, 이메일이있는경우에는이메일발송) {

	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITH_EMAIL };
	EXPECT_CALL(testableMailSender, sendMail(schedule))
		.Times(1);
	
	bookingScheduler.addSchedule(schedule);
}

TEST_F(BookingSchedulerTest, 현재날짜가일요일인경우예약불가예외처리) {

	TestableBookingScheduler mockScheduler{ CAPACITY_PER_HOUR };
	EXPECT_CALL(mockScheduler, getNow)
		.WillRepeatedly(testing::Return(mktime(&SUNDAY_DATETIME)));
	BookingScheduler* bookingScheduler = &mockScheduler;

	try {
		Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITH_EMAIL };
		bookingScheduler->addSchedule(schedule);
		FAIL();
	}
	catch (std::runtime_error& e) {
		EXPECT_EQ(string{ e.what() }, string{ "Booking system is not available on sunday" });
	}

}

TEST_F(BookingSchedulerTest, 현재날짜가일요일이아닌경우예약가능) {

	TestableBookingScheduler mockScheduler{ CAPACITY_PER_HOUR };
	EXPECT_CALL(mockScheduler, getNow)
		.WillRepeatedly(testing::Return(mktime(&MONDAY_DATETIME)));
	BookingScheduler* bookingScheduler = &mockScheduler;

	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITH_EMAIL };
	bookingScheduler->addSchedule(schedule);
	//assert
	EXPECT_EQ(true, bookingScheduler->hasSchedule(schedule));
}

int main() {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}