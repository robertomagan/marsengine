#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <list>
#include <vector>
#include <stdio.h>
#include <signal.h>
#include "mengine/MEthread.h"
#include "motion/MotionMobility.h"

using namespace std;

/**
 * Search an ID into a vector
 *
 * @param nodeID
 * @param anIDs
 * @return
 */
volatile sig_atomic_t interrupt = 0;
bool isAN(int nodeID, vector<int> & ids);
void signal_callback_handler(int signum);

int main(int argc, char * argv[]) {

	signal(SIGINT, signal_callback_handler);

	if (argc < 4) {

		printf("\nUse: ./marsengine <lcm listener channel> <lcm publisher channel> <H> <debug> <AN IDs>\n");
		return 1;

	} else {

		// LCM channel to listener from tracking system
		string lcmChannelListener = argv[1];
		// LCM channel to publish the new positions of the ANs
		string lcmChannelPublisher = argv[2];
		// H
		int H = atoi(argv[3]);
		// matlab engine debug
		int debug = atoi(argv[4]);

		// Last U4 Ns positions
		list<vector<double> > vit4last;

		// AN Ids
		vector<int> anIDs;

		for (int i = 5; i < argc; i++) {
			anIDs.push_back(atoi(argv[i]));
			printf("AN %d read as ID = %d\n", i - 4, anIDs[i - 5]);
		}

		if (anIDs.size() == 0) {
			printf("Error: a number of AN IDs is needed :(");
			return 1;
		}

		map<int, MotionMobility> motionNodes;
		map<int, MotionMobility>::iterator it;

		printf("Obtained %d ANs from command line ...\n", anIDs.size());

		double minDistanceToTargetPoint = 0.12;

		/** MATLAB Engine */
		METhread meThread;
		meThread.setLCMPublisher(H,debug,lcmChannelPublisher);
		//meThread.setMEngine(mEngine);
		meThread.setRunning(false);
		meThread.setANnumber(anIDs.size());

		// List of node obstacles retrieved by LCM-tracking system. The other nodes are an obstacles for me.
		map<UInt8, Node> listNodes;
		// List of nodes with a new target point
		map<UInt8, Node> listNodesToPublish;

		/** LCM listener */
		LCMHandler * lcmHandler = new LCMHandler("udpm://239.255.76.67:7667?ttl=1", lcmChannelListener);
		printf("Listening on %s channel\n", lcmChannelListener.c_str());

		// Wait until we receive the LCM message to establish the intial positions of the ANs
		// Their initial positions will be the same as the initial target point to make the ANs statics
		lcmHandler->getAvailableMessages();
		listNodes = lcmHandler->retrieveNodeList();

		//AN ids sorted???
		vector<int> anIDsSorted;
		vector<int> unIDsSorted;

		printf("Setting AN initial positions. %d nodes received from tracking system ...\n", listNodes.size());

		for (map<UInt8, Node>::iterator it = listNodes.begin(); it != listNodes.end(); it++) {

			// Get the node from
			Node nodeAux = it->second;

			//printf("Node ID from listNodes %d, (%f,%f)\n", it->first, (it->second).getPosition().GetX(),(it->second).getPosition().GetX());

			// if this node is and AN to an_array
			if (isAN(nodeAux.getId(), anIDs)) {

				MotionMobility motion;
				motion.setInitialPosition(nodeAux.getPosition().GetX(), nodeAux.getPosition().GetY());
				motion.setTargetPosition(nodeAux.getPosition().GetX(), nodeAux.getPosition().GetY());
				motionNodes.insert(std::pair<int, MotionMobility>(nodeAux.getId(), motion));
				anIDsSorted.push_back(nodeAux.getId());
				printf("AN ID initial position (%f,%f)\n", it->first, motion.getTargetPosition().x, motion.getTargetPosition().y);
			} else {
				unIDsSorted.push_back(nodeAux.getId());
			}

		}

		double un_positions[anIDsSorted.size() * 2];
		double an_positions[unIDsSorted.size() * 2];

		double last_un_positions[2 * 4 * unIDsSorted.size()];

		for (int i = 0; i < 2 * 4 * unIDsSorted.size(); i++) {
			last_un_positions[i] = 0.0f;
		}

		vector<double> unPositions;
		vector<double> anPositions;

		int t = 1;

		bool allNodesReached = true;

		bool matlabEngineRunning = false;

		while (true) {

			if (!interrupt) {

				/** LCM related Nodes information */
				//To get the other nodes locations through LCM - from tracking system channel
				//Wait until an LCM message is received.
				lcmHandler->getAvailableMessages();
				listNodes = lcmHandler->retrieveNodeList();
				lcmHandler->printNodeListElements();

				printf("\n**************** ITERATION %d *******************\n", t);
				printf("Retrieved %d nodes from tracking system ...\n", listNodes.size());

				anPositions.erase(anPositions.begin(), anPositions.end());
				unPositions.erase(unPositions.begin(), unPositions.end());

				if (listNodes.size() != 0) {

					for (map<UInt8, Node>::iterator it = listNodes.begin(); it != listNodes.end(); it++) {

						// Get the node from
						Node nodeAux = it->second;

						//printf("Node ID from listNodes %d, (%f,%f)\n", it->first, (it->second).getPosition().GetX(),(it->second).getPosition().GetX());

						// if this node is and AN to an_array
						if (isAN(nodeAux.getId(), anIDs)) {
							anPositions.push_back(nodeAux.getPosition().GetX());
							anPositions.push_back(nodeAux.getPosition().GetY());
							//anIDsSorted.push_back(nodeAux.getId());
							printf("AN ID from listNodes %d, (%f,%f)\n", it->first, *(anPositions.end() - 2), anPositions.back());
						} else {
							// to a UN array
							unPositions.push_back(nodeAux.getPosition().GetX());
							unPositions.push_back(nodeAux.getPosition().GetY());
							//unIDsSorted.push_back(nodeAux.getId());
							printf("UN ID from listNodes %d, (%f,%f)\n", it->first, *(unPositions.end() - 2), unPositions.back());
						}

					}

					/* h_pred */

					if (H != 0) {
						if (t < 4 || t == 4) {

							vit4last.push_back(unPositions);

						} else {

							//Remove first element
							vit4last.pop_front();
							//Add new position
							vit4last.push_back(unPositions);

						}

						if (t > 4 || t == 4) {

							std::list<std::vector<double> >::iterator itl;
							std::vector<double>::iterator itv;

							//printf("vit4last:\n");

							int i = 0;
							int j = 0;
							for (itl = vit4last.begin(); itl != vit4last.end(); itl++) {
								for (itv = itl->begin(); itv != itl->end(); itv++) {

									//printf("%f,", (*itv));

									last_un_positions[2 * unIDsSorted.size() * i + j] = (*itv);

									j++;
								}
								j = 0;

								i++;
							}
							//printf("\n");

						}

					}

//					map<int, bool> allANreachedTP;
//
//					for (map<UInt8, Node>::iterator it = listNodes.begin(); it != listNodes.end(); it++) {
//
//						// Get the node from
//						Node nodeAux = it->second;
//
//						if (isAN(nodeAux.getId(), anIDs)) {
//							// Get the last target point
//							Coord targetPosition = motionNodes.at(nodeAux.getId()).getTargetPosition();
//
//							printf("Last target point of node %d (%f,%f)\n", nodeAux.getId(), targetPosition.x, targetPosition.y);
//
//							// Get the distance between the last target position and the current position
//							double ed = sqrt(pow((nodeAux.getPosition().GetX() - targetPosition.x), 2) + pow((nodeAux.getPosition().GetY() - targetPosition.y), 2));
//
//							printf("Current position of node %d (%f,%f)\n", nodeAux.getId(), nodeAux.getPosition().GetX(), nodeAux.getPosition().GetY());
//							printf("Distance of node %d = %f\n", nodeAux.getId(), ed);
//
//							// if the node has reached the last target point we need to setup the new by calling matlab engine
//							if (ed < minDistanceToTargetPoint) {
//								allANreachedTP[nodeAux.getId()] = true;
//							} else {
//								allANreachedTP[nodeAux.getId()] = false;
//							}
//						}
//					}

//					for (map<int, bool>::iterator it = allANreachedTP.begin(); it != allANreachedTP.end(); it++) {
//						if ((it->second) == false) {
//							allNodesReached = false;
//						}
//					}
//
//					allNodesReached ? printf("All ANs has reached their TP\n") : printf("At least on AN has not reached its TP\n");
//
//					allANreachedTP.erase(allANreachedTP.begin(), allANreachedTP.end());

//					printf("last_un_positions:\n");
//					for (int j = 0; j < 4; j++) {
//						printf("%d positions:", j);
//						for (int i = 0; i < 2 * unIDsSorted.size(); i++) {
//							printf("%f,", last_un_positions[2 * j * unIDsSorted.size() + i]);
//						}
//						printf("\n");
//					}

//					if (allNodesReached) {

					/* Transform maps positions to vectors. */

					if (!meThread.isRunning()) {

						/** MATLAB engine */

						for (int i = 0; i < unIDsSorted.size(); i++) {

							//printf("UNs, i=%d, value=(%f,%f)\n", i, unPositions.at(2 * i), unPositions.at(2 * i + 1));
							un_positions[2 * i] = unPositions.at(2 * i);
							un_positions[2 * i + 1] = unPositions.at(2 * i + 1);

						}

						for (int i = 0; i < anIDsSorted.size(); i++) {

							an_positions[2 * i] = anPositions.at(2 * i);
							an_positions[2 * i + 1] = anPositions.at(2 * i + 1);

							//printf("AN previous %d - ", i);
							//printf("(%f,%f)\n", an_positions[2 * i], an_positions[2 * i + 1]);

						}

						// Matlab engine set-up
						meThread.getMEngine()->setAnNumber(anIDsSorted.size());
						meThread.getMEngine()->setAnPositions(an_positions);
						meThread.getMEngine()->setUnNumber(unIDsSorted.size());
						meThread.getMEngine()->setUnPositions(un_positions);
						meThread.getMEngine()->setTime(t);
						meThread.getMEngine()->setVit4Last(last_un_positions);
						meThread.setAnIDsSorted(anIDsSorted);

						printf("Running MATLAB ######################################%d\n", t);
						meThread.startInternalThread();
						printf("Running MATLAB AFTER ######################################%d\n", t);

						//} //allNodesreached
					}
				}

				//usleep(500000);
				t++;
				//allNodesReached = true;

			} else {
				// I there wass a ctrl+c signal
				printf("\nCTRL+C - Closing MATLAB ENGINE and EXIT\n");
				meThread.getMEngine()->closeEngine();
				exit(1);
			}
		}

	}
	return 0;
}

/**
 * Search an ID into a vector
 *
 * @param nodeID
 * @param anIDs
 * @return
 */
bool isAN(int nodeID, vector<int> & ids) {
	return (find(ids.begin(), ids.end(), nodeID) != ids.end());
}

void signal_callback_handler(int signum) {
	//printf("Caught signal %d\n", signum);
	// Cleanup and close up stuff here
	interrupt = true;

	// Terminate program
	//exit(signum);
}
