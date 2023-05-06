#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networkdata.h"
#include <QPushButton>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 本地 IP，所有电脑都可以用这个 IP 指向自己
    IP = "127.0.0.1";
    // 端口，不要太简单，要避免和别的软件冲突
    PORT = 16667;

    this->ui->IPEdit->setText(IP);
    this->ui->PORTEdit->setText(QString::number(PORT));

    // 创建一个服务端
    this->server = new NetworkServer(this);

    lastOne = nullptr;

    // 创建一个客户端
    this->socket = new NetworkSocket(new QTcpSocket(),this);

    connect(this->socket->base(),&QTcpSocket::connected,this,[&](){this->ui->connectLabel->setText("connection succeed");});
    connect(this->socket,&NetworkSocket::receive,this,&MainWindow::receieveDataFromServer);
    connect(this->server,&NetworkServer::receive,this,&MainWindow::receieveData);
    connect(this->ui->clientSendButton,&QPushButton::clicked,this,&MainWindow::onClientSendButtonClicked);
    connect(this->ui->serverSendButton,&QPushButton::clicked,this,&MainWindow::onServerSendButtonClicked);
    connect(this->ui->showClientButton,&QPushButton::clicked,this,[&](){qDebug()<<clients;});
    connect(this->ui->reSetButton,&QPushButton::clicked,this,&MainWindow::reSet);
    connect(this->ui->reConnectButton,&QPushButton::clicked,this,&MainWindow::reConnect);
    connect(this->ui->reStartButton,&QPushButton::clicked,this,&MainWindow::reStart);

    // 客户端向 IP:PORT 连接
    // 注意，如果你双开了这个 demo，第二个进程的服务端会因为端口被第一个进程占用而监听失败，所以第二个进程事实上是没有服务端的
    // 所以你可以用第二个进程当测试用客户端，第一个当测试用服务端，此时表现行为像是两台机器。
    // 但如果你用第一个当客户端尝试连接，会连到第一个进程自己的服务端。
    this->socket->hello(IP,PORT);
    // 阻塞等待，2000ms超时
    this->socket->base()->waitForConnected(2000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receieveData(QTcpSocket* client, NetworkData data)
{
    qDebug()<<"Server get a data: "<<client<<" "<<data.encode();
    lastOne=client;
    // 获得地址的字符串表示，调试用
    QString ptrStr = QString("0x%1").arg((quintptr)client,
                        QT_POINTER_SIZE, 16, QChar('0'));
    this->ui->lastOneLabel->setText("LastOne: "+ptrStr);
    this->clients.insert(client);
    this->ui->serverGetEdit->setText(data.data1);
}

void MainWindow::receieveDataFromServer(NetworkData data)
{
    qDebug()<<"Client get a data: "<<data.encode();
    this->ui->clientGetEdit->setText(data.data1);
}

void MainWindow::onClientSendButtonClicked()
{
    this->socket->send(NetworkData(OPCODE::CHAT_OP,this->ui->clientSendEdit->text(),"send by client"));
}

void MainWindow::onServerSendButtonClicked()
{
    if(lastOne)
        this->server->send(lastOne,NetworkData(OPCODE::CHAT_OP,this->ui->serverSendEdit->text(),"send by server"));
}

void MainWindow::reStart()
{
    qDebug()<<"restart the server.";
    this->ui->lastOneLabel->setText("LastOne: ");
    this->ui->connectLabel->setText("disconnect");
    disconnect(this->server,&NetworkServer::receive,this,&MainWindow::receieveData);
    clients.clear();
    delete this->server;
    this->server = new NetworkServer(this);
    // 端口相当于传信息的窗户，收的人要在这守着
    this->server->listen(QHostAddress::Any,PORT);
    lastOne = nullptr;
    connect(this->server,&NetworkServer::receive,this,&MainWindow::receieveData);
}

void MainWindow::reConnect()
{
    qDebug()<<"client reconnect to the server.";
    this->ui->connectLabel->setText("connection fail");

    this->socket->bye();
    this->socket->hello(IP,PORT);
    if(!this->socket->base()->waitForConnected(3000)){
        qDebug()<<"reconnect fail";
    }
}

void MainWindow::reSet()
{
    this->ui->connectLabel->setText("connection fail");
    IP=this->ui->IPEdit->text();
    PORT=this->ui->PORTEdit->text().toInt();
    this->reStart();
    this->reConnect();
}
