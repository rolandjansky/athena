/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoTmpHashCluster.cxx,v 1.8 2008-08-28 05:15:06 ssnyder Exp $
//
// Description: see CaloTopoTmpHashCluster.h
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
#include "CaloRec/CaloTopoTmpHashCluster.h"
#include "CaloRec/CaloTopoTmpClusterCell.h"

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

//-----------
// Methods --
//-----------

void CaloTopoTmpHashCluster::add(const HashCell& hashCell)
{
  // take the cell with the largest Signal to Noise and report its E_t as the
  // relevant maxEt for the cluster - Note that there might be a cell with a
  // larger E_t ...
  CaloTopoTmpClusterCell* pCell = hashCell.getCaloTopoTmpClusterCell();
  if ( pCell->getSignedRatio() > m_maxRatio )
    m_maxEt = pCell->getSignedEt();
  Base::add (hashCell);
}

void CaloTopoTmpHashCluster::add(CaloTopoTmpHashCluster& rClus)
{
  if ( rClus.m_maxRatio > m_maxRatio)
    m_maxEt = rClus.m_maxEt;
  Base::add (rClus);
}
