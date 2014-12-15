/*
 * Copyright (C) 2014, IDSIA (Institute Dalle Molle for Artificial Intelligence), http://http://www.idsia.ch/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <string>
#include <lcm/lcm-cpp.hpp>

#include "mEngine.h"
#include "../lcm/lcmhandler.h"
#include "../thread/threadengine.h"

using namespace std;
using namespace lcm;

/**
 * @brief LCM thread
 *
 * @details For getting the received messages in a separated thread.
 *
 * @author Roberto Magán Carrión, rmagan@ugr.es, rmagan@idsia.ch
 * @date 05/11/2014
 *
 */

class METhread: public ThreadEngine {
public:

	void setLCMPublisher(int horizont, int debug, string &channel) {

		matlabEngine = new mEngine(horizont,debug);

		lcmCommand = new LCMHandler("udpm://239.255.76.67:7667?ttl=1", channel);
		printf("Channel to publish %s \n", channel.c_str());
	}

	/**
	 * Is listening for messages on an specific channel that was previously established.
	 */
	void internalThreadEntry() {
		running = true;
		printf("Runing matlab engine...\n");
		newAnPositions = matlabEngine->runEgine();
		printf("Runing matlab engine... 1 \n");
		for (int i = 0; i < ANnumber; i++) {
			// Conform the list of nodes to publish.
			Node nodeToPublish;

			CVector3 positionsAux;
			positionsAux.Set(newAnPositions.at(2 * i) * 1000.0f, newAnPositions.at(2 * i + 1) * 1000.0f, 0.0f);

			nodeToPublish.setId(anIDsSorted.at(i));
			nodeToPublish.setPosition(positionsAux);

			// If does not exist, add it!
			if (listNodesToPublish.end() == listNodesToPublish.find(nodeToPublish.getId())) {
				listNodesToPublish.insert(pair<UInt8, Node>(nodeToPublish.getId(), nodeToPublish));
			} else {
				//Remove
				listNodesToPublish.erase(nodeToPublish.getId());
				//Insert
				listNodesToPublish.insert(pair<UInt8, Node>(nodeToPublish.getId(), nodeToPublish));
			}

			// Set new target positions
			//motionNodes.at(anIDsSorted.at(i)).setTargetPosition(positionsAux.GetX() / 1000.0f, positionsAux.GetY() / 1000.0f);

		}

		//Publish the updated targets points
		lcmCommand->publish(listNodesToPublish);

		printf("Node list to publish with %d elements.\n", listNodesToPublish.size());
		for (map<UInt8, Node>::iterator it = listNodesToPublish.begin(); it != listNodesToPublish.end(); it++) {

			printf("ID %d - location (%f,%f,%f)\n", it->first, (it->second).getPosition().GetX(), (it->second).getPosition().GetY(), (it->second).getPosition().GetZ());

		}

		running = false;
	}

	/* Getters & Setters */
	mEngine * getMEngine() {
		return matlabEngine;
	}

	void setMEngine(mEngine * MEngine) {
		matlabEngine = MEngine;
	}

	bool isRunning() const {
		return running;
	}

	void setRunning(bool running) {
		this->running = running;
	}

	const vector<double>& getNewAnPositions() const {
		return newAnPositions;
	}

	void setNewAnPositions(const vector<double>& newAnPositions) {
		this->newAnPositions = newAnPositions;
	}

	int getANnumber() const {
		return ANnumber;
	}

	void setANnumber(int nnumber) {
		ANnumber = nnumber;
	}

	const vector<int>& getAnIDsSorted() const {
		return anIDsSorted;
	}

	void setAnIDsSorted(const vector<int>& anIDsSorted) {
		this->anIDsSorted = anIDsSorted;
	}

private:

	/**
	 * Matlab enfine
	 */
	mEngine * matlabEngine;

	/**
	 * To determine if the Matlab engine is running
	 */
	bool running;

	/**
	 * New computed AN positions
	 */
	vector<double> newAnPositions;

	/**
	 * Number of ANs
	 */
	int ANnumber;

	// List of nodes with a new target point
	map<UInt8, Node> listNodesToPublish;

	vector<int> anIDsSorted;

	/** LCM Publisher */
	LCMHandler * lcmCommand;

};

