#ifndef WIDGET_H
#define WIDGET_H

#include <QtCharts/QChart>
#include <QWidget>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGraphicsWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGraphicsGridLayout>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtCharts/QLineSeries>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QLabel>
#include <QtCore/QIODevice>
#include <QTimer>
#include <QtCore/QTime>
#include <QtCharts/QValueAxis>
namespace Ui {
class Widget;
}
QT_CHARTS_USE_NAMESPACE
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();


private slots:
    void on_pushButton_clicked(bool checked);

    void SerialReceive();

    void on_checkX_clicked(bool checked);

    void on_checkY_clicked(bool checked);

    void handleTimeout();
    void on_dial_valueChanged(int value);

    void on_dial_2_valueChanged(int value);

    void on_checkdistX_clicked(bool checked);

    void on_checkdistY_clicked(bool checked);

private:
    QSerialPort *serial;
    QByteArray requestData;//（用于存储从串口那读取的数据）
    Ui::Widget *ui;
    QGridLayout *m_mainLayout;
    QLabel * SQUAL; //显示环境质量
    QChart *m_chart;
    QLabel *sum_label;
    QLineSeries *m_seriesX;
    QLineSeries *m_seriesY;
    QLineSeries *m_distX;
    QLineSeries *m_distY;
    QChartView *chartView ;

    int index_x,index_y;
    int count;
    int sum_data;
    int16_t flow_x;
    int16_t flow_y;
    uint8_t flow_sum;
    uint8_t flow_SQUAL;
    int16_t dist_x;
    int16_t dist_y;

    QTimer *timer;
    int frame_rate;
    int indexX_range,indexY_range,index2Y_range;
    QLabel *frameLable;

    int index_max;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QValueAxis *axis2Y;

};

#endif // WIDGET_H
