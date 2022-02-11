#ifndef LINECHARTVIEW_H
#define LINECHARTVIEW_H

#include <QChartView>
#include <QRubberBand>

QT_CHARTS_USE_NAMESPACE

class LineChartView : public QChartView
{
public:
    LineChartView(QWidget *parent = 0);
    LineChartView(QChart *chart, QWidget *parent = 0);

    void setLineChart(QChart *lineChart);
protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    QChart *lineChart;
    bool isClicking;

    int xOld;
    int yOld;
};

#endif // LINECHARTVIEW_H
