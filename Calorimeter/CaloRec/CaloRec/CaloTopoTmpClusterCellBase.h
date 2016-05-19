/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoTmpClusterCellBase.h,v 1.6 2008-12-23 02:57:16 ssnyder Exp $
//
// Description: Base class for the ClusterCell Container for the 
//              topological cluster making tools
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

#ifndef CALOTOPOTMPCLUSTERCELLBASE_H
#define CALOTOPOTMPCLUSTERCELLBASE_H

#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloCell_ID.h"

class CaloCellContainer;
class CaloCell;
class CaloTopoTmpClusterCellBase {

private:

  // Friends
  
  // Data members

  size_t m_iCell;
  CaloCell_ID::SUBCALO m_subDet;

  /// Global calorimeter hash.
  IdentifierHash m_idHash;
  float m_signedRatio;
  bool m_used;

public:
  
  // Constructors

  CaloTopoTmpClusterCellBase(const IdentifierHash & idHash, const CaloCell_ID::SUBCALO subDet, const size_t &iCell, const float & signedRatio) 
    : m_iCell(iCell),
      m_subDet(subDet),
      m_idHash(idHash),
      m_signedRatio(signedRatio),
      m_used(false)
  {
  }

  // Destructors

  // Methods
  
  inline const IdentifierHash & getID() const 
  {
    return m_idHash;
  }

  
  inline const size_t & getCaloCell() const
  {
    return m_iCell;
  }

  inline CaloCell_ID::SUBCALO getSubDet() const
  {
    return m_subDet;
  }

  inline const float & getSignedRatio() const
  {
    return m_signedRatio;
  }

  inline void setSignedRatio(const float & signedRatio)
  {
    m_signedRatio = signedRatio;
  }

  inline bool getUsed() const
  {
    return m_used;
  }

  inline void setUsed()
  {
    m_used = true;
  }

  inline void setUnused()
  {
    m_used = false;
  }
};

#endif // CALOTOPOTMPCLUSTERCELLBASE_H

