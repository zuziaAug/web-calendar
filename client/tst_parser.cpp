#include <QtTest/QTest>
#include "../client/senderthread.h"
#include <iostream>
#include <sstream>

Request parseRequest(const char* buffer) {
    std::istringstream iss(buffer);
    Request request;
    iss >> request.client_id >> request.action >> std::ws;
    if (request.action == "ADD_EVENT") {
    getline(iss, request.event_title, ' ');
    getline(iss, request.event_description, ' ');
    }
    iss >> request.start_date.year >> request.start_date.month >> request.start_date.day >> request.start_date.hour
        >> request.end_time.year >> request.end_time.month >> request.end_time.day >> request.end_time.hour
        >> request.event_id;
    return request;
}

class TestParser: public QObject
{
    Q_OBJECT
private slots:
    void parseAddRequestString();
    void parseDeleteRequestString();
};

void TestParser::parseAddRequestString(){
    char buffer[1024] = {"1ADD_EVENT title description 0 0 0 0 0 0 0 0 9"};
    auto request = parseRequest(buffer);
    QVERIFY(request.event_description == "description");
    QVERIFY(request.event_title == "title");
    QVERIFY(request.event_id == 9);


}

void TestParser::parseDeleteRequestString(){
    char buffer[1024] = {"1DELETE_EVENT   0 0 0 0 0 0 0 0 9"};
    auto request = parseRequest(buffer);
    QVERIFY(request.event_id == 9);
}

QTEST_MAIN(TestParser)
#include "tst_parser.moc"
