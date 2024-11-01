#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMutex>
#include <QMainWindow>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void updatePosition();
    bool terminato=false;

public slots:
    void start();
    void reset();

private:
    Ui::MainWindow *ui;
    void creaLabel(QList<QLabel*> lista);
    void keyPressEvent(QKeyEvent* e);
    void inizializzaGame();
    void train(uint16_t n);
    bool resetCondition(uint8_t i);
};
#endif // MAINWINDOW_H
