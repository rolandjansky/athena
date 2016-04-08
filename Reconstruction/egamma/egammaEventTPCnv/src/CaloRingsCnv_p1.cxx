///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloRingsCnv_p1.cxx 
// Implementation file for class CaloRingsCnv_p1
// Author: D.E.Ferreira de Lima<dferreir@mail.cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// egammaEvent includes
#include "egammaEvent/CaloRings.h"

#include "GaudiKernel/GaudiException.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/CaloRingsCnv_p1.h"

void CaloRingsCnv_p1::persToTrans( const CaloRings_p1* pers,
				CaloRings* trans, 
				MsgStream& /*msg*/ ) 
{
  if ( (!trans) || (!pers) ) {
    throw GaudiException("CaloRingsCnv_p1::persToTrans called with NULL pers or trans!","CaloRingsCnv_p1::persToTrans", StatusCode::FAILURE);
    return;
  }


  trans->clear();
  unsigned int layerNumber = 0;
  unsigned int ringNumber = 0;
  // for each ringset
  for (unsigned int i = 0; i < pers->m_nRings.size(); ++i) {
    // translate layer information
    std::vector<CaloCell_ID::CaloSample> layers;
    for (unsigned int j = 0; j < pers->m_nLayers[i]; ++j) {
      layers.push_back((CaloCell_ID::CaloSample) pers->m_layers[layerNumber++]);
    }
    // insert configuration
    trans->addRingSet(pers->m_nRings[i], layers, pers->m_deltaEta[i], pers->m_deltaPhi[i]);
    // copy ring values
    for (unsigned int k = 0; k < pers->m_nRings[i]; ++k){
      trans->at(i).at(k) = pers->m_rings[ringNumber++];
    }
  }

}

void CaloRingsCnv_p1::transToPers( const CaloRings* trans, 
				CaloRings_p1* pers, 
				MsgStream& /*msg*/ ) 
{
  if ( (!trans) || (!pers) ) {
    throw GaudiException("CaloRingsCnv_p1::transToPers called with NULL pers or trans!","CaloRingsCnv_p1::transToPers", StatusCode::FAILURE);
    return;
  }

  pers->m_nRings.clear();
  pers->m_rings.clear();
  pers->m_nLayers.clear();
  pers->m_layers.clear();
  pers->m_deltaEta.clear();
  pers->m_deltaPhi.clear();

  for (unsigned int i = 0; i < trans->size(); ++i) {
    pers->m_nRings.push_back(trans->at(i).m_rings.size());
    for (unsigned int j = 0; j < trans->at(i).m_rings.size(); ++j) {
      pers->m_rings.push_back(trans->at(i).m_rings[j]);
    }
    pers->m_nLayers.push_back(trans->at(i).m_layers.size());
    for (unsigned int k = 0; k < trans->at(i).m_layers.size(); ++k) {
      pers->m_layers.push_back((int) trans->at(i).m_layers[k]);
    }
    pers->m_deltaEta.push_back(trans->at(i).m_deltaEta);
    pers->m_deltaPhi.push_back(trans->at(i).m_deltaPhi);
  }
}
