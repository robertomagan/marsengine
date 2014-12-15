//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "MotionMobility.h"
#include "FWMath.h"

MotionMobility::MotionMobility() {
	//is3D = false;
	//currentLine = -1;
}

MotionMobility::~MotionMobility() {
}

void MotionMobility::initialize(int nodeId) {

	//line = bmFile.getLine(nodeId);
	//currentLine = 0;

}

void MotionMobility::setInitialPosition(double x, double y) {
	//const BonnMotionFile::Line& vec = line;
	//if (line.size() >= 3) {
		lastPosition.x = x;
		lastPosition.y = y;
	//}

	targetPosition = lastPosition;
//	for (int i = 0; i < line.size(); i++){
//		printf("line %d -> %f\n",i,line[i]);
//	}
}

void MotionMobility::setTargetPosition(double x, double y) {

//	for (int i = 0; i < line.size(); i++){
//			printf("line %d -> %f\n",i,line[i]);
//		}

	//nextChange = line[currentLine];
	targetPosition.x = x;
	targetPosition.y = y;
	targetPosition.z = 0;
	//currentLine += (is3D ? 4 : 3);
	//printf(" -> target (%f,%f,%f)\n",targetPosition.x,targetPosition.y,targetPosition.z);
}
