/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFCaloHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigEgammaHypo
 *
 * AUTHOR:   P. Conde Muino
 * CREATED:  Nov 25, 2004
 * MODIFIED: V. Perez Reale   migration to new Steering 1/12/06
 *
 *********************************************************************/
#ifndef TRIGEFCALOHYPO_H
#define TRIGEFCALOHYPO_H

#include <string>
#include <cmath> 
//#include <map.h>

// athena stuff stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

// core trigger stuff
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigInterfaces/HypoAlgo.h"


//#include <string
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"

#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
#include "LumiBlockComps/ILuminosityTool.h"

class StoreGateSvc;
class TriggerElement;



/**
 * \class TrigEFCaloHypo
 * \brief TrigEFCaloHypo is a Trigger Hypo Algorithm that retrieves the EF CaloCluster
 * container created by TrigCaloRec
 * Apply calorimeter selction cuts like ET
 *
 */


class TrigEFCaloHypo : public HLT::HypoAlgo {

 public:

  TrigEFCaloHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFCaloHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  // define the properties:
  //----------------------------
  bool m_acceptAll;
  // Cuts to be applied:

  double     m_emEt;
  double     m_isEM;
  bool       m_applyIsEM;  //!< true if isem flag required 
  bool       m_applyLH; // use LH
  unsigned int        m_IsEMrequiredBits;  //!< isem flag bits required
  // Switch on Monitoring:
  std::string m_SelectorToolName;
  std::string m_LHSelectorToolName;
  ToolHandle<IAsgElectronIsEMSelector> m_SelectorTool;
  ToolHandle<IAsgElectronLikelihoodTool> m_LHSelectorTool;
  
  /** Luminosity Tool */
  ToolHandle<ILuminosityTool>  m_lumiTool;
  
  std::vector<TrigTimer*>   m_timers;
  TrigTimer* m_totalTimer;

  // Monitor collections
  std::vector<float> m_EBE0;
  std::vector<float> m_EBE1;
  std::vector<float> m_EBE2;
  std::vector<float> m_EBE3;
  std::vector<float> m_Eta;
  std::vector<float> m_EtaCalo;
  std::vector<float> m_PhiCalo;
  std::vector<float> m_E;
  std::vector<float> m_ECalib;
  std::vector<float> m_ERes;
  std::vector<float> m_lhval;
};
#endif

