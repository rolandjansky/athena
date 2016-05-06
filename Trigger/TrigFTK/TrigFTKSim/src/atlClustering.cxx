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
#include <iomanip>
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

//#define VERBOSE_DEBUG_CLUST
//#define DEBUG_HITS
//#define DECODER_INPUT 0
//#define DECODER_OUTPUT 0
//#define CLUSTERING_PRINTOUT 0
//#define CENTROID_PRINTOUT 0

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

/*!
 * Function examining whether a hit is in a specific ROD
 * \param hit the hit to be examined
 * \return true if the hit belongs in the ROD, false otherwise
 */
bool hitSelector(const FTKRawHit &/*hit*/) {
    return true;
    //if (hit.getHitType() == PIXEL && hit.getBarrelEC() == 0 && hit.getLayer()== 1) {
        //if (hit.getPhiModule() == 13 && (hit.getEtaModule() <= 0))  {
            //return true;
        //}
    //}
    //else  {
        //return false;
    //}
    //return false;
}

cluster::~cluster()
{
    hitlist.clear();
}

/**
 * This class creates a hit whose coordinates are relative to the Front-End chip
 * of the module that it belongs to. It is used for the correct ordering of the hits 
 * in the FTK_IM hit decoder. 
 */
class FTK_FECoordsHit {
    public:
    /** 
     * Create an FTK_Hit_FECoords from an FTKRawHit
     * \param h FTKRawHit to be processed
     */
    FTK_FECoordsHit(const FTKRawHit* hit) {
        tot = hit->getTot();
        int acol = hit->getEtaStrip();
        int arow = hit->getPhiSide();
        if (arow < ftk::clustering::rowsInFEChipPerPixelModuleRow) {
            fe = ftk::clustering::feChipsInRowPixel + acol/ftk::clustering::colsInFEChipPerPixelModuleRow;
            lrow = arow;
            lcol = acol%ftk::clustering::colsInFEChipPerPixelModuleRow;
        } else {
            fe = (ftk::clustering::feChipsInRowPixel - 1) - acol/ftk::clustering::colsInFEChipPerPixelModuleRow; //-1 because we start counting from 0
            lrow = (ftk::numberOfPhiPixelsInPixelModule - 1) - arow; //We start counting from 0
            lcol = (ftk::clustering::colsInFEChipPerPixelModuleRow - 1) - acol%ftk::clustering::colsInFEChipPerPixelModuleRow; //Start counting from 0
        }
    }

    int fe; ///< The FE chip of the hit
    int tot; ///< ToT of the hit
    int lcol; ///< Column of the hit expressed in FE coordinates
    int lrow; ///< Row of the hit expressed in FE coordinates
};

/**
 * This class creates a hit whose coordinates are relative to the hit's position
 * in the cluster. It is used to calculate if a cluster is split and for
 * printing out the test vectors. 
 */
class FTK_ClusterCoordHit
{
    public:
    /** 
     * Create an FTK_Hit_FECoords from an FTKRawHit
     * \param h FTKRawHit to be processed
     * \param seed the central hit of the cluster
     */
    FTK_ClusterCoordHit(const FTKRawHit &h, const FTKRawHit* seed) {
        tot = h.getTot();
        crow = h.getPhiSide() - (seed->getPhiSide() - 10);
        if (seed->getEtaStrip() %2 != 0) ccol = h.getEtaStrip() - (seed->getEtaStrip() - 1);
        else  ccol = h.getEtaStrip() - (seed->getEtaStrip());
    }
    int tot; ///<  ToT of the hit in the cluster
    int ccol; ///< Column of the hit in the cluster
    int crow; ///< Row of the hit in the cluster
};


bool sortbyFE (const FTKRawHit* i,const  FTKRawHit* j) {
    if (i->getHitType() == SCT || j->getHitType() == SCT)
        return false;

    FTK_FECoordsHit lhit1(i);
    FTK_FECoordsHit lhit2(j);

    if (lhit1.fe != lhit2.fe) return lhit1.fe <= lhit2.fe;
    else if (lhit1.fe == lhit2.fe) {
        if (lhit1.lcol/2 != lhit2.lcol/2) return lhit1.lcol/2 <= lhit2.lcol/2;
        else if (lhit1.lcol/2 == lhit2.lcol/2) return lhit1.lrow < lhit2.lrow;
    }

    return false;
}


double eta(double px, double py, double pz) {
    double theta = atan( sqrt(px*px+py*py)/pz );
    if (theta<0) theta += 3.1415;

    //   std::cout << "px=" << px << " py=" << py << " pz=" << pz
    // 	    << " theta=" << theta << " eta=" << - log( tan (theta/2) ) << "\n";

    return - log( tan (theta/2) );
}

double eta(FTKRawHit &hit) {
    // eta of hit position
    return eta(hit.getX(), hit.getY(), hit.getZ());
}

/*!
 * Function examining whether a cluster is split. 
 * \param clu the cluster to be examined
 * \return true if the cluster is split, false otherwise
 */
bool isSplitCluster(const cluster& clu)
{
    //FTK_ClusterCoordHit seed = FTK_ClusterCoordHit(*clu.seed, clu.seed);
    hitVector::const_iterator it = clu.hitlist.begin();
    for (it = clu.hitlist.begin(); it != clu.hitlist.end(); it++) {
        if (clu.seed == NULL) 
            return false;
        FTK_ClusterCoordHit chit = FTK_ClusterCoordHit(**it , clu.seed);
        if ((chit.ccol >= GRID_COL_MAX - 1) ||  (chit.crow >= (GRID_ROW_MAX - 1)) || ( chit.crow <= 0))
            return true;
    }

    return false;
}


#if defined(CLUSTERING_PRINTOUT) || defined(CENTROID_PRINTOUT) || defined(DECODER_INPUT)

bool clusterSort (const FTK_ClusterCoordHit &i, const FTK_ClusterCoordHit &j)
{
    // reverse the hits on the first row if they are above the seed
    if (i.ccol == 0 && j.ccol == 0 && i.crow <= 10 && j.crow <= 10)
        return j.crow <= i.crow;
    else if (i.ccol == j.ccol)
        return i.crow <= j.crow;
    else
        return i.ccol <= j.ccol;
}

void printClu(const cluster &clu)
{
    hitVector hs = clu.hitlist;
    hitVector::iterator hit = hs.begin();

    vector<FTK_ClusterCoordHit> tmp;
    for (hit = hs.begin(); hit != hs.end(); hit++)
        tmp.push_back(FTK_ClusterCoordHit(**hit, clu.seed));

    //sort and print the vector.
    if (tmp.size() > 1) std::stable_sort(tmp.begin(), tmp.end(), clusterSort);

    for(vector<FTK_ClusterCoordHit>::iterator hit1 = tmp.begin(); hit1 != tmp.end(); hit1++) {
        printf("0000%.2X%.2X%.3X\n",(*hit1).tot, (*hit1).ccol, (*hit1).crow);
        //printf("0000%.2X%.2X%.3X",(*hit1).tot, (*hit1).ccol, (*hit1).crow);
        //printf(" tot: %d col: %d row: %d\n",(*hit1).tot, (*hit1).ccol, (*hit1).crow);
    }

    int boundary = 2;
    if (isSplitCluster(clu))
        boundary = 3;

    if (clu.seed->getEtaStrip() %2 != 0) {
        printf("000%d00%.2X%.3X\n",boundary, clu.seed->getEtaStrip() - 1, clu.seed->getPhiSide());
        //printf("000%d00%.2X%.3X",boundary, clu.seed->getEtaStrip() - 1, clu.seed->getPhiSide());
        //printf(" split: %d col: %d row: %d\n",boundary, clu.seed->getEtaStrip() - 1, clu.seed->getPhiSide());
    }
    else {
        printf("000%d00%.2X%.3X\n",boundary, clu.seed->getEtaStrip(), clu.seed->getPhiSide());
        //printf("000%d00%.2X%.3X",boundary, clu.seed->getEtaStrip(), clu.seed->getPhiSide());
        //printf(" split: %d col: %d row: %d\n",boundary, clu.seed->getEtaStrip(), clu.seed->getPhiSide());
    }
}

void printClusterList(clustersByModuleMap clustersByModule)
{
    clustersByModuleMap::iterator p;
    for (p = clustersByModule.begin(); p!=clustersByModule.end(); ++p) { // loop over modules
        cluList* cl = (*p).second;
        cluList::iterator b = cl->begin();
        FTKRawHit* hit = (*b).hitlist[0];
        if (hitSelector(*hit)) {
            printf("0x200000%02d\n", getModId(hit));
            for (; b != cl->end(); b++) {
                printClu(*b);
                //printf("CENTROID: %.8X X: %d Y: %d\n", (*b).clusterEquiv.getHWWord(), (*b).clusterEquiv.getRowCoordinate(), (*b).clusterEquiv.getColumnCoordinate());
            }
            printf("0x40000000\n");
        }
    }
}


void printCentroidList(clustersByModuleMap clustersByModule)
{
    clustersByModuleMap::iterator p;
    for (p = clustersByModule.begin(); p!=clustersByModule.end(); ++p) { // loop over modules
        cluList* cl = (*p).second;
        cluList::iterator b = cl->begin();
        FTKRawHit* hit = (*b).hitlist[0];
        if (hitSelector(*hit)) {
           printf("0x8%.7x\n", getModId(hit));
            for (; b != cl->end(); b++) {
                //printToFile(outcentroid, (*b).clusterEquiv.getHWWord());
                printf("0x%.8X\n",(*b).clusterEquiv.getHWWord());
            }
            printf("0x40000000\n");
        }
    }
}

void printDecoderOutput(hitVector* currentHits)
{
    hitVector::iterator hit = currentHits->begin();
    printf("0x8%.7x\n", getModId(*hit));
    for(hit = currentHits->begin(); hit!= currentHits->end(); hit++) {
        printf("0x0%.2X%.2X%.3X",(*hit)->getTot(), (*hit)->getEtaStrip(), (*hit)->getPhiSide());
        std::cout << (*hit)->getTot() << " " << (*hit)->getEtaStrip() << " " << (*hit)->getPhiSide() << std::endl;
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


/*!
 * Function printing the hit details
 * \param hit the hit 
 * \return void
 */
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

/*!
 * Function examining whether the a given row has ganged pixels
 * \param the row to be examined
 * \return true 
 */
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


/*!
 * Function calculating the module id in which the hit belongs to. 
 * The default mode for now is to check the Identifier Hash of the hit. 
 * \param hit the hit 
 * \return the module id
 *
 */
int hitToModuleId(const FTKRawHit &hit) {

    if (DEBUG_HITS)
        printHit(hit);

    //Default mode, return the Identifier Hash of the hit
    if (hit.getIdentifierHash() > 0 && hit.getHitType() == PIXEL) {
        return hit.getIdentifierHash();
    }
    else if (hit.getIdentifierHash() > 0 && hit.getHitType() == SCT) {
        return 0x8000 + hit.getIdentifierHash();
    }

    unsigned int ieta = 0;
    if (FTKSetup::getFTKSetup().getIBLMode()==1){
        assert(hit.getEtaModule()>=-8 && hit.getEtaModule()<=8); // cy for ibl
    }
    else if (FTKSetup::getFTKSetup().getIBLMode()==2){
        assert(hit.getEtaModule()>=-10 && hit.getEtaModule()<=10); // version with 3D sensors
    }
    else{
        assert(hit.getEtaModule()>=-6 && hit.getEtaModule()<=6);
    }

    if (FTKSetup::getFTKSetup().getIBLMode()==1 && hit.getBarrelEC() == ftk::BARREL && hit.getLayer() == 0 && hit.getIsPixel() == 1) { // for ibl
        // eta index should range -8 to 8 for ibl
        ieta = 8 + hit.getEtaModule(); // cy was 6
    } else if (FTKSetup::getFTKSetup().getIBLMode()==2 && hit.getBarrelEC() == ftk::BARREL && hit.getLayer() == 0 && hit.getIsPixel() == 1) { // for ibl
        // eta index should range -8 to 8 for ibl
        ieta = 10 + hit.getEtaModule(); // cy was 6
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
            if (FTKSetup::getFTKSetup().getIBLMode()>=1) {
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

/*!
 * Check (and define) if two hits are next to each other
 * For the time being it accepts only pair of hits from the same module and same SCT side.
 * It crashes if hits are the same.
 * \param hit1 the first hit
 * \param hit2 the second hit
 * \return true if hits are next to each other, false otherwise. 
 */
bool neighborhood(const FTKRawHit &hit1, const FTKRawHit &hit2) {

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
            tmpHit->setModuleType( ftk::MODULETYPE_PIXEL );
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

/*! 
 * Function calculating the central position of the cluster (i.e. the centroid) for all possible 
 * scenarios (SCT, Pixel, IBL). During the execution of this function, the
 * following variables are set for each cluster: X, Y, Z, 
 *
 * \param cluster the cluster whose average is going to be calculated
 * \return void
 */
void averageCluster(cluster &clu) {
    const unsigned int &nHits = clu.hitlist.size();
    assert(nHits>0); // sanity check

    FTKRawHit &av = clu.clusterEquiv; ///< get pointer to clusterEquivalent
    FTKRawHit *first = *(clu.hitlist.begin()); ///< get 1st hit
    /// reset values for clusterEquivalent (alias av)
    av.reset();
    av.setX(0);
    av.setY(0);
    av.setZ(0);
    av.setIdentifierHash(first->getIdentifierHash());
    av.setHitType(first->getHitType());
    av.setModuleType(first->getModuleType());
    av.setBarrelEC(first->getBarrelEC());
    av.setLayer(first->getLayer());
    av.setPhiModule(first->getPhiModule());
    av.setEtaModule(first->getEtaModule());
    av.setEtaStrip(0);
    av.setNStrips(0);
    av.setEtaWidth(0);
    //printf("zeroing from %.8X %d %d\n", av.getHWWord(), av.getEtaWidth(), av.getPhiWidth());
    av.setPhiWidth(0);
    av.setIncludesGanged(false);

    int tmp;
    int etaMax = -1;
    int etaMin = 4000;
    int phiMax = -1;
    int phiMin = 4000;
    int rowMin = 99999; //int(2*(design->width()/design->phiPitch()))+1;;
    int rowMax = 0;
    int colMin = 99999; //int(2*(design->length()/design->etaPitch()))+1;
    int colMax = 0;
    int qRowMin = 0;  
    int qRowMax = 0;
    int qColMin = 0;  
    int qColMax = 0;

    if (DEBUG_AVERAGE)
        std::cout << "DEBUG_AVERAGE:  isPixel=" << av.getIsPixel()
            << ", nHits=" << nHits<< "\n";

    hitVector::iterator p;
    /// precalculate values for case PIXEL_CLUSTERING_MODE>0
    int BarrelEndCap = first->getBarrelEC();

    //Calculate the active layer. Start with the layer of the hit. In case 
    // always count IBL as layer0 and BLayer as layer1
    int layer = first->getLayer();
    bool fixEndcapL0 = FTKSetup::getFTKSetup().getfixEndcapL0();
    bool isIBLmodule  = FTKSetup::getFTKSetup().getIBLMode()!=0 && layer==0 && (BarrelEndCap==0 || !fixEndcapL0);
    bool isPixelmodule = FTKSetup::getFTKSetup().getIBLMode()!=0 && layer!=0;
    if (FTKSetup::getFTKSetup().getIBLMode()==0)
        layer++; 
    float radius = ftk::clustering::radii[layer];

    //Default values for a pixel module. They are modified for IBL. 
    float sensorThickness = ftk::sensorThicknessPixel;
    float etaPitch = ftk::etaPitchPixel;
    float numberOfEtaPixelsInModule = ftk::numberOfEtaPixelsInPixelModule;
    float lengthOfPixelModuleInUmPixels = ftk::lengthOfPixelModuleIn400umPixels;
    float moduleActiveLength = ftk::lengthOfPixelModuleIn400umPixels*ftk::etaPitchPixel/ftk::micrometer;
    // FlagAA: 2015-01-29 making default the units of 6.25um for r-phi coordinates
    float pixYScaleFactor = ftk::clustering::yScaleFactorPixel; ///<multiply by 16 to count in unit of 25um
    float pixXScaleFactor = ftk::clustering::xScaleFactorPixel;
    float etaModule = first->getEtaModule()-6;
    // FlagAA 2013-07-31: IBL code assumes fully planar geometry as in mc12+IBL
    // This will change for the real detector!!!
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;
        etaPitch = ftk::etaPitchIbl;
        numberOfEtaPixelsInModule = ftk::numberOfEtaPixelsInIblModule;
        lengthOfPixelModuleInUmPixels = ftk::lengthOfIblModuleIn250umPixels;
        moduleActiveLength = ftk::lengthOfIblModuleIn250umPixels*ftk::etaPitchIbl/ftk::micrometer; // planar sensors
        pixYScaleFactor = ftk::clustering::yScaleFactorPixel; ///<multiply by 10 to count in unit of 25um
        pixXScaleFactor = ftk::clustering::xScaleFactorPixel;
        etaModule = first->getEtaModule()-8;
        //float sensorThickness = 230*micrometer; // 3D sensors
        //pixelModuleActiveLength = 80*0.25;  // 3D sesors ???
        //numberOfEtaPixelsInModule = 80; // 3D sesors ???
    }
    bool hasGanged = false;

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
            av.setStripCoordinate(firstStrip+lastStrip);
            av.setColumnCoordinate( 0 );
            av.setDeltaEta(0);
            av.setNStrips(lastStrip+1-firstStrip);
            av.setEtaWidth(1);
            //std::cout << " strip " << std::endl;
            av.setPhiWidth(lastStrip+1-firstStrip);
            if (DEBUG_AVERAGE_SCT) { // && clu.hitlist.size()>1) {
                std::cout << "SCT AVERAGE: ";
                printHit(av);
                std::cout << std::endl;
            }
            break; // end of SCT
            }
        case PIXEL: {
            av.setPhiSide(0); // eta is reset a few lines above

            if (PIXEL_CLUSTERING_MODE == 0) {
                for (p=clu.hitlist.begin(); p!=clu.hitlist.end(); ++p) { //loop over hits in cluster
                    assert(av.getLayer()==(*p)->getLayer() && av.getPhiModule()==(*p)->getPhiModule() && av.getEtaModule()==(*p)->getEtaModule() );
                    if (SAVE_CLUSTER_CONTENT) { // if enabled the cluster also stores also the single channels
                        FTKRawHit tmpch = *(*p);
                        // set the barcode of the single channel, this may allow a very refined debugging of
                        // the cluster content accounting for the single barcode of each channel
                        MultiTruth mt;
                        MultiTruth::Barcode uniquecode(tmpch.getEventIndex(),tmpch.getBarcode());
                        mt.maximize(uniquecode,tmpch.getBarcodePt());
                        tmpch.setTruth(mt);
                        av.addChannel(tmpch);
                    }
                    av.addX((*p)->getX());
                    av.addY((*p)->getY());
                    av.addZ((*p)->getZ());
                    av.addPhiSide((*p)->getPhiSide());   // phi index
                    // pixels are counted starting from 0 (left edge) and not 0.5 (center position)
                    // if only pixel 0 is hit the output value will be 0 and not 0.5 (pixel center)
                    av.addEtaStrip((*p)->getEtaStrip()); // eta index
                }
                tmp = (int)round((av.getEtaStrip()*1.)/nHits);
                av.setDeltaEta((av.getEtaStrip()*1.)/nHits-tmp);
                av.setEtaStrip(tmp);
                tmp = (int)round((av.getPhiSide()*1.)/nHits);
                av.setDeltaPhi((av.getPhiSide()*1.)/nHits-tmp);
                av.setPhiSide(tmp);
                break;
            }

            /* For PIXEL_CLUSTERING_MODE > 0
             * calculate cluster center following code at line 701 of
             * https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetRecTools/SiClusterizationTool/trunk/src/MergedPixelsTool.cxx#L701
             * I.e. m_posStrategy == 1
             * I.e. use charge imbalance between the two outer most ros (columns) to calculate the centroid
             * In this code I'll use etaTrack instead of pixel eta for simplicity
             */

            for (p=clu.hitlist.begin(); p!=clu.hitlist.end(); ++p) { //loop over hits in cluster
                assert(av.getLayer()==(*p)->getLayer() && av.getPhiModule()==(*p)->getPhiModule() && av.getEtaModule()==(*p)->getEtaModule() );

                if (SAVE_CLUSTER_CONTENT) { // if enabled the cluster also stores also the single channels
                    FTKRawHit tmpch = *(*p);
                    // set the barcode of the single channel, this may allow a very refined debugging of
                    // the cluster content accounting for the single barcode of each channel
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

                if (isPixelmodule) {
                    // account for 600um pixels in the centroid calculation
                    // will use units of 100um and the convert to normal pixel units below
                    // will also indicate the center of the pixel instead of the left edge
                    // considering center of pixel along eta (left (or top if you prefer) edge along phi)
                    // each FE number is worth 19*400um pixels (400um*16 + 600um*2)
                    // multiply FE column by 4 to convert to 100um units
                    int FEnumber = col/ftk::clustering::colsInFEChipPerPixelModuleRow;
                    int FEcolumn = col%ftk::clustering::colsInFEChipPerPixelModuleRow;
                    col = FEnumber*(ftk::clustering::colsInFEChipPerPixelModuleRow+1) + FEcolumn;
                    col *= pixYScaleFactor;
                    col += pixYScaleFactor/2; // add half a pixel to align to pixel center (assume normal 400um pixel)
                    col += pixYScaleFactor/2; // assume 1<=FEcolumn<=16 add half a pixel coming from 600um pixel in FEcolumn==0
                    if (FEcolumn==0) col -= pixYScaleFactor/4; // correct position for first column in FE chip
                    if (FEcolumn==(ftk::clustering::colsInFEChipPerPixelModuleRow-1)) col += pixYScaleFactor/4; // correct position for last column in FE chip
                } else if (isIBLmodule) { // IBL case
                    //Modifications have to be made to include 3d modules 
                    int orig_col = col;
                    col *= pixYScaleFactor; // use units of 25um
                    col += pixYScaleFactor/2; // add half a pixel to align to pixel center
                    if (orig_col==0) col += pixYScaleFactor/2; // add half pixel (500um pixel in col0)
                    if (orig_col>0) col += pixYScaleFactor; // add a pixel (500um pixel in col0)

                    // for 3D modules only
                    // if (orig_col==79) col += pixYScaleFactor*5/10; // add 5/10 of pixel i.e. 100um (500um pixel in col79)

                    // for planar modules only
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

                if ((*p)->getEtaStrip() > etaMax) 
                    etaMax = (*p)->getEtaStrip();
                if ((*p)->getEtaStrip() < etaMin) 
                    etaMin = (*p)->getEtaStrip();
                if ((*p)->getPhiSide() > phiMax) 
                    phiMax = (*p)->getPhiSide();
                if ((*p)->getPhiSide() < phiMin) 
                    phiMin = (*p)->getPhiSide();
                if (DEBUG_AVERAGE)
                    printHit(**p);
                av.addTot(first->getTot()); // sum ToT for pixel clusters
            }
            av.setEtaWidth(etaMax-etaMin+1);
            av.setPhiWidth(phiMax-phiMin+1);

            // calculate eta index and eta delta
            double eta_average, phi_average;
            eta_average = (colMin + colMax) / 2.;
            phi_average = (rowMin + rowMax) / 2.;

            //Correction to the eta_average and phi_average used for
            //PIXEL_CLUSTERING_MODE 1 and 100 
            if (PIXEL_CLUSTERING_MODE <= PIXEL_CLUSTERING_MIXED) {
                float pixelEstimateCotTheta = -9999.;

                /* The next lines calculate CotTheta of a hypothetic track starting from ATLAS (0,0,0) 
                 * and crossing the center of the bounding box of the cluster.
                 * The Z global position is estimated as: module Z position + cluster Z position within the module.
                 *  The radius is a fixed costant depending on the layer. It could be better estimated accounting 
                 *  also for the rphi position within the module.
                */
                pixelEstimateCotTheta = (etaModule+(rowMin+rowMax)/2./pixYScaleFactor/numberOfEtaPixelsInModule-0.5) * moduleActiveLength / radius;
                if (PIXEL_CLUSTERING_MODE>=PIXEL_CLUSTERING_IDEAL_APRIL_2014_FIX) /* Fixing an error in the formula */
                pixelEstimateCotTheta = (etaModule+(colMin+colMax)/2./pixYScaleFactor/numberOfEtaPixelsInModule-0.5) * moduleActiveLength / radius;

                // Compute eta for charge interpolation correction (if required)
                // Two pixels may have tot=0 (very rarely, hopefully)
                float etaRow = -1;
                float etaCol = -1;
                if(qRowMin+qRowMax > 0) 
                    etaRow = qRowMax/float(qRowMin+qRowMax);
                if(qColMin+qColMax > 0) 
                    etaCol = qColMax/float(qColMin+qColMax);

                // Charge interpolation. Very rough guess (one can do better with
                // candidate track information later) TL
                //      bool hasGanged = av.getIncludesGanged();
                float deltax = 0;
                float deltay = 0;
                if (BarrelEndCap==0) { //pixelID.is_barrel(elementID)){
                    deltax = 30*ftk::micrometer*(sensorThickness/(250*ftk::micrometer));
                    deltay = sensorThickness*fabs(pixelEstimateCotTheta);
                    if(deltay > etaPitch ) deltay = etaPitch;
                } else {
                    deltax = 10*ftk::micrometer*sqrt(sensorThickness/(250*ftk::micrometer));
                    deltay = 10*ftk::micrometer*sqrt(sensorThickness/(250*ftk::micrometer));
                }

                // Width of the region of charge sharing. For disks assume normal incidence: delta is small, due to diffusion
                // of drifting charges in silicon. For barrel, assume 10 deg. incidence in Rphi, in z compute from pseudorapidity 
                // This may be improved with better parameterization, but it is probably better to use candidate track information 
                // later in reconstruction. TL
                // Values are made dependent on the sensor thickness to accomodate // different sensors layout. AA
                //    Point3D<double> globalPos = element->globalPosition(centroid);
                //    InDetDD::SiLocalPosition totCorrection(0,0,0);
                int m_posStrategy = 1; //Same as m_posStrategy == 1 in InDetRecTools/SiClusterizationTool/trunk/src/MergedPixelsTool.cxx#L701
                if(m_posStrategy == 1 && !hasGanged && etaRow>0 && etaCol > 0){
                    phi_average += deltax*(etaRow-0.5)/ftk::phiPitch;
                    eta_average += pixYScaleFactor*deltay*(etaCol-0.5)/etaPitch;
                } 
            }

            if (phi_average<0) {
                assert(phi_average<0);
                phi_average = 0;
            }
            if (eta_average<0) {
                assert(eta_average<0);
                eta_average = 0;
            }

            if (BarrelEndCap!=0) phi_average += ftk::clustering::pixelEndCapRPhiCorrection;
            if (isIBLmodule) phi_average += ftk::clustering::pixelIblRPhiCorrection;

            av.setRowCoordinate( lround(phi_average*pixXScaleFactor) );
            av.setColumnCoordinate( lround(eta_average) );
            av.setSplit(false);

            if (PIXEL_CLUSTERING_MODE >= PIXEL_CLUSTERING_MIXED && !isIBLmodule && isSplitCluster(clu)) {
                    av.setSplit(true);
            }

            eta_average*=numberOfEtaPixelsInModule/lengthOfPixelModuleInUmPixels/pixYScaleFactor;
            //if (isPixelmodule) {
                //// rescale full module length 152*400um to the range 0-144
                //// here 1 units is 400*19/18um (i.e. average 400/600um pixel length)
                //eta_average*=numberOfEtaPixelsInModule/ftk::lengthOfPixelModuleIn400umPixels/pixYScaleFactor;
            //} else if (isIBLmodule && FTKSetup::getFTKSetup().getIBLMode()==1) { 
                //// rescale from 25um units to 250um (1 "normal" IBL pixel) unit
                //eta_average*=numberOfEtaPixelsInModule/ftk::lengthOfIblModuleIn250umPixels/pixYScaleFactor;
            //}
            //else if (isIBLmodule && FTKSetup::getFTKSetup().getIBLMode()==2)
                //// rescale from 25um units to 250um (1 "normal" IBL pixel) unit
                //eta_average*=numberOfEtaPixelsInModule/ftk::lengthOfIblModuleIn250umPixels/pixYScaleFactor;
            //}

            double delta;
            delta = eta_average - (int) eta_average;
            av.setDeltaEta(delta);
            av.setEtaStrip( (int) eta_average );
            delta = phi_average - (int) phi_average;
            av.setDeltaPhi(delta);
            av.setPhiSide( (int) phi_average );

            break; // end of PIXEL
        }
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



void realisticPixelDecoder(hitVector* &currentHits)
{
    hitVector::iterator hit = currentHits->begin();

#if defined(DECODER_INPUT)
    printf("0x8%.7x\n", getModId(*hit));
    std::cout << "HitType: " << (*hit)->getHitType()  << " Barrel-EC: " << (*hit)->getBarrelEC() <<  " Layer: " << (*hit)->getLayer() << " PhiModule:" << (*hit)->getPhiModule() << " EtaModule: " << (*hit)->getEtaModule() << std::endl;
    //for (hit = currentHits->begin(); hit!= currentHits->end(); hit++) {
    //fe_hit fehit = fe_hit((*hit));
    //printf("0x08%.1X%.2X%.2X%.2X -- %d %d %d", fehit.fe, fehit.tot, fehit.lcol, fehit.lrow , fehit.tot, fehit.lcol, fehit.lrow);
    //std::cout << " hitType " << (*hit)->getHitType() << " BEC " << (*hit)->getBarrelEC() << " layer " << (*hit)->getLayer() << " phimod " << (*hit)->getPhiModule() << " etamod " << (*hit)->getEtaModule() << std::endl;
    //}
#endif

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


/*!
 * Function responsible for hit clustering and the subsequent centroid
 * calculation 
 * \param hits hits which will be clustered
 * \return void
 */
void atlClusteringLNF(vector<FTKRawHit> &hits)
{
    /*
     * make clusters in every module
     */
    hitsByModuleMap hitsByModule; ///< store hits by module
    hitsByModuleMap hitsByModuleFrozen; ///< store hits by module

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
    clustersByModuleMap clustersByModule; ///< store clusters by module
    hitsByModuleFrozen = hitsByModule; ///< keep hits structure
    hitsByModuleMap::iterator p;
    for (p = hitsByModule.begin(); p!=hitsByModule.end(); ++p) { // loop over modules
        hitVector *currentHits = & (p->second);
        FTKRawHit &firstHit = **(currentHits->begin());
        int modId = hitToModuleId( firstHit );

#if defined(DECODER_OUTPUT)
        printDecoderOutput(currentHits);
#endif
        bool isIBLmodule = FTKSetup::getFTKSetup().getIBLMode()!=0 && firstHit.getLayer()==0;
        cluList *currentClusters = new cluList(); // instantiate cluster list
        clustersByModule[modId] = currentClusters;

        //The ideal clustering is going to be used in the following cases: 
        //1. PIXEL_CLUSTERING_MODE is less than 100 (PIXEL_CLUSTERING_MIXED)
        //2. The module is not a pixel module
        if (PIXEL_CLUSTERING_MODE < PIXEL_CLUSTERING_MIXED || !firstHit.getIsPixel() || isIBLmodule) {
            makeClustersLNF(currentHits, currentClusters); // use ideal clustering
        }
        else { // PIXEL_CLUSTERING_MODE >= 100 and pixel module 
            realisticPixelDecoder(currentHits);

            boost::circular_buffer<FTKRawHit*> circular_buffer (256);
            hitVector gridhits;
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

#if defined(CLUSTERING_PRINTOUT)
    printClusterList(clustersByModule);
#endif

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
            averageCluster(*cluP); // use floating-point average
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

#if defined(CENTROID_PRINTOUT)
    printCentroidList(clustersByModule);
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

/*!
 * Function examining if a hit is ganged. In case the hit belongs to a pixel
 * module and at a row with ganged pixels, the phi coordinate of the pixel is
 * returned. It always returns 0 if the hit belongs to an IBL module. 
 * \param hit hit to be examined
 * \return The phi coordinate of the hit if the hit is ganged, 0 otherwise
 */
int hitIsGanged(const FTKRawHit &hit) {
    if (FTKSetup::getFTKSetup().getIBLMode()>=1 && hit.getLayer()==0)
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
