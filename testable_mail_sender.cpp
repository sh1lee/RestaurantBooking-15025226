#include "mail_sender.cpp"

class TestableMailSender : public MailSender {
public:
    void sendMail(Schedule* schedule) override {
        countSendMailMethodIsCalled++;
    }

    int getCountSendMailMethodIsCalled() {
        return countSendMailMethodIsCalled;
    }

private:
    int countSendMailMethodIsCalled = 0;
};