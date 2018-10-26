#include "widget.h"
#include "ui_widget.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtCore/QDebug>
#include <QtWidgets/QFormLayout>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtCore/QtGlobal>

#include <QtCharts/QXYSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
using namespace QtCharts;
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    index_x(0),
    index_y(0),
    count(0),
    sum_data(0),
    flow_SQUAL(0),
    frame_rate(0),
    //indexX_range(1000),
    indexY_range(100),
    index2Y_range(500),
    index_max(500)
{

    ui->setupUi(this);
    timer = new QTimer();
    timer->setInterval(500);  //定时触发显示环境质量和帧率
    timer->start();
    QLabel *frameshow = new QLabel();
    frameLable = new QLabel();
    frameshow->setText(tr("frame rate:"));
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    QLabel *showsum = new QLabel();
    showsum->setText(tr("data count:"));
    sum_label = new QLabel();
    QLabel * showdial = new QLabel();
    showdial->setText(tr("speed Y:"));
    QLabel * showdial1 = new QLabel();
    showdial1->setText(tr("distance Y:"));


    serial = new QSerialPort();
    m_mainLayout = new QGridLayout();

    m_seriesX = new QLineSeries();
    m_seriesX->setName(tr("X speed"));
    m_seriesY = new QLineSeries();
    m_seriesY->setName(tr("Y speed"));
    m_distX = new QLineSeries();
    m_distX->setName(tr("X distance"));
    m_distY = new QLineSeries();
    m_distY->setName(tr("Y distance"));

    m_chart = new QChart();
    QChartView *chartView = new QChartView(m_chart);
    chartView->setMinimumSize(800, 600);
    m_chart->addSeries(m_seriesX);
    m_chart->addSeries(m_seriesY);
    m_chart->addSeries(m_distX);
    m_chart->addSeries(m_distY);

    axisX = new QValueAxis; //设置图标坐标和文字等
    axisX->setRange(0, index_max);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("sample");

    axisY = new QValueAxis;
    axisY->setRange(-indexY_range, indexY_range);
    axisY->setTitleText("speed");

    axis2Y = new QValueAxis();
    axis2Y->setRange(-50,50);
    axis2Y->setLabelFormat("%g");
    axis2Y->setTitleText("distance");

    axisY->setTickCount(11);
    m_chart->setAxisX(axisX,m_seriesX);
    m_chart->setAxisY(axisY,m_seriesX);
    m_chart->setAxisX(axisX,m_seriesY);
    m_chart->setAxisY(axisY,m_seriesY);
    m_chart->addAxis(axis2Y,Qt::AlignRight);
    m_chart->setAxisY(axis2Y,m_distX);
    m_chart->setAxisY(axis2Y,m_distY);
    m_chart->setAxisX(axisX,m_distX);
    m_chart->setAxisX(axisX,m_distY);

    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    m_chart->setTitle("Optcial Flow Data");

    QLabel * showSQUAL = new QLabel(); //显示环境质量
    SQUAL = new QLabel();
    showSQUAL->setText(tr("SQUAL:"));

    QFormLayout *SQUALlayout = new QFormLayout();  //子layout
    SQUALlayout->addWidget(showSQUAL);
    SQUALlayout->addWidget(SQUAL);
    SQUALlayout->addWidget(frameshow);
    SQUALlayout->addWidget(frameLable);
    SQUALlayout->addWidget(showsum);
    SQUALlayout->addWidget(sum_label);
    SQUALlayout->addWidget(showdial);  //adjust speed axis
    SQUALlayout->addWidget(ui->dial);
    SQUALlayout->addWidget(showdial1);  //adjust distance axis
    SQUALlayout->addWidget(ui->dial_2);
    chartView->setRenderHint(QPainter::Antialiasing);
    m_mainLayout->addWidget(chartView,0,0,8,1);
    m_mainLayout->addWidget(ui->comboBox,0,2,1,1);
    m_mainLayout->addWidget(ui->pushButton,1,2,1,1);
    m_mainLayout->addWidget(ui->checkX,2,2,1,1);
    m_mainLayout->addWidget(ui->checkY,3,2,1,1);
    m_mainLayout->addWidget(ui->checkdistX,4,2,1,1);
    m_mainLayout->addWidget(ui->checkdistY,5,2,1,1);
    m_mainLayout->addLayout(SQUALlayout,6,2,8,1);

    axis2Y->setTickCount(11);
    ui->checkX->setChecked(true);
    ui->checkY->setChecked(true);
    ui->checkdistX->setChecked(true);
    ui->checkdistY->setChecked(true);
    ui->dial->setRange(10,indexY_range);
    ui->dial->setValue(100);
    ui->dial_2->setRange(10,index2Y_range);
    ui->dial_2->setValue(100);
    setLayout(m_mainLayout);
}

Widget::~Widget()
{
    serial->close();
    delete ui;
}

/*打开串口*/
void Widget::on_pushButton_clicked(bool checked)
{
    Q_UNUSED(checked)
    static int flag=0;
        if(flag ==0)
        {
            foreach( const QSerialPortInfo &Info,QSerialPortInfo::availablePorts())//读取串口信息
            {
                qDebug() << "portName    :"  << Info.portName();
                qDebug() << "Description   :" << Info.description();
                qDebug() << "Manufacturer:" << Info.manufacturer();
                ui->comboBox->addItem(Info.portName());
            }
            if(ui->comboBox->count()>0)
            {
                flag = 1;
                ui->pushButton->setText(tr("打开串口"));
            }
        }
        else if(flag == 1)
        {
            serial->setPortName(ui->comboBox->currentText());
            serial->open(QIODevice::ReadOnly);
            serial->setBaudRate(19200);
            connect(serial,SIGNAL(readyRead()),this,SLOT(SerialReceive()));
            qDebug()<<"打开串口:"<<serial->portName();
            ui->pushButton->setText(tr("关闭串口"));
            ui->comboBox->setEnabled(false);
            flag =2;

        }
        else
        {
            qDebug()<<"关闭串口";
            serial->close();
            ui->pushButton->setText(tr("刷新"));
            ui->comboBox->clear();
            ui->comboBox->setEnabled(true);
            disconnect(serial, &QSerialPort::readyRead, this, &Widget::SerialReceive);
            flag = 0;
        }

}

/*串口数据接收处理槽函数*/
void Widget::SerialReceive()
{
    requestData = serial->readAll();
    uint8_t head,tail,sum;
#define USE_UAV 0  //通过无人机发数据还是直接用光流计

#if USE_UAV
    head = requestData[0];
    tail = requestData[12];
    flow_sum  = requestData[10];

    count++;  /*统计帧率*/

    if(index_x>index_max)
    {
        index_x=0;
        m_seriesX->clear();
        m_distX->clear();
    }
    if(index_y>index_max)
    {
        index_y=0;
        m_seriesY->clear();
        m_distY->clear();
    }
    if(head == 0xFE && tail ==0xAA)
    {
        sum = requestData[2]+requestData[3]+requestData[4]+requestData[5]
                +requestData[6]+requestData[7]+requestData[8]+requestData[9];
        if(sum == flow_sum)
        {
            flow_SQUAL = requestData[11];

            dist_x = (int16_t)(requestData[2]<<8 | requestData[3]);
            dist_y = (int16_t)(requestData[4]<<8 | requestData[5]);
            flow_x = (int16_t)(requestData[6]<<8 | requestData[7]);
            flow_y = (int16_t)(requestData[8]<<8 | requestData[9]);
            m_seriesX->append(index_x,flow_x);
            m_seriesY->append(index_y,flow_y);
            m_distX->append(index_x,dist_x);
            m_distY->append(index_y,dist_y);
            index_x++;
            index_y++;
            sum_data++;
            sum_label->setNum(sum_data);

        }
    }
    // qDebug()<<"head:"<<head<<"tail:"<<tail<<"sum:"<<sum<<"SQUAL:"<<SQUAL;
    // qDebug()<<"head:"<<head<<"tail:"<<tail<<"flow_sum:"<<flow_sum<<"sum:"<<sum;
     qDebug()<<"head:"<<head<<"tail:"<<tail<<"flow_x:"<<flow_x<<"flow_y:"<<flow_y;
#else
    head = requestData[0];
    tail = requestData[8];
    flow_sum  = requestData[6];

    count++;  /*统计帧率*/

    if(index_x>index_max)
    {
        index_x=0;
        m_seriesX->clear();
    }
    if(index_y>index_max)
    {
        index_y=0;
        m_seriesY->clear();
    }
    if(head == 0xFE && tail ==0xAA)
    {
        sum = requestData[2]+requestData[3]+requestData[4]+requestData[5];

        if(sum == flow_sum)
        {
            flow_SQUAL = requestData[7];
            flow_x = (int16_t)(requestData[3]<<8 | requestData[2]);
            flow_y = (int16_t)(requestData[5]<<8 | requestData[4]);
            m_seriesX->append(index_x++,flow_x);
            m_seriesY->append(index_y++,flow_y);
            sum_data++;
            sum_label->setNum(sum_data);

        }
    }
    // qDebug()<<"head:"<<head<<"tail:"<<tail<<"sum:"<<sum<<"SQUAL:"<<SQUAL;
    // qDebug()<<"head:"<<head<<"tail:"<<tail<<"flow_sum:"<<flow_sum<<"sum:"<<sum;
     qDebug()<<"head:"<<head<<"tail:"<<tail<<"flow_x:"<<flow_x<<"flow_y:"<<flow_y;
#endif

}

/*显示隐藏坐标槽函数*/
void Widget::on_checkX_clicked(bool checked)
{
    if(checked)
    {
        m_seriesX->setVisible(true);
    }
    else
    {
        m_seriesX->setVisible(false);
    }
}

void Widget::on_checkY_clicked(bool checked)
{
    if(checked)
    {
        m_seriesY->setVisible(true);
    }
    else
    {
       m_seriesY->setVisible(false);
    }
}
/*刷新帧率和环境质量*/
void Widget::handleTimeout()
{
    static int state=0;
    state++;
    if(state==2)
    {
        frame_rate=count;
        count=0;
        frameLable->setNum(frame_rate);
        state=0;
    }
    SQUAL->setNum((flow_SQUAL));
}

void Widget::on_dial_valueChanged(int value)
{
    indexY_range=value;
    axisY->setRange(-indexY_range,indexY_range);
}

void Widget::on_dial_2_valueChanged(int value)
{
    index2Y_range = value;
    axis2Y->setRange(-index2Y_range,index2Y_range);
}

void Widget::on_checkdistX_clicked(bool checked)
{
    if(checked)
    {
        m_distX->setVisible(true);
    }
    else
    {
        m_distX->setVisible(false);
    }
}

void Widget::on_checkdistY_clicked(bool checked)
{
    if(checked)
    {
        m_distY->setVisible(true);
    }
    else
    {
        m_distY->setVisible(false);
    }
}
