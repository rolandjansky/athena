// Yo emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoSplitterHashCluster.h,v 1.5 2008-12-23 02:57:16 ssnyder Exp $
//
// Description: temporary Cluster Container for topological cluster splitter
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

#ifndef CALOTOPOSPLITTERHASHCLUSTER_H
#define CALOTOPOSPLITTERHASHCLUSTER_H

#include "CaloRec/CaloTopoTmpHashClusterBase.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Geometry/Vector3D.h>

class CaloCluster;
class CaloTopoSplitterClusterCell;
class CaloTopoSplitterHashCluster
  : public CaloTopoTmpHashClusterBase<CaloTopoSplitterClusterCell>
{
  typedef CaloTopoTmpHashClusterBase<CaloTopoSplitterClusterCell> Base;
  typedef CaloTopoTmpHashCell<CaloTopoSplitterClusterCell> HashCell;

private:

  // Friends
  
  // Data members

  const xAOD::CaloCluster *m_parentCluster;
  size_t m_parentClusterIndex;
  float m_energy;
  bool m_hasValidEnergy;
  bool m_containsLocalMax;
  HepGeom::Vector3D<double> * m_centroid;
  // Helper functions

public:
  
  // Constructors
  CaloTopoSplitterHashCluster (pool_type& pool)
    : Base (pool)
  {
    m_parentCluster = 0;
    m_parentClusterIndex =0;
    m_energy = 0;
    m_hasValidEnergy = false;
    m_containsLocalMax = false;
    m_centroid = 0;
  }

  // Destructors
  ~CaloTopoSplitterHashCluster();

  // Operators
  
  // Selectors 

  inline const xAOD::CaloCluster * getParentCluster() const
  {
    return m_parentCluster;
  }

  inline size_t getParentClusterIndex() const
  {
    return m_parentClusterIndex;
  }

  void removeAll() 
  {
    Base::removeAll();
    m_parentCluster = 0;
    m_energy = 0;
    m_hasValidEnergy = false;
    m_containsLocalMax = false;
  }

  void add(const HashCell& hashCell);

  void remove(const HashCell& hashCell);

  void add(CaloTopoSplitterHashCluster& hashCluster);
  
  float getEnergy();

  const HepGeom::Vector3D<double> * getCentroid();

  inline bool getContainsLocalMax() const
  {
    return m_containsLocalMax;
  }

  inline void setContainsLocalMax()
  {
    m_containsLocalMax = true;
  }

 private:

  void calcEnergy();

  void calcCentroid();

};


#endif // CALOTOPOSPLITTERHASHCLUSTER_H

