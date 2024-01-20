#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>

struct DateTime {
    int year;
    int month;
    int day;
    int hour;
};

struct CalendarEvent {
    int event_id;
    std::string event_title;
    std::string event_description;
    DateTime start_date;
    DateTime end_time;
};

struct Request {
    int client_id;
    std::string action;
    std::string event_description;
    std::string event_title;
    DateTime start_date;
    DateTime end_time;
    int event_id;
};

class ClientThread : public QThread
{
    Q_OBJECT
public:
    ClientThread(QObject *parent = nullptr);
    ~ClientThread();

    void connectClient(const QString &hostName, quint16 port, quint16 clientId);
    void run() override;
    void sendRequest(int sockfd, const Request& request);
    void* receiveThread(void* arg);

signals:
    void newEvent(const QString &event);
    void error(int socketError, const QString &message);

private:
    QString hostName;
    quint16 port;
    quint16 clientId;
    std::mutex mtx;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
};



#endif // CLIENTTHREAD_H
