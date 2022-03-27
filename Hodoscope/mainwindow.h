#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "draw_widget.h"
#include "Tools.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Draw_Widget   *FrontView;
    Draw_Widget   *SideView;

private slots:
    void on_Menu_StandParameters_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
