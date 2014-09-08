/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigEFDielectronMassFex.h
 **
 **   Description: - Hypothesis algorithm: searc for electron pairs with 
 **                invariant mass in some interval; intended for Z->ee
 **                - Modified from TrigL2DielectronMassHypo by R Goncalo
 **
 **   Author: Tetiana Hrynova <thrynova@mail.cern.ch>
 **
 **   Created:   Nov 10, 2009
 **
 **   Author(xAOD): Ryan Mackenzie White
 **   Modified: April 2014
 **************************************************************************/ 

#ifndef TRIGEGAMMAHYPO_TRIGEFDIELECTRONMASSFEX_H 
#define TRIGEGAMMAHYPO_TRIGEFDIELECTRONMASSFEX_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
// trigger includes
#include "TrigInterfaces/ComboAlgo.h"

// trigger EDM
//#include "egammaEvent/egammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"

/* class StoreGateSvc; */
/* class TriggerElement; */

/**
 * \class TrigEFDielectronMassFex
 * \brief TrigEFDielectronMassFex is a Trigger Fex Algorithm that retrieves the trig egamma
 * container
 * apply loose inv mass cuts (upper and lower cut) to the two electrons
 *
 */

class TrigEFDielectronMassFex: public HLT::ComboAlgo  {
  
public:
  TrigEFDielectronMassFex(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEFDielectronMassFex();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );
  HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
    
private:
  void processElectronElectron(HLT::TEConstVec& inputTE, std::vector<float>& masses);
  void processElectronCluster(HLT::TEConstVec& inputTE, std::vector<float>& masses);

private:
  // cuts
  bool m_useElectronElectron;
  bool m_useElectronCluster;
  float m_lowerMassCut; //!<  lower inv mass cut (e-e pair)
  float m_upperMassCut; //!<  upper inv mass cut (e-e pair)

  // output data
  std::vector<float> m_massElectronElectron;
  std::vector<float> m_massElectronCluster;
  
  // to set Accept-All mode: should be done with force-accept when possible
  bool m_acceptAll; //!<  true will accept all events

  // monitoring
  double m_monMassAccepted;
  int    m_monCut;
};




#endif
