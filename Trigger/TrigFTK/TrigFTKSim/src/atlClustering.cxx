/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * atlClusteringLNF
 * ---------------
 *
 * Routines to perform clustering in the pixels.
 *
 * 21-11-2007 Alberto Annovi (alberto.annovi@lnf.infn.it)
 *
 */

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/atlClustering.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/MultiTruth.h"
#include "TrigFTKSim/ftkdefs.h"

#include <boost/circular_buffer.hpp>

#include <cmath>
#include <cassert>
#include <iostream>
#include <stack>
#include <queue>
using namespace std;

#include "TrigFTKSim/ftkdefs.h"
using ftk::PIXEL;
using ftk::SCT;
using ftk::BARREL;
using ftk::ENDCAP;
using ftk::POSEC;
using ftk::NEGEC;

// #define VERBOSE_DEBUG_CLUST
// #define DEBUG_HITS
#define CLUSTERING_PRINTOUT 0

const bool DEBUG_HITS = 0;
const bool DEBUG_CLUSTERS = 0;
const bool DEBUG_AVERAGE = 0;
const bool DEBUG_AVERAGE_SCT = 0;

const unsigned int MOD_ID_PHI_VAL  = 1;
const unsigned int MOD_ID_PHI_MASK = 100;
const unsigned int MOD_ID_ETA_VAL  = MOD_ID_PHI_MASK;
const unsigned int MOD_ID_ETA_MASK = 100;
const unsigned int MOD_ID_LAYER_VAL  = MOD_ID_PHI_MASK*MOD_ID_ETA_MASK;
const unsigned int MOD_ID_LAYER_MASK = 100;

const unsigned int MAX_BARREL_ETA=20;
const unsigned int MAX_ETA=MAX_BARREL_ETA+6;
const unsigned int MAX_PHI=56;
const unsigned int MAX_LAYER_PIXEL=3;
const unsigned int MAX_LAYER=56;


bool SAVE_CLUSTER_CONTENT;
bool DIAG_CLUSTERING;
bool SCT_CLUSTERING;
int PIXEL_CLUSTERING_MODE;
bool DUPLICATE_GANGED;
bool GANGED_PATTERN_RECOGNITION;
bool SPLIT_BLAYER_MODULES;
//bool CLUSTERING_PRINTOUT;
bool BOUNDING_BOX;

bool hitSelector(const FTKRawHit &hit) {
    if (hit.getHitType() == PIXEL && hit.getBarrelEC() == 0 && hit.getLayer()== 1 && hit.getPhiModule() == 13  
            && (hit.getEtaModule() <= 0))  {
        return true;
    }   
    else  {
        return false;
    }   
}
cluster::~cluster()
{
    hitlist.clear();
}

struct fe_hit {
    int fe, tot, lcol, lrow;
    fe_hit(const FTKRawHit* hit) {
        tot = hit->getTot();
        int acol = hit->getEtaStrip();
        int arow = hit->getPhiSide();
        if (arow <= 163) {
            fe = 8 + acol/18;
            lrow = arow;
            lcol = acol%18;
        } else {
            fe = 7 - acol/18;
            lrow = 327 - arow;
            lcol = 17 - acol%18;
        }
    }
};


struct cluster_hit 
{
    int tot, ccol, crow; //cluster coordinates
    cluster_hit(const FTKRawHit &h, const FTKRawHit* seed) {
        tot = h.getTot();
        crow = h.getPhiSide() - (seed->getPhiSide() - 10);
        if (seed->getEtaStrip() %2 != 0) ccol = h.getEtaStrip() - (seed->getEtaStrip() - 1);
        else  ccol = h.getEtaStrip() - (seed->getEtaStrip());
    }
};

bool sortbyFE (const FTKRawHit* i,const  FTKRawHit* j) { 
    if (i->getHitType() == SCT || j->getHitType() == SCT) 
        return false;

    fe_hit lhit1(i);
    fe_hit lhit2(j);

    if (lhit1.fe != lhit2.fe) return lhit1.fe <= lhit2.fe; 
    else if (lhit1.fe == lhit2.fe) {
        if (lhit1.lcol/2 != lhit2.lcol/2) return lhit1.lcol/2 <= lhit2.lcol/2;
        else if (lhit1.lcol/2 == lhit2.lcol/2) return lhit1.lrow < lhit2.lrow;
    }

    return false;
}

bool pixelRowIsGanged(const int row) {
  switch (row) {
  case 153:
  case 155:
  case 157:
  case 159:
  case 168:
  case 170:
  case 172:
  case 174:
    return row;        // pixel ganged found (readout channel)
  }
  if (160<=row && row<=167)
    return row;        // pixel ganged found (non readout channel)
  return false;
}

// int moduleIdToLayer(int modId) {
//   return (modId/MOD_ID_LAYER_VAL)%MOD_ID_LAYER_MASK;
// }

// int moduleIdToIsPixel(int modId) {
//   if (FTKSetup::getFTKSetup().getIBLMode()==1) {
//     return ( (modId/MOD_ID_LAYER_VAL)%MOD_ID_LAYER_MASK ) < MAX_LAYER_PIXEL+1; // addition of one pixel layer for ibl
//   }
//   else {
//     return ( (modId/MOD_ID_LAYER_VAL)%MOD_ID_LAYER_MASK ) < MAX_LAYER_PIXEL;
//   }
// }

// int moduleIdIsEndcap(int modId) {
//   unsigned int eta = ( (modId/MOD_ID_ETA_VAL)%MOD_ID_ETA_MASK );
//   if (MAX_BARREL_ETA <= eta && eta < MAX_BARREL_ETA+3) 
//     return 2; // positive endcap
//   if (MAX_BARREL_ETA+3 <= eta && eta < MAX_ETA) 
//     return -2; // negative encap
//   return 0;
// }

double eta(double px, double py, double pz) {
  double theta = atan( sqrt(px*px+py*py)/pz );
  if (theta<0) theta += 3.1415;
  
//   std::cout << "px=" << px << " py=" << py << " pz=" << pz 
// 	    << " theta=" << theta << " eta=" << - log( tan (theta/2) ) << "\n";

  return - log( tan (theta/2) );
}
// double eta(double pt, double pz) {
//   double theta = atan(pt/pz);
//   return - log( tan (theta/2) );
// }
double eta(FTKRawHit &hit) {
  // eta of hit position
  return eta(hit.getX(), hit.getY(), hit.getZ());
}

bool isSplitCluster(const cluster& clu)
{
    //cluster_hit seed = cluster_hit(*clu.seed, clu.seed);
    hitVector::const_iterator it = clu.hitlist.begin(); 
    for (it = clu.hitlist.begin(); it != clu.hitlist.end(); it++) {
        cluster_hit chit = cluster_hit(**it , clu.seed);
        if ((chit.ccol >= GRID_COL_MAX - 1) ||  (chit.crow >= (GRID_ROW_MAX - 1)) || ( chit.crow <= 0))
            return true;
    }

    return false;
}


#ifdef CLUSTERING_PRINTOUT

bool clusterSort (const cluster_hit &i, const cluster_hit &j) 
{ 
    // reverse the hits on the first row if they are above the seed  
    if (i.ccol == 0 && j.ccol == 0 && i.crow <= 10 && j.crow <= 10)
        return j.crow <= i.crow;
    else if (i.ccol == j.ccol)
        return i.crow <= j.crow;
    else 
        return i.ccol <= j.ccol;
}

void printModId(const FTKRawHit* h) 
{
    int modid = 0;
    if (h->getLayer() == 1) {
        if (h->getPhiModule() == 13 && h->getEtaModule() == 0) modid = 72; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -1) modid = 70; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -2) modid = 62; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -3) modid = 60; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -4) modid = 10; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -5) modid = 02; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -5) modid = 00; 
    }   
    else if (h->getLayer() == 2) {

    }   
    else if (h->getLayer() == 3) {
        if (h->getPhiModule() == 11 && h->getEtaModule() == 0) modid = 72; 
        else if (h->getPhiModule() == 11 && h->getEtaModule() == -1) modid = 71; 
        else if (h->getPhiModule() == 11 && h->getEtaModule() == -2) modid = 70; 
        else if (h->getPhiModule() == 11 && h->getEtaModule() == -3) modid = 63; 
        else if (h->getPhiModule() == 11 && h->getEtaModule() == -4) modid = 62; 
        else if (h->getPhiModule() == 11 && h->getEtaModule() == -5) modid = 61; 
        else if (h->getPhiModule() == 11 && h->getEtaModule() == -6) modid = 60; 

        else if (h->getPhiModule() == 12 && h->getEtaModule() == -1) modid = 41; 
        else if (h->getPhiModule() == 12 && h->getEtaModule() == -2) modid = 42; 
        else if (h->getPhiModule() == 12 && h->getEtaModule() == -3) modid = 43; 
        else if (h->getPhiModule() == 12 && h->getEtaModule() == -4) modid = 50; 
        else if (h->getPhiModule() == 12 && h->getEtaModule() == -5) modid = 51; 
        else if (h->getPhiModule() == 12 && h->getEtaModule() == -6) modid = 52; 

        else if (h->getPhiModule() == 13 && h->getEtaModule() == -0) modid = 32; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -1) modid = 31; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -2) modid = 30; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -3) modid = 23; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -4) modid = 22; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -5) modid = 21; 
        else if (h->getPhiModule() == 13 && h->getEtaModule() == -6) modid = 20; 

        else if (h->getPhiModule() == 14 && h->getEtaModule() == -1) modid = 01; 
        else if (h->getPhiModule() == 14 && h->getEtaModule() == -2) modid = 02; 
        else if (h->getPhiModule() == 14 && h->getEtaModule() == -3) modid = 03; 
        else if (h->getPhiModule() == 14 && h->getEtaModule() == -4) modid = 10; 
        else if (h->getPhiModule() == 14 && h->getEtaModule() == -5) modid = 11; 
        else if (h->getPhiModule() == 14 && h->getEtaModule() == -6) modid = 12; 
    }   

    printf("000800000%02d\n", modid);
}

void printClu(const cluster &clu) 
{
    hitVector hs = clu.hitlist;
    hitVector::iterator hit = hs.begin();

    vector<cluster_hit> tmp;
    for (hit = hs.begin(); hit != hs.end(); hit++)
        tmp.push_back(cluster_hit(**hit, clu.seed));

    //sort and print the vector.
    if (tmp.size() > 1) std::stable_sort(tmp.begin(), tmp.end(), clusterSort);

    for(vector<cluster_hit>::iterator hit1 = tmp.begin(); hit1 != tmp.end(); hit1++)
        printf("0000%.2X%.2X%.3X\n",(*hit1).tot, (*hit1).ccol, (*hit1).crow);

    int boundary = 2;
    if (isSplitCluster(clu)) 
        boundary = 3;

    if (clu.seed->getEtaStrip() %2 != 0) 
        printf("000%d00%.2X%.3X\n",boundary, clu.seed->getEtaStrip() - 1, clu.seed->getPhiSide());
    else 
        printf("000%d00%.2X%.3X\n",boundary, clu.seed->getEtaStrip(), clu.seed->getPhiSide());
}

void printClusterList(clustersByModuleMap clustersByModule) 
{
    clustersByModuleMap::iterator p;
    for (p = clustersByModule.begin(); p!=clustersByModule.end(); ++p) { // loop over modules
        cluList* cl = (*p).second;
        cluList::iterator b = cl->begin();
        FTKRawHit* hit = (*b).hitlist[0];
        if (hitSelector(*hit)) {
            printModId(hit);
            for (; b != cl->end(); b++) {
                printClu(*b);
            }
            printf("00040000000\n");
        }
    }
}
#endif

#ifdef BOUNDING_BOX
void calcBoundingBox(cluster& clu) {
    int col_min = (clu.hitlist[0])->getEtaStrip();
    int col_max = (clu.hitlist[0])->getEtaStrip();
    int row_min = (clu.hitlist[0])->getPhiSide();
    int row_max = (clu.hitlist[0])->getPhiSide();

    hitVector::iterator hit;
    for (hit = clu.hitlist.begin(); hit != clu.hitlist.end(); hit++) {

        if ((*hit)->getEtaStrip() < col_min) col_min = (*hit)->getEtaStrip();
        else if ((*hit)->getEtaStrip() > col_max) col_max = (*hit)->getEtaStrip();
       
        if ((*hit)->getPhiSide() < row_min) row_min = (*hit)->getPhiSide();
        else if ((*hit)->getPhiSide() > row_max) row_max = (*hit)->getPhiSide();
    }
    std::cout <<  "BOUNDING_BOX" << col_max - col_min + 1 << " " << row_max - row_min + 1<< std::endl;
}
#endif



void printHit(const FTKRawHit &hit) {
  std::cout << "DEBUG_HITS: "
	    << "  isPixel=" << hit.getIsPixel() 
	    << "  barrel_ec=" << hit.getBarrelEC()
	    << "  layer_disk=" << hit.getLayer()
	    << "  phi_module=" << hit.getPhiModule()
	    << "  eta_module=" << hit.getEtaModule()
	    << "  pi_side=" << hit.getPhiSide()
	    << "  ei_strip=" << hit.getEtaStrip()
	    << "  n_strips=" << hit.getNStrips()
	    << "  deltaPhi=" << hit.getDeltaPhi()
	    << "  deltaEta=" << hit.getDeltaEta()
	    << "  ( x=" << hit.getX()
	    << " y=" << hit.getY()
	    << " z=" << hit.getZ()
	    << " eventindex=" << hit.getEventIndex()
	    << " barcode=" << hit.getBarcode()
	    << " bar_pt=" << hit.getBarcodePt()
	    << " parentage_mask=" << hit.getParentageMask()
	    << " )\n";
}

int hitToModuleId(const FTKRawHit &hit) {
  // returns moduleId of given hit

  if (DEBUG_HITS)
    printHit(hit);

  unsigned int ieta = 0;
  if (FTKSetup::getFTKSetup().getIBLMode()==1){
    assert(hit.getEtaModule()>=-8 && hit.getEtaModule()<=8); // cy for ibl
  }
  else{
    assert(hit.getEtaModule()>=-6 && hit.getEtaModule()<=6); 
  }

  if (FTKSetup::getFTKSetup().getIBLMode()==1 && hit.getBarrelEC() == ftk::BARREL && hit.getLayer() == 0 && hit.getIsPixel() == 1) { // for ibl
    // eta index should range -8 to 8 for ibl
    ieta = 8 + hit.getEtaModule(); // cy was 6
  }  else if (hit.getBarrelEC() == BARREL){
    // eta index should range -6 to 6 for barrel
    ieta = 6 + hit.getEtaModule(); // cy was 6
  }  else {
    // eta index should range 0 to 2 for endcaps (just 0 for pixels)
    assert(hit.getEtaModule()>=0 && hit.getEtaModule()<3);
    // removed dependency on getIBLMode because it is not needed
    if (hit.getBarrelEC() == POSEC)
      ieta = MAX_BARREL_ETA+hit.getEtaModule();
    if (hit.getBarrelEC() == NEGEC)
      ieta = MAX_BARREL_ETA+3+hit.getEtaModule();
  }

//   if (FTKSetup::getFTKSetup().getIBLMode()==1){
//     assert(ieta<MAX_ETA+3); // additions for ibl the first +3 is for ibl (13->16), and the +2 is for ibl (6->8), remove the +2 for 11L ibl 
//   }  
//   else {
    assert(ieta<MAX_ETA);
    //  }
  
  unsigned int iphi = hit.getPhiModule();
  assert(iphi<MAX_PHI);

  unsigned int ilayer = 0;
  switch (hit.getHitType()) {
  case PIXEL:
    ilayer = hit.getLayer();
    if (FTKSetup::getFTKSetup().getIBLMode()==1) {
      assert(ilayer<MAX_LAYER_PIXEL+1); // +1 for ibl layer as pixel
      break;
    }
    else {
      assert(ilayer<MAX_LAYER_PIXEL);
      break;
    }
    break;
  case SCT:
    // for endcaps hit.layer_disk goes from 0 to 17
    //printHit(hit);
    ilayer = 10 + hit.getLayer();
    assert(ilayer>=10 && ilayer<MAX_LAYER);
    break;
  default:
    assert(0);
  }
  assert(ilayer<MOD_ID_LAYER_MASK);
  assert(iphi<MOD_ID_PHI_MASK);
  assert(ieta<MOD_ID_ETA_MASK);

  unsigned int ModuleId = ilayer*MOD_ID_LAYER_VAL + ieta*MOD_ID_ETA_VAL + iphi*MOD_ID_PHI_VAL;

  if (DEBUG_HITS)
    std::cout << "  ModuleId=" << ModuleId
	      << "  ilayer=" << ilayer 
	      << "  ieta=" << ieta 
	      << "  iphi=" << iphi
	      << "\n";

  return ModuleId;
}

bool neighborhood(const FTKRawHit &hit1, const FTKRawHit &hit2) {
  /*
   * Check (and define) if two hits are neighborhood
   */

  /*
   * For the time being it accepts only pair of hits from the same module and same SCT side.
   * It crash if hits are the same.
   */

  // 1st check same module
  if (hit1.getBarrelEC() != hit2.getBarrelEC() || hit1.getHitType() != hit2.getHitType() ||
      hit1.getLayer() != hit2.getLayer() || hit1.getEtaModule() != hit2.getEtaModule() ||
      hit1.getPhiModule() != hit2.getPhiModule()) { // if module is NOT the same
    //printHit(hit1);
    //printHit(hit2);
    assert(0); // temporary check
    return false;
  }
  
  switch (hit1.getHitType()) {
  case SCT:
    if (!SCT_CLUSTERING)
      return false; 

    if (hit1.getPhiSide() != hit2.getPhiSide()) { // if different side
       assert(0); // temporary check
       return false;
    }
    assert(hit1.getEtaStrip() != hit2.getEtaStrip()); // sanity check
    //    assert(hit1.getNStrips()==1 && hit2.getNStrips()==1); // should be false some times... just a test
    //    if (hit1.getEtaStrip() == hit2.getEtaStrip()+1 || hit1.getEtaStrip()+1 == hit2.getEtaStrip())
    //      return true;
    if (hit1.getEtaStrip() == hit2.getEtaStrip()+hit2.getNStrips() 
	|| hit1.getEtaStrip()+hit1.getNStrips() == hit2.getEtaStrip())
      return true;
    return false;

  case PIXEL:
    assert(hit1.getEtaStrip() != hit2.getEtaStrip() || hit1.getPhiSide() != hit2.getPhiSide()); // sanity check
    /* need a common edge, i.e. contiguity along diagonal is not enough (see below) */
    if ( (hit1.getEtaStrip() == hit2.getEtaStrip()+1 || hit1.getEtaStrip()+1 == hit2.getEtaStrip())
	 && hit1.getPhiSide() == hit2.getPhiSide()) return true;
    if ( (hit1.getPhiSide() == hit2.getPhiSide()+1 || hit1.getPhiSide()+1 == hit2.getPhiSide())
	 && hit1.getEtaStrip() == hit2.getEtaStrip()) return true;

    
    if (DIAG_CLUSTERING) { /* Accept contiguity along diagonal as well */
      if ( (hit1.getEtaStrip() == hit2.getEtaStrip()+1 || hit1.getEtaStrip()+1 == hit2.getEtaStrip())
	   && (hit1.getPhiSide() == hit2.getPhiSide()+1 || hit1.getPhiSide()+1 == hit2.getPhiSide()) )
	return true;
    }

    return false;
  }
   assert(0); // should not get here!
   return false;
}

int buildUpCluster(hitVector *currentHits, cluster &clu) {
  /*
   * Recursively adds hits to a given cluster. 
   * Returns number of hits in final cluster.
   * If no cluster is given (or empty cluster) make a cluster out of the 1st hit.
   */
  assert(currentHits->size()>0); // need at least one hit!

  hitVector::iterator hitP;
  if (clu.hitlist.size()==0) {     // if cluster is empty
    hitP = currentHits->begin();
    clu.hitlist.push_back(*hitP);
    currentHits->erase(hitP);

    // FlagAK - dirty fix to disable clustering on 1st layer
    //    if( (*hitP).getLayer()==0 && (*hitP).getIsPixel() ) {
    //      return clu->hitlist.size();
    //    }
    
  } 

  /* Now we have a non empty cluster */
  bool newHitsAdded = false;
  int distanceis = clu.hitlist.size();
  hitVector::iterator hitCluP; // hits in cluster
  //for (hitCluP=clu.hitlist.begin(); hitCluP!=clu.hitlist.end(); ++hitCluP) { // loop over hits in cluster
  for(int i = 0; i < distanceis; i++) {
      FTKRawHit* hit = clu.hitlist.at(i); 
      for (hitP=currentHits->begin(); hitP!=currentHits->end();) { // loop over hits in module
          //if (neighborhood(**hitCluP, **hitP)) { // if hits are neighborhood
          if (neighborhood(*hit, **hitP)) { // if hits are neighborhood
              // add hit to cluster
              clu.hitlist.push_back(*hitP);
              //hitCluP = clu.hitlist.begin();
              currentHits->erase(hitP);
              distanceis++;
              hitP=currentHits->begin(); // if a hit is erased then restart the loop
              newHitsAdded = true;
          } else { 
              ++hitP; // if hit is not a neighborhood then go to next hit
          }
      } // end of loop over hits in module 
  } // end of loop over hits in cluster

  // if at least one hit was added, check for more neighborhood hits
  if (newHitsAdded && currentHits->size()) buildUpCluster(currentHits, clu);
  return clu.hitlist.size();
}

void makeClustersLNF(hitVector *currentHits, cluList *currentClusters) {
    /*
     * Group all hits from one module into clusters
     */

    int index=0;
    while (currentHits->size()) { // as long as we have hits
        cluster clu;
        clu.isSafe = false;               // initialitaion
        clu.isKilled = false;             // initialitaion
        int cluSize = buildUpCluster(currentHits, clu);
        currentClusters->push_back(clu);
        if (0)
            std::cout << "DEBUG_makeCluster:"
                << "  index=" << index
                << "  cluSize=" << cluSize
                << "  Nclusters=" << currentClusters->size()
                << "\n";
        index++;
    }
}

void makeClusterFromSeed(hitVector *currentHits, cluList *currentClusters, FTKRawHit* &seed) {
    //erase seed fom currentHits
    hitVector::iterator position = std::find(currentHits->begin(), currentHits->end(), seed);
    if (position != currentHits->end()) {
        currentHits->erase(position);
    }

    cluster clu;
    clu.seed = seed;
    clu.hitlist.push_back(seed);
    clu.isSafe = false;
    clu.isKilled = false;
    if (currentHits->size() > 0) buildUpCluster(currentHits, clu);
    currentClusters->push_back(clu);

#ifdef BOUNDING_BOX
    calcBoundingBox(clu);
#endif
}
//
bool gangedHitHasNeighborhood(const FTKRawHit &hit, const cluster &clu, hitVector &connectedHits) {
    bool hasNeighborhood = false;
    int phi = hitIsGanged(hit);
    //  std::cout << "phi: " << phi << std::endl;
    if (phi) { // check hit is ganged
        int eta = hit.getEtaStrip();
        hitVector hv = clu.hitlist;
        hitVector::iterator pp;
        for (pp=hv.begin(); pp!=hv.end(); ++pp) {
            if ( eta != (*pp)->getEtaStrip() ) continue;
            if ( abs(phi - (*pp)->getPhiSide() ) != 1 ) continue;
            hasNeighborhood = true;
            FTKRawHit *tmpHit = new FTKRawHit();
            tmpHit->setHitType( ftk::PIXEL );
            tmpHit->setEtaStrip( eta );
            tmpHit->setLayer( hit.getLayer() );
            tmpHit->setPhiSide( gangedPartner(hit) );
            if ( !hitIsGanged(*tmpHit) ) {
                printHit(*tmpHit);
                assert(0);
            }
            connectedHits.push_back( tmpHit ); // store connected hit pointer
        } // end of nested loop over hits
    } //end of "if (phi)"
    return hasNeighborhood;
}

bool findConnectedGanged(cluster &clu, hitVector &connectedHits) {
    assert(clu.hitlist.size()>0); // sanity check

    hitVector::iterator p; 
    bool hasNeighborhood = false;
    for (p=clu.hitlist.begin(); p!=clu.hitlist.end(); ++p) {
        if ( gangedHitHasNeighborhood(**p, clu, connectedHits) ) {
            hasNeighborhood = true;
        }
    } // end of first loop over hits

    return hasNeighborhood;
}

bool cluIsGanged(const cluster &clu) {
    hitVector hv = clu.hitlist;
    hitVector::iterator p; 
    for (p=hv.begin(); p!=hv.end(); ++p) {
        if ( hitIsGanged(**p) ) return true;
    } 

    return false;
}

bool isKilled(const cluster &clu, const hitVector &connectedHits) {
    hitVector tmphv = clu.hitlist;
    hitVector::iterator p; 
    for (p=tmphv.begin(); p!=tmphv.end(); ++p) {
        if ( !hitIsGanged(**p) ) continue;
        hitVector hv = connectedHits;
        hitVector::iterator c;
        for (c=(hv).begin(); c!=(hv).end(); ++c) {
            assert( hitIsGanged(**c) ); // all connected hits should be ganged !
            if ( (*c)->getPhiSide() == (*p)->getPhiSide() 
                    && (*c)->getEtaStrip() == (*p)->getEtaStrip() ) return true;
        }
    } // loop over hits in cluster

    return false;
}

void averageCluster(cluster &clu) {
    const unsigned int &nHits = clu.hitlist.size();
    assert(nHits>0); // sanity check

    FTKRawHit &av = clu.clusterEquiv; // get pointer to clusterEquivalent
    FTKRawHit *first = *(clu.hitlist.begin()); // get 1st hit
    // reset values for clusterEquivalent (alias av)
    av.setX(0);
    av.setY(0);
    av.setZ(0);  
    av.setIdentifierHash(first->getIdentifierHash());
    av.setHitType(first->getHitType());
    av.setBarrelEC(first->getBarrelEC());
    av.setLayer(first->getLayer());
    av.setPhiModule(first->getPhiModule());
    av.setEtaModule(first->getEtaModule());
    av.setEtaStrip(0);
    av.setNStrips(0);
    av.setEtaWidth(0);
    av.setPhiWidth(0);
    av.setIncludesGanged(false);

    if (DEBUG_AVERAGE) 
        std::cout << "DEBUG_AVERAGE:  isPixel=" << av.getIsPixel() 
            << ", nHits=" << nHits<< "\n";

    hitVector::iterator p;
    int tmp;
    int etaMax = -1;
    int etaMin = 4000;
    int phiMax = -1;
    int phiMin = 4000;
    // precalculate values for case PIXEL_CLUSTERING_MODE>0
    int BarrelEndCap = first->getBarrelEC();
    int layer = first->getLayer();
    bool isIBLmodule = FTKSetup::getFTKSetup().getIBLMode()!=0 && layer==0;
    float sensorThickness = ftk::sensorThicknessPixel;
    float etaPitch = ftk::etaPitchPixel;
    float numberOfEtaPixelsInModule = ftk::numberOfEtaPixelsInPixelModule;
    float pixelModuleActiveLength = ftk::lengthOfPixelModuleIn400umPixels*ftk::etaPitchPixel/ftk::micrometer;
    float pixYScaleFactor = 16.; //multiply by 16 to count in unit of 25um 
    float pixXScaleFactor = 10.; //count r-phi coordinate in units of 5um
    float etaModule = first->getEtaModule()-6; 
    const float pixelEndCapRPhiCorrection = 25.4*ftk::micrometer/ftk::phiPitch; // Lorentz angle?
    const float pixelIblRPhiCorrection = 7*ftk::micrometer/ftk::phiPitch; // Lorentz angle?
    // FlagAA 2013-07-31: IBL code assumes fully planar geometry as in mc12+IBL
    // This will change for the real detector!!!
    if (isIBLmodule) {
        etaModule = first->getEtaModule()-8; 
        pixYScaleFactor = 10.; //multiply by 10 to count in unit of 25um 
        etaPitch = ftk::etaPitchIbl;
        //    float sensorThickness = 230*micrometer; // 3D sensors
        //    pixelModuleActiveLength = 80*0.25;  // 3D sesors ???
        //    numberOfEtaPixelsInModule = 80; // 3D sesors ???
        sensorThickness = ftk::sensorThicknessIbl;
        pixelModuleActiveLength = ftk::lengthOfIblModuleIn250umPixels*ftk::etaPitchIbl/ftk::micrometer; // planar sensors
        numberOfEtaPixelsInModule = ftk::numberOfEtaPixelsInIblModule; 
    }
    if (FTKSetup::getFTKSetup().getIBLMode()==0)
        layer++; // always count IBL as layer0 ad BLayer as layer1
    float radius = 35.78; // IBL (from fig5 in IBL TDR)
    if (layer==1)
        radius = 50.5; // Layer 0 (B-Layer)
    if (layer==2)
        radius = 88.5; // Layer 1
    if (layer==3)
        radius = 122.5; // Layer 2                             
    bool hasGanged=false;
    int rowMin = 99999; //int(2*(design->width()/design->phiPitch()))+1;;
    int rowMax = 0;
    int colMin = 99999; //int(2*(design->length()/design->etaPitch()))+1;
    int colMax = 0;
    int qRowMin = 0;  int qRowMax = 0;
    int qColMin = 0;  int qColMax = 0;

    switch (av.getHitType()) {
        case SCT: {
            int firstStrip =  99999;
            int lastStrip  = -99999;
            av.setPhiSide(first->getPhiSide());
            for (p=clu.hitlist.begin(); p!=clu.hitlist.end(); ++p) {
                av.addX((*p)->getX());
                av.addY((*p)->getY());
                av.addZ((*p)->getZ());
                if ((*p)->getEtaStrip() < firstStrip)
                    firstStrip = (*p)->getEtaStrip();
                if ((*p)->getEtaStrip()+(*p)->getNStrips()-1 > lastStrip)
                    lastStrip = (*p)->getEtaStrip() + (*p)->getNStrips() - 1;
                av.addEtaStrip((*p)->getEtaStrip());
                if (DEBUG_AVERAGE_SCT) // && clu.hitlist.size()>1)
                    printHit(**p);
            }
            float stripNumberFloat = (firstStrip*1.+lastStrip)/2;
            //(av.getEtaStrip()*1.)/nHits;
            av.setEtaStrip( (int) stripNumberFloat );
            av.setDeltaPhi( stripNumberFloat - (int) stripNumberFloat ); 
	    // The convention used for SCT is that we are providing as output
	    // the cluster center minus half a strip (i.e. use the left edge of strip for a single strip cluster) 
	    // where output = av.setEtaStrip + av.setDeltaPhi
	    av.setRowStripCoordinate(firstStrip+lastStrip);
	    av.setColumnCoordinate( 0 ); 
            av.setDeltaEta(0);
            av.setNStrips(lastStrip+1-firstStrip); 
            av.setEtaWidth(1);
            av.setPhiWidth(lastStrip+1-firstStrip);
            if (DEBUG_AVERAGE_SCT) { // && clu.hitlist.size()>1) {
                std::cout << "SCT AVERAGE: ";
                printHit(av);
                std::cout << std::endl;
            }
            break; // end of SCT
        }
        case PIXEL: 
            av.setPhiSide(0); // eta is reset a few lines above 
            for (p=clu.hitlist.begin(); p!=clu.hitlist.end(); ++p) { //loop over hits in cluster
                assert(av.getLayer()==(*p)->getLayer() && av.getPhiModule()==(*p)->getPhiModule() && av.getEtaModule()==(*p)->getEtaModule() );
                //      if (hitIsGanged(**p))
                //	av.setIncludesGanged(true);

            if (SAVE_CLUSTER_CONTENT) { // if enabled the cluster also stores also the single channels
                FTKRawHit tmpch = *(*p);
                // set the barcode of the single channel, this may allow a very refined debugging of the cluster content accounting for the single barcode of each channel
                MultiTruth mt;
                MultiTruth::Barcode uniquecode(tmpch.getEventIndex(),tmpch.getBarcode());
                mt.maximize(uniquecode,tmpch.getBarcodePt());
                tmpch.setTruth(mt);	  
                av.addChannel(tmpch);
            }

            av.addX((*p)->getX());
            av.addY((*p)->getY());
            av.addZ((*p)->getZ());

            int row = (*p)->getPhiSide();
            int col = (*p)->getEtaStrip();
            int tot = (*p)->getTot(); // ToT for pixels 
            if (!isIBLmodule && pixelRowIsGanged(row))
                hasGanged = true;

            if (PIXEL_CLUSTERING_MODE>0) {
                // calculate cluster center following code at line 701 of 
                // https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetRecTools/SiClusterizationTool/trunk/src/MergedPixelsTool.cxx#L701
                // I.e. m_posStrategy == 1 
                // I.e. use charge imbalance between the two outer most ros (columns) to calculate the centroid
                // In this code I'll use etaTrack instead of pixel eta for simplicity

                if (!isIBLmodule) {   
                    // account for 600um pixels in the centroid calculation
                    // will use units of 100um and the convert to normal pixel units below
                    // will also indicate the center of the pixel instead of the left edge
                    int FEnumber = col/18;
                    int FEcolumn = col%18;
                    // considering center of pixel along eta (left (or top if you prefer) edge along phi)
                    // each FE number is worth 19*400um pixels (400um*16 + 600um*2)
                    // multiply FE column by 4 to convert to 100um units
                    // add 0.5 to account for the extra 200um of the pixel with FEcolumn==0
                    // add 0.5 to indicate center of pixel
                    col = FEnumber*19 + FEcolumn;
                    col *= pixYScaleFactor;
                    col += pixYScaleFactor/2; // add half a pixel to align to pixel center (assume normal 400um pixel)
                    col += pixYScaleFactor/2; // assume 1<=FEcolumn<=16 add half a pixel coming from 600um pixel in FEcolumn==0
                    if (FEcolumn==0) col -= pixYScaleFactor/4; // correct position for first column in FE chip
                    if (FEcolumn==17) col += pixYScaleFactor/4; // correct position for last column in FE chip
                } else { // IBL case
                    int orig_col = col;
                    col = col*pixYScaleFactor; // use units of 25um
                    col += pixYScaleFactor/2; // add half a pixel to align to pixel center
                    if (orig_col==0) col += pixYScaleFactor/2; // add half pixel (500um pixel in col0)
                    if (orig_col>0) col += pixYScaleFactor; // add a pixel (500um pixel in col0)
                    if (orig_col==79) col += pixYScaleFactor*4/10; // add 4/10 of pixel i.e. 100um (450um pixel in col79)
                    if (orig_col==80) col += pixYScaleFactor*12/10; // add 12/10 of pixel i.e. 300um (450um pixel in col79 and col80)
                    if (orig_col>80) col += pixYScaleFactor*16/10; // add 16/10 of pixel i.e. 400um (450um pixel in col79 and col80)
                    if (orig_col==159) col += pixYScaleFactor/2; // add half pixel (500um pixel in col159)
                }

                if (row == rowMin) qRowMin += tot;
                if (row < rowMin){
                    rowMin = row;
                    qRowMin = tot;
                }

                if (row == rowMax) qRowMax += tot;
                if (row > rowMax){
                    rowMax = row;
                    qRowMax = tot;
                }

                if (col == colMin) qColMin += tot;
                if (col < colMin){
                    colMin = col;
                    qColMin = tot;
                }

                if (col == colMax) qColMax += tot;
                if (col > colMax){
                    colMax = col;
                    qColMax = tot;
                }
            } else { // PIXEL_CLUSTERING_MODE == 0
                av.addPhiSide((*p)->getPhiSide());   // phi index
                // pixels are counted starting from 0 (left edge) and not 0.5 (center position)
                // if only pixel 0 is hit the output value will be 0 and not 0.5 (pixel center)
                av.addEtaStrip((*p)->getEtaStrip()); // eta index
            }
            if ((*p)->getEtaStrip() > etaMax) etaMax = (*p)->getEtaStrip();
            if ((*p)->getEtaStrip() < etaMin) etaMin = (*p)->getEtaStrip();
            if ((*p)->getPhiSide() > phiMax) phiMax = (*p)->getPhiSide();
            if ((*p)->getPhiSide() < phiMin) phiMin = (*p)->getPhiSide();
            if (DEBUG_AVERAGE)
                printHit(**p);
            av.addTot(first->getTot()); // sum ToT for pixel clusters
            } 
            av.setEtaWidth(etaMax-etaMin+1); 
            av.setPhiWidth(phiMax-phiMin+1);

            // calculate eta index and eta delta 
            double eta_average, phi_average, delta;
            if (PIXEL_CLUSTERING_MODE>0) {
                float pixelEstimateCotTheta = -9999.;
                pixelEstimateCotTheta = (etaModule+(rowMin+rowMax)/2./pixYScaleFactor/numberOfEtaPixelsInModule-0.5) * pixelModuleActiveLength / radius;
                if (PIXEL_CLUSTERING_MODE>=PIXEL_CLUSTERING_MODE_APRIL2014_FIX)
                    pixelEstimateCotTheta = (etaModule+(colMin+colMax)/2./pixYScaleFactor/numberOfEtaPixelsInModule-0.5) * pixelModuleActiveLength / radius;
                // Compute eta for charge interpolation correction (if required)
                // Two pixels may have tot=0 (very rarely, hopefully)
                float etaRow = -1;
                float etaCol = -1;
                if(qRowMin+qRowMax > 0) etaRow = qRowMax/float(qRowMin+qRowMax);
                if(qColMin+qColMax > 0) etaCol = qColMax/float(qColMin+qColMax);

                // Charge interpolation. Very rough guess (one can do better with
                // candidate track information later) TL
                //      bool hasGanged = av.getIncludesGanged();
                int m_posStrategy = 1;
                if(m_posStrategy == 1 && !hasGanged && etaRow>0 && etaCol > 0){
                    // width of the region of charge sharing
                    // For disks assume normal incidence: delta is small, due to diffusion
                    // of drifting charges in silicon
                    // For barrel, assume 10 deg. incidence in Rphi, in z compute from
                    // pseudorapidity
                    // this may be improved with better parameterization, but it is
                    // probably better to use candidate track information later in
                    // reconstruction. TL
                    // Values are made dependent on the sensor thickness to accomodate
                    // different sensors layout. AA
                    float deltax = 0;
                    float deltay = 0;
                    //    Point3D<double> globalPos = element->globalPosition(centroid);
                    //    InDetDD::SiLocalPosition totCorrection(0,0,0);
                    if (BarrelEndCap==0) { //pixelID.is_barrel(elementID)){
                        deltax = 30*ftk::micrometer*(sensorThickness/(250*ftk::micrometer));
                        //      deltay = sensorThickness*fabs(globalPos.z())/globalPos.r();
                        deltay = sensorThickness*fabs(pixelEstimateCotTheta);
                        if(deltay > etaPitch ) deltay = etaPitch;
                    } else {
                        deltax = 10*ftk::micrometer*sqrt(sensorThickness/(250*ftk::micrometer));
                        deltay = 10*ftk::micrometer*sqrt(sensorThickness/(250*ftk::micrometer));
                    }
                    // calculate centroid expressed in pixel units
                    phi_average = (rowMin+rowMax)/2. + deltax*(etaRow-0.5)/ftk::phiPitch;
                    eta_average = (colMin+colMax)/2. + pixYScaleFactor*deltay*(etaCol-0.5)/etaPitch;
                    //    std::cout << "CENTER: rowMin=" << rowMin << ", rowMax=" << rowMax << ", deltax=" << deltax << ", etaRow=" << etaRow << ", phiPixel=" << phiPixel << std::endl;
                    //    std::cout << "CENTER: colMin=" << colMin << ", colMax=" << colMax << ", deltay=" << deltay << ", etaCol=" << etaCol << ", etaPixel=" << etaPixel << ", trackCotTheta=" << trackCotTheta << std::endl;
                    } else { // if ganged or no imbalance (etaRow, etaCol) available use center of enclosing box 
                        eta_average = (colMin + colMax) / 2.;
                        phi_average = (rowMin + rowMax) / 2.;
                    }
                    if (!isIBLmodule) {
                        // rescale full module length 152*400um to the range 0-144
                        // here 1 units is 400*19/18um (i.e. average 400/600um pixel length)
                        eta_average*=ftk::numberOfEtaPixelsInPixelModule/ftk::lengthOfPixelModuleIn400umPixels/pixYScaleFactor;
                    } else { // IBL case
                        // rescale from 25um units to 250um (1 "normal" IBL pixel) unit
                        eta_average*=ftk::numberOfEtaPixelsInIblModule/ftk::lengthOfIblModuleIn250umPixels/pixYScaleFactor;
                    }
                    if (phi_average<0) {
                        assert(phi_average<0);
                        phi_average = 0;
                    }
                    if (eta_average<0) {
                        assert(eta_average<0);
                        eta_average = 0;
                    }
                    delta = eta_average - (int) eta_average;
                    av.setDeltaEta(delta);
                    av.setEtaStrip( (int) eta_average );
                    if (BarrelEndCap!=0)
                        phi_average += pixelEndCapRPhiCorrection;
                    if (isIBLmodule)
                        phi_average += pixelIblRPhiCorrection;
                    delta = phi_average - (int) phi_average;
                    av.setDeltaPhi(delta);
                    av.setPhiSide( (int) phi_average );
		    // Set the integer variables that will contain the same information produced by FTK_IM board
		    av.setRowStripCoordinate( lround(phi_average*pixXScaleFactor) );
		    av.setColumnCoordinate( lround(eta_average*pixYScaleFactor) );
                } else { // PIXEL_CLUSTERING_MODE == 0
                    tmp = (int)round((av.getEtaStrip()*1.)/nHits);
                    av.setDeltaEta((av.getEtaStrip()*1.)/nHits-tmp);
                    av.setEtaStrip(tmp);
                    tmp = (int)round((av.getPhiSide()*1.)/nHits);
                    av.setDeltaPhi((av.getPhiSide()*1.)/nHits-tmp);
                    av.setPhiSide(tmp);
                }
                break; // end of PIXEL
                default:
                assert(0); // should not get here!
            } // end of switch

            // finally divide by nHits
            av.divX(nHits);
            av.divY(nHits);
            av.divZ(nHits);

            // AB - perform geant parent matching and store with the cluster
            // data.  this involves passing all raw hits associated with the
            // cluster to a MultiTruth which will be stored with the hit
            // when it is converted to an FTKHit.
            MultiTruth mt;
            for( hitVector::iterator ihit=clu.hitlist.begin(), fhit=clu.hitlist.end(); ihit!=fhit; ++ihit ) {
                // record highest pt contribution to the combination (cluster)
                if( (*ihit)->getTruth() ) {
                    mt.maximize( *((*ihit)->getTruth()) );
                } else {
                    MultiTruth::Barcode uniquecode((*ihit)->getEventIndex(),
                            (*ihit)->getBarcode());
                    mt.maximize(uniquecode,(*ihit)->getBarcodePt());
                }
            } // end record truth for each raw channel in the cluster
            clu.clusterEquiv.setTruth(mt);

            if (DEBUG_AVERAGE) {
                std::cout << "    AVERAGE IS:\n";
                printHit(av);
                std::cout << "\n\n";
            }
}

void atlClusteringBlayer(vector<FTKRawHit> &hits) {
    /*
     * Split blayer modules in half along phi
     * this effectively makes 22*2=44 modules along phi!
     */
    if(!SPLIT_BLAYER_MODULES) return;
    for(unsigned int i = 0; i < hits.size(); i++) {
#define HIT hits[i]
        // AK BLAYER : change # modules in barrel blayer from 22 to 44:
        if(HIT.getIsPixel() && HIT.getBarrelEC()==ftk::BARREL && HIT.getLayer()==0) {
            HIT.setPhiModule(HIT.getPhiModule()*2);
            // mid-module sector division: assume SS=20 or 10.
            // Then the division is right at SS boundary
            if(HIT.getPhiSide()>159) HIT.setPhiModule(HIT.getPhiModule()+1); // HARDCODED (328/2 rounded to SS=16)
        }
#undef HIT
    }
}

void printDecoderOutput(hitVector* currentHits)
{
    hitVector::iterator hit;
    for(hit = currentHits->begin(); hit!= currentHits->end(); hit++) {
        std::cout << (*hit)->getTot() << " " << (*hit)->getEtaStrip() << " " << (*hit)->getPhiSide() << std::endl;
    }
}

void realisticDecoder(hitVector* &currentHits)
{
    hitVector::iterator hit;

    if (currentHits->size() > 1) 
        std::stable_sort(currentHits->begin(), currentHits->end(), sortbyFE);

    std::stack<FTKRawHit*> lifo;
    std::queue<FTKRawHit*> fifo;
    for(hit = currentHits->begin(); hit != currentHits->end(); hit++) {
        if ((*hit)->getPhiSide() <= 163) 
            fifo.push(*hit);
        else lifo.push(*hit); 
    }

    currentHits->clear();

    while(!lifo.empty() && !fifo.empty()){
        if ((*lifo.top()).getEtaStrip() <= (*fifo.front()).getEtaStrip()) {
            currentHits->push_back(lifo.top());
            lifo.pop();
        }
        else {
            currentHits->push_back(fifo.front());
            fifo.pop();
        }
    }

    while(!lifo.empty()) {
        currentHits->push_back(lifo.top());
        lifo.pop();
    }
    while(!fifo.empty()) {
        currentHits->push_back(fifo.front());
        fifo.pop();
    }

}


FTKRawHit* gridAUTH( boost::circular_buffer<FTKRawHit*> &cb, hitVector &fifo, hitVector &gridhits) 
{ 
    //seed is set from cb if there are hits and from 
    //fifo if the seed is empty. 
    FTKRawHit* seed = fifo.front(); 
    if (cb.size() != 0) {
        boost::circular_buffer<FTKRawHit*>::iterator cbi = cb.begin();
        seed = *cbi;
        for( cbi = cb.begin(); cbi != cb.end(); cbi++){
            if ((*cbi)->getEtaStrip() < seed->getEtaStrip())
                seed  = *cbi;
            else if ((*cbi)->getEtaStrip() == seed->getEtaStrip()) {
                if ((*cbi)->getPhiSide() < seed->getPhiSide())
                    seed = *cbi;
            }
        }
    }

    //step 2: set grid window limits
    int gridStrCol ;
    if (seed->getEtaStrip() % 2 != 0) gridStrCol = seed->getEtaStrip() - 1;
    else gridStrCol = seed->getEtaStrip();
    int gridCntRow = seed->getPhiSide();

    //step 3: put hits from cb to the grid window
    int dist = distance(cb.begin(), cb.end());
    for(int i = 0; i < dist; i++) {
        FTKRawHit* h = cb.front();
        if (!hitColInGrid(gridStrCol, h)) {
            cb.pop_front(); 
            continue;
        }
        if (hitRowInGrid(gridCntRow, h))
            gridhits.push_back(h); 
        else
            cb.push_back(h);
        cb.pop_front();
    }

    //step 4: put hits from fifo to the grid window
    hitVector::iterator fifo_it = fifo.begin();
    while (fifo_it != fifo.end()) {
        FTKRawHit* hit = *fifo_it;
        if (!hitColInGrid(gridStrCol, hit)) 
            break;
        if (hitRowInGrid(gridCntRow, hit))
            gridhits.push_back(hit);
        else 
            cb.push_back(hit);
        fifo.erase(fifo_it);
    }

    return seed;
}


void atlClusteringLNF(vector<FTKRawHit> &hits) 
{
    /*
     * make clusters in every module
     */
    hitsByModuleMap hitsByModule; // store hits by module
    hitsByModuleMap hitsByModuleFrozen; // store hits by module

    // Temporary storage for duplicate copy of ganged pixels
    // Automatically deleted at the end of this function
    std::vector<FTKRawHit> gangedPixelList;
    gangedPixelList.reserve(hits.size()); // reserve memory in excess

    /* 
     * First: organize raw hits by module 
     */
    for(unsigned int i = 0; i < hits.size(); i++) {
        int modId = hitToModuleId(hits[i]);
        if (modId>=0) {
            hitsByModule[modId].push_back( &(hits[i]) );
            if (DUPLICATE_GANGED && hitIsGanged(hits[i])) {
                //use the copy constructor instead of manually assigning each
                //variable, as was done previously. the usage here is the
                //whole point of having a copy constructor!
                gangedPixelList.push_back( hits[i] );
                FTKRawHit &lastPointer = gangedPixelList.back();
                lastPointer.setPhiSide(gangedPartner( hits[i] ));
                hitsByModule[modId].push_back( &lastPointer );
            }
        } else {
            assert(0); // should not need this any more
        }
    }
#ifdef HAVE_ROOT
    if(ROOTOUTCLU) rootOutput_clu_moduleStat(hitsByModule);
#endif
#ifdef VERBOSE_DEBUG_CLUST
    std::cout << "Ready to do clusters\n";
#endif

    /*
     * Second: build the list of clusters 
     */
    clustersByModuleMap clustersByModule; // store clusters by module
    hitsByModuleFrozen = hitsByModule; // keep hits structure
    hitsByModuleMap::iterator p;
    for (p = hitsByModule.begin(); p!=hitsByModule.end(); ++p) { // loop over modules
        hitVector *currentHits = & (p->second); 
        FTKRawHit &firstHit = **(currentHits->begin());
        int modId = hitToModuleId( firstHit );
        cluList *currentClusters = new cluList(); // instantiate cluster list
        clustersByModule[modId] = currentClusters;
        bool isIBLmodule = FTKSetup::getFTKSetup().getIBLMode()!=0 && firstHit.getLayer()==0;
        if (PIXEL_CLUSTERING_MODE < REALISTIC_CLUSTERING_MODE || !firstHit.getIsPixel() || isIBLmodule)
            makeClustersLNF(currentHits, currentClusters); // use ideal clustering
        else { // use realistic clustering
            realisticDecoder(currentHits);
            boost::circular_buffer<FTKRawHit*> circular_buffer (256);
            hitVector gridhits;
            cluList *currentClusters = new cluList();//  instantiate cluster list
            clustersByModule[modId] = currentClusters;

            hitVector fifo = *currentHits;
            while (!fifo.empty() || !circular_buffer.empty()) {
                FTKRawHit* seed = gridAUTH(circular_buffer, fifo, gridhits);
                makeClusterFromSeed(&gridhits, currentClusters, seed);

                for(hitVector::iterator hsi = gridhits.begin(); hsi != gridhits.end(); hsi++) {
                    circular_buffer.push_back(*hsi);
                }

                gridhits.clear();
            }
        }


        if (DEBUG_CLUSTERS) {
            std::cout << "DEBUG_CLUSTERS:"
                << "  modId=" << modId 
                << "  HitListSize=" << currentHits->size()
                << "  ClusterListSize=" << currentClusters->size()
                << "\n";
        }    
    } // end of loop over modules
#ifdef VERBOSE_DEBUG_CLUST
    std::cout << "Clusters made\n";
#endif

    if (CLUSTERING_PRINTOUT)
        printClusterList(clustersByModule);
    /*
     * Third: take the "average" of each cluster
     */
    int nTotClu = 0;
    clustersByModuleMap::iterator cluModP; // loop over modules
    for (cluModP=clustersByModule.begin(); cluModP!=clustersByModule.end(); cluModP++) {
        hitVector connectedHits; // list of ganged hits that are connected to a confirmed ganged hit
        cluList::iterator cluP;
        for (cluP=cluModP->second->begin(); cluP!=cluModP->second->end(); cluP++) {
            // do pattern recognition in the ganged region	
            if ( (*cluP->hitlist.begin())->getIsPixel() && findConnectedGanged(*cluP, connectedHits) ) // set isSafe
                cluP->isSafe = true;
        }

        for (cluP=cluModP->second->begin(); cluP!=cluModP->second->end(); cluP++) {
            if ( (*cluP->hitlist.begin())->getIsPixel() && isKilled(*cluP, connectedHits) ) // set isKilled
                cluP->isKilled = true;

            if (PIXEL_CLUSTERING_MODE < REALISTIC_CLUSTERING_MODE) {
                averageCluster(*cluP); // use floating-point average
            } else {
                averageCluster(*cluP); // use floating-point average
                //if (PIXEL_CLUSTERING_MODE == SIMPLE_INTEGER_CENTROID)
                //	else // FULL_INTEGER_CENTROID
                //NULL; // use integer average 
            }

            nTotClu++; // count all clusters
        }

        // delete connected hits
        hitVector::iterator connectedP;
        for (connectedP = connectedHits.begin(); connectedP!=connectedHits.end(); ++connectedP) { 
            delete *connectedP;
        }
    }
#ifdef VERBOSE_DEBUG_CLUST
    std::cout << "Ready to call rootOutput_clu_moduleClusterStat\n";
#endif
#ifdef HAVE_ROOT
    if(ROOTOUTCLU) rootOutput_clu_moduleClusterStat(clustersByModule, hitsByModuleFrozen, event);
#endif

#ifdef VERBOSE_DEBUG_CLUST
    std::cout << "Averages made\n";
#endif

    /*
     * Fourth: put clusters/hits back in the event hit list
     */

    hits.clear();

    int hitCounter=0;
    int deletedGangedClusters=0;
    for (cluModP=clustersByModule.begin(); cluModP!=clustersByModule.end(); cluModP++) {
        cluList::iterator cluP;
        for (cluP=cluModP->second->begin(); cluP!=cluModP->second->end(); cluP++) {
            // kill clusters with ganged hits according to ganged pattern recognition
            if ( GANGED_PATTERN_RECOGNITION && cluP->isKilled && !cluP->isSafe ) {
                // AA 2009-07-07 removing the "isSafe" protection does not change 
                // number of combinations to fit for single muon events
                deletedGangedClusters++;
                continue;
            }

            // store hit by value
            FTKRawHit cluEquiv = (*cluP).clusterEquiv;
            hits.push_back(cluEquiv);
            ++hitCounter;
        }
        delete cluModP->second;
    }
    assert( (hitCounter+deletedGangedClusters) == nTotClu );
#ifdef VERBOSE_DEBUG_CLUST
    std::cout << "Added clusters to the event hit list\n";
#endif

    /*
     * Fifth: free memory (could optimize and do this in previous step)
     */

    // The atlhit data from event is allocated and destroyed outside this function.
    // clustersByModuleMap is automatically deleted.
    clustersByModule.clear();

#ifdef VERBOSE_DEBUG_CLUST
    std::cout << "Deleted cluster structure\n\n";
#endif
}

int hitIsGanged(const FTKRawHit &hit) {
    if (FTKSetup::getFTKSetup().getIBLMode()==1 && hit.getLayer()==0)
        return 0; // FlagAA: IBL is never ganged
    if (hit.getIsPixel()) {
        int phi = hit.getPhiSide();
        if (pixelRowIsGanged(phi))
            return phi;
    }
    return 0;
}

int gangedPartner(const FTKRawHit &hit) {
    if (hit.getIsPixel()) {
        switch (hit.getPhiSide()) {
            case 153:
                return 160;
            case 160:
                return 153;
            case 155:
                return 161;
            case 161:
                return 155;
            case 157:
                return 162;
            case 162:
                return 157;
            case 159:
                return 163;
            case 163:
                return 159;
            case 168:
                return 164;
            case 164:
                return 168;
            case 170:
                return 165;
            case 165:
                return 170;
            case 172:
                return 166;
            case 166:
                return 172;
            case 174:
                return 167;
            case 167:
                return 174;
            default:
                return hit.getPhiSide();
        }
    }
    return -1;
}
