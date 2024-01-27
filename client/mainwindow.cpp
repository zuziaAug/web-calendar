#include <QtWidgets>
#include <QtNetwork>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    hostLabel = new QLabel(tr("&Server name:"));
    portLabel = new QLabel(tr("S&erver port:"));
    clientIdLabel = new QLabel(tr("&Client id:"));

    // find out which IP to connect to
    QString ipAddress;
    const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (const QHostAddress &entry : ipAddressesList) {
        if (entry != QHostAddress::LocalHost && entry.toIPv4Address()) {
            ipAddress = entry.toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    //Connection group
    hostLineEdit = new QLineEdit(ipAddress);
    portLineEdit = new QLineEdit("8888");
    portLineEdit->setValidator(new QIntValidator(1, 65535, this));
    clientIdLineEdit = new QLineEdit("1");
    clientIdLineEdit->setValidator(new QIntValidator(1, 65535, this));

    hostLabel->setBuddy(hostLineEdit);
    portLabel->setBuddy(portLineEdit);
    clientIdLabel->setBuddy(clientIdLineEdit);

    statusLabel = new QLabel(tr("Waiting for connection..."));
    statusLabel->setWordWrap(true);

    connectButton = new QPushButton(tr("Connect"));
    connectButton->setDefault(true);
    connectButton->setEnabled(true);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(connectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(connectButton, &QPushButton::clicked,
            this, &MainWindow::connectClient);
    connect(quitButton, &QPushButton::clicked,
            this, &MainWindow::close);

    connect(hostLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableGetEventButton);
    connect(portLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableGetEventButton);
    connect(&receiverThread, &ReceiverThread::newEvent,
            this,&MainWindow::displayResponse);
    connect(&senderThread, &SenderThread::error,
            this, &MainWindow::displayError);
    connect(&senderThread, &SenderThread::newSockfd,
                &receiverThread, &ReceiverThread::setSockfd);
    //! [0]

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(hostLabel, 0, 0);
    gridLayout->addWidget(hostLineEdit, 0, 1);
    gridLayout->addWidget(portLabel, 1, 0);
    gridLayout->addWidget(portLineEdit, 1, 1);
    gridLayout->addWidget(clientIdLabel, 2, 0);
    gridLayout->addWidget(clientIdLineEdit, 2, 1);
    gridLayout->addWidget(statusLabel, 3, 0, 1, 2);
    gridLayout->addWidget(buttonBox, 4, 0, 1, 2);
    QGroupBox *connectionGroup = new QGroupBox("Connection");
    connectionGroup->setLayout(gridLayout);

    //Add Event group
    titleLabel = new QLabel("Title:");
    descriptionLabel = new QLabel("Description:");

    titleEdit = new QLineEdit();
    descriptionEdit = new QLineEdit();

    titleLabel->setBuddy(titleEdit);
    descriptionLabel->setBuddy(descriptionEdit);

    startTimeLabel = new QLabel;
    endTimeLabel = new QLabel;
    startTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime());
    endTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(3600));

    addEventButton = new QPushButton(tr("Add Event"));
    connect(addEventButton, &QPushButton::clicked,
            this, &MainWindow::addEvent);

    showAllEventsButton = new QPushButton(tr("Show All Events"));
    connect(showAllEventsButton, &QPushButton::clicked,
            this, &MainWindow::showEvents);

    vBoxLayout = new QVBoxLayout();

    //Add event group
    vBoxLayout->addLayout(gridLayout);
    vBoxLayout->addWidget(titleLabel);
    vBoxLayout->addWidget(titleEdit);
    vBoxLayout->addWidget(descriptionLabel);
    vBoxLayout->addWidget(descriptionEdit);
    vBoxLayout->addWidget(startTimeLabel);
    vBoxLayout->addWidget(startTimeEdit);
    vBoxLayout->addWidget(endTimeLabel);
    vBoxLayout->addWidget(endTimeEdit);
    vBoxLayout->addWidget(addEventButton);
    vBoxLayout->addWidget(showAllEventsButton);
    QGroupBox *addEventGroup = new QGroupBox("Add New Event");
    addEventGroup->setLayout(vBoxLayout);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(connectionGroup,0,0);
    mainLayout->addWidget(addEventGroup,1,0);

    setLayout(mainLayout);

    setWindowTitle(tr("Web Calendar Client"));
    portLineEdit->setFocus();
}

MainWindow::~MainWindow() {}


void MainWindow::connectClient()
{
    connectButton->setEnabled(false);
    senderThread.connectClient(hostLineEdit->text(),
                             portLineEdit->text().toInt(),
                         clientIdLineEdit->text().toInt());
    receiverThread.start();

}
//! [1]
void MainWindow::addEvent()
{
    addEventButton->setEnabled(false);

    QDateTime startTime = startTimeEdit->dateTime();
    QDateTime endTime = endTimeEdit->dateTime();

    DateTime start{startTime.date().year(),startTime.date().month(),startTime.date().day(),startTime.time().hour()};
    DateTime end{endTime.date().year(),endTime.date().month(),endTime.date().day(),endTime.time().hour()};

    Request request{
        this->clientIdLineEdit->text().toInt(),
        "ADD_EVENT",
        descriptionEdit->text().toStdString(),
        titleEdit->text().toStdString(),
        start,
        end,
        -1
    };
    senderThread.sendRequest(request);
    addEventButton->setEnabled(true);

}

/*
                // DELETE_EVENT
                string event_id_to_delete;
                qDebug() << "Enter event ID to delete: ";
                getline(cin, event_id_to_delete);

                try {
                    action = "DELETE_EVENT";
                    int event_id = stoi(event_id_to_delete);
                    Request request{client_id, action, "", "", DateTime{}, DateTime{}, event_id};
                    sendRequest(request);
                } catch (const invalid_argument& e) {
                    qDebug() << "Invalid input. Please enter a valid integer.";
                } catch (const out_of_range& e) {
                    qDebug() << "Input out of range for integer.";
                }
*/


/*
                action = "SHOW_ALL_CLIENTS";
                Request request{ client_id, action, "", "", DateTime{}, DateTime{}, -1 };
                sendRequest(request);
 */

//! [2]
void MainWindow::showEvents()
{
    showAllEventsButton->setEnabled(false);

    Request request{
        this->clientIdLineEdit->text().toInt(),
        "SHOW_ALL_EVENTS", "", "", DateTime{}, DateTime{}, -1};

    senderThread.sendRequest(request);

    showAllEventsButton->setEnabled(true);
}

void MainWindow::displayResponse(const QString &nextEvent)
{
        statusLabel->setText(nextEvent);
    connectButton->setEnabled(true);
}

void MainWindow::displayError(int socketError, const QString &message)
{
    switch (socketError) {
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Web Calendar Client"),
                                 tr("The host was not found. Please check the "
                                    "host and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Web Calendar Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Web Calendar Client"),
                                 tr("The following error occurred: %1.")
                                     .arg(message));
    }

    connectButton->setEnabled(true);
}

void MainWindow::enableGetEventButton()
{
    bool enable(!hostLineEdit->text().isEmpty() && !portLineEdit->text().isEmpty());
    connectButton->setEnabled(enable);
}

