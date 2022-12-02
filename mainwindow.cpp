#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "core.h"

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

    // Open camera
    if (!capture.open(0))
    {
        QMessageBox::critical(this, u"Error"_s, u"Failed to open the camera."_s);
        exit();
    }
    move(0, 0);

    // Initialize timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update);
    ui->startStopBtn->setText(u"Stop"_s);
    timer->start(25);
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
    capture.release();
    QApplication::exit();
}

void MainWindow::update()
{
    // capture the current frame
    capture >> mat;

    const QImage imageBefore =
        QImage(mat.data, mat.cols, mat.rows, QImage::Format_RGB888)
            .rgbSwapped();

    // Do fragmentation
    Core::segmentation(mat);

    const QImage imageAfter =
        QImage(mat.data, mat.cols, mat.rows, QImage::Format_RGB888)
            .rgbSwapped();

    // Show the fragmentated picture
    ui->beforeLabel->setPixmap(QPixmap::fromImage(imageBefore));
    ui->afterLabel->setPixmap(QPixmap::fromImage(imageAfter));
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
