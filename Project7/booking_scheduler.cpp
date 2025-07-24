#pragma once
#include <vector>
#include <stdexcept>
#include <algorithm>

#include "schedule.cpp"
#include "mail_sender.cpp"
#include "sms_sender.cpp"

using std::vector;

class BookingScheduler {
public:
    BookingScheduler(int capacityPerHour) :
        capacityPerHour{ capacityPerHour } {
        smsSender = new SmsSender();
        mailSender = new MailSender();
    }

    void addSchedule(Schedule* schedule) {

        // 정각에 예약하지 않을 경우 RuntimeException 발생
        if (schedule->getDateTime().tm_min != 0) {
            throw std::runtime_error("Booking should be on the hour.");
        }

        // 시간당 예약인원을 초과할 경우 RuntimeException 발생
        int numberOfPeople = schedule->getNumberOfPeople();
        for (Schedule* bookedSchedule : schedules) {
            if (isSameTime(bookedSchedule->getDateTime(), schedule->getDateTime())) {
                numberOfPeople += bookedSchedule->getNumberOfPeople();
            }
        }
        if (numberOfPeople > capacityPerHour) {
            throw std::runtime_error("Number of people is over restaurant capacity per hour");
        }

        
        // 일요일에는 시스템을 오픈하지 않는다.
        time_t now = getNow(); // 이걸 일요일 리턴하게 스텁건다.

        //컴퓨터 피씨 시간 날짜 읽어오는 코드. 일요일 시간 기다릴 수 없으니 모킹 필요
        if (getDayOfWeek(now) == "Sunday") {
            throw std::runtime_error("Booking system is not available on sunday");
        }
        

        schedules.push_back(schedule);

        // 고객에게 SMS 발송 // 돈 들어간다 여기도 모킹 대상.
        smsSender->send(schedule);

        // 고객이 E-Mail을 가지고 있을 경우 E-Mail 발송 // SMTP 서버 쓴다 유료다. 모킹대상임.
        if (schedule->getCustomer().getEmail() != "") {
            mailSender->sendMail(schedule);
        }
    }

    virtual time_t getNow()
    {
        return time(nullptr);
    }

    bool hasSchedule(Schedule* schedule) {
        //check if an element exists in array
        return std::find(schedules.begin(), schedules.end(), schedule) != schedules.end();
    }

    void setSmsSender(SmsSender* smsSender) {
        this->smsSender = smsSender;
    }

    void setMailSender(MailSender* mailSender) {
        this->mailSender = mailSender;
    }

private:
    //두 시간이 같은지 확인
    bool isSameTime(tm a, tm b) {
        return mktime(&a) == mktime(&b);
    }

    //요일을 알려주는 함수
    string getDayOfWeek(time_t tm_t) {
        tm tmTime;
        localtime_s(&tmTime, &tm_t);
        char buffer[100] = { 0 };
        std::strftime(buffer, sizeof(buffer), "%A", &tmTime);
        return string{ buffer };
    }

    int capacityPerHour;
    vector<Schedule*> schedules;
    SmsSender* smsSender;
    MailSender* mailSender;
};

