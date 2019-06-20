/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKCLUSTERINGENGINE_H
#define FTKCLUSTERINGENGINE_H

#include "TrigFTKSim/ftktypes.h"
#include "TrigFTKSim/FTKClusteringPrintout.h"
#include "TrigFTKSim/FTKLogging.h"

#define GRID_ROW_MAX 21
#define GRID_COL_MAX 8
#define hitRowInGrid(gridCntRow, hit) ((hit->getPhiSide() <= gridCntRow + GRID_ROW_MAX/2) && \
        (hit->getPhiSide() >= gridCntRow - GRID_ROW_MAX / 2))? true : false
#define hitColInGrid(gridStrCol, hit) (hit->getEtaStrip() < gridStrCol + GRID_COL_MAX)? true : false

#include <ctype.h>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <boost/circular_buffer.hpp>

typedef std::vector<std::unique_ptr<FTKRawHit>> hitVector;
typedef std::map<uint32_t, hitVector> hitsByModuleMap;


class cluster {
public:
    hitVector hitlist; //< list of hits that make the cluster
    FTKRawHit clusterEquiv; //< hit equivalent to the cluster
    FTKRawHit seed; //< The first hit, which is used to start the cluster in PCM > 100
    bool isSafe; // confirmed by ganged pattern recognition
    bool isKilled; // ganged pattern recognition suggest to remove this cluster

  ~cluster();
};


typedef std::vector<std::unique_ptr<cluster>> cluList;
typedef std::map<uint32_t, cluList> clustersByModuleMap;

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
    FTK_FECoordsHit(const FTKRawHit& hit) {
        tot = hit.getTot();
        int acol = hit.getEtaStrip();
        int arow = hit.getPhiSide();
        bool isIBLmodule = (hit.getModuleType() == ftk::MODULETYPE_IBL_PLANAR || hit.getModuleType() == ftk::MODULETYPE_IBL3D);
	//The relative coordinates are different for pixel and IBL, 
        if(!isIBLmodule){
            if (arow < ftk::clustering::rowsInFEChipPerPixelModuleRow) {
                fe = ftk::clustering::feChipsInRowPixel + acol/ftk::clustering::colsInFEChipPerPixelModuleRow;
                lrow = arow;
                lcol = acol%ftk::clustering::colsInFEChipPerPixelModuleRow;
            } else {
                fe = (ftk::clustering::feChipsInRowPixel - 1) - acol/ftk::clustering::colsInFEChipPerPixelModuleRow; //-1 because we start counting from 0
                lrow = (ftk::numberOfPhiPixelsInPixelModule - 1) - arow; //We start counting from 0
                lcol = (ftk::clustering::colsInFEChipPerPixelModuleRow - 1) - acol%ftk::clustering::colsInFEChipPerPixelModuleRow; //Start counting from 0
            }
        }else{
            if (acol < ftk::clustering::colsInFEChipPerIblModuleRow) {
                fe = 0;
                lcol = acol;
            } else {
                fe = 1;
                lcol = acol-ftk::clustering::colsInFEChipPerIblModuleRow;
            }
            lrow = (ftk::clustering::rowsInFEChipPerIblModuleRow - 1) - arow;
        }
    }

    int fe; ///< The FE chip of the hit
    int tot; ///< ToT of the hit
    int lcol; ///< Column of the hit expressed in FE coordinates
    int lrow; ///< Row of the hit expressed in FE coordinates
};


class FTKClusteringEngine : FTKLogging
{
private:

    bool m_saveClusterContent;
    bool m_diagClustering; //!< If true. clusters are allowed to be connected only by diagonal pixels
    bool m_sctClustering; //!< If true, sct hits are also clustered
    bool m_ibl3DRealistic; //!<If true, you can get the same IBL 3D centorid as the FTK_IM FW.
    bool m_duplicateGanged; //!< If true, if a ganged pixel is hit, its partner is selected as an active hit
    bool m_gangedPatternRecognition; //!< The code contains functions that allow for some "smart"
				   //!< recognition on which hits are the "real" ones and which are
                                   //!< and which are fake. If true, this test is enabled.
    bool m_splitBlayerModules;
    int m_pixelClusteringMode; //!< Which clustering mode to use. 

    static bool sortPixelInput (const std::unique_ptr<FTKRawHit>& i, const  std::unique_ptr<FTKRawHit>& j);
    static bool sortIBLInput(const std::unique_ptr<FTKRawHit>& i, const std::unique_ptr<FTKRawHit>& j);

    /*!
    * Function that examines if a cluster is in contact with the sliding window borders
    * @param Cluster to be examined
    * @return true if the cluster is in contact with the borders, false otherwise
    */
    bool isSplitCluster(const cluster* clu);

    /*!
    * Function that examines if a hit belongs in an IBL module
    * @param Hit to be examined
    * @return true if the hit is on an IBL module, false otherwise
    */
    bool hitOnIBLmodule(const FTKRawHit &hit);

    /*!
    * Function that finds the HashID of the module for a given hit. 
    * @param Hit to be examined
    * @return a HashID. 
    */
    uint32_t getModuleId(const FTKRawHit &hit);

    /*!
    * Function that replicates the Pixel & IBL decoder. It is bit-accurate
    * with the firmware implementation. More details can be found in the FTK_IM 
    * documentation.
    * @param Vector of hits that have to be sorted.
    */
    void realisticPixelDecoder(hitVector& currentHits);


    /*!
    * Function that examines whether two hits share a border. If diagClustering is true, 
    * it counts as neighbours also diagonal hits. 
    * @param hit1 A hit
    * @param hit2 Well, the other hit
    * @return true if hits are in contact
    */
    bool neighborhood(const FTKRawHit &hit1, const FTKRawHit &hit2);

    /*!
    * Function that examines whether two hits share a border. If diagClustering is true, 
    * it counts as neighbours also diagonal hits. 
    * @param hit1 A hit
    * @param hit2 Well, the other hit
    * @return true if hits are in contact
    */

    /*!
    * Recursively adds hits to a given cluster.
    * If no cluster is given (or empty cluster) make a cluster out of the 1st hit.
    * @param currentHits the hits from which the cluster will be formed
    * @param the cluster
    * @return the number of hits in the final cluster
    */
    int buildUpCluster(hitVector& currentHits, cluster& clu);

    /*!
    * Group all hits form one module into clusters. 
    * It is the default clustering implementation for SCT and it does not use the Sliding 
    * window mechanism. 
    * @param currentHits the hits that have to be clustered. 
    * @param currentClusters the vector holding the clulsters which have been created
    */
    void makeClustersLNF(hitVector& currentHits, cluList& currentClusters);

    /*!
    * Group all hits form one module into clusters. 
    * This implementation uses the Sliding window mechanism, replicating the FW iplementation. 
    * @param currentHits the hits that have to be clustered. 
    * @param currentClusters the vector holding the clulsters which have been created
    */
    void makeClustersGridWindow(hitVector& currentHits, cluList& currentClusters);

    /*!
    * Function that replicates the sliding window mechanism. For more details about the 
    * implementation, please see the FTK_IM documentation. 
    * @param cb the circular buffer containing hits
    * @param fifo another hit container
    * @param gridHits a vector with the hits of inside the window.  
    * @return The hit that the cluster will be formed around. 
    */
    FTKRawHit gridAUTH( boost::circular_buffer<std::unique_ptr<FTKRawHit>> &cb,
                                         hitVector &fifo, hitVector &gridhits);
    /*!
    * Function creating a cluster around a given hit (the seed).
    * @param currentHits the hits from which the cluster will be formed 
    * @param currentClusters vector which will hold the cluster
    * @param seed the hit around which the cluster will be formed
    */
    void makeClusterFromSeed(hitVector& currentHits, cluList& currentClusters, FTKRawHit& seed);

    /*!
    * Function calculating the central position of the cluster (i.e. the centroid) for all possible
    * scenarios (SCT, Pixel, IBL). During the execution of this function, the
    * following variables are set for each cluster: X, Y, Z,
    *
    * \param cluster the cluster whose average is going to be calculated
    */
    void averageCluster(std::unique_ptr<cluster>& clu);

    /*!
    * Function examining whether the a given row has ganged pixels
    * \param the row to be examined
    * \return true if the row has ganged pixels, false otherwise
    */
    bool pixelRowIsGanged(const int row);

    /*! 
    * Function which finds the partner of a ganged pixel
    * @param hit The hit 
    * @return the row of the ganged partner
    */
    int gangedPartner(const FTKRawHit &);

    /*!
    * Function examining if a hit is ganged. In case the hit belongs to a pixel
    * module and at a row with ganged pixels, the phi coordinate of the pixel is
    * returned. It always returns 0 if the hit belongs to an IBL module.
    * \param hit hit to be examined
    * \return The phi coordinate of the hit if the hit is ganged, 0 otherwise
    */
    int hitIsGanged(const FTKRawHit &);
   
    /*! 
    * Function which examines whether a hit that belongs to a ganged pixel 
    * has a neighbouring hit. This is used in the smart ganged partner recognition
    * @param hit The hit 
    * @param cluster The cluster to be examined
    * @param connectedHits list of ganged hits that are connected to a confirmed ganged hit
    * @return true if the ganged hit has a neighboor, false otherwise
    */
    bool gangedHitHasNeighborhood(const FTKRawHit &hit, const cluster* clu, hitVector &connectedHits);

    
    /*! 
    * Function which finds the hits that are in contact with a ganged pixel. 
    * @param cluster The cluster to be examined
    * @param connectedHits list of ganged hits that are connected to a confirmed ganged hit
    * @return true if the ganged hit has connected hits
    */
    bool findConnectedGanged(const std::unique_ptr<cluster> &clu, hitVector &connectedHits);

    /*!
    * Function which examines whether the ganged pattern recognition suggest the cluster removal
    * @param cluster The cluster to be examined
    * @param connectedHits list of ganged hits that are connected to a confirmed ganged hit
    * @return true if the cluster should be removed, false otherwise
    */
    bool isKilled(const std::unique_ptr<cluster> &clu, const hitVector &connectedHits);

    /*! 
    * Function used in PCM200, to be reimplemented
    */
    double getDeltaX1A(const cluster* clu);
    /*! 
    * Function used in PCM200, to be reimplemented
    */
    double getDeltaX2A(const cluster* clu);

    /*! 
    * Function used in PCM200, to be reimplemented
    */
    int getDeltaXEC1A(const cluster* clu);

    /*! 
    * Function used in PCM200, to be reimplemented
    */
    int getDeltaXEC2A(const cluster* clu);

    /*! 
    * Function used in PCM200, to be reimplemented
    */
    int getDeltaYEC1A(const cluster* clu);

    /*! 
    * Function used in PCM200, to be reimplemented
    */
    int getDeltaYEC2A(const cluster* clu);

    /*! 
    * Function used in PCM200, to be reimplemented
    */
    int getDeltaY1A(const cluster* clu);

    /*! 
    * Function used in PCM200, to be reimplemented
    */
    int getDeltaY2A(const cluster* clu);

    /*! 
    * Function used in PCM200, to be reimplemented
    */
    int getDeltaY1B(const cluster* clu);

    /*! 
    * Function used in PCM200, to be reimplemented
    */
    int getDeltaY2B(const cluster* clu);

public:
    FTKClusteringEngine();
    FTKClusteringEngine(bool saveClusterContent, bool diagClustering,
			bool sctClustering, bool ibl3DRealistic,
                        bool duplicateGanged, bool gangedPatternRecognition,
			bool splitBlayerModules, int pixelClusteringMode);
    void atlClusteringBlayer(std::vector<FTKRawHit> &);
    void atlClusteringLNF(std::vector<FTKRawHit> &);
};

#endif // FTKCLUSTERINGENGINE_H
