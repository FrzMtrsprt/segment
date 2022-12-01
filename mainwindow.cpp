#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include <QTimer>

using namespace cv;
using namespace Qt;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::exit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->startStopBtn, &QPushButton::clicked, this, &MainWindow::startStop);
    connect(ui->exitBtn, &QPushButton::clicked, this, &MainWindow::exit);

    // Initialize timer
    if (capture.open(0))
    {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::update);
        ui->startStopBtn->setText(u"Stop"_s);
        timer->start(25);
    }
    else
    {
        QMessageBox::critical(this, u"Error"_s, u"Failed to open the camera."_s);
        exit();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::about()
{
    QMessageBox::information(this, u"About"_s, u"Image Processing Project\n\nBy Huaiyu Chen"_s);
}

void MainWindow::exit()
{
    timer->stop();
    timer->~QTimer();
    QApplication::exit();
}

void MainWindow::update()
{
    capture >> mat;
    const QImage image =
        QImage(mat.data, mat.cols, mat.rows, QImage::Format_RGB888)
            .rgbSwapped();
    const QPixmap pixmap = QPixmap::fromImage(image);
    ui->beforeLabel->setPixmap(pixmap);
    ui->afterLabel->setPixmap(pixmap);
}

void MainWindow::startStop()
{
    if (timer->isActive())
    {
        timer->stop();
        ui->startStopBtn->setText(u"Start"_s);
    }
    else
    {
        timer->start();
        ui->startStopBtn->setText(u"Stop"_s);
    }
}
