
#ifndef _MTPlanning_SCENERY_H_
#define _MTPlanning_SCENERY_H_

#include <string.h>
#include <time.h>

#include <MotionPlanning/Planner/PlanningThread.h>
#include <MotionPlanning/PostProcessing/PathProcessingThread.h>
#include <MotionPlanning/CSpace/CSpaceSampled.h>
#include <MotionPlanning/CSpace/CSpaceNode.h>
#include <MotionPlanning/Planner/MotionPlanner.h>
#include <MotionPlanning/Planner/BiRrt.h>

#include "VirtualRobot/Model/ModelSet.h"

#include "../../../Gui/AbstractViewer.h"
#include "../../../Gui/ViewerFactory.h"

using namespace VirtualRobot;
using namespace MotionPlanning;

class MTPlanningScenery
{
public:
    MTPlanningScenery(const std::string &robotFile, SimoxGui::AbstractViewerPtr viewer);
    ~MTPlanningScenery();

    void loadRobotMTPlanning(bool bMultiCollisionCheckers);

    // switches the visualisation of the robot (colModel <-> fullModel)
    void setRobotModelShape(bool collisionModel);

    void buildScene();

    void reset();

    // if bMultiCollisionCheckers is set, more than one collision checker is used
    void buildPlanningThread(bool bMultiCollisionCheckers, int id);
    PathProcessingThreadPtr buildOptimizeThread(CSpaceSampledPtr cspace, CSpacePathPtr path);
    void startPlanning();
    void stopPlanning();

    void startOptimizing();
    void stopOptimizing();

    void checkPlanningThreads();
    void checkOptimizeThreads();

    void getThreadCount(int& nWorking, int& nIdle);
    void getOptimizeThreadCount(int& nWorking, int& nIdle);

    bool getPlannersStarted()
    {
        return this->plannersStarted;
    }
    bool getOptimizeStarted()
    {
        return this->optimizeStarted;
    }

    int getThreads();

protected:

    void addBBCube();

    void getRandomPos(float& x, float& y, float& z);

    std::string robotFilename;
    std::string colModel;
    std::string kinChainName;

    SimoxGui::AbstractViewerPtr viewer;

    bool plannersStarted;
    bool optimizeStarted;

    std::vector<PlanningThreadPtr> planningThreads;
    std::vector<PathProcessingThreadPtr> optimizeThreads;
    std::vector<CSpaceSampledPtr> CSpaces;
    std::vector<RrtPtr> planners;
    std::vector<CSpacePathPtr> solutions;
    std::vector<CSpacePathPtr> optiSolutions;
    //std::map<VisualizationPtr > visualisations;
    std::vector<RobotPtr> robots;

    std::vector< Eigen::VectorXf > startPositions;
    std::vector< Eigen::VectorXf > goalPositions;

    ModelSetPtr environment;
    ObstaclePtr environmentUnited;

    bool robotModelVisuColModel;

    std::string TCPName;
};

#endif
