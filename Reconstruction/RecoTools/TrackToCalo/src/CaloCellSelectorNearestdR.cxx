/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloCellSelectorNearestdR.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrackToCalo/CaloCellSelectorUtils.h"
#include "TrackToCalo/CaloCellSelectorNearestdR.h"
#include "CaloEvent/CaloCell.h"
#include "TrkCaloExtension/CaloExtension.h"

namespace Trk 
{

  CaloCellSelectorNearestdR::CaloCellSelectorNearestdR( double coneSize ):
    m_caloExtension(nullptr),
    m_coneSize2( coneSize*coneSize ){
  }

  CaloCellSelectorNearestdR::~CaloCellSelectorNearestdR(){}

  bool CaloCellSelectorNearestdR::preSelectAction( const Trk::CaloExtension& caloExtension ) {
    m_caloExtension = &caloExtension;
    return true;
  }


  bool CaloCellSelectorNearestdR::select( const CaloCell& cell )const {
    if( !m_caloExtension ) return false;

    const CaloDetDescrElement* dde = cell.caloDDE();
    if(!dde) return false;

    Amg::Vector3D cellPos(dde->x(), dde->y(), dde->z());

    int nearestIdx;
    Amg::Vector3D nearestPos,nearestMom;
    Utils::findNearestPoint( cellPos, m_caloExtension, nearestIdx, nearestPos, nearestMom);

    return Utils::deltaR2( nearestPos.eta(),dde->eta(),nearestPos.phi(),dde->phi()) < m_coneSize2;
  }

} // end of namespace

