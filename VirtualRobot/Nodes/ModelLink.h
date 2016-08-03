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
* @author     Adrian Knobloch
* @copyright  2016 Adrian Knobloch
*             GNU Lesser General Public License
*
*/
#ifndef _VirtualRobot_ModelLink_h_
#define _VirtualRobot_ModelLink_h_

#include "ModelNode.h"

namespace VirtualRobot
{
    class VIRTUAL_ROBOT_IMPORT_EXPORT ModelLink : ModelNode
    {
    public:
        struct VIRTUAL_ROBOT_IMPORT_EXPORT Physics
        {
            enum CoMLocation
            {
                eCustom,            //!< Not related to 3d model, the position is set by hand
                eVisuBBoxCenter     //!< The CoM position is automatically computed from the bounding box of the collision model
            };
            enum SimulationType
            {
                eStatic,        // cannot move, but collide
                eKinematic,     // can be moved, but no dynamics
                eDynamic,       // full dynamic simulation
                eUnknown        // not specified
            };

            // methods
            Physics();
            std::string getString(SimulationType s) const;

            void print() const;

            bool isSet();

            virtual std::string toXML(int tabs);

            Physics scale(float scaling) const;

            // data members
            Eigen::Vector3f localCoM;   //!< Defined in the local coordinate system of this object [mm]
            float massKg;               //!< The mass of this object
            CoMLocation comLocation;    //!< Where is the CoM located
            Eigen::Matrix3f inertiaMatrix; //! in kg*m^2
            SimulationType simType;
            std::vector< std::string > ignoreCollisions; // ignore collisions with other objects (only used within collision engines)
        };

    protected:
        /*!
         * Constructor with settings.
         *
         * @param model A pointer to the Model, which uses this Node.
         * @param name The name of this ModelNode. This name must be unique for the Model.
         * @param localTransformation The transformation from the parent of this node to this node.
         * @param visualization The visualisation of this link.
         * @param collisionModel The collision model of this link.
         * @param p The physics object of this link.
         * @param colChecker The collision checker for this link.
         */
        ModelLink(ModelWeakPtr model,
                  const std::string& name,
                  Eigen::Matrix4f& localTransformation,
                  VisualizationNodePtr visualization = VisualizationNodePtr(),
                  CollisionModelPtr collisionModel = CollisionModelPtr(),
                  const Physics& p = Physics(),
                  CollisionCheckerPtr colChecker = CollisionCheckerPtr());

    public:
        /*!
         * Destructor.
         */
        virtual ~ModelLink();

        virtual ModelNodeType getType()
        {
            return ModelNode::ModelNodeType::Link;
        }

        /*!
         * Get the collision model of this link.
         *
         * @return The collision model.
         */
        CollisionModelPtr getCollisionModel();

        /*!
         * Set a new collision model.
         *
         * @param colModel The new collision model.
         */
        void setCollisionModel(CollisionModelPtr colModel);

        /*!
         * Get the collision checker of this node.
         *
         * @return The collision checker.
         */
        CollisionCheckerPtr getCollisionChecker();

        /*!
         * Set a new visualisation.
         *
         * @param visualization The new visualisation.
         */
        void setVisualization(VisualizationNodePtr visualization);

        /*!
         * Get visualization object.
         *
         * @param visuType Set the type of visualization.
         * @return The visualisation of this link.
         */
        VisualizationNodePtr getVisualization(SceneObject::VisualizationType visuType = SceneObject::Full);

        /*!
         * Get number of faces (i.e. triangles) of this object.
         *
         * @param collisionModel Indicates weather the faces of the collision model or the full model should be returned.
         * @return The number of faces.
         */
        virtual int getNumFaces(bool collisionModel = false);

        /*!
         * Get the physics information of this node.
         *
         * @return A physics object containing all physics information.
         */
        Physics getPhysics();

        /*!
         * Get the simulation type of this node.
         *
         * The simulation type is of interest in SimDynamics.
         *
         * @return The simulation type of this node.
         */
        Physics::SimulationType getSimulationType() const;

        /*!
         * Set the simulation type of this node.
         *
         * The simulation type is of interest in SimDynamics.
         *
         * @param s The new simulation type of this node.
         */
        void setSimulationType(Physics::SimulationType s);

        /*!
         * Get the names of all collision models, this model should not collide with.
         *
         * Collisions with these models are ignored by physics engine (only considered within the SimDynamics package!).
         *
         * @return A vector of all names.
         */
        std::vector<std::string> getIgnoredCollisionModels();

        /*!
         * Return Center of Mass in local coordinate frame. This method does not consider children.
         *
         * @return The CoM.
         */
        virtual Eigen::Vector3f getCoMLocal();

        /*!
         * Return Center of Mass in global coordinates. This method does not consider children.
         *
         * @return The CoM.
         */
        virtual Eigen::Vector3f getCoMGlobal();

        /*!
         * Get the mass in Kg.
         *
         * @return The mass in Kg.
         */
        float getMass() const;

        /*!
         * Set the mass.
         *
         * @param m The mass in Kg.
         */
        void setMass(float m);

        /*!
         * Get the inertia matrix in kg*m^2.
         *
         * @return The inertia matrix in kg*m^2.
         */
        Eigen::Matrix3f getInertiaMatrix();

        /*!
         * Set the inertia matrix.
         *
         * @param im The inertia matrix in kg*m^2.
         */
        void setInertiaMatrix(const Eigen::Matrix3f& im);
    };
}

#endif
