// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_SLIDINGWINDOWFINDER_H
#define CALOREC_SLIDINGWINDOWFINDER_H
/********************************************************************

NAME:     SlidingWindowFiner.h
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Jan 2000

PURPOSE:  Class to search for cluster in LArTowerCollection
           initialized with cluster size (n,m), which means (2n+1)X(2m+1) 
                  Et threshold, 
                  and a pointer to a LArTowerContainer in the constructor.  
          public methods: 
            nextcluster() returns true if new cluster found.
            eta(), phi() returns the location of the new cluster. 
            cell_begin(), cell_end() returns iterators to cells
			  in the cluster. 

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Oct. 2000       (SR)
          to use Tower Collection instead

Updated:  Jan 3, 2001    (HM)
          QA. 

Updated:  copied from LArClusterRec to CaloRec.
          Renamed from LArSWFinder to SlidingWindowFinder

Updated:  Apr 2, 2004  (PLoch)
          Adapt to new CaloCluster design

Updated:  Sep 6, 2005  (DLelas) 
          Modifications needed for proper filling of LAr clusters
          -Combined clustering is unchanged. 
          -LAr clustering: Only seeds and zero cells are passed to
           CaloClusterBuilderSW. LAr cells are put later in 
           CaloFillRectangularCluster, a derivative of CaloComputeEMlayers.
          -Note, extra ring(nextra) is meaningful only for combined clusters.

Updated:  Dec, 2006 (sss)
          Speed up & clean up.

********************************************************************/

// INCLUDE HEADER FILES:


#include "CaloEvent/CaloTowerContainer.h"

class CaloCell;
class CaloCellContainer;

#include <vector>
#include <utility>
#include <cassert>

class SlidingWindowFinder
{
 public:

  typedef  std::pair<const CaloCellContainer*,size_t> data_type;
  typedef  std::vector<data_type>                     data_store;
  typedef  data_store::const_iterator                 data_iterator;

  /**
   * @brief Constructor.
   * @param neta eta size of the window (real size is neta*2 + 1).
   * @param nphi phi size of the window (real size is nphi*2 + 1).
   * @param thresh Energy threshold for cluster finding.
   * @param towerColl Container of calorimeter towers.
   * @param FillClusterCells If true, then make a list of the cells
   *                         in found clusters.
   * @param nextra Extra towers on each size to be stored in the cluster's
   *               cell list.
   */
  SlidingWindowFinder (int neta,
		       int nphi,
		       double threshold,
		       const CaloTowerContainer* towerColl,
		       bool FillClusterCells = true,
		       int nextra = 0);

  /// Return the eta position of the last found cluster.
  int  eta() const;

  /// Return the phi position of the last found cluster.
  int  phi() const ;

  /// Return a starting iterator over the cells in the last found cluster.
  data_iterator cell_begin() const; 

  /// Return an ending iterator over the cells in the last found cluster.
  data_iterator cell_end() const;

  /// Return the cell pointer from a cell iterator.
  const CaloCell*           getCell(const data_iterator& iter) const;

  /// Return the container pointer from a cell iterator.
  const CaloCellContainer*  getCellContainer(const data_iterator& iter) const;

  /// Return the cell index from a cell iterator.
  size_t                    getCellIndex(const data_iterator& iter) const;

  /**
   * @brief Search for another cluster.
   * @return True if one is found; false if no more.
   */
  bool nextcluster(); 


 private:
  /// Type used for Et calculation.
  typedef  float ettype;

  // --- Private methods ---
  /// Initialize cached array of Et values.
  void init_et ();

  /// Return pointer to array of Et values for a given ieta slice.
  ettype* etslice (int ieta);
  const ettype* etslice (int ieta) const;

  /**
   * @brief Sum over one all phi cells in one eta strip of the cluster.
   * @param jx The ieta index of the slice.
   * @param jy The first iphi index to sum.
   * @param jymax The last iphi index to sum (inclusive).
   * @return The Et sum over cells.
   */
  ettype sum_etastrip (int jx, int jy, int jymax) const;

  /**
   * @brief Shift the window one position in eta.
   * @param sn Sum of phi cells for new far-right eta strip.
   * @return The total energy in the window.
   */
  float shift_window (float sn);
  
  /**
   * @brief Test to see if we're at a local maximum in eta.
   * @return True if we're at a local maximum; false otherwise.
   */
  bool test_etamax() const;

  /**
   * @brief Test to see if we're at a local maximum in eta.
   * @param j1 Index of 1 row higher than the cluster.
   * @param j2 Index of upper cluster row.
   * @param j3 Index of lower cluster row.
   * @param j4 Index of 1 row lower than the cluster.
   * @return True if we're at a local maximum; false otherwise.
   */
  bool test_phimax(int j1, int j2, int j3, int j4) const;

  // --- Configuration parameters ---
  /// Eta cluster size (actual size is 2*m_neta+1).
  int  m_neta;

  /// Phi cluster size (actual size is 2*m_nphi+1).
  int  m_nphi; 

  /// Et threshold for the cluster.
  double m_ethr;

  /// Source collection of towers.
  const CaloTowerContainer* m_towerColl; 

  /// If true, make a list of cells found clusters.
  bool m_FillClusterCells;

  /// Extra towers on each side to be stored in cluster.
  int m_nextra; 


  // --- Other variables set during initialization ---
  /// Size of the tower matrix.
  int  m_meta;
  int  m_mphi; 

  /// Cached array of tower Ets.
  //  This is laid out in the same manner as the towers:
  //   index = (ieta-1) * m_mphi + (iphi-1)
  //  (Note: ieta and iphi are 1-based.)
  std::vector<ettype> m_et;


  // --- Loop variables ---
  /// Current eta, phi positions.
  int  m_ieta;
  int  m_iphi;	

  /// True if the last call found a cluster.
  bool m_found;

  /// List of cells in the last cluster found.
  data_store m_cells;
    
  /// Array of energy sums in eta.
  // Each element of this array is a sum of cells over phi within the window
  // in one eta strip.  This array contains the eta values within the
  // window, plus one extra strip on each side.  So, the center of the
  // window is at index m_neta+1; the left-most strip in the window
  // is at index 1, and the right-most strip in the window is at the
  // next-to-last index.
  std::vector<float> m_ss;
};


#include "CaloUtils/SlidingWindowFinder.icc"


#endif 
