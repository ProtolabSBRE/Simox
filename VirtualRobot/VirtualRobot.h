/**
* This file is part of Simox.
*
* Simox is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* Simox is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* @package    VirtualRobot
* @author     Nikolaus Vahrenkamp
* @copyright  2011 Nikolaus Vahrenkamp
*             GNU Lesser General Public License
*
*/
#ifndef _VirtualRobot_BasicDefinitions_h_
#define _VirtualRobot_BasicDefinitions_h_

#include "VirtualRobotImportExport.h"

/*! \defgroup VirtualRobot The VirtualRobot Library
* With the VirtualRobot library you can define complex robot structures,
* perform collision detection, visualize robots and environments, do reachability analysis and generic IK solvers are provided.
*/



/** \mainpage Simox: A simulation, motion and grasp planning toolbox.

  \section Introduction Introduction

  The aim of the lightweight platform independent C++ toolbox \b Simox is to provide a set of
  algorithms for 3D simulation of robot systems, sampling based motion planning and grasp
  planning. Simox consists of three libraries (VirtualRobot, MotionPlanning and GraspPlanning) and numerous
  examples showing how these libraries can be used to build complex tools in the
  context of mobile manipulation.

  Further information and documentation can be found at the wiki pages: https://gitlab.com/Simox/simox/wikis/home

  \section VirtualRobot VirtualRobot

  The library \b VirtualRobot can be used to define complex
  robot systems, which may cover multiple robots with many degrees of freedom. The robot
  structure and its visualization can be easily defined via XML files and environments with
  obstacles and objects to manipulate are supported. Further, basic robot simulation components,
  as Jacobian computations and generic Inverse Kinematics (IK) solvers, are offered by
  the library. Beyond that, extended features like tools for analyzing the reachable workspace
  for robotic manipulators or contact determination for grasping are included.
  \image html VR.png

  \section MotionPlanning Motion Planning

  With \b MotionPlanning, a library for planning collision-free motions is offered, which directly incorporates
  with the data provided by VirtualRobot. The algorithms cover state-of-the-art implementations
  of sampling-based motion planning approaches (e.g. Rapidly-exploring Random Trees)
  and interfaces that allow to conveniently implement own planners. Since MotionPlanning was designed
  for planning in high-dimensional configuration spaces, complex planning problems for robots
  with a high number of degrees of freedom (DoF) can be solved efficiently.

  \image html MotionPlanning.png

  \section GraspPlanning Grasp Planning

  \b GraspPlanning offers possibilities to compute the grasp quality for generic end-effector definitions,
  e.g. a humanoid hand. The implemented 6D wrench-space computations can be used
  to easily (and quickly) determine the quality of an applied grasp to an object. Furthermore,
  the implemented planners are able to generate grasp maps for given objects automatically.

  \image html GraspStudio.png

  \section Wiki Installation, tutorials and documentation

  Since complex frameworks have to incorporate with several libraries in order to provide full
  functionality, several issues may arise when setting up the environment, such as dependency
  problems, incompatible library versions or even non-existing ports of needed libraries for the
  used operating systems. Hence, only a limited set of libraries are used by the Simox core in
  order to make it compile. Extended functionality (e.g. visualization) can be turned off in
  order to allow Simox compiling on most platforms. Further dependencies are encapsulated
  with interfaces, making it easy to exchange e.g. the collision engine or the visualization
  functionality. As a reference implementation Simox offers Coin3D/SoQt-based visualization
  support.

  Please have a look at the wiki pages: https://gitlab.com/Simox/simox/wikis/home
 *
 */

// include compile time defines, generated by cmake
//#include "definesVR.h"
// for now we know that PQP is used- ToDo: Change CollisionChecker implementation, use AbstractFactoryMethods
#define VR_COLLISION_DETECTION_PQP


#ifdef WIN32
// needed to have M_PI etc defined
#if !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif

#endif


// allow std vector to be used with Eigen objects

#include<Eigen/StdVector>
#ifndef EIGEN_STL_VECTOR_SPECIFICATION_DEFINED
#define EIGEN_STL_VECTOR_SPECIFICATION_DEFINED
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Vector2f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Vector3f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Vector4f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::VectorXf)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix2f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix3f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix4f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::MatrixXf)

EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Vector3d)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Vector3i)
#endif

#ifndef Q_MOC_RUN // workaround for some bug in some QT/boost versions
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#endif


#include <iostream>
#include <sstream>
#include <cmath>
#include <memory>
#include <mutex>

//#ifdef _DEBUG
//#ifdef WIN32
// ENABLE MEMORY LEAK CHECKING FOR WINDOWS
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#endif
//#endif

namespace VirtualRobot
{
    // only valid within the VirtualRobot namespace
    using std::cout;
    using std::endl;

    class CoMIK;
    class DifferentialIK;
    class HierarchicalIK;
    class Constraint;
    class TSRConstraint;
    class BalanceConstraint;
    class PoseConstraint;
    class PositionConstraint;
    class OrientationConstraint;
    class SupportPolygon;
    class EndEffector;
    class EndEffectorActor;
    class CollisionChecker;
    class CollisionModel;
    class TriMeshModel;
    class Obstacle;
    class VisualizationSet;
    class Visualization;
    class VisualizationFactory;
    class OffscreenRenderer;
    class Scene;
    class ModelConfig;
    class Grasp;
    class GraspSet;
    class ManipulationObject;
    class CDManager;
    class Reachability;
    class WorkspaceRepresentation;
    class WorkspaceData;
    class PoseQualityMeasurement;
    class PoseQualityManipulability;
    class Trajectory;
    class SphereApproximator;
    class BasicGraspQualityMeasure;
    class WorkspaceGrid;
    class WorkspaceDataArray;
    class Model;
    class ModelNode;
    class ModelLink;
    class ModelJoint;
    class ModelJointFixed;
    class ModelJointPrismatic;
    class ModelJointRevolute;
    class ModelSet;
    class ModelNodeSet;
    class LinkSet;
    class JointSet;
    class ModelNodeAttachment;
	class ModelNodeAttachmentFactory;
    class Frame;
    class ColorMap;
    class Sensor;

    typedef Model Robot; //!< A typedef to make a differ between a robot and a simple object
    typedef ModelNode RobotNode; //!< A typedef to make a differ between a robot and a simple object
    typedef ModelLink RobotLink; //!< A typedef to make a differ between a robot and a simple object
    typedef ModelJoint RobotJoint; //!< A typedef to make a differ between a robot and a simple object
    typedef ModelJointFixed RobotJointFixed; //!< A typedef to make a differ between a robot and a simple object
    typedef ModelJointPrismatic RobotJointPrismatic; //!< A typedef to make a differ between a robot and a simple object
    typedef ModelJointRevolute RobotJointRevolute; //!< A typedef to make a differ between a robot and a simple object
    typedef ModelNodeSet RobotNodeSet; //!< A typedef to make a differ between a robot and a simple object
    typedef ModelNodeAttachment RobotNodeAttachment; //!< A typedef to make a differ between a robot and a simple object
    typedef ModelNodeAttachmentFactory RobotNodeAttachmentFactory; //!< A typedef to make a differ between a robot and a simple object
    typedef ModelConfig RobotConfig; //!< A typedef to make a differ between a robot and a simple object

    typedef std::shared_ptr<CoMIK> CoMIKPtr;
    typedef std::shared_ptr<HierarchicalIK> HierarchicalIKPtr;
    typedef std::shared_ptr<DifferentialIK> DifferentialIKPtr;
    typedef std::shared_ptr<Constraint> ConstraintPtr;
    typedef std::shared_ptr<TSRConstraint> TSRConstraintPtr;
    typedef std::shared_ptr<BalanceConstraint> BalanceConstraintPtr;
    typedef std::shared_ptr<PoseConstraint> PoseConstraintPtr;
    typedef std::shared_ptr<PositionConstraint> PositionConstraintPtr;
    typedef std::shared_ptr<OrientationConstraint> OrientationConstraintPtr;
    typedef std::shared_ptr<SupportPolygon> SupportPolygonPtr;
    typedef std::shared_ptr<EndEffector> EndEffectorPtr;
    typedef std::shared_ptr<EndEffectorActor> EndEffectorActorPtr;
    typedef std::shared_ptr<CollisionModel> CollisionModelPtr;
    typedef std::shared_ptr<CollisionChecker> CollisionCheckerPtr;
    typedef std::shared_ptr<TriMeshModel> TriMeshModelPtr;
    typedef std::shared_ptr<Obstacle> ObstaclePtr;
    typedef std::shared_ptr<VisualizationSet> VisualizationSetPtr;
    typedef std::shared_ptr<Visualization> VisualizationPtr;
    typedef std::shared_ptr<VisualizationFactory> VisualizationFactoryPtr;
    typedef std::shared_ptr<OffscreenRenderer> OffscreenRendererPtr;
    typedef std::shared_ptr<WorkspaceData> WorkspaceDataPtr;
    typedef std::shared_ptr<WorkspaceDataArray> WorkspaceDataArrayPtr;
    typedef std::shared_ptr<WorkspaceRepresentation> WorkspaceRepresentationPtr;
    typedef std::shared_ptr<Reachability> ReachabilityPtr;
    typedef std::shared_ptr<Scene> ScenePtr;
    typedef std::shared_ptr<ModelConfig> ModelConfigPtr;
    typedef std::shared_ptr<Grasp> GraspPtr;
    typedef std::shared_ptr<GraspSet> GraspSetPtr;
    typedef std::shared_ptr<ManipulationObject> ManipulationObjectPtr;
    typedef std::shared_ptr<CDManager> CDManagerPtr;
    typedef std::shared_ptr<PoseQualityMeasurement> PoseQualityMeasurementPtr;
    typedef std::shared_ptr<PoseQualityManipulability> PoseQualityManipulabilityPtr;
    typedef std::shared_ptr<Trajectory> TrajectoryPtr;
    typedef std::shared_ptr<SphereApproximator> SphereApproximatorPtr;
    typedef std::shared_ptr<BasicGraspQualityMeasure> BasicGraspQualityMeasurePtr;
    typedef std::shared_ptr<WorkspaceGrid> WorkspaceGridPtr;
    typedef std::shared_ptr<Model> ModelPtr;
    typedef std::weak_ptr<Model> ModelWeakPtr;
    typedef std::shared_ptr<ModelNode> ModelNodePtr;
    typedef std::weak_ptr<ModelNode> ModelNodeWeakPtr;
    typedef std::shared_ptr<ModelLink> ModelLinkPtr;
    typedef std::shared_ptr<ModelJoint> ModelJointPtr;
    typedef std::shared_ptr<ModelJointFixed> ModelJointFixedPtr;
    typedef std::shared_ptr<ModelJointPrismatic> ModelJointPrismaticPtr;
    typedef std::shared_ptr<ModelJointRevolute> ModelJointRevolutePtr;
    typedef std::shared_ptr<ModelSet> ModelSetPtr;
    typedef std::shared_ptr<ModelNodeSet> ModelNodeSetPtr;
    typedef std::shared_ptr<LinkSet> LinkSetPtr;
    typedef std::shared_ptr<JointSet> JointSetPtr;
    typedef std::shared_ptr<ModelNodeAttachment> ModelNodeAttachmentPtr;
    typedef std::shared_ptr<ModelNodeAttachmentFactory> ModelNodeAttachmentFactoryPtr;
    typedef std::shared_ptr<Frame> FramePtr;
    typedef std::shared_ptr<ColorMap> ColorMapPtr;
    typedef std::shared_ptr<Sensor> SensorPtr;

	typedef std::shared_ptr<Robot> RobotPtr; //!< A typedef to make a differ between a robot and a simple object
    typedef std::weak_ptr<Robot> RobotWeakPtr; //!< A typedef to make a differ between a robot and a simple object
    typedef std::shared_ptr<RobotNode> RobotNodePtr; //!< A typedef to make a differ between a robot and a simple object
    typedef std::weak_ptr<RobotNode> RobotNodeWeakPtr; //!< A typedef to make a differ between a robot and a simple object
    typedef std::shared_ptr<RobotLink> RobotLinkPtr; //!< A typedef to make a differ between a robot and a simple object
    typedef std::shared_ptr<RobotJoint> RobotJointPtr; //!< A typedef to make a differ between a robot and a simple object
    typedef std::shared_ptr<RobotJointFixed> RobotJointFixedPtr; //!< A typedef to make a differ between a robot and a simple object
    typedef std::shared_ptr<RobotJointPrismatic> RobotJointPrismaticPtr; //!< A typedef to make a differ between a robot and a simple object
    typedef std::shared_ptr<RobotJointRevolute> RobotJointRevolutePtr; //!< A typedef to make a differ between a robot and a simple object
    typedef std::shared_ptr<RobotNodeSet> RobotNodeSetPtr; //!< A typedef to make a differ between a robot and a simple object
    typedef std::shared_ptr<RobotNodeAttachment> RobotNodeAttachmentPtr; //!< A typedef to make a differ between a robot and a simple object
    typedef std::shared_ptr<RobotNodeAttachmentFactory> RobotNodeAttachmentFactoryPtr; //!< A typedef to make a differ between a robot and a simple object
	typedef std::shared_ptr<RobotConfig> RobotConfigPtr; //!< A typedef to make a differ between a robot and a simple object

    /*
     * Predefine for Tools/MathTools.h
     */
    namespace MathTools
    {
        struct Quaternion;
        struct SphericalCoord;
        struct Segment2D;
        struct ConvexHull2D;
        struct ConvexHull3D;
        struct ConvexHull6D;
        struct Plane;
        template<typename VectorT> struct BaseLine;
        struct Segment;
        struct OOBB;
        struct ContactPoint;
        struct TriangleFace6D;

        typedef BaseLine<Eigen::Vector3f> Line;
        typedef BaseLine<Eigen::Vector2f> Line2D;
        typedef std::shared_ptr<ConvexHull2D> ConvexHull2DPtr;
        typedef std::shared_ptr<ConvexHull3D> ConvexHull3DPtr;
        typedef std::shared_ptr<ConvexHull6D> ConvexHull6DPtr;

   }


#define VR_INFO std::cout <<__FILE__ << ":" << __LINE__ << ": "
#define VR_WARNING std::cerr <<__FILE__ << ":" << __LINE__ << " -Warning- "
#define VR_ERROR std::cerr <<__FILE__ << ":" << __LINE__ << " - ERROR - "
#define VR_ERROR_ONCE(stream)  {\
    static bool printed = false;\
    if (!printed) \
    { \
        VR_ERROR << stream << std::endl; \
        printed = true; \
    } \
}
#define VR_ERROR_ONCE_NYI VR_ERROR_ONCE(__FILE__ << " " << __LINE__ << " - " << __FUNCTION__ << "(): Not yet implemented!")

#ifdef NDEBUG

#define VR_ASSERT(a) do{}while(false)
#define VR_ASSERT_MESSAGE(a,b) do{}while(false)

#else
	/*!
	This assert macro does nothing on RELEASE builds.
	*/
#define VR_ASSERT( a )  BOOST_ASSERT( a )
	//THROW_VR_EXCEPTION_IF(!(a), "ASSERT failed (" << #a << ")" );

#define VR_ASSERT_MESSAGE(a,b) BOOST_ASSERT_MSG(a,b)
	//THROW_VR_EXCEPTION_IF(!(a), "ASSERT failed (" << #a << "): " << b );

#endif


    /*!
    Initialize the runtime envionment. This method calls VisualizationFactory::init().
    */
    void VIRTUAL_ROBOT_IMPORT_EXPORT init(int &argc, char* argv[], const std::string &appName);
    void VIRTUAL_ROBOT_IMPORT_EXPORT init(const std::string &appName);

    // init method is storing appName, since the c_string is passed by refrence to QT -> we must ensure that the string stays alive
    VIRTUAL_ROBOT_IMPORT_EXPORT extern std::string globalAppName;

} // namespace

#endif // _VirtualRobot_BasicDefinitions_h_
