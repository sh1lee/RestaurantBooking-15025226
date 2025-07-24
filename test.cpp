#include "gmock/gmock.h"
#include "booking_scheduler.cpp"
#include "testable_sms_sender.cpp"
#include "testable_mail_sender.cpp"
#include "testable_booking_scheduler.cpp"
using namespace testing;

class BookingItem : public Test {
protected:
	void SetUp() override {

		NOT_ON_THE_HOUR = getTime(2025, 7, 24, 9, 25);
		ON_THE_HOUR = getTime(2025, 7, 24, 9, 0);
		bs.setSmsSender(&testableSmsSender);
		bs.setMailSender(&testableMailSender);
	}
public:
	tm getTime(int year, int mon, int day, int hour, int min) {
		tm result = { 0, min, hour, day, mon - 1, year - 1900, 0,0, -1 };
		mktime(&result);

		return result;
	}

	tm plusHour(tm base, int hour) {
		base.tm_hour += hour;
		mktime(&base);
		return base;
	}

	tm NOT_ON_THE_HOUR;
	tm ON_THE_HOUR;
	Customer CUSTOMER{ "NoName", "010 - 1234 - 5678" };
	Customer CUSTOMER_WITH_MAIL{ "Fake Name", "010-1234-5678" , "test@test.com" };
	const int UNDER_CAPACITY = 1;
	const int CAPACITY_PER_HOUR = 3;
	BookingScheduler bs{ CAPACITY_PER_HOUR };
	TestableSmsSender testableSmsSender;
	TestableMailSender testableMailSender;
};


TEST_F(BookingItem, ���������ÿ��������ϴ����ð��ƴѰ�쿹��Ұ�) {

	Schedule* schedule = new Schedule{ NOT_ON_THE_HOUR, UNDER_CAPACITY , CUSTOMER };

	EXPECT_THROW({ bs.addSchedule(schedule); }, std::runtime_error);
}

TEST_F(BookingItem, ���������ÿ��������ϴ������ΰ�쿹�డ��) {

	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY , CUSTOMER };
		
	bs.addSchedule(schedule);

	EXPECT_EQ(true, bs.hasSchedule(schedule));
}

TEST_F(BookingItem, �ð��뺰�ο��������ִٰ����ð��뿡Capacity�ʰ��Ұ�쿹�ܹ߻�) {
	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR , CUSTOMER };
	bs.addSchedule(schedule);

	try {
		Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY , CUSTOMER };
		bs.addSchedule(schedule);
		FAIL();
	}
	catch (std::runtime_error &e){
		EXPECT_EQ(string{ e.what() }, string{ "Number of people is over restaurant capacity per hour" });
	}

}

TEST_F(BookingItem, �ð��뺰�ο��������ִٰ����ð��밡�ٸ���Capacity���־�������߰�����) {
	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR , CUSTOMER };
	bs.addSchedule(schedule);
	tm differentHour = plusHour(ON_THE_HOUR ,1);

	Schedule* newschedule = new Schedule{ differentHour, UNDER_CAPACITY , CUSTOMER };

	bs.addSchedule(newschedule);

	EXPECT_EQ(true, bs.hasSchedule(newschedule));
}

TEST_F(BookingItem, ����Ϸ��SMS�¹����ǹ߼�) {

	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR , CUSTOMER };

	bs.addSchedule(schedule);

	EXPECT_EQ(true, testableSmsSender.isSendMethodIsCalled());
}

TEST_F(BookingItem, �̸����̾��°�쿡���̸��Ϲ̹߼�) {
	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY , CUSTOMER };

	bs.addSchedule(schedule);

	EXPECT_EQ(0, testableMailSender.getCountSendMailMethodIsCalled());
}

TEST_F(BookingItem, �̸������ִ°�쿡���̸��Ϲ߼�) {

	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY , CUSTOMER_WITH_MAIL };

	bs.addSchedule(schedule);

	EXPECT_EQ(1, testableMailSender.getCountSendMailMethodIsCalled());
}

TEST_F(BookingItem, ���糯¥���Ͽ����ΰ�쿹��Ұ�����ó��) {
	tm SUNDAY = getTime(2021, 3, 28, 17, 0);
	BookingScheduler* bookingScheduler = new TestableBookingScheduler{ CAPACITY_PER_HOUR , SUNDAY};

	try {
		Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY , CUSTOMER_WITH_MAIL };
		bookingScheduler->addSchedule(schedule);
		FAIL();
	}
	catch (std::runtime_error& e) {
		EXPECT_EQ(string{ e.what() }, string{ "Booking system is not available on sunday" });
	}

}

TEST_F(BookingItem, ���糯¥���Ͽ����̾ƴѰ�쿹�డ��) {
	tm MONDAY = getTime(2024, 6, 3, 17, 0);
	BookingScheduler* bookingScheduler = new TestableBookingScheduler{ CAPACITY_PER_HOUR , MONDAY};

	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY , CUSTOMER_WITH_MAIL };
	bookingScheduler->addSchedule(schedule);
	
	EXPECT_EQ(true, bookingScheduler->hasSchedule(schedule));
}

int main() {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}