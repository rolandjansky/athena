///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOTOPOTOWERCONTAINER_H
#define CALOEVENT_CALOTOPOTOWERCONTAINER_H
/********************************************************************

NAME:     CaloTopoTowerContainer.h
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  Ian M. Nugent <inugent@triumf.ca>
CREATED:  March 15, 2010
          Based on CaloTowerConatainer

PURPOSE:  container class for CaloTopoTower objects.
          Baseclass: ObjectVector<CaloTower>. 
          The tower objects are stored as CaloTopoTower in ObjectVector
	  In addition to providing  iterators over all towers, 
	  the towers (and their et and energy) can be access with 
          index (ieta,iphi), or in real (eta,phi) space. 


UPDATE:   March 15, 2010
	  add CLID

********************************************************************/

// INCLUDE HEADER FILES:
#include "AthenaKernel/CLASS_DEF.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCell2ClusterMap.h"
#include "CaloEvent/CaloTowerDataVector.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "AthLinks/DataLink.h"

#include <utility>
#include <vector>
#include <string>

class CaloTopoTowerContainer : public CaloTowerContainer
{
public:

  /////////////////////////////
  // Constructors/Destructor //
  /////////////////////////////

  /*! \brief Default constructor
   *
   *  Instantiates a \a CaloTopoTowerContainer without a
   *  \f$ \Delta \eta \times \Delta \varphi \f$ grid definition. Limited use
   *  only.
   */ 
  CaloTopoTowerContainer();
  /*! \brief Useable Constructor
   *
   *  \param tseg  input a \a CaloTowerSeg object containing a
   *               \f$ \Delta \eta \times \Delta \varphi \f$ tower grid
   *               description
   *  \param noTowers If true, don't create the contained towers.
   *
   *  Creates an empty \a CaloTower at each \f$ ( \eta, \varphi ) \f$ 
   *  location of the grid.
   */   
  CaloTopoTowerContainer(const CaloTowerSeg& tseg, bool noTowers = false); 

  /*! \brief Copy constructor. */
  CaloTopoTowerContainer (const CaloTopoTowerContainer& other);

  /*! \brief Assignment. */
  CaloTopoTowerContainer& operator= (const CaloTopoTowerContainer& other);

  /*! \brief Swap. */
  void swap (CaloTopoTowerContainer& other);

  /*! \brief Destructor */ 
  virtual ~CaloTopoTowerContainer();

  //////////////////////////////
  //TopoTower Info
  //////////////////////////////
  DataLink<CaloClusterContainer> GetClusters() const;
  DataLink<CaloTowerContainer> GetTowers() const;
  DataLink<CaloCellContainer> GetCells() const;
  const CaloCell2ClusterMap*  GetCellToClusterMap() const;

  // Selection criteria
  double GetMinimumCellEnergy() const;
  double GetMinimumClusterEnergy() const;
  bool   GetUseCellWeights() const;

  // Noise stuff
  float GetNoiseSigma() const;
  float GetCellESignificanceThreshold() const;

  // List of calorimeters from which to use cells
  // ??? Redundant with regions in base class?
  std::vector<CaloCell_ID::SUBCALO> GetCaloIndices() const;
  bool GetCaloSelection() const;

  void SetClusters(const CaloClusterContainer* c);
  void SetTowers(const CaloTowerContainer* c);
  void SetCells(const CaloCellContainer* c);
  void SetCellToClusterMap(const CaloCell2ClusterMap*  c);

  // Selection criteria
  void SetMinimumCellEnergy(double c);
  void SetMinimumClusterEnergy(double c);
  void SetUseCellWeights(bool c);

  // Noise stuff
  void SetNoiseSigma(float c);
  void SetCellESignificanceThreshold(float c);

  // List of calorimeters from which to use cells
  void SetCaloIndices(const std::vector<CaloCell_ID::SUBCALO>& c);
  void SetCaloSelection(bool c);

 
 private:
  friend class CaloTopoTowerContainerCnv_p1;

  DataLink<CaloClusterContainer>  m_clusters;
  DataLink<CaloTowerContainer> m_towers;
  DataLink<CaloCellContainer>  m_cells;
  const CaloCell2ClusterMap*  m_celltoclustermap;

  // Selection criteria
  double m_minimumCellEnergy;
  double m_minimumClusterEnergy;
  bool   m_useCellWeights;

  // Noise stuff
  float m_noiseSigma;
  float m_cellESignificanceThreshold;

  // List of calorimeters from which to use cells
  std::vector<CaloCell_ID::SUBCALO> m_caloIndices;
  bool m_caloSelection;

};
CLASS_DEF( CaloTopoTowerContainer , 1140015421 , 1 )
SG_BASE(CaloTopoTowerContainer, CaloTowerContainer );


///////////////////
//TopoTower
//////////////////
inline DataLink<CaloClusterContainer> CaloTopoTowerContainer::GetClusters() const
{return m_clusters;}
inline DataLink<CaloTowerContainer> CaloTopoTowerContainer::GetTowers() const
{return m_towers;}
inline DataLink<CaloCellContainer> CaloTopoTowerContainer::GetCells() const
{return m_cells;}
inline const CaloCell2ClusterMap* CaloTopoTowerContainer::GetCellToClusterMap() const
{return m_celltoclustermap;}

// Selection criteria
inline double CaloTopoTowerContainer::GetMinimumCellEnergy() const
{return m_minimumCellEnergy;}
inline double CaloTopoTowerContainer::GetMinimumClusterEnergy() const
{return m_minimumClusterEnergy;}
inline bool   CaloTopoTowerContainer::GetUseCellWeights() const
{return m_useCellWeights;}

// Noise stuff
inline float CaloTopoTowerContainer::GetNoiseSigma() const
{return m_noiseSigma;}
inline float CaloTopoTowerContainer::GetCellESignificanceThreshold() const
{return m_cellESignificanceThreshold;}

// List of calorimeters from which to use cells
inline std::vector<CaloCell_ID::SUBCALO>  CaloTopoTowerContainer::GetCaloIndices() const
{ return m_caloIndices;}
inline bool CaloTopoTowerContainer::GetCaloSelection() const
{return m_caloSelection;}


inline void CaloTopoTowerContainer::SetClusters(const CaloClusterContainer* c){ m_clusters=c;}
inline void CaloTopoTowerContainer::SetTowers(const CaloTowerContainer* c){m_towers=c;}
inline void CaloTopoTowerContainer::SetCells(const CaloCellContainer* c){ m_cells=c;}
inline void CaloTopoTowerContainer::SetCellToClusterMap(const CaloCell2ClusterMap* c){m_celltoclustermap=c;}

// Selection criteria
inline void CaloTopoTowerContainer::SetMinimumCellEnergy(double c){ m_minimumCellEnergy=c;}
inline void CaloTopoTowerContainer::SetMinimumClusterEnergy(double c){ m_minimumClusterEnergy=c;}
inline void CaloTopoTowerContainer::SetUseCellWeights(bool c){ m_useCellWeights=c;}

// Noise stuff
inline void CaloTopoTowerContainer::SetNoiseSigma(float c){  m_noiseSigma=c;}
inline void CaloTopoTowerContainer::SetCellESignificanceThreshold(float c){ m_cellESignificanceThreshold=c;}

// List of calorimeters from which to use cells
inline void CaloTopoTowerContainer::SetCaloIndices(const std::vector<CaloCell_ID::SUBCALO>& c){m_caloIndices=c;}
inline void CaloTopoTowerContainer::SetCaloSelection(bool c){ m_caloSelection=c;}


/*! \class CaloTopoTowerContainer
 *
 *  \brief Storable container class for \a CaloTower
 *
 *  \author P. Loch <inugent@triumf.ca>
 *  \date March 15, 2010
 *  \version 1
 *
 *  The \a CaloTopoTowerContainer holds an ordered list of pointers to
 *  \a CaloTower s, where the index of a given tower in the list is
 *  determined by its \f$ ( \eta, \varphi ) \f$ location on the tower
 *  grid. When persistified, only the tower grid as stored in a
 *  \a CaloTowerSeg object held by the \a CaloTopoTowerContainer object,
 *  and the list of contributing calorimeters are saved to disk. The
 *  \a CaloTopoTowerContainer is refilled on the fly using this information when
 *  retrieved from the persistent store into the transient store 
 *  (requires original \a CaloCellContainer to be available also).
 *  
 */
#endif 
