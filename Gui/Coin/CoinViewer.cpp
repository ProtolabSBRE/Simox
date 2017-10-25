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
* @package    Gui
* @author     Peter Kaiser
* @copyright  2016 Peter Kaiser
*             GNU Lesser General Public License
*
*/

#include "CoinViewer.h"

#include <VirtualRobot/Visualization/CoinVisualization/CoinVisualization.h>
#include <VirtualRobot/Visualization/CoinVisualization/CoinVisualizationNode.h>

#include <Inventor/actions/SoLineHighlightRenderAction.h>
#include <Inventor/Qt/SoQt.h>
#include <Inventor/misc/SoChildList.h>
#include <QGLWidget>

using namespace SimoxGui;

CoinViewer::CoinViewer(QWidget *parent) :
    SoQtExaminerViewer(parent, "", true, BUILD_POPUP),
    parent(parent)
{
    sceneSep = new SoSeparator;
    sceneSep->ref();

    setBackgroundColor(SbColor(1, 1, 1));
    setAccumulationBuffer(true);
    setAntialiasing(true, 4);
    setGLRenderAction(new SoLineHighlightRenderAction);
    setFeedbackVisibility(true);
    setSceneGraph(sceneSep);

    viewAll();
    setAntialiasing(true, 4);
}

CoinViewer::~CoinViewer()
{
    sceneSep->unref();
}

void CoinViewer::addVisualization(const std::string &layer, const std::string &id, const VirtualRobot::VisualizationPtr &visualization)
{
    VirtualRobot::CoinVisualizationPtr coinVisu = std::dynamic_pointer_cast<VirtualRobot::CoinVisualization>(visualization);
    if (!coinVisu)
    {
        VR_ERROR << "Not able to display visualization which is not of type CoinVisualization" << std::endl;
        return;
    }

    addLayer(layer);
    removeVisualization(layer, id);

    visualizations[id] = coinVisu->getCoinVisualization();
    layers[layer]->addChild(visualizations[id]);
}

void CoinViewer::addVisualization(const std::string &layer, const std::string &id, const VirtualRobot::VisualizationNodePtr &visualization)
{
    VirtualRobot::CoinVisualizationNodePtr cvn = std::dynamic_pointer_cast<VirtualRobot::CoinVisualizationNode>(visualization);
    VirtualRobot::CoinVisualizationPtr coinVisu(new VirtualRobot::CoinVisualization(cvn));
    addVisualization(layer, id, coinVisu);
}

void CoinViewer::removeVisualization(const std::string &layer, const std::string &id)
{
    if (visualizations[id] == nullptr) return;

    layers[layer]->removeChild(visualizations[id]);
    visualizations.erase(id);
}

void CoinViewer::clearLayer(const std::string &layer)
{
    addLayer(layer);
    // remove all visualizations in the given layer
    for (const std::string & id : getVisualizationIDs(layer))
    {
        removeVisualization(layer, id);
    }
    layers[layer]->removeAllChildren();
}

QImage CoinViewer::getScreenshot()
{
    getSceneManager()->render();
    getSceneManager()->scheduleRedraw();
    QGLWidget* w = (QGLWidget*)getGLWidget();

    QImage i = w->grabFrameBuffer();
    return i;
}

void CoinViewer::start(QWidget *mainWindow)
{
    SoQt::show(mainWindow);
    SoQt::mainLoop();
}

void CoinViewer::stop()
{
    SoQt::exitMainLoop();
}

void CoinViewer::resetView()
{
    viewAll();
}

void CoinViewer::viewAll()
{
    SoQtExaminerViewer::viewAll();
}

void CoinViewer::addLayer(const std::string &layer)
{
    if(layers.find(layer) != layers.end())
    {
        // Layer already exists
        return;
    }

    layers[layer] = new SoSeparator;
    sceneSep->addChild(layers[layer]);
}

std::vector<std::string> CoinViewer::getVisualizationIDs(const std::string &layer)
{
    std::vector<std::string> result;
    for(const auto & visu : visualizations)
    {
        if (layers[layer]->getChildren()->find(visu.second) != -1)
        {
            result.push_back(visu.first);
        }
    }
    return result;
}

