/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloCellSelectorLayerdR.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrackToCalo/CaloCellSelectorUtils.h"
#include "TrackToCalo/CaloCellSelectorLayerdR.h"
#include "CaloEvent/CaloCell.h"
#include "TrkCaloExtension/CaloExtension.h"
namespace Trk 
{

  CaloCellSelectorLayerdR::CaloCellSelectorLayerdR(double coneSize):
    m_coneSize2( coneSize*coneSize ){
  }

  CaloCellSelectorLayerdR::~CaloCellSelectorLayerdR(){}

  bool CaloCellSelectorLayerdR::preSelectAction( const Trk::CaloExtension& caloExtension ){
    if( caloExtension.caloLayerIntersections().empty() ) return false;
    CaloExtensionHelpers::midPointEtaPhiHashLookupVector( caloExtension, m_midPoints );
    return true;
  }

  bool CaloCellSelectorLayerdR::preSelectAction( const xAOD::CaloCluster& caloCluster ){
    m_midPoints.clear();
    m_midPoints.resize(CaloSampling::getNumberOfSamplings());
    for(unsigned int i=0; i<CaloSampling::Unknown; i++){
      auto s = static_cast<CaloSampling::CaloSample>(i);
      if(!caloCluster.hasSampling(s)) m_midPoints.push_back(std::make_tuple(false,0.,0.));
      else m_midPoints.push_back(std::make_tuple(true,caloCluster.etaSample(s),caloCluster.phiSample(s)));
    }
    return true;
  }


  bool CaloCellSelectorLayerdR::select( const CaloCell& cell )const {
    // select cell within dR from the midPoint of the same calo layer
    const CaloDetDescrElement* dde = cell.caloDDE();
    if(!dde) return false;

    int samplingID = dde->getSampling();
    if ( !std::get<0>(m_midPoints[samplingID]) ) return false;
    double dr = Utils::deltaR2( std::get<1>(m_midPoints[samplingID]),dde->eta(),std::get<2>(m_midPoints[samplingID]),dde->phi());
    if( dr < m_coneSize2){
      return true;
    }
    return false;
  }


} // end of namespace

