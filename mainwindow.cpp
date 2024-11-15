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
#include <QFile>

#include <string>
#include "onnxruntime_cxx_api.h"
#include <array>


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
//QFile file("logDati.csv");
//QTextStream out(&file);
QStringList header;
uint8_t nearest;



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer=new QTimer(this);
    /*
    srand(static_cast<unsigned int>(time(0)));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Impossibile aprire il file:" << file.errorString();
        return;
    }
    */
    //header csv file
    //header << "BirdX" << "BirdY" << "UpperPipeX" << "UpperPipeY" << "BottomPipeX" << "BottomPipeY" << "Jump";
    //out << header.join(",") << "\n";


    inizializzaGame();
    start();
}

MainWindow::~MainWindow()
{
    //file.close();
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
    premuto=true;

    if(e->key()==Qt::Key_Space && !e->isAutoRepeat()){
        yPos-=16;
        ui->bird->move(ui->bird->x(), yPos);
    }
    //scriviFile(ui->bird->x(),ui->bird->y(),listaUpperPipe[nearest]->x(),listaUpperPipe[nearest]->y(),listaBottomPipe[nearest]->x(),listaBottomPipe[nearest]->y(),1);
    premuto=false;
}


void MainWindow::updatePosition(){
    //qDebug()<<"aggiorno";
    nearest=-1;
    yPos += 1; // Sposta verso il basso
    ui->bird->move(ui->bird->x(), yPos); // Aggiorna la posizione della QLabel

    // Riavvolgi se esce dallo schermo
    if (yPos > height()) { 
        reset();
        terminato=true;
    }


    uint16_t min=UINT16_MAX;
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
        if(ui->bird->x()<=listaUpperPipe[i]->x()){
            if(listaUpperPipe[i]->x()-ui->bird->x()<min){
                min=listaUpperPipe[i]->x()-ui->bird->x();
                nearest=i;
                //qDebug()<<"nearest: "<<nearest<<"diff "<<min;
            }
        }
    }

    if(scegliAzione(ui->bird->x(),ui->bird->y(),listaUpperPipe[nearest]->x(),listaUpperPipe[nearest]->y(),listaBottomPipe[nearest]->x(),listaBottomPipe[nearest]->y())!=0){
        yPos-=16;
        ui->bird->move(ui->bird->x(), yPos);
    }

    //if(premuto==false)
        //scriviFile(ui->bird->x(),ui->bird->y(),listaUpperPipe[nearest]->x(),listaUpperPipe[nearest]->y(),listaBottomPipe[nearest]->x(),listaBottomPipe[nearest]->y(),0);
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


int MainWindow::scegliAzione(int birdX,int birdY,int UpperPipeX,int UpperPipeY,int BottomPipeX,int BottomPipeY){

    std::wstring model_path = L"C:/Users/giaco/OneDrive/Documenti/flappybird/model_final.onnx";
    Ort::RunOptions runOptions;
    // Inizializza l'ambiente ONNX Runtime
    try{
        Ort::Env env=Ort::Env(ORT_LOGGING_LEVEL_WARNING, "ONNXRuntimeExample");
        Ort::Session session=Ort::Session(env, model_path.c_str(), Ort::SessionOptions{});

    // Definisci la forma dell'input e dell'output
    const std::array<int64_t, 2> inputShape = {1, 6};  // 1 campione, 6 caratteristiche
    const std::array<int64_t, 1> outputShape = {1};    // 1 output

    // Prepara l'input
    std::array<float, 6> input = {(float) birdX,(float) birdY,(float) UpperPipeX,(float) UpperPipeY,(float) BottomPipeX,(float) BottomPipeY};
    std::array<int64_t, 1> result;  // Variabile per memorizzare l'output

    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    auto inputTensor = Ort::Value::CreateTensor<float>(memory_info, input.data(), input.size(), inputShape.data(), inputShape.size());
    auto outputTensor = Ort::Value::CreateTensor<int64_t>(memory_info, result.data(), result.size(), outputShape.data(), outputShape.size());

    // Ottieni i nomi degli input e degli output
    Ort::AllocatorWithDefaultOptions ort_alloc;
    // Otteniamo il nome dell'input e dell'output senza usare get()
    auto inputName = session.GetInputNameAllocated(0, ort_alloc);
    auto outputName = session.GetOutputNameAllocated(0, ort_alloc);

    // Definisci gli array di nomi degli input e output
    const std::array<const char*, 1> inputNames = {inputName.get()};
    const std::array<const char*, 1> outputNames = {outputName.get()};

    try{
        session.Run(runOptions, inputNames.data(), &inputTensor, 1, outputNames.data(), &outputTensor, 1);

        // Estrai e stampa il risultato
        int* output_data = outputTensor.GetTensorMutableData<int>();
        qDebug() << "Prediction: " << output_data[0];
        return output_data[0];
    }
    catch(const Ort::Exception& e){
        qDebug()<<"Errore ONNX Runtime: " << e.what();
        return 1;
    }
    }catch(Ort::Exception e){
        qDebug()<<"Errore "<<e.what();
        return 0;
    }

}

/*
void MainWindow::scriviFile(int birdX,int birdY,int UpperPipeX,int UpperPipeY,int BottomPipeX,int BottomPipeY,int Jump){
    mutex.lock();
    header.clear();
    header << QString::number(birdX) << QString::number(birdY) << QString::number(UpperPipeX) << QString::number(UpperPipeY) << QString::number(BottomPipeX) << QString::number(BottomPipeY) << QString::number(Jump);
    out << header.join(",") << "\n";
    mutex.unlock();
}
*/
