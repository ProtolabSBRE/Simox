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
* @copyright  2017 Adrian Knobloch
*             GNU Lesser General Public License
*
*/
#ifndef _VirtualRobot_CoinElement_h_
#define _VirtualRobot_CoinElement_h_

#include <VirtualRobot/VirtualRobot.h>
#include <memory>

class SoNode;

namespace VirtualRobot
{
    class VIRTUAL_ROBOT_IMPORT_EXPORT CoinElement
    {
    public:
        virtual SoNode* getMainNode() const = 0;
    };
    typedef std::shared_ptr<CoinElement> CoinElementPtr;
} // namespace VirtualRobot

#endif // _VirtualRobot_CoinVisualization_h_