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

    hostLineEdit = new QLineEdit(ipAddress);
    portLineEdit = new QLineEdit;
    portLineEdit->setValidator(new QIntValidator(1, 65535, this));
    clientIdLineEdit = new QLineEdit;
    clientIdLineEdit->setValidator(new QIntValidator(1, 65535, this));

    hostLabel->setBuddy(hostLineEdit);
    portLabel->setBuddy(portLineEdit);
    clientIdLabel->setBuddy(clientIdLineEdit);

    statusLabel = new QLabel(tr("Waiting for connection..."));
    statusLabel->setWordWrap(true);

    getEventButton = new QPushButton(tr("Connect"));
    getEventButton->setDefault(true);
    getEventButton->setEnabled(false);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(getEventButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(getEventButton, &QPushButton::clicked,
            this, &MainWindow::requestNewEvent);
    connect(quitButton, &QPushButton::clicked,
            this, &MainWindow::close);

    connect(hostLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableGetEventButton);
    connect(portLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableGetEventButton);
    //! [0]
    connect(&thread, &ClientThread::newEvent,
            this, &MainWindow::showEvent);
    connect(&thread, &ClientThread::error,
            this, &MainWindow::displayError);
    //! [0]

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostLineEdit, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(clientIdLabel, 2, 0);
    mainLayout->addWidget(clientIdLineEdit, 2, 1);
    mainLayout->addWidget(statusLabel, 3, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 4, 0, 1, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Web Calendar Client"));
    portLineEdit->setFocus();
}

MainWindow::~MainWindow() {}

void MainWindow::requestNewEvent()
{
    getEventButton->setEnabled(false);
    thread.connectClient(hostLineEdit->text(),
                             portLineEdit->text().toInt(),
                         clientIdLineEdit->text().toInt());
}
//! [1]

//! [2]
void MainWindow::showEvent(const QString &nextEvent)
{
    if (nextEvent == currentEvent) {
        requestNewEvent();
        return;
    }
    //! [2]

    //! [3]
    currentEvent = nextEvent;
    statusLabel->setText(currentEvent);
    getEventButton->setEnabled(true);
}
//! [3]

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

    getEventButton->setEnabled(true);
}

void MainWindow::enableGetEventButton()
{
    bool enable(!hostLineEdit->text().isEmpty() && !portLineEdit->text().isEmpty());
    getEventButton->setEnabled(enable);
}

