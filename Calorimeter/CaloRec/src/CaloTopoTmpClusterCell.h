// Yo emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
//
// Description: ClusterCell Container for the topological cluster maker
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

#ifndef CALOTOPOTMPCLUSTERCELL_H
#define CALOTOPOTMPCLUSTERCELL_H

#include "CaloTopoTmpClusterCellBase.h"
class CaloTopoTmpHashCluster;

class CaloTopoTmpClusterCell: public CaloTopoTmpClusterCellBase 
{

private:

  // Friends
  
  // Data members

  CaloTopoTmpHashCluster *m_cluster;
  float m_signedEt;

public:
  
  // Constructors

  CaloTopoTmpClusterCell(const IdentifierHash & idHash, const CaloCell_ID::SUBCALO subDet, const size_t & iCell, const float & signedRatio, const float & signedEt) 
    : CaloTopoTmpClusterCellBase(idHash,subDet,iCell,signedRatio),
    m_cluster (0),
    m_signedEt(signedEt)
  {  
  }

  inline const float & getSignedEt() const
  {
    return m_signedEt;
  }

  inline const CaloTopoTmpHashCluster *getCaloTopoTmpHashCluster() const
  {
    return m_cluster;
  }

  inline CaloTopoTmpHashCluster *getCaloTopoTmpHashCluster()
  {
    return m_cluster;
  }

  inline void setCaloTopoTmpHashCluster(CaloTopoTmpHashCluster * cluster) 
  {
    m_cluster = cluster;
  }

};

#endif // CALOTOPOTMPCLUSTERCELL_H

