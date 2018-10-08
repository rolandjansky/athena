/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleFexRun2Algo.h
 **
 **   Description: - Combo algorithm: search for pairs of electron and muons with 
 **                delta phi in some interval + some additional cuts; 
 **
 **
 **   Author: Olya Igonkina
 **
 **   Created:  Mar 7, 2018
 **
 **************************************************************************/

#ifndef TRIGEGAMMAMUONCOMBHYPO_TRIGEFELECTRONMUONANGLEFEXRUN2ALGO_H 
#define TRIGEGAMMAMUONCOMBHYPO_TRIGEFELECTRONMUONANGLEFEXRUN2ALGO_H

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
 * \class TrigEFElectronMuonAngleFexRun2Algo
 * \brief TrigEFElectronMuonAngleFexRun2Algo is a Trigger Fex Algorithm that retrieves the EF electron
 * container and EF MuID track container
 * apply loose cuts on distance (it is possible to cut delta phi and on the full distance dR)
 * apply loose inv mass cuts (upper and lower cut) to the two leptons
 * 
 */

class TrigEFElectronMuonAngleFexRun2Algo: public HLT::ComboAlgo  {
  
public:
  TrigEFElectronMuonAngleFexRun2Algo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEFElectronMuonAngleFexRun2Algo();
  
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
