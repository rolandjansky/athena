/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "HTTClusteringTool.h"
#include "TrigHTTObjects/HTTMultiTruth.h"
#include "TrigHTTObjects/HTTConstants.h"
#include <map>
#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <stack>
#include <queue>

//For deciding eta || phi columns in modules
const unsigned int ETA = 1;
const unsigned int PHI = 0;


HTTClusteringTool::HTTClusteringTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  base_class(algname, name, ifc)
{
}


StatusCode HTTClusteringTool::DoClustering(HTTLogicalEventInputHeader &header, std::vector<HTTCluster> &clusters) const
{
    for (int i = 0; i<header.nTowers(); i++)
    {
        // Retreive the hits from the tower
        HTTTowerInputHeader& tower = *header.getTower(i);
        std::vector<HTTHit> hits = tower.hits();

        std::vector<std::vector<HTTHit>> hitsPerModule;
        std::vector<HTTCluster> towerClusters;
        splitAndSortHits(hits, hitsPerModule);
        SortedClustering(hitsPerModule, towerClusters);
        normaliseClusters(towerClusters);

        //remove the old hits from the tower...
        tower.clearHits();
        tower.reserveHits(towerClusters.size());
        clusters.clear();
        clusters.reserve(towerClusters.size());
        if(i > 1)
          ATH_MSG_WARNING("more than one tower, m_clusters is only going to contain those from the last one");
        unsigned cluster_count = 0;
        for ( auto &cluster: towerClusters){
          HTTHit cluster_as_HTThit = cluster.getClusterEquiv();
          cluster_as_HTThit.setHitType(HitType::clustered);
          cluster_as_HTThit.setParentageMask(cluster_count); // making use of unused m_parentageMask to keep track of cluster index
          tower.addHit(cluster_as_HTThit);
          //send back a copy for monitoring and to check when writing out hits in each road
          clusters.push_back(cluster);
          cluster_count++;
        }
    }
  ATH_MSG_DEBUG("Produced "<< clusters.size()<< " clusters");
  return StatusCode::SUCCESS;
}

//Attempt to implement clustering using HTTSim objects.
void HTTClusteringTool::SortedClustering(const std::vector<std::vector<HTTHit> >& sorted_hits, std::vector<HTTCluster> &clusters) const {
  std::vector<HTTCluster> moduleClusters;
  //Loop over the sorted modules that we have
  for( auto& moduleHits:sorted_hits){
    //Make the clusters for this module
    Clustering(moduleHits, moduleClusters);
    //Put these clusters into the output list
    clusters.insert(clusters.end(), moduleClusters.begin(), moduleClusters.end());
    //Clear the vector or this will get messy
    moduleClusters.clear();
  }
}

void HTTClusteringTool::Clustering(std::vector<HTTHit> moduleHits, std::vector<HTTCluster> &moduleClusters) const {
  //To hold the current cluster vars for comparison
  int count = 0;
  //loop over the hits that we have been passed for this module
  for( auto& hit: moduleHits){
    int is_clustered_hit =0;
    // int nclustered =0;
    //Loop over the clusters we have already made, check if this hit should be added to them?
    for( auto& cluster: moduleClusters){
      if(hit.isPixel()){
	        is_clustered_hit = HTTCLUSTERING::updatePixelCluster(cluster, hit, false);
      }
      if(hit.isStrip()){
	        is_clustered_hit = HTTCLUSTERING::updateStripCluster(cluster, hit, false);
      }
    }
    //If it is the first hit or a not clustered hit, then start a new cluster and add it to the output vector
    if((is_clustered_hit==0) or (moduleClusters.size()==0)){
      HTTCluster cluster;
      if(hit.isPixel()){
	is_clustered_hit = HTTCLUSTERING::updatePixelCluster(cluster, hit, true);
      }
      if(hit.isStrip()){
	is_clustered_hit = HTTCLUSTERING::updateStripCluster(cluster, hit, true);
      }
      //Put this cluster into the output hits. Will update it in place.
      moduleClusters.push_back(cluster);
    }
    count++;
  }
}

void HTTClusteringTool::splitAndSortHits(std::vector<HTTHit> &hits, std::vector<std::vector<HTTHit> > &hitsPerModule, int &eta_phi) const {
  splitHitsToModules(hits, hitsPerModule);
  sortHitsOnModules(hitsPerModule, eta_phi);
}

void HTTClusteringTool::splitAndSortHits(std::vector<HTTHit> &hits, std::vector<std::vector<HTTHit> > &hitsPerModule) const{
  splitHitsToModules(hits, hitsPerModule);
  sortHitsOnModules(hitsPerModule);
}

/*Temporarilly sort the hits into module by module packets
 */
void HTTClusteringTool::splitHitsToModules(std::vector<HTTHit> &hits, std::vector<std::vector<HTTHit> > &hitsPerModule) const{
  //To hold the current module
  std::vector<HTTHit> currentModule;
  uint hashing = 0;
  //Split the incoming hits into hits by module
  for ( auto& hit:hits){
    if(hashing == 0){
      currentModule.push_back(hit);
      hashing = hit.getHTTIdentifierHash();
    } else if (hit.getHTTIdentifierHash() == hashing) {
      currentModule.push_back(hit);
    } else {
      hitsPerModule.push_back(currentModule);
      currentModule.clear();
      hashing = hit.getHTTIdentifierHash();
      currentModule.push_back(hit);
    }
  }

  // Now push that last one
  if (currentModule.size() > 0) hitsPerModule.push_back(currentModule);
}

void HTTClusteringTool::sortHitsOnModules(std::vector<std::vector<HTTHit> > &hitsPerModule, int &eta_phi) const{
  //Loop over the module separated hits
  for ( auto& module:hitsPerModule){
    //Work out if columns are ETA (1) || PHI (0)
    if(etaOrPhi(module.at(0)) == true){
      //Sort by ETA first
      eta_phi = ETA;
      if (module.size() > 1) std::stable_sort(module.begin(), module.end(), HTTCLUSTERING::sortITkInputEta);
      if (module.size() > 1) std::stable_sort(module.begin(), module.end(), HTTCLUSTERING::sortITkInputPhi);
    } else {
      //Sort by PHI first
      eta_phi = PHI;
      if (module.size() > 1) std::stable_sort(module.begin(), module.end(), HTTCLUSTERING::sortITkInputPhi);
      if (module.size() > 1) std::stable_sort(module.begin(), module.end(), HTTCLUSTERING::sortITkInputEta);
    }
  }
}

void HTTClusteringTool::sortHitsOnModules(std::vector<std::vector<HTTHit> > &hitsPerModule) const{
  //Loop over the module separated hits
  for ( auto& module:hitsPerModule){
    if (module.size() > 1) std::stable_sort(module.begin(), module.end(), HTTCLUSTERING::sortITkInputEta);
    if (module.size() > 1) std::stable_sort(module.begin(), module.end(), HTTCLUSTERING::sortITkInputPhi);
  }
}



//Need to remove the htt::scaleHitFactor and normalise the widths
void HTTClusteringTool::normaliseClusters(std::vector<HTTCluster> &clusters) const {
  for( auto &cluster:clusters){
    //Grab the cluster equiv
    HTTHit clusterEquiv = cluster.getClusterEquiv();
    //Update the clusterEquiv's position and width
    if(clusterEquiv.isStrip()){
      //Clear the groupsize, set this to be one as we are only clustering one row.
      clusterEquiv.setEtaWidth(1);
      clusterEquiv.setPhiCoord(clusterEquiv.getPhiCoord()/htt::scaleHitFactor);
      clusterEquiv.setPhiWidth(clusterEquiv.getPhiWidth()+1);
    } else {
      clusterEquiv.setEtaCoord(clusterEquiv.getEtaCoord()/htt::scaleHitFactor);
      clusterEquiv.setEtaWidth(clusterEquiv.getEtaWidth()+1);
      clusterEquiv.setPhiCoord(clusterEquiv.getPhiCoord()/htt::scaleHitFactor);
      clusterEquiv.setPhiWidth(clusterEquiv.getPhiWidth()+1);
    }
    cluster.setClusterEquiv(clusterEquiv);
  }
}


/*Function to work out if we need to sort by eta or phi first.
 *Depends on the position and orientation of the module in the detector.
 * Currently backwards engineered from the MC. Need to ask ITk people to confirm.
 */
bool HTTClusteringTool::etaOrPhi(const HTTHit& hit) const {

  /*This currently might get complicated as eta/phi ordering varies depending on the position in the detector.
   * For ITK-20-00-00 Step 2.2 inclined duals layout, worked out by Naoki.
   * Detector position               |  Module Type (# chip)  |  Column  |  Row  |
   * =============================================================================
   * Barrel Layer 0 Eta module 1-6   |  2           (double)  |  eta     |  phi  |
   * Barrel Layer 0 Eta module 7-22  |  1           (single)  |  phi     |  eta  |
   * Barrel Layer 1 Eta module 1-6   |  3           (quad)    |  eta     |  phi  |
   * Barrel Layer 1 Eta module 7-19  |  3                     |  phi     |  eta  |
   * Barrel Layer 2 Eta module 1-11  |  3                     |  eta     |  phi  |
   * Barrel Layer 2 Eta module 12-22 |  2                     |  phi     |  eta  |
   * Barrel Layer 3 Eta module 1-12  |  3                     |  eta     |  phi  |
   * Barrel Layer 3 Eta module 13-25 |  2                     |  phi     |  eta  |
   * Barrel Layer 4 Eta module 1-13  |  3                     |  eta     |  phi  |
   * Barrel Layer 4 Eta module 14-26 |  2                     |  phi     |  eta  |
   * All Endcap modules              |  3                     |  eta     |  phi  |
   * =============================================================================
   * Module Type 1 = Single, 2, Dual, 3 Quad
   * 328x400 blocks
   * Hit type is essentially isPixel
   * DetectorZone 0 = Barrel, -ive/+ive = endcaps
   */

  //Check if the two hits are from the same module
  //If it is not a barrel module then sort eta as column
  if(hit.getDetectorZone() != DetectorZone::barrel){
    return ETA;
  }
  //Otherwise it is a barrel module and now things get more complicated
  else {
    //Start by looking at what layer it is in
    if (hit.getPhysLayer() == 0 || hit.getPhysLayer() == 1) {
      if (std::abs(hit.getHTTEtaModule()) <=6) {
	return ETA;
      } else {
	return PHI;
      }
    } else if (hit.getPhysLayer() == 2) {
      if (std::abs(hit.getHTTEtaModule()) <=11) {
	return ETA;
      } else {
	return PHI;
      }
    } else if (hit.getPhysLayer() == 3) {
      if (std::abs(hit.getHTTEtaModule()) <=12) {
	return ETA;
      } else {
	return PHI;
      }
    } else if (hit.getPhysLayer() == 4) {
      if (std::abs(hit.getHTTEtaModule()) <=13) {
	return ETA;
      } else {
	return PHI;
      }
    }
  }
  //Default to ETA, but shouldn't reach here
  return ETA;
}



void HTTCLUSTERING::attachTruth(std::vector<HTTHit> &hits){
  for( auto& hit : hits) {
    HTTMultiTruth mt;
    // record highest pt contribution to the combination (cluster
    if(!hit.getTruth().isEmpty()) {
      mt.add(hit.getTruth());
      hit.setTruth(mt);
    } else {
      HTTMultiTruth::Barcode uniquecode(hit.getEventIndex(), hit.getBarcode());
      mt.maximize(uniquecode, hit.getBarcodePt());
      hit.setTruth(mt);
    }
  }
} //record truth for each raw channel in the cluster

/*
 * This function is used in the HTTClusteringTools to see if a new hit should be added to the current cluster under construction. It assumes double precision hits.
 * It checks if the hit is in a number of positions w.r.t. the cluster being formed: up/right, down/right, above, right, or inside a cluster that has formed a horseshoe.
 */
bool HTTCLUSTERING::updatePixelCluster(HTTCluster &currentCluster, HTTHit &incomingHit, bool newCluster){

  if(newCluster){
    HTTHit newHit = incomingHit;
    //Double the precision on the positions
    //By doing this the hardware is able to handle clusters where the centre of the cluster is on a boundary between two clusters without needing a float
    newHit.setEtaIndex(incomingHit.getEtaIndex()*htt::scaleHitFactor);
    newHit.setPhiIndex(incomingHit.getPhiIndex()*htt::scaleHitFactor);
    //Set the initial clusterEquiv to be the incoming hit with double precision
    currentCluster.setClusterEquiv(newHit);
    //Add the current hit to the list of hits
    currentCluster.push_backHitList(incomingHit);
    //It doesn't really matter, as we will be at the end of the hit loop, but we did technically "cluster" this hit
    return true;
  } else {
    int hitRow = incomingHit.getEtaIndex();
    int hitCol = incomingHit.getPhiIndex();

    HTTHit clusterEquiv = currentCluster.getClusterEquiv();
    int clusterRow = clusterEquiv.getEtaIndex();
    int clusterRowWidth = clusterEquiv.getEtaWidth();
    int clusterCol = clusterEquiv.getPhiIndex();
    int clusterColWidth = clusterEquiv.getPhiWidth();

    //Looking for a neighbour in up/right position to the currentCluster
    if((hitRow*htt::scaleHitFactor == clusterRow+clusterRowWidth+htt::scaleHitFactor) &&
       (hitCol*htt::scaleHitFactor == clusterCol+clusterColWidth+htt::scaleHitFactor) ){
      clusterRow++;
      clusterRowWidth++;
      clusterCol++;
      clusterColWidth++;
      HTTCLUSTERING::updateClusterContents(currentCluster, clusterRow, clusterRowWidth, clusterCol, clusterColWidth, incomingHit);
      return true;
    }

    //Looking for a neighbour in down right
    else if((hitRow*htt::scaleHitFactor == clusterRow-clusterRowWidth-htt::scaleHitFactor) && //because row then col sorted data, i.e. col sorted.
	    (hitCol*htt::scaleHitFactor == clusterCol+clusterColWidth+htt::scaleHitFactor) ){
      clusterRow--; // important
      clusterRowWidth++;
      clusterCol++;
      clusterColWidth++;
      HTTCLUSTERING::updateClusterContents(currentCluster, clusterRow, clusterRowWidth, clusterCol, clusterColWidth, incomingHit);
      return true;
    }

    //Looking for a neighbour above
    else if((hitRow*htt::scaleHitFactor == clusterRow+clusterRowWidth+htt::scaleHitFactor) &&
	    (hitCol*htt::scaleHitFactor == clusterCol+clusterColWidth) ){
      clusterRow++;
      clusterRowWidth++;
      HTTCLUSTERING::updateClusterContents(currentCluster, clusterRow, clusterRowWidth, clusterCol, clusterColWidth, incomingHit);
      return true;
    }

    //Looking for a neighbour to the right
    else if(((hitRow*htt::scaleHitFactor >  clusterRow-clusterRowWidth-htt::scaleHitFactor) && (hitRow*htt::scaleHitFactor <  clusterRow+clusterRowWidth+htt::scaleHitFactor)) &&
	    (hitCol*htt::scaleHitFactor == clusterCol+clusterColWidth+htt::scaleHitFactor) ){
      clusterCol++;
      clusterColWidth++;
      HTTCLUSTERING::updateClusterContents(currentCluster, clusterRow, clusterRowWidth, clusterCol, clusterColWidth, incomingHit);
      return true;
    }

    //Checking for hits inside the box
    else if((hitRow*htt::scaleHitFactor > clusterRow-clusterRowWidth-htt::scaleHitFactor) &&
	    (hitRow*htt::scaleHitFactor < clusterRow+clusterRowWidth+htt::scaleHitFactor) &&
	    (hitCol*htt::scaleHitFactor < clusterCol+clusterColWidth+htt::scaleHitFactor) ){
      //We still want to do this as we are not changing the position of the cluster, but we are adding to its hitlist
      HTTCLUSTERING::updateClusterContents(currentCluster, clusterRow, clusterRowWidth, clusterCol, clusterColWidth, incomingHit);
      return true;
    }
    //if we made it here then this cluster then start again
    else return false;
  }
}

/*
 * This function is used in the HTTClusteringTools to see if a new hit should be added to the current cluster under construction. It assumes double precision hits.
 * It checks if the hit is in a number of positions w.r.t. the cluster being formed: up/right, down/right, above, right, or inside a cluster that has formed a horseshoe.
 */
bool HTTCLUSTERING::updateStripCluster(HTTCluster &currentCluster, HTTHit &incomingHit, bool newCluster){

  // Shift initial widths 1->0, 2->2, 3->4, 4->6 etc...
  //The groupSize is stored in the EtaWidth
  int tempWidth = (incomingHit.getEtaWidth()*htt::scaleHitFactor)-htt::scaleHitFactor;
  // Now shift to pixel width equivalents, 0->0, 2->1, 4->2, 6->3 etc...
  if(tempWidth > 0) tempWidth = tempWidth/htt::scaleHitFactor;
  if(newCluster){
    HTTHit newHit = incomingHit;
    //Double the precision of the strip positions.
    int tempCentroid = incomingHit.getPhiCoord()*htt::scaleHitFactor;
    // Now shift the centroid phi+phiWidth, and store the width (put it back in the PhiWidth)
    newHit.setPhiCoord(tempCentroid+tempWidth);
    newHit.setPhiWidth(tempWidth);
    //Set the initial clusterEquiv to be the incoming hit with double precision
    currentCluster.setClusterEquiv(newHit);
    //Add the current hit to the list of hits
    currentCluster.push_backHitList(incomingHit);
    //It doesn't really matter, as we will be at the end of the hit loop, but we did technically "cluster" this hit
    return true;
  } else {
    //Now get the --START-- of the new strip cluster
    int hitRow = incomingHit.getEtaCoord();
    int hitCol = incomingHit.getPhiCoord()*htt::scaleHitFactor;

    HTTHit clusterEquiv = currentCluster.getClusterEquiv();
    int clusterRow = clusterEquiv.getEtaCoord();
    int clusterRowWidth = clusterEquiv.getEtaWidth();
    int clusterCol = clusterEquiv.getPhiCoord();
    int clusterColWidth = clusterEquiv.getPhiWidth();

    //Looking for a neighbour to the right. i.e. find the end of the current cluster (Col+width) and look in the next cell (+2). Compare this to the start of the new cluster. This is unlikely/impossible(?) to happen due to preclustering.
    if(hitCol == clusterCol+clusterColWidth+htt::scaleHitFactor && hitRow == clusterRow) {
      //The new centroid will be the original column position, minus its width, plus the new width
      //So subtract the original width...
      clusterCol = clusterCol - clusterColWidth;
      //The new width will be the combination of the current widths, ++
      clusterColWidth = clusterColWidth+tempWidth+1;
      //And add on the new width
      clusterCol = clusterCol + clusterColWidth;
      HTTCLUSTERING::updateClusterContents(currentCluster, clusterRow, clusterRowWidth, clusterCol, clusterColWidth, incomingHit);
      return true;
    } else return false;
  }
}


void HTTCLUSTERING::updateClusterContents(HTTCluster &currentCluster, int &clusterRow, int &clusterRowWidth, int &clusterCol, int &clusterColWidth, HTTHit &incomingHit) {
  //Grab the cluster equiv
  HTTHit clusterEquiv = currentCluster.getClusterEquiv();

  //Update the clusterEquiv's position and width
  clusterEquiv.setEtaIndex(clusterRow);
  clusterEquiv.setEtaWidth(clusterRowWidth);
  clusterEquiv.setPhiIndex(clusterCol);
  clusterEquiv.setPhiWidth(clusterColWidth);


  float xOld = clusterEquiv.getX();
  float yOld = clusterEquiv.getY();
  float zOld = clusterEquiv.getZ();
  float xNew = incomingHit.getX();
  float yNew = incomingHit.getY();
  float zNew = incomingHit.getZ();
  //As strips arrive pre-clustered, this is different for pixels/strips
  if(incomingHit.isPixel()){
    int n = currentCluster.getHitList().size();
    // n+1 because that is old + new now
    clusterEquiv.setX((xOld*n + xNew) / (n+1));
    clusterEquiv.setY((yOld*n + yNew) / (n+1));
    clusterEquiv.setZ((zOld*n + zNew) / (n+1));
  } else {
    //Phi width + 1 for the seed is the width of the current cluster
    int N = currentCluster.getClusterEquiv().getPhiWidth()+1;
    //Phi width of an incoming strip is the width of the cluster
    int newN = incomingHit.getPhiWidth();
    //Now as above, N+newN
    clusterEquiv.setX((xOld*N + xNew*newN) / (N+newN));
    clusterEquiv.setY((yOld*N + yNew*newN) / (N+newN));
    clusterEquiv.setZ((zOld*N + zNew*newN) / (N+newN));
  }
  //Put it back
  currentCluster.setClusterEquiv(clusterEquiv);

  //Pushback the hit into the hitlist
  currentCluster.push_backHitList(incomingHit);
}

/* Sort for the ordering of ITk modules: Sort by ETA.
 */
bool HTTCLUSTERING::sortITkInputEta(const HTTHit& hitA, const HTTHit& hitB)
{
  return hitA.getEtaIndex() < hitB.getEtaIndex();
}

/* Sort for the ordering of ITk modules: Sort by PHI.
 */
bool HTTCLUSTERING::sortITkInputPhi(const HTTHit& hitA, const HTTHit& hitB)
{
  return hitA.getPhiIndex() < hitB.getPhiIndex();
}

