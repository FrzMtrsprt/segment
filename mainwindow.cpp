#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <opencv2/opencv.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

#include "core.h"

using namespace Qt::Literals::StringLiterals;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->openFileBtn, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(ui->startStopBtn, &QPushButton::clicked, this, &MainWindow::startStop);
    connect(ui->sBox, &QSpinBox::valueChanged, this, &MainWindow::captureFrame);
    connect(ui->mBox, &QSpinBox::valueChanged, this, &MainWindow::captureFrame);
    connect(ui->captureBtn, &QPushButton::clicked, this, &MainWindow::captureFrame);
    connect(ui->exitBtn, &QPushButton::clicked, this, &MainWindow::close);

    // Open camera
    if (!capture.open(0))
    {
        QMessageBox::critical(this, u"Error"_s, u"Failed to open the camera."_s);
        close();
    }

    setWindowState(Qt::WindowMaximized);

    ui->sBox->setValue(50);
    ui->mBox->setValue(10);

    // Initialize timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update);
    ui->startStopBtn->setText(u"Stop"_s);
    timer->setInterval(25);
    timer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::about()
{
    QMessageBox::information(this, u"About"_s, u"Image Segmentation\n\nBy Huaiyu Chen"_s);
}

void MainWindow::update()
{
    // Capture the current frame
    capture.read(frame);

    setMatToLabel(ui->beforeLabel, frame);
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

    setMatToLabel(ui->beforeLabel, frame);
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

void MainWindow::captureFrame()
{
    // Get the current frame & parameters
    frame.copyTo(core.mat);
    core.s = ui->sBox->value();
    core.m = ui->mBox->value();

    core.segmentation();

    setMatToLabel(ui->afterLabel, core.mat);
}

void MainWindow::setMatToLabel(QLabel *label, const cv::Mat &mat)
{
    label->setPixmap(
        QPixmap::fromImage(
            QImage(mat.data, mat.cols, mat.rows, QImage::Format_RGB888)
                .scaled(label->size(), Qt::KeepAspectRatio)
                .rgbSwapped()));
}
