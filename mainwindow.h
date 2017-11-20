#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QUrl>
#include <QDebug>
#include <QDesktopServices>
#include <QUrlQuery>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QAxBase>
#include "webaxwidget.h"
#include <QtNetwork>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>
#include <QList>


class  Register;
class  QStackedLayout;
class  QVBoxLayout;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public:
    void init();
signals:
    void finishedRecived();
private slots:
    //socket操作函数
    void sendMessage();
    void onReciveData();
    void newListen();
    void acceptConnection();
    void showError(QAbstractSocket::SocketError);
    //经纬度操作
    void on_submitButton_clicked();
    void finishedSlot(QNetworkReply* reply);
    void get_weather(QString city);
    void replyweatherFinished(QNetworkReply *reply);
    void on_mapButton_clicked();


private:
    Ui::MainWindow *ui;
private:
    QTcpSocket *tcpSocket;
    QTcpServer *tcpServer;
    QList<QTcpSocket*> *socket_list;
    QTimer *timer;
    QByteArray mChat;
    QString address;
    QString weather;
    QString lat;
    QString lon;
    QString message;
    bool HaveMessage=false;


};

#endif // MAINWINDOW_H
