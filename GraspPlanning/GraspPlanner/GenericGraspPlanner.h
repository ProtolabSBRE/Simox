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
* @package    GraspStudio
* @author     Nikolaus Vahrenkamp
* @copyright  2011 Nikolaus Vahrenkamp
*             GNU Lesser General Public License
*
*/
#pragma once

#include "../GraspStudio.h"
#include "GraspPlanner.h"
#include "../ApproachMovementGenerator.h"
#include "../GraspQuality/GraspQualityMeasure.h"

namespace GraspStudio
{
    /*!
    *
    *
    * A general grasp planning class that utilizes ApprachMovementGenerator for generating grasp hypothesis and
    * GraspQualityMeasure to score them.
    *
    */
    class GRASPSTUDIO_IMPORT_EXPORT GenericGraspPlanner : public GraspPlanner
    {
    public:

        /*!
            Constructor
            \param graspSet All planned grasps are added to this GraspSet.
            \param graspQuality The quality of generated grasps is evaluated with this object
            \param approach Approach movements are generated by this object.
            \param minQuality The quality that must be achieved at minimum by the GraspQualityMesurement module
            \param forceClosure When true, only force closure grasps are generated.
        */
        GenericGraspPlanner(VirtualRobot::GraspSetPtr graspSet, GraspStudio::GraspQualityMeasurePtr graspQuality, GraspStudio::ApproachMovementGeneratorPtr approach, float minQuality = 0.0f, bool forceClosure = true);

        // destructor
        ~GenericGraspPlanner() override;

        /*!
            Creates new grasps.
            \param nrGrasps The number of grasps to be planned.
            \param timeOutMS The time out in milliseconds. Planning is stopped when this time is exceeded. Disabled when zero.
            \return Number of generated grasps.
        */
        int plan(int nrGrasps, int timeOutMS = 0, VirtualRobot::SceneObjectSetPtr obstacles = VirtualRobot::SceneObjectSetPtr()) override;

        VirtualRobot::GraspPtr planGrasp(VirtualRobot::SceneObjectSetPtr obstacles = VirtualRobot::SceneObjectSetPtr());

        VirtualRobot::EndEffector::ContactInfoVector getContacts() const;

        void setParameters(float minQuality, bool forceClosure);

        //! if enabled (default), the planner retreates the hand if the number of contacts is <2.
        //! During retreat, the contacts are checked if a better situation can be achieved.
        //! This procedure helps in case the object is small.
        void setRetreatOnLowContacts(bool enable);

    protected:

        bool moveEEFAway(const Eigen::Vector3f& approachDir, float step, int maxLoops);

        bool timeout();


        VirtualRobot::SceneObjectPtr object;
        VirtualRobot::EndEffectorPtr eef;

        clock_t startTime;
        int timeOutMS;
        VirtualRobot::EndEffector::ContactInfoVector contacts;
        GraspStudio::GraspQualityMeasurePtr graspQuality;
        GraspStudio::ApproachMovementGeneratorPtr approach;
        float minQuality;
        bool forceClosure;

        bool retreatOnLowContacts;
    };
}

