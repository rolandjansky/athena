/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoSplitterHashCluster.cxx,v 1.8 2008-12-23 02:57:16 ssnyder Exp $
//
// Description: see CaloTopoSplitterHashCluster.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloTopoSplitterHashCluster.h"
#include "CaloTopoSplitterClusterCell.h"


//----------------------------
// This Class's Base Header --
//----------------------------

//---------------
// C++ Headers --
//---------------

//---------------
// C Headers --
//---------------

//----------------
// Constructors --
//----------------

CaloTopoSplitterHashCluster::~CaloTopoSplitterHashCluster()
{
  delete m_centroid;
}
    
//-----------
// Methods --
//-----------

void CaloTopoSplitterHashCluster::add (const HashCell& hashCell)
{
  if ( !m_parentCluster ) {
    CaloTopoSplitterClusterCell* pCell = hashCell.getCaloTopoTmpClusterCell();
    m_parentCluster = pCell->getParentCluster();
    m_parentClusterIndex = pCell->getParentClusterIndex();
  }
  
  Base::add(hashCell);
  m_hasValidEnergy = false;
  if (m_centroid) {
    delete m_centroid;
    m_centroid = nullptr;
  }
}

void CaloTopoSplitterHashCluster::remove (const HashCell& hashCell)
{
  CaloTopoSplitterClusterCell* cell = hashCell.getCaloTopoTmpClusterCell();
  {
    pointer_list::iterator iter =
      std::find (m_members.begin(), m_members.end(), cell);
    if (iter != m_members.end())
      m_members.erase (iter);
  }

  float ratio = cell->getSignedRatio(); 
  if ( ratio >= m_maxRatio ) {
    for(iterator iter = begin(); iter!= end(); ++iter)
    {
      float myRatio = iter->getSignedRatio();
      if ( iter == m_members.begin() || myRatio > m_maxRatio ) 
        m_maxRatio = myRatio;
    }
  }
  m_hasValidEnergy = false;
  if (m_centroid) {
    delete m_centroid;
    m_centroid = nullptr;
  }
}

void CaloTopoSplitterHashCluster::add
  (CaloTopoSplitterHashCluster& rClus)
{
  if ( !m_parentCluster) {
    m_parentCluster = rClus.m_parentCluster;
    m_parentClusterIndex = rClus.m_parentClusterIndex;
  }

  Base::add(rClus);
  m_hasValidEnergy = false;
  if (m_centroid) {
    delete m_centroid;
    m_centroid = nullptr;
  }
}

float CaloTopoSplitterHashCluster::getEnergy()
{
  if ( !m_hasValidEnergy) 
    this->calcEnergy();
  return m_energy;
}

const HepGeom::Vector3D<double> * CaloTopoSplitterHashCluster::getCentroid()
{
  if ( !m_centroid) 
    this->calcCentroid();
  return m_centroid;
}

void CaloTopoSplitterHashCluster::calcEnergy()
{
  if ( ! m_members.empty() ) {
    m_energy = 0;
    for( iterator iter=begin(); iter!= end(); ++iter)
    {
      CaloTopoSplitterClusterCell *pClusCell = *iter;
      xAOD::CaloCluster::cell_iterator itrCell = pClusCell->getCellIterator();
      float myWeight = itrCell.weight();//pClusCell->getParentCluster()->getCellWeight(itrCell);
      if ( pClusCell->getShared() ) {
	if ( pClusCell->getCaloTopoTmpHashCluster() == this ) 
	  myWeight *= pClusCell->getSharedWeight();
	else  
	  myWeight *= (1.-pClusCell->getSharedWeight());
      }	
      m_energy += myWeight*itrCell->e();
    }
  }
  m_hasValidEnergy = true;
}

void CaloTopoSplitterHashCluster::calcCentroid()
{
  if ( m_centroid ) 
    delete m_centroid;

  HepGeom::Vector3D<double> theCentroid(0,0,0);

  if ( !m_members.empty() ) {
    double thisAbsEng,absEng = 0;
    
    for( iterator iter = begin(); iter != end(); ++iter)
    {
      CaloTopoSplitterClusterCell *pClusCell = *iter;
      xAOD::CaloCluster::cell_iterator itrCell = pClusCell->getCellIterator();
      float myWeight = itrCell.weight();//pClusCell->getParentCluster()->getCellWeight(itrCell);
      if ( pClusCell->getShared() ) {
	if ( pClusCell->getCaloTopoTmpHashCluster() == this ) 
	  myWeight *= pClusCell->getSharedWeight();
	else  
	  myWeight *= (1.-pClusCell->getSharedWeight());
      }	

      thisAbsEng = fabs(myWeight*itrCell->e());
      absEng += thisAbsEng;
      HepGeom::Vector3D<double> thisPos(itrCell->x(), itrCell->y(), itrCell->z());
      theCentroid += thisAbsEng*thisPos;
    }
    if ( absEng > 0 ) 
      theCentroid *= (1./absEng);
  }
  m_centroid = new HepGeom::Vector3D<double>(theCentroid);
}

