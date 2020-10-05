/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     SlidingWindowFinder.cxx
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

Updated:  Feb 5, 2001    (HM)
          Moved from LArClusterRec to CaloRec.

Updated:  Apr 2, 2004    (PLoch)
          Adapt to new CaloTower/CaloCluster design

Updated:  Sep 6, 2005    (DLelas)
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
#include "CaloUtils/SlidingWindowFinder.h"
#include <algorithm>


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
SlidingWindowFinder::SlidingWindowFinder (int neta,
					  int nphi,
					  double threshold,
					  const CaloTowerContainer* towerColl,
					  bool FillClusterCells /*= true*/,
					  int nextra /*= 0*/)
  : m_neta (neta),
    m_nphi (nphi),
    m_ethr (threshold),
    m_towerColl (towerColl),
    m_FillClusterCells (FillClusterCells),
    m_nextra (nextra),
    m_meta (m_towerColl->neta()),
    m_mphi (m_towerColl->nphi()),
    m_et (m_meta * m_mphi),
    m_ieta (0),
    m_iphi (0),
    m_found (false),
    m_ss (m_neta*2 + 3)
{
  init_et();
}


/**
 * @brief Return pointer to array of Et values for a given ieta slice.
 * @param ieta ieta slice index (1-based)
 * @return Pointer to the element at (ieta, 1).
 */
inline SlidingWindowFinder::ettype* SlidingWindowFinder::etslice (int ieta)
{
  return &m_et[(ieta-1) * m_mphi - 1];
}


/**
 * @brief Return pointer to array of Et values for a given ieta slice.
 * @param ieta ieta slice index (1-based)
 * @return Pointer to the element at (ieta, 1).
 */
inline const SlidingWindowFinder::ettype*
SlidingWindowFinder::etslice (int ieta) const
{
  return &m_et[(ieta-1) * m_mphi - 1];
}


/**
 * @brief Helper to allow inlining the operation of getting the energy
 *        and eta from a tower.  (This really should be added in FourVec...)
 */
class CaloTowerEEtaGetter
  : public CaloTower
{
public:
  double e_inline() const { return m_e; }
  double eta_inline() const { return m_eta; }
};


/**
 * @brief Initialize the cached array of Et values.
 *
 * Note: this code assumes that all towers with the same ieta
 * have the same eta.  It looks like that's consistent with
 * the present CaloTowerContainer code, but does that handle
 * misalignments correctly?
 *
 * This isn't the most straightforward way to write this,
 * but as this takes a significant portion of the cluster finding time,
 * it has been optimized to generate tight code and minimize memory
 * references for the inner loop on ia86 with gcc.
 *
 * The main idea is that we only need to calculate sin(theta) once
 * per ieta slice.  The secondary counter jphi is used to tell when
 * we go to a new ieta slice.  Some care is needed to properly handle
 * possibly missing tower pointers.
 *
 * Since the indexing we're using in m_et is the same as that used
 * in the tower container, we only need a single loop.
 */
void SlidingWindowFinder::init_et()
{
  ettype* et = &*m_et.begin();
  ettype* et_end = et + m_meta * m_mphi;
  int mphi = m_mphi;
  double sinth = 0;
  CaloTowerContainer::const_iterator tower_it = m_towerColl->begin();
  int jphi = 1;

  // Loop over all cells.
  while (et < et_end) {
    const CaloTowerEEtaGetter* tower;
    if (--jphi != 0) {
      // Same ieta slice --- we can use the sinth value we've previously
      // calculated.  Just get the tower pointer.
      // (This is on the hot path of the loop.)
      tower = reinterpret_cast<const CaloTowerEEtaGetter*> (*tower_it);
      ++tower_it;
    }
    else {
      // New ieta slice.  Reset jphi and get the tower.
      // (This is out of the hot path of the loop.)
      jphi = mphi;
      tower = reinterpret_cast<const CaloTowerEEtaGetter*> (*tower_it);
      ++tower_it;

      // If this tower is missing, keep going until we find one
      // (or we reach the end).  Be sure to keep the jphi counter
      // up-to-date.
      while (tower == nullptr && et < et_end) {
        *et++ = 0;
        if (--jphi == 0) jphi = mphi;
        tower = reinterpret_cast<const CaloTowerEEtaGetter*> (*tower_it);
        ++tower_it;
      }

      // Stop if we hit the end.
      if (!tower) break;

      // Recalculate sinth from this tower.
      sinth = 1 / cosh (tower->eta_inline());
    }

    if (tower) {
      // Have a tower --- calculate the et.
      *et = static_cast<float> (tower->e_inline() * sinth);
    }
    else {
      // No tower --- no energy.
      *et = 0;
    }
    ++et;
  }
}


/**
 * @brief Sum over one all phi cells in one eta strip of the cluster.
 * @param jx The ieta index of the slice.
 * @param jy The first iphi index to sum.
 * @param jymax The last iphi index to sum (inclusive).
 * @return The Et sum over cells.
 */
inline
SlidingWindowFinder::ettype
SlidingWindowFinder::sum_etastrip (int jx, int jy, int jymax) const
{
  // We spend a significant amount of time in this loop, so be careful here.
  // This is written to that the common case of no wrapping should
  // be fast.

  const ettype* ets = etslice (jx);
  ettype sn = 0;

  // Handle phi wraparound.
  if (jymax < jy) {
    int matrixm_nphi = m_mphi; 
    for (; jy <= matrixm_nphi; ++jy)
    {
      sn += ets[jy];
    }
    jy = 1;
  }

  for (; jy <= jymax; ++jy)
  {
    sn += ets[jy];
  }

  return sn;
}


/**
 * @brief Shift the window one position in eta.
 * @param sn Sum of phi cells for new far-right eta strip.
 * @return The total energy in the window.
 *
 * Slide all elements in m_ss to the left, and replace the
 * last element with @sn.  Find the sum over all elements
 * now in the window (which doesn't include @c sn at this time).
 */
inline
float SlidingWindowFinder::shift_window (float sn)
{
  float* ss = &*m_ss.begin();
  float* ss_end = ss + m_ss.size() - 1;

  // Shift the first element --- it's not included in the sum.
  ss[0] = ss[1];
  ++ss;

  // Shift the cluster body, summing it in the process.
  float en = 0;
  for (; ss < ss_end; ++ss) {
    float si = ss[1];
    en += si;
    *ss = si;
  }

  // The last element (not included in the sum).
  *ss = sn;

  // Return cluster energy sum.
  return en;
}


/**
 * @brief Test to see if we're at a local maximum in eta.
 * @return True if we're at a local maximum; false otherwise.
 */
inline
bool SlidingWindowFinder::test_etamax() const
{
  int neta = m_neta;
  const float* ss_neta1 = &*m_ss.begin() + neta + 1;
  float sl = 0;
  float sr = 0;
  for(int m=neta;m>=0;m--)
  {
    // sliding in eta, left and right.  
    sl+=ss_neta1[-m-1] - ss_neta1[m];
    sr+=ss_neta1[m+1]  - ss_neta1[-m]; 
    //           check a local maximum in eta:
    if (sl>0 || sr>0) return false;
    if (sl*sr  != 0) break;
    // if no change for one direction, keep going.   
  }
  return true;
}


/**
 * @brief Test to see if we're at a local maximum in eta.
 * @param j1 Index of 1 row higher than the cluster.
 * @param j2 Index of upper cluster row.
 * @param j3 Index of lower cluster row.
 * @param j4 Index of 1 row lower than the cluster.
 * @return True if we're at a local maximum; false otherwise.
 */
inline
bool SlidingWindowFinder::test_phimax(int j1, int j2, int j3, int j4) const
{
  int nphi = m_nphi;
  int neta = m_neta;
  int matrixm_neta = m_meta; 
  int matrixm_nphi = m_mphi; 
  float su = 0;
  float sd = 0;
  for( int m=nphi;m>=0;m--)
  {
    int jmin=((m_ieta-neta)>1)?(m_ieta-neta):1 ;
    int jmax=((m_ieta+neta)<matrixm_neta)?(m_ieta+neta):matrixm_neta; 
    const ettype* ets_max = etslice (jmax);
    for (const ettype* ets = etslice (jmin);
         ets <= ets_max;
         ets += matrixm_nphi)
    {
      su += ets[j1] - ets[j3];
      sd += ets[j4] - ets[j2];
    }
    //           check a vertical local maximum
    if (su>0 || sd>0) break;
    if (su * sd > 0)  return true;
    j1=j2; j2=j2+1; if (j2>matrixm_nphi) j2 = 1;
    j4=j3; j3=j3-1; if (j3==0) j3 = matrixm_nphi;				    
  }
  return false;
}


/**
 * @brief Search for another cluster.
 * @return True if one is found; false if no more.
 *
 * The algorithm is copied from calocand.age in atrecon. 
 *                                                        H.Ma... Jan 2000 
 */
bool SlidingWindowFinder::nextcluster()
{
  // look for next cluster in the Et matrix
  
  // map the variables 
  int matrixm_neta = m_meta; 
  int matrixm_nphi = m_mphi; 

  int neta1 = m_neta+1; 

  int i1=0,i2=0,i3=0,i4=0; 
  float* ss = &*m_ss.begin();
  int neta = m_neta;
  int nphi = m_nphi;
	
  m_found=false ; 

  // :position: loop
  for(;;)
  {  // loop 
    if(m_iphi==0 || m_ieta>=matrixm_neta)     //  "  next phi slice  "
    {   // start of a new line - clear everything
      m_iphi=m_iphi+1;
      m_ieta=-neta-1;
      std::fill (ss, ss + neta1*2+1, 0);
      i1 = 0;
    }

    if (i1 == 0) {
      // prepare index in phi. 
      i2=m_iphi-nphi;  
      if (i2<=0)          i2+=matrixm_nphi ;    //! upper row
      i1=i2-1;         
      if (i1<=0)          i1+=matrixm_nphi ;    //! 1 row higher the cluster
      i3=m_iphi+nphi;  
      if (i3>matrixm_nphi) i3-=matrixm_nphi ;    //! bottom row
      i4=i3+1;         
      if (i4>matrixm_nphi) i4-=matrixm_nphi ;    //! 1 row below the cluster
    }
	
    // We're done if we go outside of the calorimeter.
    if (m_iphi>matrixm_nphi) return false ;
			  
    // Look forward before move the window to next position
    m_ieta=m_ieta+1;
    int jx=m_ieta+neta+1; 
	 
    // en is current energy sum in the cluster, 
    // sn is energy in the next eta strip, summed over phi. 
    float sn = 0;
    if (1<=jx && jx<=matrixm_neta)    //  "  still inside calo   "
      sn = sum_etastrip (jx, i2, i3);

    // Now shift the window left and sum the cluster energy.
    float en = shift_window (sn);

    // Check " preliminary thresh " m_ieta>0 & en  > thre
    if (m_ieta<=0 || en < m_ethr) continue ; 
    // m_ieta<=0 means the center is not in the matrix yet, keep moving.
    // energy sum has to be above threshold. 

    // Test to see if we're at a local maximum in eta/phi.
    // If so, exit the loop.
    if (!test_etamax()) continue;
    if (test_phimax(i1,i2,i3,i4)) break;
  }

  // Found a cluster.
  m_found = true; 
  m_cells.clear();

  //
  // collect cells according to a flag (i.e. only for combined clusters)
  //

  if(m_FillClusterCells){
	 
    // cluster size, taking into account the extra ring
    int ne = neta+m_nextra;
    int np = nphi+m_nextra;
	 
    int imin=((m_ieta- ne)>1)?(m_ieta- ne):1 ;
    int imax=((m_ieta+ ne)<matrixm_neta)?(m_ieta+ ne):matrixm_neta; 
    for(int i=imin;i<=imax;i++)
    {
      for(int j0=m_iphi- np;j0<=m_iphi+ np;j0++)
      {
        int j = j0;
        if (j<=0)j=j+matrixm_nphi ;
        if (j>matrixm_nphi) j=j-matrixm_nphi ;
		 
        const CaloTower* tower = m_towerColl->getTower(i,j);
        if (nullptr != tower) 
        {
          CaloTower::cell_iterator it1 = tower->cell_begin();
          CaloTower::cell_iterator it2 = tower->cell_end();
		     
          for(;it1!=it2;it1++)
          {
            const CaloCell* cell_ptr = (*it1);
            size_t cell_index = 0;
            if ( tower->getCellIndex(cell_ptr,cell_index) )
            {
              const CaloCellContainer* cont_ptr = 
                tower->getCellContainer(cell_ptr);
              data_type theData(cont_ptr,cell_index);
              m_cells.push_back(theData);
            }
          }
        }
	
      }
    }	       
  }

  // std::cout << "SlidingWindowFinder: number of cells before sort: " << m_cells.size() << std::endl;
  // m_cells.sort();	
  // m_cells.unique();
  // std::cout << "SlidingWindowFinder: number of cells after sort: " << m_cells.size() << std::endl;

  return m_found; 
}	












