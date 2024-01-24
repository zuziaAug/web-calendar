#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QTcpSocket>

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

class SenderThread : public QThread
{
    Q_OBJECT
public:
    SenderThread(QObject *parent = nullptr);
    ~SenderThread();

    void connectClient(const QString &hostName, quint16 port, quint16 clientId);
    void run() override;
    void sendRequest(const Request& request);

signals:
    void newEvent(const QString &event);
    void error(int socketError, const QString &message);
    void sendSocket(QTcpSocket *socket);

private:
    void* receiveThread(void* arg);

    QString hostName;
    quint16 port;
    quint16 clientId;
    QMutex mutex;
    QWaitCondition cond;
    QTcpSocket socket;
    bool quit;
};

#endif // SENDERTHREAD_H
