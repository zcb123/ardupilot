/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

/*
  define common vehicle build types.
  Also note that code needs to support other APM_BUILD_DIRECTORY
  values for example sketches
 */
/*
 定义一般飞行器的构建类型
 另请注意，代码需要支持其他 APM_BUILD_DIRECTORY 值，例如草图
 */
#define APM_BUILD_Rover      1
#define APM_BUILD_ArduCopter     2
#define APM_BUILD_ArduPlane      3
#define APM_BUILD_AntennaTracker 4
#define APM_BUILD_UNKNOWN        5
#define APM_BUILD_Replay         6
#define APM_BUILD_ArduSub        7
#define APM_BUILD_iofirmware     8
#define APM_BUILD_AP_Periph      9
#define APM_BUILD_AP_DAL_Standalone 10
#define APM_BUILD_AP_Bootloader  11
#define APM_BUILD_Blimp      12

#ifndef APM_BUILD_DIRECTORY
#define APM_BUILD_DIRECTORY APM_BUILD_UNKNOWN
#endif

/*
  using this macro catches cases where we try to check vehicle type on
  build systems that don't support it
 */
/*
  使用这个宏可以捕获我们尝试在不支持它的构建系统上检查车辆类型的情况
*/
#define APM_BUILD_TYPE(type) ((type) == APM_BUILD_DIRECTORY)
