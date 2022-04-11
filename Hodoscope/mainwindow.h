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
    Stand_Info  StandInfo;
    Draw_Widget* FrontView;
    Draw_Widget* SideView;

    Draw_Widget* FrontView_test;
    Draw_Widget* SideView_test;

private slots:
    void on_Menu_StandParameters_triggered();

private:
    Ui::MainWindow *ui;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
};
#endif // MAINWINDOW_H
