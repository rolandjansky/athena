// Yo emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoTmpHashCluster.h,v 1.6 2008-12-23 02:57:16 ssnyder Exp $
//
// Description: temporary Cluster Container for topological cluster maker
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

#ifndef CALOTOPOTMPHASHCLUSTER_H
#define CALOTOPOTMPHASHCLUSTER_H

#include "CaloRec/CaloTopoTmpHashClusterBase.h"
class CaloTopoTmpClusterCell;

class CaloTopoTmpHashCluster
  : public CaloTopoTmpHashClusterBase<CaloTopoTmpClusterCell>
{
  typedef CaloTopoTmpHashClusterBase<CaloTopoTmpClusterCell> Base;
  typedef CaloTopoTmpHashCell<CaloTopoTmpClusterCell> HashCell;
private:

  // Friends
  
  // Data members

  float m_maxEt;
  // Helper functions

public:
  
  // Constructors
  CaloTopoTmpHashCluster (pool_type& pool)
    : Base (pool)
  {
    m_maxEt = 0;
  }

  // Destructors

  // Operators
  
  // Selectors 

  inline float getMaxEt() const
  {
    return m_maxEt;
  }

  void removeAll() 
  {
    Base::removeAll();
    m_maxEt = 0;
  }

  void add(const HashCell& hashCell);

  void add(CaloTopoTmpHashCluster& hashCluster);

};


#endif // CALOTOPOTMPHASHCLUSTER_H

