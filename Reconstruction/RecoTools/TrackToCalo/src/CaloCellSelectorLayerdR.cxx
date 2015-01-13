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

    //std::cout << "CaloCellSelectorLayerdR p00" << std::endl;
    if( caloExtension.caloLayerIntersections().empty() ) return false;
    //std::cout << "CaloCellSelectorLayerdR p01" << std::endl;
    CaloExtensionHelpers::midPointEtaPhiHashLookupVector( caloExtension, m_midPoints );
    return true;
  }

  bool CaloCellSelectorLayerdR::select( const CaloCell& cell )const {
    // select cell within dR from the midPoint of the same calo layer
    const CaloDetDescrElement* dde = cell.caloDDE();
    //std::cout << "CaloCellSelectorLayerdR p1" << std::endl;
    if(!dde) return false;

    int samplingID = dde->getSampling();
    //std::cout << "CaloCellSelectorLayerdR p2 " << samplingID << std::endl;
    //for (auto entry : m_midPoints){
    //  std::cout << std::get<0>(entry) << std::endl;
    //}
    if ( !std::get<0>(m_midPoints[samplingID]) ) return false;
    double dr = Utils::deltaR2( std::get<1>(m_midPoints[samplingID]),dde->eta(),std::get<2>(m_midPoints[samplingID]),dde->phi());
    if( dr < m_coneSize2){
      //std::cout << "CaloCellSelectorLayerdR p3" << std::endl;
      return true;
    }
    //std::cout << "CaloCellSelectorLayerdR p4" << std::endl;
    return false;
  }


} // end of namespace

