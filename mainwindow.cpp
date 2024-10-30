/*
#undef slots
#include "torch/torch.h"
#include "torch/jit.h"
#include "torch/nn.h"
#include "torch/script.h"
#define slots Q_SLOTS
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QRandomGenerator>
#include <QKeyEvent>


uint16_t yPos=200;

QList<QLabel*> listaUpperPipe;
QList<QLabel*> listaBottomPipe;

uint16_t xTrasl=135;


uint16_t minUpper;
uint16_t maxUpper;

uint16_t minBottom;
uint16_t maxBottom;

QLabel* punteggioLabel;
uint16_t point=0;

QLabel* upperPipe1;
QLabel* upperPipe2;

QLabel* bottomPipe1;
QLabel* bottomPipe2;
QTimer* timer;





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer=new QTimer(this);

    srand(static_cast<unsigned int>(time(0)));

    inizializzaGame();
    start();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::inizializzaGame(){


    ui->bird->setGeometry(50, yPos, 100, 30);

    minUpper=0;
    maxUpper=ui->upperPipe->height()/2;

    int valoreUpperY = QRandomGenerator::global()->bounded(minUpper, maxUpper);

    minBottom=340;
    maxBottom=640-(ui->bottomPipe->height()/2);
    int valoreBottomY = QRandomGenerator::global()->bounded(minBottom, maxBottom);

    ui->upperPipe->setGeometry(220,-valoreUpperY,49,301);
    ui->bottomPipe->setGeometry(220,valoreBottomY,49,301);

    listaUpperPipe.append(ui->upperPipe);
    listaBottomPipe.append(ui->bottomPipe);

    upperPipe1=new QLabel(this);
    upperPipe1->setStyleSheet(listaUpperPipe.last()->styleSheet());
    QRect lastGeometry = listaUpperPipe.last()->geometry();
    upperPipe1->setGeometry(lastGeometry.translated(xTrasl, 0));
    listaUpperPipe.append(upperPipe1);

    upperPipe2=new QLabel(this);
    upperPipe2->setStyleSheet(listaUpperPipe.last()->styleSheet());
    lastGeometry = listaUpperPipe.last()->geometry();
    upperPipe2->setGeometry(lastGeometry.translated(xTrasl, 0));
    listaUpperPipe.append(upperPipe2);




    bottomPipe1=new QLabel(this);
    bottomPipe1->setStyleSheet(listaBottomPipe.last()->styleSheet());
    lastGeometry = listaBottomPipe.last()->geometry();
    bottomPipe1->setGeometry(lastGeometry.translated(xTrasl, 0));
    listaBottomPipe.append(bottomPipe1);

    bottomPipe2=new QLabel(this);
    bottomPipe2->setStyleSheet(listaBottomPipe.last()->styleSheet());
    lastGeometry = listaBottomPipe.last()->geometry();
    bottomPipe2->setGeometry(lastGeometry.translated(xTrasl, 0));
    listaBottomPipe.append(bottomPipe2);


    punteggioLabel=new QLabel(this);
    punteggioLabel->setStyleSheet("");
    QFont font;
    font.setPointSize(14);
    font.setBold(true);
    punteggioLabel->setFont(font);
    punteggioLabel->setText(QString::number(point));


}

void MainWindow::start(){
    qDebug()<<"start timer";
    connect(timer, &QTimer::timeout,this,&MainWindow::updatePosition);
    timer->start(16); // Aggiorna ogni 16 ms per 60 fps
}


void MainWindow::keyPressEvent(QKeyEvent* e){
    if(e->key()==Qt::Key_Space && !e->isAutoRepeat()){
        yPos-=16;
        ui->bird->move(ui->bird->x(), yPos);
    }
}


void MainWindow::updatePosition(){
    //qDebug()<<"aggiorno";
    yPos += 1; // Sposta verso il basso
    ui->bird->move(ui->bird->x(), yPos); // Aggiorna la posizione della QLabel

    // Riavvolgi se esce dallo schermo
    if (yPos > height()) { 
        reset();
        terminato=true;
    }


    uint16_t valoreUpperY=0;
    uint16_t valoreBottomY=370;
    for(uint8_t i=0;i<listaBottomPipe.size();i++){
        if((ui->bird->x()+ui->bird->width())==listaBottomPipe[i]->x()){
            point+=1;
            punteggioLabel->setText(QString::number(point));

            if(resetCondition(i)){
                //qDebug()<<ui->bird->y()<<listaBottomPipe[i]->y()<<"sotto";
                //qDebug()<<ui->bird->y()<<listaUpperPipe[i]->y()+listaUpperPipe[i]->height()<<"sopra";
                terminato=true;
                reset();
            }
        }



        listaBottomPipe[i]->move(listaBottomPipe[i]->x()-1,listaBottomPipe[i]->y());
        listaUpperPipe[i]->move(listaBottomPipe[i]->x()-1,listaUpperPipe[i]->y());

        if(listaUpperPipe[i]->x()+ui->upperPipe->width()<0){
            valoreUpperY = QRandomGenerator::global()->bounded(minUpper, maxUpper);
            valoreBottomY = QRandomGenerator::global()->bounded(minBottom, maxBottom);
            //qDebug()<<valoreUpperY<<" "<<i;
            listaUpperPipe[i]->setGeometry(ui->centralwidget->width(),-valoreUpperY,49,301);
            listaBottomPipe[i]->setGeometry(ui->centralwidget->width(),valoreBottomY,49,301);
        }
    }
}


void MainWindow::reset(){
    yPos = 200;
    point=0;
    punteggioLabel->setText(QString::number(point));
    ui->bird->setGeometry(50, yPos, 100, 30);

    minUpper=0;
    maxUpper=ui->upperPipe->height()/2;

    uint16_t valoreUpperY = QRandomGenerator::global()->bounded(minUpper, maxUpper);

    minBottom=340;
    maxBottom=640-(ui->bottomPipe->height()/2);
    uint16_t valoreBottomY = QRandomGenerator::global()->bounded(minBottom, maxBottom);

    ui->upperPipe->setGeometry(220,-valoreUpperY,49,301);
    ui->bottomPipe->setGeometry(220,valoreBottomY,49,301);

    for(uint8_t i=1;i<listaBottomPipe.size();i++){
        listaBottomPipe[i]->setGeometry(yPos+(i*xTrasl),valoreBottomY,49,301);
        listaUpperPipe[i]->setGeometry(yPos+(i*xTrasl),-valoreUpperY,49,301);
    }

    terminato=false;
}


bool MainWindow::resetCondition(uint8_t i){
    if(ui->bird->y()>=(listaBottomPipe[i]->y())||ui->bird->y()<=(listaUpperPipe[i]->y()+listaUpperPipe[i]->height())){
        return true;
    }
    return false;
}

