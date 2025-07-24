#include "gmock/gmock.h"
#include "booking_scheduler.cpp"

TEST(BookingSchedulerTest, ���������ÿ��������ϴ����ð��ƴѰ�쿹��Ұ�) {
	tm notOnTheHour = { 0 };
	notOnTheHour.tm_year = 2021 - 1900;
	notOnTheHour.tm_mon = 03 - 1;
	notOnTheHour.tm_mday = 26;
	notOnTheHour.tm_hour = 9;
	notOnTheHour.tm_min = 5;
	notOnTheHour.tm_isdst = -1;
	mktime(&notOnTheHour);

	Customer customer{"Fake name", "010-1234-5678"};
	Schedule* schedule = new Schedule{ notOnTheHour, 1, customer };
	BookingScheduler bookingScheduler{ 3 };

	EXPECT_THROW({
		bookingScheduler.addSchedule(schedule);
		}, std::runtime_error);

}

TEST(BookingSchedulerTest, ���������ÿ��������ϴ������ΰ�쿹�డ��) {
	tm onTheHour = { 0 };
	onTheHour.tm_year = 2021 - 1900;
	onTheHour.tm_mon = 03 - 1;
	onTheHour.tm_mday = 26;
	onTheHour.tm_hour = 9;
	onTheHour.tm_min = 0;
	onTheHour.tm_isdst = -1;
	mktime(&onTheHour);

	Customer customer{ "Fake name", "010-1234-5678" };
	Schedule* schedule = new Schedule{ onTheHour, 1, customer };
	BookingScheduler bookingScheduler{ 3 };

	bookingScheduler.addSchedule(schedule);

	EXPECT_EQ(true, bookingScheduler.hasSchedule(schedule));

}

TEST(BookingSchedulerTest, �ð��뺰�ο��������ִٰ����ð��뿡Capacity�ʰ��Ұ�쿹�ܹ߻�) {

}

TEST(BookingSchedulerTest, �ð��뺰�ο��������ִٰ����ð��밡�ٸ���Capacity���־�������߰�����) {

}

TEST(BookingSchedulerTest, ����Ϸ��SMS�¹����ǹ߼�) {

}

TEST(BookingSchedulerTest, �̸����̾��°�쿡���̸��Ϲ̹߼�) {

}

TEST(BookingSchedulerTest, �̸������ִ°�쿡���̸��Ϲ߼�) {

}

TEST(BookingSchedulerTest, ���糯¥���Ͽ����ΰ�쿹��Ұ�����ó��) {

}

TEST(BookingSchedulerTest, ���糯¥���Ͽ����̾ƴѰ�쿹�డ��) {

}

int main() {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}