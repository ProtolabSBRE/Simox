
#ifndef __ShowRobot_WINDOW_H_
#define __ShowRobot_WINDOW_H_

#include <VirtualRobot/VirtualRobot.h>
#include <VirtualRobot/Robot.h>
#include <VirtualRobot/VirtualRobotException.h>
#include <VirtualRobot/Nodes/RobotNode.h>
#include <VirtualRobot/XML/RobotIO.h>
#include <VirtualRobot/Visualization/VisualizationFactory.h>
#include <VirtualRobot/Obstacle.h>
#include <VirtualRobot/RobotNodeSet.h>
#include <VirtualRobot/Visualization/CoinVisualization/CoinVisualizationNode.h>
#include <VirtualRobot/Visualization/CoinVisualization/CoinVisualization.h>

#include <Gui/Ogre/OgreViewer.h>

#include <QtCore/QtGlobal>
#include <QtGui/QtGui>
#include <QtCore/QtCore>

#include <string>
#include <vector>

#include "ui_RobotViewer.h"

class showRobotWindow : public QMainWindow
{
    Q_OBJECT
public:
    showRobotWindow(std::string& sRobotFilename);
    ~showRobotWindow();

    int main();

public slots:
    void quit();

    /*!< Overriding the close event, so we know when the window was closed by the user. */
    void closeEvent(QCloseEvent* event);

    void resetSceneryAll();
    void rebuildVisualization();
    void showRobot();
    void loadRobot();
    void selectJoint(int nr);
    void selectRNS(int nr);
    void jointValueChanged(int pos);
    void showCoordSystem();
    void robotStructure();
    void robotCoordSystems();
    void robotFullModel();
    void showSensors();
    void closeHand();
    void openHand();
    void selectEEF(int nr);
    void selectRobot();
    void displayPhysics();

    void exportXML();

protected:
    void setupUI();
    QString formatString(const char* s, float f);
    void updateJointBox();
    void updateRNSBox();
    void updateEEFBox();
    void displayTriangles();
    void updatRobotInfo();
    Ui::MainWindowShowRobot UI;

    Gui::OgreViewer* viewer;

    VirtualRobot::RobotPtr robot;
    std::string m_sRobotFilename;
    std::vector < VirtualRobot::RobotNodePtr > allRobotNodes;
    std::vector < VirtualRobot::RobotNodePtr > currentRobotNodes;
    std::vector < VirtualRobot::RobotNodeSetPtr > robotNodeSets;
    std::vector < VirtualRobot::EndEffectorPtr > eefs;
    VirtualRobot::EndEffectorPtr currentEEF;
    VirtualRobot::RobotNodeSetPtr currentRobotNodeSet;
    VirtualRobot::RobotNodePtr currentRobotNode;

    bool useColModel;
    bool structureEnabled;
    bool physicsCoMEnabled;
    bool physicsInertiaEnabled;

    VirtualRobot::CoinVisualizationPtr visualization;

    void testPerformance(VirtualRobot::RobotPtr robot, VirtualRobot::RobotNodeSetPtr rns);
};

#endif // __ShowRobot_WINDOW_H_