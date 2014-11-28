/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigEFPhotonMuonAngleFexAlgo.h
 **
 **   Description: - Combo algorithm: search for pairs of photon and muons with 
 **                delta phi in some interval + some additional cuts; 
 **
 **
 **   Author: Pavel Jez <pavel.jez@cern.ch>, Olya Igonkina
 **
 **   Created:  Nov 21, 2011
 **   Migrated to xAOD Nov 28 2014
 **
 **************************************************************************/

#ifndef TRIGEGAMMAMUONCOMBHYPO_TRIGEFPHOTONMUONANGLEFEXALGO_H 
#define TRIGEGAMMAMUONCOMBHYPO_TRIGEFPHOTONMUONANGLEFEXALGO_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
#include <vector>
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
// trigger includes
#include "TrigInterfaces/ComboAlgo.h"


/**
 * \class TrigEFPhotonMuonAngleFexAlgo
 * \brief TrigEFPhotonMuonAngleFexAlgo is a Trigger Fex Algorithm that retrieves the EF photon
 * container and EF MuID track container
 * apply loose cuts on distance (it is possible to cut delta phi and on the full distance dR)
 * apply loose inv mass cuts (upper and lower cut) to the two leptons
 * 
 */

class TrigEFPhotonMuonAngleFexAlgo: public HLT::ComboAlgo  {
  
public:
  TrigEFPhotonMuonAngleFexAlgo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEFPhotonMuonAngleFexAlgo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);

    
private:
  // monitoring
  double m_monMass;  
  double m_monDPhi;
  double m_monDR;
};




#endif
