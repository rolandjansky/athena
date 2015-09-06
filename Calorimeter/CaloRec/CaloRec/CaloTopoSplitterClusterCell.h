// Yo emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoSplitterClusterCell.h,v 1.7 2008-12-23 02:57:16 ssnyder Exp $
//
// Description: ClusterCell Container for the topological cluster splitter
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

#ifndef CALOTOPOSPLITTERCLUSTERCELL_H
#define CALOTOPOSPLITTERCLUSTERCELL_H

#include "CaloRec/CaloTopoTmpClusterCellBase.h"
#include "xAODCaloEvent/CaloCluster.h"

class CaloTopoSplitterHashCluster;

class CaloTopoSplitterClusterCell: public CaloTopoTmpClusterCellBase 
{

private:

  // Friends
  
  // Data members

  CaloTopoSplitterHashCluster *m_cluster;
  const xAOD::CaloCluster *m_parentCluster;
  xAOD::CaloCluster::cell_iterator m_itrCell;
  size_t m_parentClusterIndex;
  CaloTopoSplitterHashCluster *m_secondCluster;
  bool m_shared;
  float m_weight;
  bool m_secondary;

public:
  
  // Constructors

  CaloTopoSplitterClusterCell(const IdentifierHash & idHash,
                              const CaloCell_ID::SUBCALO subDet,
                              xAOD::CaloCluster::cell_iterator & itrCell,
                              const size_t & iCell, //This is actually obsolete, can be obtained from iterator
                              const float & signedRatio,
                              const xAOD::CaloCluster *pClus,
                              const size_t caloClusterIndex,
                              const bool isSecondary=false) 
    : CaloTopoTmpClusterCellBase(idHash,subDet,iCell,signedRatio),
      m_cluster(0),
      m_parentCluster(pClus),
      m_itrCell(itrCell),
      m_parentClusterIndex(caloClusterIndex),
      m_secondary(isSecondary)
  {  
    m_shared = false;
    m_secondCluster = 0;
    m_weight=1.;
  }

  inline const xAOD::CaloCluster * getParentCluster() const
  {
    return m_parentCluster;
  }

  inline const xAOD::CaloCluster::cell_iterator & getCellIterator() const
  {
    return m_itrCell;
  }

  inline size_t getParentClusterIndex() const
  {
    return m_parentClusterIndex;
  }

  inline bool getShared() const
  {
    return m_shared;
  }

  inline void
  setSecondCaloTopoTmpHashCluster(CaloTopoSplitterHashCluster * cluster) 
  {
    m_secondCluster = cluster;
  }

  inline CaloTopoSplitterHashCluster * getSecondCaloTopoTmpHashCluster()
  {
    return m_secondCluster;
  }

  inline void setShared()
  {
    m_shared = true;
  }

  inline void setSharedWeight(const float &weight)
  {
    m_weight = weight;
  }

  inline const float & getSharedWeight()
  {
    return m_weight;
  }

  inline void setUnshared()
  {
    m_shared = false;
  }

  inline CaloTopoSplitterHashCluster *getCaloTopoTmpHashCluster() const
  {
    return m_cluster;
  }

  inline void setCaloTopoTmpHashCluster(CaloTopoSplitterHashCluster * cluster) 
  {
    m_cluster = cluster;
  }
  
  inline bool getSecondary() const
  {
    return m_secondary;
  }
  
  
  
};

#endif // CALOTOPOSPLITTERCLUSTERCELL_H

