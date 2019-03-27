
#include "SceneObject.h"
#include "CollisionDetection/CollisionModel.h"
#include "CollisionDetection/CollisionChecker.h"
#include "Visualization/TriMeshModel.h"
#include "Visualization/VisualizationFactory.h"
#include "Visualization/Visualization.h"
#include "VirtualRobotException.h"
#include "Robot.h"
#include <cmath>
#include <algorithm>
#include <Eigen/Dense>
#include "math/Helpers.h"



namespace VirtualRobot
{


    SceneObject::SceneObject(const std::string& name, VisualizationNodePtr visualization /*= VisualizationNodePtr()*/, CollisionModelPtr collisionModel /*= CollisionModelPtr()*/, const Physics& p /*= Physics()*/, CollisionCheckerPtr colChecker /*= CollisionCheckerPtr()*/)
    {
        this->name = name;
        this->visualizationModel = visualization;
        this->collisionModel = collisionModel;


        this->physics = p;
        this->globalPose = Eigen::Matrix4f::Identity();
        this->initialized = false;
        updateVisualization = true;
        updateCollisionModel = true;

        if (visualization)
        {
            updateVisualization = visualization->getUpdateVisualizationStatus();
        }

        if (!colChecker)
        {
            this->collisionChecker = CollisionChecker::getGlobalCollisionChecker();
        }
        else
        {
            this->collisionChecker = colChecker;
        }

        setGlobalPose(Eigen::Matrix4f::Identity());
    }


    SceneObject::~SceneObject()
    {
        for (size_t i = 0; i < children.size(); i++)
        {
            children[i]->detachedFromParent();
        }
    }


    bool SceneObject::initialize(SceneObjectPtr parent, const std::vector<SceneObjectPtr>& children)
    {
        // parent
        if (parent)
        {
            if (!parent->hasChild(shared_from_this()))
            {
                parent->attachChild(shared_from_this());
            }
        }

        this->parent = parent;

        // children
        for (size_t i = 0; i < children.size(); i++)
        {
            if (!this->hasChild(children[i]))
            {
                this->attachChild(children[i]);
            }
        }

        // init all children
        std::vector<SceneObjectPtr> c = getChildren();

        for (size_t i = 0; i < c.size(); i++)
        {
            c[i]->initialize(shared_from_this());
        }

        Eigen::Matrix4f m = Eigen::Matrix4f::Identity();

        if (parent)
        {
            m = parent->getGlobalPose();
        }

        initialized = true;
        updatePose(m);
        return initializePhysics();
    }

    void SceneObject::setGlobalPose(const Eigen::Matrix4f& pose)
    {
        SceneObjectPtr p = getParent();

        if (p)
        {
            VR_ERROR << "Could not set pose of <" << name << ">. The object is attached to <" << p->getName() << ">" << endl;
            return;
        }

        updatePose(pose);
    }

    void SceneObject::setGlobalPoseNoChecks(const Eigen::Matrix4f& pose)
    {
        updatePose(pose);
    }


    void SceneObject::updatePose(bool updateChildren)
    {
        if (visualizationModel && updateVisualization)
        {
            visualizationModel->setGlobalPose(globalPose);
        }

        if (collisionModel && updateCollisionModel)
        {
            collisionModel->setGlobalPose(globalPose);
        }

        if (updateChildren)
            for (size_t i = 0; i < children.size(); i++)
            {
                children[i]->updatePose(globalPose);
            }
    }

    void SceneObject::copyPoseFrom(const SceneObjectPtr& other)
    {
        //if you change code here, you have to update
        //void RobotNode::copyPoseFrom(const RobotNodePtr &other)
        //since this code is inlined in the other function
        globalPose = other->globalPose;
        if (visualizationModel && updateVisualization)
        {
            visualizationModel->setGlobalPose(globalPose);
        }
        if (collisionModel && updateCollisionModel)
        {
            collisionModel->setGlobalPose(globalPose);
        }
    }

    void SceneObject::updatePose(const Eigen::Matrix4f& parentPose, bool updateChildren)
    {
        globalPose = parentPose;
        updatePose(updateChildren);
    }

    std::string SceneObject::getName() const
    {
        return name;
    }

    VirtualRobot::CollisionModelPtr SceneObject::getCollisionModel()
    {
        return collisionModel;
    }

    VirtualRobot::CollisionCheckerPtr SceneObject::getCollisionChecker()
    {
        return collisionChecker;
    }

    VirtualRobot::VisualizationNodePtr SceneObject::getVisualization(SceneObject::VisualizationType visuType)
    {
        if (visuType == SceneObject::Full)
        {
            return visualizationModel;
        }
        else
        {
            if (collisionModel)
            {
                if (visuType == SceneObject::Collision)
                {
                    return collisionModel->getVisualization();
                }
                else
                {
                    return collisionModel->getModelDataVisualization();
                }
            }
            else
            {
                //VR_WARNING << "<" << name << "> No collision model present ..." << endl;
                return VisualizationNodePtr();
            }
        }
    }

    bool SceneObject::ensureVisualization(const std::string& visualizationType)
    {
        if (visualizationModel)
        {
            return true;
        }


        VisualizationFactoryPtr visualizationFactory;

        if (visualizationType == "")
        {
            visualizationFactory = VisualizationFactory::first(nullptr);
        }
        else
        {
            visualizationFactory = VisualizationFactory::fromName(visualizationType, nullptr);
        }

        if (!visualizationFactory)
        {
            VR_WARNING << "VisualizationFactory of type '" << visualizationType << "' not present. Could not create visualization data in Robot Node <" << name << ">" << endl;
            return false;
        }

        // create dummy visualization
        setVisualization(visualizationFactory->createVisualization());
        return true;
    }

    void SceneObject::showCoordinateSystem(bool enable, float scaling, std::string* text, const std::string& visualizationType)
    {
        if (!enable && !visualizationModel)
        {
            return;    // nothing to do
        }

        if (!ensureVisualization(visualizationType))
        {
            return;
        }

        std::string coordName = name;

        if (text)
        {
            coordName = *text;
        }

        if (visualizationModel->hasAttachedVisualization("CoordinateSystem"))
        {
            visualizationModel->detachVisualization("CoordinateSystem");
        }

        if (enable)
        {
            VisualizationFactoryPtr visualizationFactory = VisualizationFactory::first(nullptr);

            if (!visualizationFactory)
            {
                VR_ERROR << " Could not determine a valid VisualizationFactory!!" << endl;
                return;
            }

            // create coord visu
            VisualizationNodePtr visualizationNode = visualizationFactory->createCoordSystem(scaling, &coordName);
            visualizationModel->attachVisualization("CoordinateSystem", visualizationNode);
        }
    }

    void SceneObject::showPhysicsInformation(bool enableCoM, bool enableInertial, VisualizationNodePtr comModel)
    {
        if (!enableCoM && !enableInertial && !visualizationModel)
        {
            return;    // nothing to do
        }

        if (!visualizationModel)
        {
            return;
        }

        if (physics.massKg <= 0)
        {
            // no valid physics information
            // check for valid model
            TriMeshModelPtr tm = visualizationModel->getTriMeshModel();

            if (!tm || tm->vertices.size() < 2)
            {
                return;
            }
        }

        if (visualizationModel->hasAttachedVisualization("__CoMLocation"))
        {
            visualizationModel->detachVisualization("__CoMLocation");
        }

        if (visualizationModel->hasAttachedVisualization("__InertiaMatrix"))
        {
            visualizationModel->detachVisualization("__InertiaMatrix");
        }

        VisualizationFactoryPtr visualizationFactory = VisualizationFactory::first(nullptr);

        if (enableCoM && visualizationFactory)
        {
            // create visu
            if (!comModel)
            {
                VisualizationNodePtr comModel1 = visualizationFactory->createSphere(7.05f, 1.0f, 0.2f, 0.2f);
                VisualizationNodePtr comModel2 = visualizationFactory->createBox(10.0f, 10.0f, 10.0f, 0.2f, 0.2f, 1.0f);
                std::vector<VisualizationNodePtr> v;
                v.push_back(comModel1);
                v.push_back(comModel2);
                comModel = visualizationFactory->createUnitedVisualization(v);
            }

            if (comModel)
            {
                std::stringstream ss;
                ss << "COM:" << getName();
                std::string t = ss.str();
                VisualizationNodePtr vText = visualizationFactory->createText(t, true, 1.0f, VisualizationFactory::Color::Blue(), 0, 10.0f, 0);
                std::vector<VisualizationNodePtr> v;
                v.push_back(comModel);
                v.push_back(vText);
                comModel = visualizationFactory->createUnitedVisualization(v);
            }

            VisualizationNodePtr comModelClone = comModel->clone();
            Eigen::Vector3f l = getCoMLocal();
            //cout << "COM:" << l << endl;
            Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
            m.block(0, 3, 3, 1) = l;

            // now we have transformation in RobotNode (end) coordsystem
            // we need to transform to RobotNode's visualization system
            // -> not needed here, since we don't use postr joint transformations any more
            //m = getGlobalPoseVisualization().inverse() * getGlobalPose() * m;

            // convert mm to m
            //m.block(0, 3, 3, 1) *= 0.001f;

            visualizationFactory->applyDisplacement(comModelClone, m);

            visualizationModel->attachVisualization("__CoMLocation", comModelClone);
        }

        if (enableInertial && visualizationFactory)
        {
            // create inertia visu
            //cout << "INERTIA MATRIX:" << endl << physics.intertiaMatrix << endl;
            Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> eigensolver(physics.inertiaMatrix);

            if (eigensolver.info() == Eigen::Success)
            {
                /*cout << "The eigenvalues of A are:\n" << eigensolver.eigenvalues() << endl;
                cout << "Here's a matrix whose columns are eigenvectors of A \n"
                    << "corresponding to these eigenvalues:\n"
                    << eigensolver.eigenvectors() << endl;*/
                //Eigen::Vector3f v1 = eigensolver.eigenvectors().block(0, 0, 3, 1);
                //Eigen::Vector3f v2 = eigensolver.eigenvectors().block(0, 1, 3, 1);
                //Eigen::Vector3f v3 = eigensolver.eigenvectors().block(0, 2, 3, 1);
                /*cout << "v1:" << v1 << endl;
                cout << "v2:" << v2 << endl;
                cout << "v3:" << v3 << endl;*/

                float xl = static_cast<float>(eigensolver.eigenvalues().rows() > 0 ?
                                              eigensolver.eigenvalues()(0) : 1e-6);
                float yl = static_cast<float>(eigensolver.eigenvalues().rows() > 1 ?
                                              eigensolver.eigenvalues()(1) : 1e-6);
                float zl = static_cast<float>(eigensolver.eigenvalues().rows() > 2 ?
                                              eigensolver.eigenvalues()(2) : 1e-6);

                if (std::abs(xl) < 1e-6f)
                {
                    xl = 1e-6f;
                }

                if (std::abs(yl) < 1e-6f)
                {
                    yl = 1e-6f;
                }

                if (std::abs(zl) < 1e-6f)
                {
                    zl = 1e-6f;
                }

                xl = 1.0f / sqrtf(xl);
                yl = 1.0f / sqrtf(yl);
                zl = 1.0f / sqrtf(zl);
                float le = sqrtf(xl * xl + yl * yl + zl * zl);
                float scaleFactor = 5.0f;
                float axesSize = 4.0f * le * scaleFactor / 20.0f;

                if (axesSize > 4.0f)
                {
                    axesSize = 4.0f;
                }

                if (axesSize < 0.4f)
                {
                    axesSize = 0.4f;
                }

                xl *= scaleFactor;
                yl *= scaleFactor;
                zl *= scaleFactor;

                float minSize = 10.0f;
                float maxSize = 50.0f;
                float maxAx = xl;

                if (yl > xl && yl > zl)
                {
                    maxAx = yl;
                }

                if (zl > xl && zl > xl)
                {
                    maxAx = zl;
                }

                if (maxAx < minSize)
                {
                    if (maxAx < 1e-6f)
                    {
                        maxAx = 1e-6f;
                    }

                    xl = xl / maxAx * minSize;
                    yl = yl / maxAx * minSize;
                    zl = zl / maxAx * minSize;
                }

                if (maxAx > maxSize)
                {
                    xl = xl / maxAx * maxSize;
                    yl = yl / maxAx * maxSize;
                    zl = zl / maxAx * maxSize;
                }

                VisualizationNodePtr inertiaVisu = visualizationFactory->createEllipse(xl, yl, zl, true, axesSize, axesSize * 2.0f);

                Eigen::Vector3f l = getCoMLocal();
                //cout << "COM:" << l << endl;
                Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
                m.block(0, 3, 3, 1) = l;
                m.block(0, 0, 3, 3) = eigensolver.eigenvectors().block(0, 0, 3, 3); // rotate according to EV

                // now we have transformation in RobotNode (end) coordsystem
                // we need to transform to RobotNode's visualization system
                // not needed, we don't have post joint transformations any more
                //m = getGlobalPoseVisualization().inverse() * getGlobalPose() * m;

                // convert mm to m
                //m.block(0, 3, 3, 1) *= 0.001f;

                visualizationFactory->applyDisplacement(inertiaVisu, m);

                visualizationModel->attachVisualization("__InertiaMatrix", inertiaVisu);

            }
            else
            {
                VR_INFO << " Could not determine eigenvectors of inertia matrix" << endl;
            }
        }
    }

    bool SceneObject::showCoordinateSystemState()
    {
        if (visualizationModel)
        {
            return visualizationModel->hasAttachedVisualization("CoordinateSystem");
        }
        else
        {
            return false;
        }
    }

    void SceneObject::setUpdateVisualization(bool enable)
    {
        updateVisualization = enable;

        if (visualizationModel)
        {
            visualizationModel->setUpdateVisualization(enable);
        }

        if (enable) // only activate but not deactivate update of collision model with this function
        {
            updateCollisionModel = enable;
            if (collisionModel)
            {
                collisionModel->setUpdateVisualization(enable);
            }
        }
    }
    void SceneObject::setUpdateCollisionModel(bool enable)
    {
        updateCollisionModel = enable;
    }

    bool SceneObject::getUpdateVisualizationStatus()
    {
        return updateVisualization;
    }

    bool SceneObject::getUpdateCollisionModelStatus()
    {
        return updateCollisionModel;
    }

    void SceneObject::setVisualization(VisualizationNodePtr visualization)
    {
        visualizationModel = visualization;

        if (visualizationModel)
        {
            visualizationModel->setUpdateVisualization(updateVisualization);
            visualizationModel->setGlobalPose(globalPose);
        }
    }

    void SceneObject::setCollisionModel(CollisionModelPtr colModel)
    {
        collisionModel = colModel;

        if (collisionModel)
        {
            collisionModel->setUpdateVisualization(updateCollisionModel);
            collisionModel->setGlobalPose(globalPose);
        }
    }


    Eigen::Matrix4f SceneObject::toLocalCoordinateSystem(const Eigen::Matrix4f& poseGlobal) const
    {
        return math::Helpers::InvertedPose(getGlobalPose()) * poseGlobal;
    }


    Eigen::Matrix4f SceneObject::toGlobalCoordinateSystem(const Eigen::Matrix4f& poseLocal) const
    {
        return getGlobalPose() * poseLocal;
    }

    Eigen::Vector3f SceneObject::toLocalCoordinateSystemVec(const Eigen::Vector3f& positionGlobal) const
    {
        Eigen::Matrix4f t = t.Identity();
        math::Helpers::Position(t) = positionGlobal;
        t = toLocalCoordinateSystem(t);
        return math::Helpers::Position(t);
    }


    Eigen::Vector3f SceneObject::toGlobalCoordinateSystemVec(const Eigen::Vector3f& positionLocal) const
    {
        Eigen::Matrix4f t = t.Identity();
        math::Helpers::Position(t) = positionLocal;
        t = toGlobalCoordinateSystem(t);
        return math::Helpers::Position(t);
    }

    Eigen::Matrix4f SceneObject::getTransformationTo(const SceneObjectPtr otherObject)
    {
        return math::Helpers::InvertedPose(getGlobalPose()) * otherObject->getGlobalPose();
    }

    Eigen::Matrix4f SceneObject::getTransformationFrom(const SceneObjectPtr otherObject)
    {
        return math::Helpers::InvertedPose(otherObject->getGlobalPose()) * getGlobalPose();
    }

    Eigen::Matrix4f SceneObject::transformTo(const SceneObjectPtr otherObject,
            const Eigen::Matrix4f& poseInOtherCoordSystem)
    {
        Eigen::Matrix4f mat = getTransformationTo(otherObject);
        return mat * poseInOtherCoordSystem;
    }

    Eigen::Vector3f SceneObject::transformTo(const SceneObjectPtr otherObject,
            const Eigen::Vector3f& positionInOtherCoordSystem)
    {
        Eigen::Matrix4f mat = getTransformationTo(otherObject);
        Eigen::Vector4f res = mat * positionInOtherCoordSystem.homogeneous();
        return res.head<3>();
    }

    void SceneObject::setupVisualization(bool showVisualization, bool showAttachedVisualizations)
    {
        if (visualizationModel)
        {
            visualizationModel->setupVisualization(showVisualization, showAttachedVisualizations);
        }

    }

    int SceneObject::getNumFaces(bool collisionModel /*=false*/)
    {
        if (collisionModel)
        {
            if (this->collisionModel)
            {
                return this->collisionModel->getNumFaces();
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (visualizationModel)
            {
                return visualizationModel->getNumFaces();
            }
            else
            {
                return 0;
            }
        }
    }

    Eigen::Matrix4f SceneObject::getGlobalPose() const
    {
        return globalPose;
    }

    Eigen::Vector3f SceneObject::getGlobalPosition() const
    {
        return getGlobalPose().block<3, 1>(0, 3);
    }

    Eigen::Matrix3f SceneObject::getGlobalOrientation() const
    {
        return getGlobalPose().block<3, 3>(0, 0);
    }

    Eigen::Matrix4f SceneObject::getGlobalPose(const Eigen::Matrix4f& localPose) const
    {
        return getGlobalPose() * localPose;
    }

    Eigen::Vector3f SceneObject::getGlobalPosition(const Eigen::Vector3f& localPosition) const
    {
        return math::Helpers::TransformPosition(getGlobalPose(), localPosition);
    }

    Eigen::Vector3f SceneObject::getGlobalDirection(const Eigen::Vector3f& localDircetion) const
    {
        return math::Helpers::TransformDirection(getGlobalPose(), localDircetion);
    }

    Eigen::Matrix3f SceneObject::getGlobalOrientation(const Eigen::Matrix3f& localOrientation) const
    {
        return math::Helpers::TransformOrientation(getGlobalPose(), localOrientation);
    }

    Eigen::Vector3f SceneObject::getCoMLocal()
    {
        return physics.localCoM;
    }

    void SceneObject::setCoMLocal(const Eigen::Vector3f& comLocal)
    {
        physics.localCoM = comLocal;
    }

    Eigen::Vector3f SceneObject::getCoMGlobal()
    {
        Eigen::Vector3f result = getCoMLocal();
        return toGlobalCoordinateSystemVec(result);
    }

    float SceneObject::getMass() const
    {
        return physics.massKg;
    }

    bool SceneObject::initializePhysics()
    {
        // check if physics node's CoM location has to be calculated
        if (physics.comLocation == SceneObject::Physics::eVisuBBoxCenter)
        {
            /*if (!visualizationModel && !collisionModel)
            {
                VR_WARNING << getName() << ": Physics tag CoM is set to eVisuBBoxCenter, but no visualization model is loaded, setting CoM to local position (0/0/0)" << endl;
            } else*/
            if (visualizationModel || collisionModel)
            {
                TriMeshModelPtr tm;
                // since globalPose and visualization's global pose may differ, we transform com to local coord system (globalpose)
                Eigen::Matrix4f posVisu;

                if (collisionModel)
                {
                    tm = collisionModel->getTriMeshModel();
                    posVisu = collisionModel->getGlobalPose();
                }
                else
                {
                    tm = visualizationModel->getTriMeshModel();
                    posVisu = visualizationModel->getGlobalPose();
                }

                if (!tm)
                {
                    VR_WARNING << "Could not create trimeshmodel for CoM computation, setting CoM to local position (0/0/0)" << endl;
                }
                else
                {
                    Eigen::Vector3f minS, maxS;
                    tm->getSize(minS, maxS);
                    physics.localCoM = minS + (maxS - minS) * 0.5f;

                    // trimeshmodel is without any pose transformations, so apply visu global pose
                    Eigen::Matrix4f visuComGlobal;
                    visuComGlobal.setIdentity();
                    visuComGlobal.block(0, 3, 3, 1) = physics.localCoM;
                    visuComGlobal = posVisu * visuComGlobal;

                    // transform to this object's global pose (this might be a different one, e.g. when postJointTransformations are considered)
                    Eigen::Matrix4f comLocal = toLocalCoordinateSystem(visuComGlobal);
                    physics.localCoM = comLocal.block(0, 3, 3, 1);
                }
            }
        }

        // check for inertia matrix determination
        if (physics.inertiaMatrix.isZero())
        {
            if (physics.massKg <= 0)
            {
                // standard box
                physics.inertiaMatrix.setIdentity();
            }
            else
            {
                TriMeshModelPtr tm;
                // since globalPose and visualization's global pose may differ, we transform com to local coord system (globalpose)
                Eigen::Matrix4f posVisu;

                if (collisionModel)
                {
                    tm = collisionModel->getTriMeshModel();
                    posVisu = collisionModel->getGlobalPose();
                }
                else if (visualizationModel)
                {
                    tm = visualizationModel->getTriMeshModel();
                    posVisu = visualizationModel->getGlobalPose();
                }

                if (!tm)
                {
                    // standard box
                    physics.inertiaMatrix.setIdentity();
                    physics.inertiaMatrix *= 0.01f; // 10 cm bbox
                    physics.inertiaMatrix *= physics.massKg;
                }
                else
                {
                    Eigen::Vector3f minS, maxS;
                    tm->getSize(minS, maxS);
                    MathTools::OOBB bbox(minS, maxS, posVisu);

                    Eigen::Matrix4f coordCOM = getGlobalPose();
                    coordCOM.block(0, 3, 3, 1) = physics.localCoM;
                    // get bbox in local coord system
                    bbox.changeCoordSystem(coordCOM);
                    Eigen::Vector3f l = bbox.maxBB - bbox.minBB;
                    l *= 0.001f; // mm -> m

                    physics.inertiaMatrix.setZero();
                    physics.inertiaMatrix(0, 0) = (l(1) * l(1) + l(2) * l(2)) / 12.0f;
                    physics.inertiaMatrix(1, 1) = (l(0) * l(0) + l(2) * l(2)) / 12.0f;
                    physics.inertiaMatrix(2, 2) = (l(0) * l(0) + l(1) * l(1)) / 12.0f;

                    float mass = physics.massKg;
                    physics.inertiaMatrix *= mass;
                }
            }
        }

        return true;
    }

    void SceneObject::print(bool printChildren, bool printDecoration /*= true*/) const
    {
        if (printDecoration)
        {
            cout << "**** SceneObject ****" << endl;
        }

        cout << " * Name: " << name << endl;
        cout << " * GlobalPose: " << endl << globalPose << endl;

        physics.print();

        cout << " * Visualization:" << endl;

        if (visualizationModel)
        {
            visualizationModel->print();
        }
        else
        {
            cout << "<not set>" << endl;
        }

        cout << " * Update visualization status: ";

        if (updateVisualization)
        {
            cout << "enabled" << endl;
        }
        else
        {
            cout << "disabled" << endl;
        }

        cout << " * Collision Model:" << endl;

        if (collisionModel)
        {
            collisionModel->print();
        }
        else
        {
            cout << "<not set>" << endl;
        }

        cout << " * Update collision model status: ";

        if (updateCollisionModel)
        {
            cout << "enabled" << endl;
        }
        else
        {
            cout << "disabled" << endl;
        }

        std::vector<SceneObjectPtr> children = this->getChildren();

        if (children.size() > 0)
        {
            cout << " * Children:" << endl;

            for (size_t i = 0; i < children.size(); i++)
            {
                cout << " ** " << children[i]->getName();
            }
        }

        if (printDecoration)
        {
            cout << endl;
        }

        if (printChildren)
        {
            for (unsigned int i = 0; i < children.size(); i++)
            {
                children[i]->print(true, true);
            }
        }
    }

    void SceneObject::showBoundingBox(bool enable, bool wireframe)
    {
        if (!enable && !visualizationModel)
        {
            return;    // nothing to do
        }

        if (!ensureVisualization())
        {
            return;
        }


        if (visualizationModel->hasAttachedVisualization("BoundingBox"))
        {
            visualizationModel->detachVisualization("BoundingBox");
        }

        if (enable)
        {
            VisualizationFactoryPtr visualizationFactory = VisualizationFactory::first(nullptr);

            if (!visualizationFactory)
            {
                VR_ERROR << " Could not determine a valid VisualizationFactory!!" << endl;
                return;
            }

            // create bbox visu
            if (collisionModel)
            {
                BoundingBox bbox = collisionModel->getBoundingBox(false);
                VisualizationNodePtr visualizationNode = visualizationFactory->createBoundingBox(bbox, wireframe);
                visualizationModel->attachVisualization("BoundingBox", visualizationNode);
            }
        }
    }


    void SceneObject::highlight(VisualizationPtr visualization, bool enable)
    {
        if (!visualization)
        {
            return;
        }

        if (getVisualization(Full) && visualization->isVisualizationNodeRegistered(getVisualization(Full)))
        {
            visualization->highlight(getVisualization(Full), enable);
        }

        if (getVisualization(Collision) && visualization->isVisualizationNodeRegistered(getVisualization(Collision)))
        {
            visualization->highlight(getVisualization(Collision), enable);
        }

        // collision model data will be created if calling these methods->huge models may be built (arrows that show the normals are build for every triangle) -> disabling highlight option for col model data
        //if (getVisualization(CollisionData) && visualization->isVisualizationNodeRegistered(getVisualization(CollisionData)))
        //  visualization->highlight(getVisualization(CollisionData),enable);
    }

    SceneObjectPtr SceneObject::clone(const std::string& name, CollisionCheckerPtr colChecker, float scaling) const
    {
        return SceneObjectPtr(_clone(name, colChecker, scaling));
    }
    SceneObjectPtr SceneObject::clone(CollisionCheckerPtr colChecker, float scaling) const
    {
        return clone(getName(), colChecker, scaling);
    }

    void SceneObject::setName(const std::string& name)
    {
        this->name = name;
    }

    SceneObject* SceneObject::_clone(const std::string& name, CollisionCheckerPtr colChecker, float scaling) const
    {
        VisualizationNodePtr clonedVisualizationNode;

        if (visualizationModel)
        {
            clonedVisualizationNode = visualizationModel->clone(true, scaling);
        }

        CollisionModelPtr clonedCollisionModel;

        if (collisionModel)
        {
            clonedCollisionModel = collisionModel->clone(colChecker, scaling);
        }

        Physics p = physics.scale(scaling);
        SceneObject* result = new SceneObject(name, clonedVisualizationNode, clonedCollisionModel, p, colChecker);

        if (!result)
        {
            VR_ERROR << "Cloning failed.." << endl;
            return result;
        }

        result->setGlobalPose(getGlobalPose());

        return result;
    }


    Eigen::Matrix3f SceneObject::getInertiaMatrix()
    {
        return physics.inertiaMatrix;
    }

    Eigen::Matrix3f SceneObject::getInertiaMatrix(const Eigen::Vector3f& shift)
    {
        Eigen::Matrix3f skew;
        skew << 0, -shift(2), +shift(1),
             +shift(2), 0, -shift(0),
             -shift(1), +shift(0), 0;
        return getInertiaMatrix() + getMass() * skew.transpose() * skew;
    }

    Eigen::Matrix3f SceneObject::getInertiaMatrix(const Eigen::Vector3f& shift, const Eigen::Matrix3f& rotation)
    {
        return rotation * getInertiaMatrix(shift) * rotation.transpose();
    }
    Eigen::Matrix3f SceneObject::getInertiaMatrix(const Eigen::Matrix4f& transform)
    {
        return getInertiaMatrix(transform.block<3, 1>(0, 3), transform.block<3, 3>(0, 0));
    }

    std::string SceneObject::getSceneObjectXMLString(const std::string& basePath, int tabs)
    {
        std::stringstream ss;
        std::string pre = "";

        for (int i = 0; i < tabs; i++)
        {
            pre += "\t";
        }

        if (visualizationModel)
        {
            ss << visualizationModel->toXML(basePath, tabs);
        }

        if (collisionModel && collisionModel->getVisualization())
        {
            ss << collisionModel->toXML(basePath, tabs);
        }

        Eigen::Matrix4f gp = getGlobalPose();

        if (!gp.isIdentity())
        {
            ss << pre << "<GlobalPose>\n";
            ss << pre << "\t<Transform>\n";
            ss << MathTools::getTransformXMLString(gp, tabs + 2);
            ss << pre << "\t</Transform>\n";
            ss << pre << "</GlobalPose>\n";
        }

        if (physics.isSet())
        {
            ss << physics.toXML(tabs);
        }

        return ss.str();
    }

    void SceneObject::setMass(float m)
    {
        physics.massKg = m;
    }

    float SceneObject::getFriction()
    {
        return physics.friction;
    }

    void SceneObject::setFriction(float friction)
    {
        physics.friction = friction;
    }

    SceneObject::Physics::SimulationType SceneObject::getSimulationType() const
    {
        return physics.simType;
    }

    void SceneObject::setSimulationType(SceneObject::Physics::SimulationType s)
    {
        if (physics.massKg <= 0 && s == SceneObject::Physics::eDynamic)
        {
            VR_WARNING << "Setting simulation type to dynamic, but mass==0, object might be handled as static object by physics engine." << endl;
        }
        physics.simType = s;
    }

    void SceneObject::setInertiaMatrix(const Eigen::Matrix3f& im)
    {
        physics.inertiaMatrix = im;
    }

    SceneObject::Physics SceneObject::getPhysics()
    {
        return physics;
    }

    bool SceneObject::hasChild(SceneObjectPtr child, bool recursive) const
    {
        VR_ASSERT(child);

        for (size_t i = 0; i < children.size(); i++)
        {
            if (children[i] == child)
            {
                return true;
            }

            if (recursive && children[i]->hasChild(child, true))
            {
                return true;
            }
        }

        return false;
    }

    bool SceneObject::hasChild(const std::string& childName, bool recursive) const
    {
        for (size_t i = 0; i < children.size(); i++)
        {
            if (children[i]->getName() == childName)
            {
                return true;
            }

            if (recursive && children[i]->hasChild(childName, true))
            {
                return true;
            }
        }

        return false;
    }

    void SceneObject::detachChild(SceneObjectPtr child)
    {
        VR_ASSERT(child);

        if (!hasChild(child))
        {
            VR_WARNING << " Trying to detach a not attached object: " << getName() << "->" << child->getName() << endl;
            return;
        }

        children.erase(std::find(children.begin(), children.end(), child));
        child->detachedFromParent();
    }

    bool SceneObject::attachChild(SceneObjectPtr child)
    {
        VR_ASSERT(child);

        if (this == child.get())
        {
            VR_WARNING << "Trying to attach object to it self object! name: " << getName() << endl;
            return false;
        }

        if (hasChild(child))
        {
            VR_WARNING << " Trying to attach already attached object: " << getName() << "->" << child->getName() << endl;
            return true; // no error
        }

        if (child->hasParent())
        {
            VR_WARNING << " Trying to attach object that has already a parent: " << getName() << "->" << child->getName() << ", child's parent:" << child->getParent()->getName() << endl;
            return false;
        }

        if (hasChild(child->getName()))
        {
            VR_ERROR << " Trying to attach object with name: " << child->getName() << " to " << getName() << ", but a child with same name is already present!" << endl;
            return false;
        }

        children.push_back(child);
        child->attached(shared_from_this());
        return true;
    }

    bool SceneObject::hasParent()
    {
        SceneObjectPtr p = getParent();

        if (p)
        {
            return true;
        }

        return false;
    }

    VirtualRobot::SceneObjectPtr SceneObject::getParent() const
    {
        SceneObjectPtr p = parent.lock();
        return p;
    }

    std::vector<SceneObjectPtr> SceneObject::getChildren() const
    {
        return children;
    }

    void SceneObject::detachedFromParent()
    {
        this->parent.reset();
    }

    void SceneObject::attached(SceneObjectPtr parent)
    {
        VR_ASSERT(parent);
        SceneObjectPtr p = getParent();
        THROW_VR_EXCEPTION_IF(p && p != parent, "SceneObject already attached to a different parent");
        this->parent = parent;
    }



    std::vector<std::string> SceneObject::getIgnoredCollisionModels()
    {
        return physics.ignoreCollisions;
    }


    bool SceneObject::saveModelFiles(const std::string& modelPath, bool replaceFilenames)
    {
        bool res = true;

        if (visualizationModel && visualizationModel->getTriMeshModel() && visualizationModel->getTriMeshModel()->faces.size() > 0)
        {
            std::string newFilename;

            if (replaceFilenames)
            {
                newFilename = getFilenameReplacementVisuModel();
                this->visualizationModel->setFilename(newFilename, false);
            }
            else
            {
                std::string fnV = visualizationModel->getFilename();
                boost::filesystem::path fn(fnV);
                boost::filesystem::path filnameNoPath = fn.filename();
                newFilename = filnameNoPath.string();
            }

            res = res & visualizationModel->saveModel(modelPath, newFilename);
        }

        if (collisionModel && collisionModel->getTriMeshModel() && collisionModel->getTriMeshModel()->faces.size() > 0)
        {
            // check if we need to replace the filename (also in case the trimesh model is stored!)
            std::string newFilename;

            if (replaceFilenames || !collisionModel->getVisualization())
            {
                newFilename = getFilenameReplacementColModel();
                collisionModel->getVisualization()->setFilename(newFilename, false);
            }
            else
            {
                std::string fnV = collisionModel->getVisualization()->getFilename();
                boost::filesystem::path fn(fnV);
                boost::filesystem::path filnameNoPath = fn.filename();
                newFilename = filnameNoPath.string();
            }

            res = res & collisionModel->saveModel(modelPath, newFilename);
        }

        return res;
    }

    std::string SceneObject::getFilenameReplacementVisuModel(const std::string standardExtension)
    {
        std::string fnV = visualizationModel->getFilename();
        boost::filesystem::path fn(fnV);
        boost::filesystem::path filnameNoPath = fn.filename();
        boost::filesystem::path extension = fn.extension();
        std::string extStr = extension.string();

        if (extStr.empty())
        {
            extStr = standardExtension;    // try with wrl
        }

        std::string newFilename = name;
        newFilename += "_visu";
        newFilename += extStr;
        return newFilename;
    }

    std::string SceneObject::getFilenameReplacementColModel(const std::string standardExtension)
    {
        std::string newFilename = name;
        newFilename += "_col";
        std::string extStr;

        if (collisionModel && collisionModel->getVisualization())
        {
            boost::filesystem::path fn(collisionModel->getVisualization()->getFilename());
            boost::filesystem::path extension = fn.extension();
            extStr = extension.string();
        }

        if (extStr.empty())
        {
            extStr = standardExtension;    // try with wrl
        }

        newFilename += extStr;
        return newFilename;
    }

    SceneObject::Physics::Physics()
    {
        localCoM.setZero();
        inertiaMatrix.setIdentity();
        massKg = 0.0f;
        friction = -1.0f;
        comLocation = eVisuBBoxCenter;
        simType = eUnknown;
    }

    std::string SceneObject::Physics::getString(SceneObject::Physics::SimulationType s) const
    {
        std::string r;

        switch (s)
        {
            case eStatic:
                r = "Static";
                break;

            case eKinematic:
                r = "Kinematic";
                break;

            case eDynamic:
                r = "Dynamic";
                break;

            default:
                r = "Unknown";
        }

        return r;
    }

    void SceneObject::Physics::print() const
    {
        std::cout << " ** Simulation Type: " << getString(simType) << endl;
        std::cout << " ** Mass: ";

        if (massKg <= 0)
        {
            std::cout << "<not set>" << std::endl;
        }
        else
        {
            std::cout << massKg << " [kg]" << std::endl;
        }

        cout << " ** local CoM [mm] ";

        if (comLocation == SceneObject::Physics::eVisuBBoxCenter)
        {
            std::cout << "(center of visualization's bounding box):";
        }
        else
        {
            std::cout << ":";
        }

        std::cout << localCoM(0) << ", " << localCoM(1) << ", " << localCoM(2) << std::endl;
        {
            // scope
            std::ostringstream sos;
            sos << std::setiosflags(std::ios::fixed);
            sos << " ** inertial matrix [kg*m^2]:" << std::endl << inertiaMatrix << std::endl;
            std::cout << sos.str();
        } // scope

        if (ignoreCollisions.size() > 0)
        {
            std::cout << " ** Ignore Collisions with:" << std::endl;

            for (size_t i = 0; i < ignoreCollisions.size(); i++)
            {
                std::cout << " **** " << ignoreCollisions[i] << std::endl;
            }
        }
    }

    bool SceneObject::Physics::isSet()
    {
        return (simType != eUnknown || massKg != 0.0f || comLocation != eVisuBBoxCenter || !inertiaMatrix.isIdentity() || ignoreCollisions.size() > 0);
    }

    std::string SceneObject::Physics::toXML(int tabs)
    {
        std::string ta;
        std::stringstream ss;

        for (int i = 0; i < tabs; i++)
        {
            ta += "\t";
        }

        ss << ta << "<Physics>\n";

        if (simType != eUnknown)
        {
            ss << ta << "\t<SimulationType value='" << getString(simType) << "'/>\n";
        }

        ss << ta << "\t<Mass unit='kg' value='" << massKg << "'/>\n";
        ss << ta << "\t<CoM location=";

        if (comLocation == eVisuBBoxCenter)
        {
            ss << "'VisualizationBBoxCenter'/>\n";
        }
        else
        {
            ss << "'Custom' x='" << localCoM(0) << "' y='" << localCoM(1) << "' z='" << localCoM(2) << "'/>\n";
        }

        ss << ta << "\t<InertiaMatrix>\n";
        ss << MathTools::getTransformXMLString(inertiaMatrix, tabs + 2, true);
        ss << ta << "\t</InertiaMatrix>\n";

        for (size_t i = 0; i < ignoreCollisions.size(); i++)
        {
            ss << ta << "\t<IgnoreCollisions name='" << ignoreCollisions[i] << "'/>\n";
        }

        ss << ta << "</Physics>\n";
        return ss.str();
    }

    SceneObject::Physics SceneObject::Physics::scale(float scaling) const
    {
        THROW_VR_EXCEPTION_IF(scaling <= 0, "Scaling must be > 0");
        Physics res = *this;
        res.localCoM *= scaling;
        return res;
    }

} // namespace
