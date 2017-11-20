#include <QObject>
#include <QString>
#include <QTextEdit>
#include <QHostAddress>
#include <QByteArray>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QStackedLayout>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();
    this->setWindowTitle(QString::fromUtf8("姿态识别服务器"));
    this->setWindowIcon(QIcon(":/new/prefix1/icon.ico"));
    this->setMaximumSize(1103,545);
    this->setMinimumSize(1103,545);
    connect(ui->sendButton,SIGNAL(clicked(bool)),SLOT(sendMessage()));

   // connect(this,SIGNAL(HaveMessage),SLOT(sendMessage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
//socket操作函数
void MainWindow::init()
{
    timer = new QTimer;
    tcpServer = new QTcpServer(this);
    tcpSocket = new QTcpSocket(this);
    socket_list=new QList<QTcpSocket*>;

    newListen();
    connect(tcpServer,SIGNAL(newConnection()),SLOT(acceptConnection()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),SLOT(showError(QAbstractSocket::SocketError)));

}
void MainWindow::sendMessage()  //发送信息
{
   // address = ui->addressEdit->text();
    QString strData =QString::fromUtf8("Time: ") + QTime::currentTime().toString() + "\n" + QString::fromUtf8("当前位置为")+address.toUtf8() + "\n"+weather.toUtf8();
    QByteArray sendMessage = strData.toUtf8();
    mChat += ("Send " + sendMessage);
    ui->messageEdit->setText(mChat);
//    tcpSocket->write(sendMessage);
    for (int i = 0;i < socket_list->length();i ++)
    {
        socket_list->at(i)->write(sendMessage);
    }

}

void MainWindow::onReciveData()
{
    qDebug()<<"server message"<<endl;
    for(int i=0;i<socket_list->length();i++)
    {
//        qDebug()<<"QDataStream"<<endl;
//        QDataStream in(socket_list->at(i));
//        in.setVersion (QDataStream::Qt_5_6);
//        in >> message;
        message = socket_list->at(i)->readAll();
        qDebug()<<message;
        if(message.isEmpty())
        {
            qDebug() << "break " ;
            break;
        }
        qDebug()<<message.section(',',0,0).trimmed()<<endl<<message.section(',',1,1).trimmed();
        if(message.section(',',0,0).trimmed()=="Terminal")
        {
            mChat +=("Recv " + message);
            ui->messageEdit->setText(mChat);
            message.replace("\n",",");
            lon=message.section(',',2,2).trimmed();
            lat=message.section(',',3,3).trimmed();
            ui->latitudelineEdit->setText(lat);
            ui->longitudelineEdit->setText(lon);
            on_submitButton_clicked();
        }
        if(message.section(',',1,1).trimmed()=="Client")
        {
            //客户端请求命令
        }
    }

}
//void MainWindow::onReciveData()  //接受数据
//{
//    QString data = tcpSocket->readAll();
//    qDebug()<<data;
//    mChat +=("Recv " + data);
//    ui->messageEdit->setText(mChat);
//    data.replace("\n",",");
//    lon=data.section(',',1,1).trimmed();
//    lat=data.section(',',2,2).trimmed();
//    ui->latitudelineEdit->setText(lat);
//    ui->longitudelineEdit->setText(lon);
//    on_submitButton_clicked();
// //   connect(this,SIGNAL(finished(replyweatherFinished(QNetworkReply))),SLOT(sendmessage()));
//  //  qDebug()<<weather<<endl<<address<<endl;

//}

void MainWindow::newListen()
{
    if(!tcpServer->listen(QHostAddress::Any,6666))
    {

     //   qDebug()<<tcpServer->errorString();
        QMessageBox::information(this,"TCPError",tcpServer->errorString());
        tcpServer->close();
    }
}
void MainWindow::acceptConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();

    socket_list->append(tcpSocket);
    connect(tcpSocket,SIGNAL(readyRead()),SLOT(onReciveData()));
}

void MainWindow::showError(QAbstractSocket::SocketError)
{
  //  qDebug()<<tcpSocket->errorString();
    QMessageBox::information(this,"TcpsocketError",tcpSocket->errorString());
    tcpSocket->close();
}


void MainWindow::on_submitButton_clicked()
{
//     double longitude=ui->longitudelineEdit->text().toDouble();//经度
//     double latitude=ui->latitudelineEdit->text().toDouble();//纬度

//     //字符串拼接,组成gps坐标
      QString str=lon;
      str.append(",");
      str.append(lat);
      //url地址编辑
     //   QUrl url("http://maps.google.com/maps/api/geocode/json?latlng=36.655922,117.033784“&language=zh-CN&sensor=false");//请求地址
      QString baseUrl = "https://maps.googleapis.com/maps/api/geocode/json?";
      QUrl url(baseUrl);
     QUrlQuery query;
     query.addQueryItem("latlng", str);
     query.addQueryItem("key", "AIzaSyBAQH9_Zmlvb0EnuaCe6XjAazYcnAbr5oo ");
     url.setQuery(query);
//     QDesktopServices::openUrl(url);
//      qDebug()<<url;
      //通过url地址获取内容，并且解析出所需内容

      QNetworkAccessManager *manager =new QNetworkAccessManager(this);//网络管理器
      QNetworkRequest  request;//网络请求
      QByteArray post_data;//
      //设置头信息
      request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
      request.setHeader(QNetworkRequest::ContentLengthHeader, post_data.length());
      request.setUrl(QUrl(url));
      //
      QNetworkReply  *reply=manager->get(request);//R
      connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
      manager->post(request,post_data);//post  ???
      //获取响应的信息，状态码为200表示正常
      QVariant status_code=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);//???
     // sendMessage();


}
void MainWindow::finishedSlot(QNetworkReply* reply)
{
    if(reply->error()==QNetworkReply::NoError)
    {
       QByteArray bytes = reply->readAll();  //获取字节
       QJsonParseError jsonError;
       QJsonDocument json=QJsonDocument::fromJson(bytes,&jsonError);//生成json文件
       QString city;
     //  qDebug()<<json<<"\n";
       if(json.isNull())
       {
           qDebug()<<"null\n";
           QMessageBox::information(this,"Address information","Url is error!");
       }
      if(jsonError.error==QJsonParseError::NoError)//表示json处理时没错误
       {

           if(json.isObject())
           {
             QJsonObject obj=json.object();
             if(obj.contains("results"))
             {
                 QJsonValue status=obj.take("status");
                 if(status.toString()!="OK")
                 {
                     qDebug()<<"State is not ok\n";
                     return;
                 }
                 QJsonValue version_value;

                 foreach(QString key,obj.keys())
                 {
                     version_value=obj.take(key);

                     QJsonValue first_value=version_value.toArray().at(0);
                  //   qDebug()<<first_value<<endl;
                     QJsonObject obj=first_value.toObject();
                    address=obj.take("formatted_address").toString();
                   //  qDebug()<<data.toString()<<endl;
                     ui->addressEdit->setText(address);
                     QJsonValue address_components=obj.take("address_components");
                     for(int i=0;i<address_components.toArray().size();i++)
                     {
                          QJsonObject citymessage=address_components.toArray().at(i).toObject();
                          QJsonValue localitytypes=citymessage.take("types");

                          if(localitytypes.toArray().contains("locality")) /*localitytypes.toString()=="locality"*/
                          {
                              city=citymessage.take("long_name").toString();
                              ui->cityEdit->setText(city);
                          }
                     }

                     get_weather(city);

                 }
             }

             else
             {
                 qDebug()<<"url访问错误\n";
                 QMessageBox::information(this,"url information","url 访问错误!");
             }
           }
       }
    }

}
void MainWindow::get_weather(QString city)
{
        QNetworkAccessManager *manage = new QNetworkAccessManager(this);
        QNetworkRequest network_request;
        /*设置发送数据*/
        QString url =QString("http://wthrcdn.etouch.cn/weather_mini?city=%1").arg(city);
       // qDebug()<<url<<endl;
     //   QDesktopServices::openUrl(url);

        network_request.setUrl(QUrl(url));
        network_request.setHeader(QNetworkRequest::UserAgentHeader,"RT-Thread ART");
        connect(manage,SIGNAL(finished(QNetworkReply *)),this,SLOT(replyweatherFinished(QNetworkReply*)));
        /*发送get网络请求*/
        manage->get(network_request);

}
void MainWindow::replyweatherFinished(QNetworkReply *reply)
{
        /*对数据进行编码*/

//        QTextCodec *codec=QTextCodec::codecForName("utf8");
//        QString all=codec->toUnicode(reply->readAll());
//        qDebug ()<<all<<endl;
//        //QString all=reply->readAll();
//        ui->weatherEdit->setText(all);
//        reply->deleteLater();
    QString date,weathertypes,coldattention,fengli,fengxiang,temp,lowtemp,hightemp;
    if(reply->error()==QNetworkReply::NoError)
    {
       QByteArray weatherbytes=reply->readAll();
       QJsonParseError jsonError;
       QJsonDocument json=QJsonDocument::fromJson(weatherbytes,&jsonError);
       if(jsonError.error==QJsonParseError::NoError)
       {
           if(json.isObject())
           {

               QJsonObject weatherobj=json.object();
               if(weatherobj.contains("desc"))
               {
                   QJsonValue desc=weatherobj.take("desc");
                   if(desc.toString()!="OK")
                   {
                       ui->weatherEdit->setText("Erroe");
                       return;
                   }
                   QJsonObject data=weatherobj.take("data").toObject();

                   temp=data.take("wendu").toString();
                   coldattention=data.take("ganmao").toString();

                   QJsonValue forecast=data.take("forecast").toArray().at(0);
                   hightemp=forecast.toObject().take("high").toString();
                   lowtemp=forecast.toObject().take("low").toString();
                   weathertypes=forecast.toObject().take("type").toString();
                   date=forecast.toObject().take("date").toString();
                   fengli=forecast.toObject().take("fengli").toString();
                   fengxiang=forecast.toObject().take("fengxiang").toString();

                   //字符串拼接
                    weather=QString("今日是%1，当前气温%2，%3，%4，天气%5,%6风力%7，注意%8").arg(date).arg(temp).arg(lowtemp).arg(hightemp).arg(weathertypes).arg(fengxiang).arg(fengli).arg(coldattention);
                   //qDebug()<<str;
                   ui->weatherEdit->setText(weather);
           }
       }
 }
}
sendMessage();
}



void MainWindow::on_mapButton_clicked()
{
        QUrl url=QString("http://api.map.baidu.com/geocoder?location=%1,%2&coord_type=gcj02&output=html&src=GPS").arg(lon).arg(lat);

        ui->webBrowser->dynamicCall("Navigate(const QString&)", url);
 //  emit Display(1);
}

