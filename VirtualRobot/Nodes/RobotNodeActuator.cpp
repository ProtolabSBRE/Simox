
#include "RobotNodeActuator.h"
#include "../VirtualRobotException.h"

namespace VirtualRobot {

RobotNodeActuator::RobotNodeActuator(RobotNodePtr node)
{
	robotNode = node;
}

RobotNodeActuator::~RobotNodeActuator()
{
}

void RobotNodeActuator::updateVisualizationPose( const Eigen::Matrix4f& pose, bool updateChildren )
{
	robotNode->updateVisualizationPose(pose, updateChildren);
}


} // namespace
