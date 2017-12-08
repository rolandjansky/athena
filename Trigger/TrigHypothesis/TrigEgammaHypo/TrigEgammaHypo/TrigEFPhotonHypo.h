/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFPhotonHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigEFPhotonHypo
 *
 * AUTHOR:   Valeria Perez Reale
 * MAINTAINED BY: R.M. White 
 * CREATED:  Nov 12, 2006
 * MODIFIED: V. Perez Reale  added doxygen comments 25/6/07
 *           A.Tricoli: added extra variables to monitor isEM flag (Jan 2010)
 * MODIFIED: R.M. White: xAOD migration (April 2014)
 *********************************************************************/
#ifndef TRIGEFPHOTONHYPO_H
#define TRIGEFPHOTONHYPO_H

#include <string>
#include <cmath> 
//#include <map.h>

// athena stuff stuff
//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
//#include "CLHEP/Units/SystemOfUnits.h"

// core trigger stuff
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigInterfaces/HypoAlgo.h"

//xAOD
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaContainer.h"

//RMW Sept 2013
#include "PATCore/IAthSelectorTool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"

//#include <string
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"

class StoreGateSvc;
class TriggerElement;
class TH1F;


/**
 * \class TrigEFPhotonHypo
 * \brief TrigEFPhotonHypo is a Trigger Hypothesis  Algorithm that retrieves the EF EgammaContainer
 * container created by the EF algorithm TrigEgammaRec
 * Applies a selection on ET, shower shape selection to select a good photon at the EF
 * an event will be accepted if all photon selection variables are satisfied
 *
 */


class TrigEFPhotonHypo : public HLT::HypoAlgo {

 public:
  TrigEFPhotonHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFPhotonHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  HLT::ErrorCode hltBeginRun();

 private:

  // define the properties:
  //----------------------------

  // m_acceptAll = true, all events are taken, not selection cuts applied
  // m_acceptAll = false, selection cuts applied

  bool m_acceptAll; //!<  true all events are taken

  std::string m_egammaPhotonCutIDToolName;
  ToolHandle<IAsgPhotonIsEMSelector> m_egammaPhotonCutIDTool;

  //Offline isEM bit cut
  unsigned int        m_IsEMrequiredBits;  //!< isem flag bits required
  bool       m_applyIsEM;  //!< true if isem flag required
  double       m_emEt;              //!< ET cut
  int        m_NofPassedCuts;
  int        m_NofPassedCutsIsEM; 
  int        m_NofPassedCutsIsEMTrig; 

  //AT: variables for isEM monitoring
  //std::vector<unsigned int> m_isEMTrig; //AT Aug2011: variable not needed - deactivate histogram egIsEM - outdated
  std::vector<unsigned int> m_NcandIsEM;  
  std::vector<unsigned int> m_NcandIsEMAfterCut; 
  std::vector<unsigned int> m_IsEMRequiredBits; 
  std::vector<unsigned int> m_IsEMRequiredBitsAfterCut;
  
  const xAOD::PhotonContainer* m_EgammaContainer; //!< pointer to PhotonContainer
 
  // Isolation
  bool m_applyIsolation; 
  unsigned int m_EtConeSizes;
  std::vector<float> m_RelEtConeCut;
  std::vector<float> m_EtConeCut;
  std::vector<float> m_IsoOffset;
  std::map<int, std::string> m_mapEtCone;
  std::map<int, std::string> m_mapRelEtCone;
  std::map<int, std::string> m_mapIsoOffset;
  
  //std::vector<IHistogram1D*> nCandHistVector;

  // Histogram Service:
  //IHistogramSvc* m_histsvc;

  // Timing:
  TrigTimer* m_totalTimer, *m_timerPIDTool_Ele, *m_timerPIDTool_Pho;

  void prepareMonitoringVars();
  float getIsolation(const float cone, const float offset, const float et){return (cone-offset)/et;}

};
#endif

