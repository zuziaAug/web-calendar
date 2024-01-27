#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include "senderthread.h"
#include "receiverthread.h"

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QAction;
class QGroupBox;
class QVBoxLayout;
class QDateTimeEdit;
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void connectClient();
    void showEvents();
    void displayResponse(const QString &event);
    void displayError(int socketError, const QString &message);
    void enableGetEventButton();
    void addEvent();


private:
    QLabel *hostLabel;
    QLabel *portLabel;
    QLabel *clientIdLabel;
    QLineEdit *hostLineEdit;
    QLineEdit *portLineEdit;
    QLineEdit *clientIdLineEdit;
    QLabel *statusLabel;
    QPushButton *connectButton;
    QPushButton *quitButton;
    QDialogButtonBox *buttonBox;

    QVBoxLayout *vBoxLayout;
    QLabel *titleLabel;
    QLabel *descriptionLabel;
    QLabel *startTimeLabel;
    QLabel *endTimeLabel;
    QLineEdit *titleEdit;
    QLineEdit *descriptionEdit;
    QDateTimeEdit *startTimeEdit;
    QDateTimeEdit *endTimeEdit;

    QPushButton *addEventButton;
    QPushButton *removeEventButton;
    QPushButton *showAllEventsButton;
    QPushButton *showAllClientsButton;

    SenderThread senderThread;
    ReceiverThread receiverThread;
    QString currentEvent;
};
#endif // MAINWINDOW_H
