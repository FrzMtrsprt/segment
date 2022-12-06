#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <opencv2/opencv.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

#include "core.h"

using namespace Qt;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->openFileBtn, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(ui->startStopBtn, &QPushButton::clicked, this, &MainWindow::startStop);
    connect(ui->captureBtn, &QPushButton::clicked, this, &MainWindow::captureFrame);
    connect(ui->exitBtn, &QPushButton::clicked, this, &MainWindow::close);

    // Open camera
    if (!capture.open(0))
    {
        QMessageBox::critical(this, u"Error"_s, u"Failed to open the camera."_s);
        close();
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

void MainWindow::update()
{
    // Capture the current frame
    capture >> frame;

    const QImage image =
        QImage(frame.data, frame.cols, frame.rows, QImage::Format_RGB888)
            .rgbSwapped();
    ui->beforeLabel->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::openFile()
{
    // Stop camera capture
    timer->stop();
    ui->startStopBtn->setText(u"Start"_s);

    // Choose image file
    const QString fileName = QFileDialog::getOpenFileName(
        this, u"Choose image"_s, QDir::homePath() + u"/Pictures"_s,
        u"Image Files (*.bmp *.jpeg *.jpg *.png *.webp)"_s);

    // Decode to local 8-bit to support Chinese file names
    frame = cv::imread(fileName.toLocal8Bit().toStdString());

    const QImage image =
        QImage(frame.data, frame.cols, frame.rows, QImage::Format_RGB888)
            .rgbSwapped();
    ui->beforeLabel->setPixmap(QPixmap::fromImage(image));
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
        timer->start(capture.get(cv::CAP_PROP_FPS));
        ui->startStopBtn->setText(u"Stop"_s);
    }
}

void MainWindow::captureFrame()
{
    // Get the current frame
    cv::Mat mat;
    frame.copyTo(mat);
    Core::segmentation(mat);

    const QImage image =
        QImage(mat.data, mat.cols, mat.rows, QImage::Format_RGB888)
            .rgbSwapped();
    ui->afterLabel->setPixmap(QPixmap::fromImage(image));
}
