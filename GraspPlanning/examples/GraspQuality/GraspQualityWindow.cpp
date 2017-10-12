
#include "GraspQualityWindow.h"
#ifdef Simox_USE_COIN_VISUALIZATION
#include "GraspPlanning/Visualization/CoinVisualization/CoinConvexHullVisualization.h"
#endif
#include "VirtualRobot/EndEffector/EndEffector.h"
#include "VirtualRobot/Workspace/Reachability.h"
#include "VirtualRobot/Model/ManipulationObject.h"
#include "VirtualRobot/Grasping/Grasp.h"
#include "VirtualRobot/IK/GenericIKSolver.h"
#include "VirtualRobot/Grasping/GraspSet.h"
#include "VirtualRobot/CollisionDetection/CDManager.h"
#include "VirtualRobot/XML/ObjectIO.h"
#include "VirtualRobot/XML/ModelIO.h"

#include <QFileDialog>
#include <Eigen/Geometry>

#include <time.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <sstream>

#ifdef Simox_USE_COIN_VISUALIZATION
    #include "../../../Gui/Coin/CoinViewerFactory.h"
    // need this to ensure that static Factory methods are called across library boundaries (otherwise coin Gui lib is not loaded since it is not referenced by us)
    SimoxGui::CoinViewerFactory f;
#endif

using namespace std;
using namespace VirtualRobot;

float TIMER_MS = 30.0f;

GraspQualityWindow::GraspQualityWindow(std::string& robFile, std::string& objFile)
    : QMainWindow(NULL)
{
    VR_INFO << " start " << endl;

    this->robotFile = robFile;
    this->objectFile = objFile;
    setupUI();


    loadObject();
    loadRobot();

    viewer->viewAll();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerCB()));
    timer->start(TIMER_MS);

}


GraspQualityWindow::~GraspQualityWindow()
{
    timer->stop();
    delete timer;
    viewer.reset();
}


void GraspQualityWindow::timerCB()
{
    float x[6];
    x[0] = (float)UI.horizontalSliderX->value();
    x[1] = (float)UI.horizontalSliderY->value();
    x[2] = (float)UI.horizontalSliderZ->value();
    x[3] = (float)UI.horizontalSliderRo->value();
    x[4] = (float)UI.horizontalSliderPi->value();
    x[5] = (float)UI.horizontalSliderYa->value();
    x[0] /= 10.0f;
    x[1] /= 10.0f;
    x[2] /= 10.0f;
    x[3] /= 300.0f;
    x[4] /= 300.0f;
    x[5] /= 300.0f;

    if (x[0] != 0 || x[1] != 0 || x[2] != 0 || x[3] != 0 || x[4] != 0 || x[5] != 0)
    {
        updateObject(x);
    }
}


void GraspQualityWindow::setupUI()
{
    UI.setupUi(this);

    SimoxGui::ViewerFactoryPtr viewerFactory = SimoxGui::ViewerFactory::first(NULL);
    THROW_VR_EXCEPTION_IF(!viewerFactory,"No viewer factory?!");
    viewer = viewerFactory->createViewer(UI.frameViewer);

    connect(UI.pushButtonReset, SIGNAL(clicked()), this, SLOT(resetSceneryAll()));
    connect(UI.pushButtonClose, SIGNAL(clicked()), this, SLOT(closeEEF()));
    connect(UI.pushButtonOpen, SIGNAL(clicked()), this, SLOT(openEEF()));
    connect(UI.pushButtonToTCP, SIGNAL(clicked()), this, SLOT(objectToTCP()));
    connect(UI.pushButtonQuality, SIGNAL(clicked()), this, SLOT(graspQuality()));

    connect(UI.checkBoxColModel, SIGNAL(clicked()), this, SLOT(colModel()));
    connect(UI.checkBoxCones, SIGNAL(clicked()), this, SLOT(frictionConeVisu()));
    connect(UI.checkBoxGWS1, SIGNAL(clicked()), this, SLOT(showGWS()));
    connect(UI.checkBoxGWS2, SIGNAL(clicked()), this, SLOT(showGWS()));
    //connect(UI.checkBoxOWS1, SIGNAL(clicked()), this, SLOT(showOWS()));
    //connect(UI.checkBoxOWS2, SIGNAL(clicked()), this, SLOT(showOWS()));

    connect(UI.horizontalSliderX, SIGNAL(sliderReleased()), this, SLOT(sliderReleased_ObjectX()));
    connect(UI.horizontalSliderY, SIGNAL(sliderReleased()), this, SLOT(sliderReleased_ObjectY()));
    connect(UI.horizontalSliderZ, SIGNAL(sliderReleased()), this, SLOT(sliderReleased_ObjectZ()));
    connect(UI.horizontalSliderRo, SIGNAL(sliderReleased()), this, SLOT(sliderReleased_ObjectA()));
    connect(UI.horizontalSliderPi, SIGNAL(sliderReleased()), this, SLOT(sliderReleased_ObjectB()));
    connect(UI.horizontalSliderYa, SIGNAL(sliderReleased()), this, SLOT(sliderReleased_ObjectG()));

    connect(UI.comboBoxEEF, SIGNAL(activated(int)), this, SLOT(selectEEF(int)));
}


void GraspQualityWindow::resetSceneryAll()
{
}


void GraspQualityWindow::closeEvent(QCloseEvent* event)
{
    quit();
    QMainWindow::closeEvent(event);
}


void GraspQualityWindow::buildVisu()
{
    viewer->clearLayer("robotLayer");

    ModelLink::VisualizationType colModel = (UI.checkBoxColModel->isChecked()) ? ModelLink::Collision : ModelLink::Full;

    // robot
    if (robot)
    {
        VisualizationPtr visu = VisualizationFactory::getGlobalVisualizationFactory()->getVisualization(robot, colModel);
        viewer->addVisualization("robotLayer", "robot", visu);
    }

    // object
    viewer->clearLayer("objectLayer");
    if (object)
    {
        VisualizationPtr visu = VisualizationFactory::getGlobalVisualizationFactory()->getVisualization(object, colModel);
        viewer->addVisualization("objectLayer", "object", visu);
    }

    // friction cones
    viewer->clearLayer("frictionLayer");
    bool fc = (UI.checkBoxCones->isChecked());
    if (fc && contacts.size() > 0 && qualityMeasure)
    {

        GraspPlanning::ContactConeGeneratorPtr cg = qualityMeasure->getConeGenerator();
        float radius = cg->getConeRadius();
        float height = cg->getConeHeight();
        float scaling = 30.0f;

        VisualizationNodePtr visu = VisualizationFactory::getGlobalVisualizationFactory()->createContactVisualization(contacts, height * scaling, radius * scaling, true);
        viewer->addVisualization("frictionLayer", "cones", visu);
    }
}

int GraspQualityWindow::main()
{
    viewer->start(this);

    return 0;
}


void GraspQualityWindow::quit()
{
    std::cout << "GraspQualityWindow: Closing" << std::endl;
    this->close();
    viewer->stop();
    timer->stop();
}

void GraspQualityWindow::loadObject()
{
    if (!objectFile.empty())
    {
        object = ObjectIO::loadManipulationObject(objectFile);
    }

    if (!object)
    {
        object = Obstacle::createBox(50.0f, 50.0f, 10.0f);
    }

    qualityMeasure.reset(new GraspPlanning::GraspQualityMeasureWrenchSpace(object));
    qualityMeasure->calculateObjectProperties();
}

void GraspQualityWindow::loadRobot()
{
    eefs.clear();
    robot.reset();
    robot = ModelIO::loadModel(robotFile);

    if (!robot)
    {
        VR_ERROR << " no robot at " << robotFile << endl;
        return;
    }


    setEEFComboBox();
    selectEEF(0);
    objectToTCP();

    buildVisu();
}

void GraspQualityWindow::objectToTCP()
{
    if (object && eef && eef->getTcp())
    {
        Eigen::Matrix4f pos =  eef->getTcp()->getGlobalPose();
        object->setGlobalPose(pos);
    }
}

void GraspQualityWindow::graspQuality()
{
    if (qualityMeasure && object && contacts.size() > 0)
    {
        qualityMeasure->setContactPoints(contacts);
        float volume = qualityMeasure->getVolumeGraspMeasure();
        float epsilon = qualityMeasure->getGraspQuality();
        bool fc = qualityMeasure->isGraspForceClosure();
        cout << "Grasp Quality (epsilon measure):" << epsilon << endl;
        cout << "v measure:" << volume << endl;
        cout << "Force closure:";

        if (fc)
        {
            cout << "yes" << endl;
        }
        else
        {
            cout << "no" << endl;
        }

    }
}
void GraspQualityWindow::selectEEF(int nr)
{
    eef.reset();

    if (nr < 0 || nr >= (int)eefs.size())
    {
        return;
    }

    eef = eefs[nr];
}

void GraspQualityWindow::setEEFComboBox()
{
    UI.comboBoxEEF->clear();
    eef.reset();
    eefs.clear();

    if (!robot)
    {
        return;
    }

    eefs = robot->getEndEffectors();

    for (size_t i = 0; i < eefs.size(); i++)
    {
        QString nameEEF(eefs[i]->getName().c_str());
        UI.comboBoxEEF->addItem(nameEEF);
    }
}

void GraspQualityWindow::closeEEF()
{
    contacts.clear();

    if (eef)
    {
        contacts = eef->closeActors(object);
    }

    buildVisu();
}

void GraspQualityWindow::openEEF()
{
    contacts.clear();

    if (eef)
    {
        eef->openActors();
    }

    buildVisu();
}



void GraspQualityWindow::updateObject(float x[6])
{
    if (object)
    {
        //cout << "getGlobalPose robot:" << endl << robotEEF->getGlobalPose() << endl;
        //cout << "getGlobalPose TCP:" << endl <<  robotEEF_EEF->getTcp()->getGlobalPose() << endl;
        Eigen::Matrix4f m;
        MathTools::posrpy2eigen4f(x, m);

        m = object->getGlobalPose() * m;
        object->setGlobalPose(m);
        cout << "object " << endl;
        cout << m << endl;

    }
}

void GraspQualityWindow::sliderReleased_ObjectX()
{
    UI.horizontalSliderX->setValue(0);
}

void GraspQualityWindow::sliderReleased_ObjectY()
{
    UI.horizontalSliderY->setValue(0);
}

void GraspQualityWindow::sliderReleased_ObjectZ()
{
    UI.horizontalSliderZ->setValue(0);
}

void GraspQualityWindow::sliderReleased_ObjectA()
{
    UI.horizontalSliderRo->setValue(0);
}

void GraspQualityWindow::sliderReleased_ObjectB()
{
    UI.horizontalSliderPi->setValue(0);
}

void GraspQualityWindow::sliderReleased_ObjectG()
{
    UI.horizontalSliderYa->setValue(0);
}

void GraspQualityWindow::frictionConeVisu()
{
    buildVisu();
}

void GraspQualityWindow::colModel()
{
    buildVisu();
}

void GraspQualityWindow::showGWS()
{
    viewer->clearLayer("gws");
    viewer->clearLayer("ows");

    Eigen::Matrix4f m = object->getGlobalPose();


    if (!qualityMeasure)
    {
        return;
    }

    VirtualRobot::MathTools::ConvexHull6DPtr ch = qualityMeasure->getConvexHullGWS();

    if (!ch)
    {
        return;
    }

    GraspPlanning::ConvexHullVisualizationPtr chv;
    GraspPlanning::ConvexHullVisualizationPtr chv2;
#ifdef Simox_USE_COIN_VISUALIZATION
    chv.reset(new GraspPlanning::CoinConvexHullVisualization(ch, true));
    chv2.reset(new GraspPlanning::CoinConvexHullVisualization(ch, false));
#endif

    if (UI.checkBoxGWS1->isChecked() && chv)
    {
        VisualizationPtr v = chv->getVisualization();
        VisualizationFactory::getGlobalVisualizationFactory()->applyDisplacement(v, m);
        viewer->addVisualization("gws","gws1",v);
    }

    if (UI.checkBoxGWS2->isChecked() && chv2)
    {
        VisualizationPtr v2 = chv2->getVisualization();
        VisualizationFactory::getGlobalVisualizationFactory()->applyDisplacement(v2, m);
        viewer->addVisualization("gws","gws2",v2);
    }
}

void GraspQualityWindow::showOWS()
{

}
