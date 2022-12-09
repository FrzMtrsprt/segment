#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <opencv2/opencv.hpp>
#include <QLabel>
#include <QMainWindow>
#include <QTimer>

#include "core.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Core core;
    QTimer *timer;
    cv::Mat frame;
    cv::VideoCapture capture;
    void about();
    void update();
    void openFile();
    void startStop();
    void captureFrame();
    void setMatToLabel(QLabel *label, const cv::Mat &mat);
};
#endif // MAINWINDOW_H
