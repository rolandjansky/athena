/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloCellSelectorRoughdR.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrackToCalo/CaloCellSelectorUtils.h"
#include "TrackToCalo/CaloCellSelectorRoughdR.h"
#include "CaloEvent/CaloCell.h"
#include "TrkCaloExtension/CaloExtension.h"


namespace Trk 
{

  CaloCellSelectorRoughdR::CaloCellSelectorRoughdR(double coneSize):
    m_coneSize( coneSize ), 
    m_midEta(0),
    m_midPhi(0),
    m_maxDiff(0)
  {
  }

  CaloCellSelectorRoughdR::~CaloCellSelectorRoughdR(){}

  bool CaloCellSelectorRoughdR::preSelectAction( const Trk::CaloExtension& caloExtension ){
    
    if( caloExtension.caloLayerIntersections().empty() ) return false;

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    CaloExtensionHelpers::entryExitPerLayerVector( caloExtension, m_crossPoints );
    // get mean direction of the calo crossings
    Amg::Vector3D meanPos(0., 0., 0.);
    for (auto entry : m_crossPoints){
      int code = std::get<0>(entry);
      if ( !(code >= 0 && code < 24) )  { continue; } // not a intersection with a calo layer
      meanPos += std::get<1>(entry).unit();
    }

    m_midEta = meanPos.eta();
    m_midPhi = meanPos.phi();

    //get individual crossings max deviation from the mean direction as tolerance
    m_maxDiff = 0.;
    for (auto entry : m_crossPoints){  
      int code = std::get<0>(entry);
      if ( !(code >= 0 && code < 24) )  { continue; } // not a intersection with a calo layer
      Amg::Vector3D pos = std::get<1>(entry);
      double rDiff = Utils::deltaR(pos.eta(), m_midEta, pos.phi(), m_midPhi);
      if (rDiff>m_maxDiff) m_maxDiff = rDiff;
    }
    m_maxDiff += m_coneSize;
    m_maxDiff *= m_maxDiff; //do comparison in dR*dR
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!




    //const std::vector<const CurvilinearParameters*>& intersections = caloExtension.caloLayerIntersections();
    //int nPts = intersections.size();

    //Amg::Vector3D meanPos(0., 0., 0.);
    //m_maxDiff = 0.;

    //// get mean direction of the calo crossings
    //for (int i=0;i<nPts;++i){  
    //  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //  int code = parsIdHelper.caloSample(intersections[i]->cIdentifier());
    //  if (!parsIdHelper.isEntryToVolume(intersections[i]->cIdentifier())) code = -code;
    //  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //  if ( !(code >= 0 && code < 24) )  { continue; } // not a intersection with a calo layer
    //  meanPos += intersections[i]->position().unit();
    //}

    //m_midEta = meanPos.eta();
    //m_midPhi = meanPos.phi();

    ////get individual crossings max deviation from the mean direction as tolerance
    //for (int i=0;i<nPts;++i){  
    //  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //  int code = parsIdHelper.caloSample(intersections[i]->cIdentifier());
    //  if (!parsIdHelper.isEntryToVolume(intersections[i]->cIdentifier())) code = -code;
    //  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //  if ( !(code >= 0 && code < 24) )  { continue; } // not a intersection with a calo layer
    //  Amg::Vector3D pos = intersections[i]->position();
    //  double rDiff = Utils::deltaR(pos.eta(), m_midEta, pos.phi(), m_midPhi);
    //  if (rDiff>m_maxDiff) m_maxDiff = rDiff;
    //}

    ////select cell within requested cone size+tolerence
    //m_maxDiff += m_coneSize;
    //m_maxDiff *= m_maxDiff; //do comparison in dR*dR

    return true;
  }

  bool CaloCellSelectorRoughdR::select( const CaloCell& cell )const {
    const CaloDetDescrElement* dde = cell.caloDDE();
    if(!dde) return false;
    return Utils::deltaR2( m_midEta,dde->eta(),m_midPhi,dde->phi()) < m_maxDiff;
  }

} // end of namespace

