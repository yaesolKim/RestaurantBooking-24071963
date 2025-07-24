#include "gmock/gmock.h"
#include "booking_scheduler.cpp"
#include "testable_sms_sender.cpp"
#include "testable_mail_sender.cpp"

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
	BookingScheduler bookingScheduler{ CAPACITY_PER_HOUR };

	TestableSmsSender testableSmsSender;
	TestableMailSender testableMailSender;
};

TEST_F(BookingSchedulerTest, ���������ÿ��������ϴ����ð��ƴѰ�쿹��Ұ�) {
	
	Schedule* schedule = new Schedule{ NOT_ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITHOUT_EMAIL };

	EXPECT_THROW({
		bookingScheduler.addSchedule(schedule);
		}, std::runtime_error);

}

TEST_F(BookingSchedulerTest, ���������ÿ��������ϴ������ΰ�쿹�డ��) {
	
	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITHOUT_EMAIL };

	bookingScheduler.addSchedule(schedule);

	EXPECT_EQ(true, bookingScheduler.hasSchedule(schedule));

}

TEST_F(BookingSchedulerTest, �ð��뺰�ο��������ִٰ����ð��뿡Capacity�ʰ��Ұ�쿹�ܹ߻�) {

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

TEST_F(BookingSchedulerTest, �ð��뺰�ο��������ִٰ����ð��밡�ٸ���Capacity���־�������߰�����) {

	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER_WITHOUT_EMAIL };
	bookingScheduler.addSchedule(schedule);

	tm diffHour = addHour(ON_THE_HOUR, 1);
	Schedule* new_schedule = new Schedule{ diffHour, UNDER_CAPACITY, CUSTOMER_WITHOUT_EMAIL };
	bookingScheduler.addSchedule(new_schedule);
	EXPECT_EQ(true, bookingScheduler.hasSchedule(schedule));
}

TEST_F(BookingSchedulerTest, ����Ϸ��SMS�¹����ǹ߼�) {

	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER_WITHOUT_EMAIL };
	bookingScheduler.addSchedule(schedule);
	EXPECT_EQ(true, testableSmsSender.isSendMethodIsCalled());

}

TEST_F(BookingSchedulerTest, �̸����̾��°�쿡���̸��Ϲ̹߼�) {
		
	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITHOUT_EMAIL };
	bookingScheduler.addSchedule(schedule);

	EXPECT_EQ(0, testableMailSender.getCountSendMailMethodIsCalled());
}

TEST_F(BookingSchedulerTest, �̸������ִ°�쿡���̸��Ϲ߼�) {

	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITH_EMAIL };
	bookingScheduler.addSchedule(schedule);

	EXPECT_EQ(1, testableMailSender.getCountSendMailMethodIsCalled());

}

TEST_F(BookingSchedulerTest, ���糯¥���Ͽ����ΰ�쿹��Ұ�����ó��) {

}

TEST_F(BookingSchedulerTest, ���糯¥���Ͽ����̾ƴѰ�쿹�డ��) {

}

int main() {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}