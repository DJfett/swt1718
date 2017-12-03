#include "simulatorwindow.h"
#include "ui_simulatorwindow.h"

SimulatorWindow::SimulatorWindow(const Map &nm, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SimulatorWindow)
{
    ui->setupUi(this);
    if(&nm == nullptr) {
        //TODO : Egal was geklickt wird : Es muss eine Map geladen werden
    }
    else {
        this->m = nm;
        //Scene erstellen
        scene = new QGraphicsScene(this);
        //Größe der Scene setzen
        scene->setSceneRect(0,0,m.getSizeX()*m.getGridSize(),m.getSizeY()*m.getGridSize());
        ui->graphicsView->setScene(scene);
        //Gesamtpfad der Map bestimmen und zeichnen
        this->mapPath = m.getMapPath();
        scene->addPath(mapPath);

        sv=0;

        // Timer erstellen
        timer = new QTimer();
        leftTimer = new QTimer();
        rightTimer = new QTimer();
        collisionDetectionTimer = new QTimer();

        //Verbinde die Timer mit der fortbewegung(advance) und auslenkung(left), (right)
        connect(timer, SIGNAL(timeout()), scene, SLOT(advance()));
        connect(leftTimer, &QTimer::timeout, [this]{ sv->left(); });
        connect(rightTimer, &QTimer::timeout, [this]{ sv->right(); });
        connect(collisionDetectionTimer, &QTimer::timeout, [this] { collisionDetection(); });

        ui->graphicsView->setEnabled(false);
    }
}

// Für Simulator umänderns
void SimulatorWindow::collisionDetection() {
    QGraphicsPathItem mapBoundaries;
    mapBoundaries.setPath(m.getMapPath());

    Qt::ItemSelectionMode mode = Qt::IntersectsItemShape; //Modus für Kollisionsabfrage

    if(!mapBoundaries.collidesWithItem(sv,mode)) {
        sv->setColor(Qt::green);
    }
    else {
        sv->setColor(Qt::red);
        timer->stop();
        leftTimer->stop();
        rightTimer->stop();
    }
}

SimulatorWindow::~SimulatorWindow()
{
    delete ui;
}

void SimulatorWindow::on_actionSimulation_starten_triggered()
{
    if(sv != 0)
    {
        for(int i = 0; i<sv->getNumberOfSensors(); i++)
        {
            scene->removeItem(sv->getSensor(i));
            delete sv->getSensor(i);
        }
        delete sv;
        timer->stop();
        leftTimer->stop();
        rightTimer->stop();
    }
    // Autonomes Fahrzeug hinzufügen
    sv = new SmartVehicle(0,1,2,m.getStartingPoint().x(), m.getStartingPoint().y());
    scene->addItem(sv);

    for(int i = 0; i<sv->getNumberOfSensors(); i++)
        scene->addItem(sv->getSensor(i));

    collisionDetectionTimer->start();
}

void SimulatorWindow::keyPressEvent(QKeyEvent *event)
{
        //Starte die Timer wenn bestimmter Knopf gedrückt wird(Nur zu tetszwecken)
        if(event->key() == Qt::Key_Up) {
            timer->start(10);
        }
        else if(event->key() == Qt::Key_Left) {
            leftTimer->start(10);
        }

        else if(event->key() == Qt::Key_Right) {
            rightTimer->start(10);
        }

        QWidget::keyPressEvent(event);
}

void SimulatorWindow::keyReleaseEvent(QKeyEvent *event)
{
        //Schalte Timer wieder aus, wenn Knopf losgelassen wird
        if(event->key() == Qt::Key_Up)
            timer->stop();
        else if(event->key() == Qt::Key_Left)
            leftTimer->stop();
        else if(event->key() == Qt::Key_Right)
            rightTimer->stop();

        QWidget::keyReleaseEvent(event);

}
