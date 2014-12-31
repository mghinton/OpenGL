#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    //ui->lcdRate->display(ui->glwidget->getRate());
    //ui->lcdLength->display(ui->glwidget->getLength());
    //ui->lcdWeight->display(ui->glwidget->getWeight());

    // ADD YOUR CODE
}

MainWindow::~MainWindow() {
    delete ui;
}
