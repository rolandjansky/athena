/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFElectronHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigEgammaHypo
 *
 * MAINTAINED BY: Ryan M. White, Alessandro Tricoli
 *
 *********************************************************************/
#ifndef TRIGEFEGAMMAHYPO_H
#define TRIGEFEGAMMAHYPO_H


#include <string>
#include <cmath> 

// athena stuff stuff
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AIDA/IHistogram1D.h"

// core trigger stuff
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

//xAOD
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaContainer.h"

// Asg SelectorTools
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"

#include "TrkSurfaces/PerigeeSurface.h"
#include "VxVertex/RecVertex.h"

#include "LumiBlockComps/ILumiBlockMuTool.h"
/**
 * \class TrigEFElectronHypo
 * \brief TrigEFElectronHypo is a Trigger Hypothesis  Algorithm that retrieves the EF EgammaContainer
 * container created by the EF algorithm TrigEgammaRec
 * It can apply the isem offline cut for electrons: loose, tight or medium
 * or it can apply ET, shower shape selection, TRT hits and E/p cuts to select a good electron at the EF
 * an event will be accepted if all electron selection variables are satisfied
 *
 */


class StoreGateSvc;
class TriggerElement;

namespace Trk
{
  class Vertex;
}

namespace Reco  { class ITrackToVertex; }//AT Jan 2010:added

class TrigEFElectronHypo : public HLT::HypoAlgo {

 public:

  TrigEFElectronHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFElectronHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  HLT::ErrorCode hltBeginRun();

  /** Find the impact parameter by extrapolating to either the beam spot or (0,0) **/
  double findImpact(const xAOD::TrackParticle* track) const;//AT: copied from Offline

 private:

  // define the properties:
  //----------------------------

  // m_acceptAll = true, all events are taken, not selection cuts applied
  // m_acceptAll = false, selection cuts applied

  bool m_acceptAll; //!<  true all events are taken

  bool m_caloCutsOnly;

  std::string m_egammaElectronCutIDToolName;
  std::string m_athElectronLHIDSelectorToolName;
  ToolHandle<IAsgElectronIsEMSelector> m_egammaElectronCutIDTool;
  ToolHandle<IAsgElectronLikelihoodTool> m_athElectronLHIDSelectorTool;
  
  /** Luminosity Tool */
  ToolHandle<ILumiBlockMuTool>  m_lumiBlockMuTool;
  
  bool m_useAthElectronLHIDSelector;
  //Offline isEM bit cut
  unsigned int        m_IsEMrequiredBits;  //!< isem flag bits required
  bool       m_applyIsEM;  //!< true if isem flag required 
  double     m_emEt;  //!< ET cut
  bool       m_applyEtIsEM; // Use object Et for isEM selection

  //Isolation
  bool m_applyIsolation; 
  unsigned int m_EtConeSizes;
  unsigned int m_PtConeSizes;
  bool m_useClusETforCaloIso;
  bool m_useClusETforTrackIso;
  std::vector<float> m_RelEtConeCut;
  std::vector<float> m_EtConeCut;
  std::vector<float> m_RelPtConeCut;
  std::vector<float> m_PtConeCut;
  std::map<int, std::string> m_mapEtCone,m_mapPtCone;
  std::map<int, std::string> m_mapRelEtCone,m_mapRelPtCone;

  int m_NofPassedCuts;
  int m_NofPassedCutsIsEM;
  int m_NofPassedCutsIsEMTrig;

  //AT: variables for isEM monitoring
  //std::vector<unsigned int> m_isEMTrig; //AT Aug2011: variable not needed: deactivate histogram egIsEM - outdated
  std::vector<unsigned int> m_NcandIsEM;   
  std::vector<unsigned int> m_NcandIsEMAfterCut; 
  std::vector<unsigned int> m_IsEMRequiredBits;
  std::vector<unsigned int> m_IsEMRequiredBitsAfterCut;
  // Likelihood output
  std::vector<float> m_lhval;
  // Lumi monitoring
  std::vector<double> m_avgmu;

  // egamma container
  const xAOD::ElectronContainer* m_EgammaContainer; //!<  pointer to ElectronContainer

 // Timing:
  TrigTimer* m_totalTimer;
  TrigTimer* m_timerPIDTool;

  Amg::Vector3D m_primaryVertex; //AT Jan 2010  
  /** @brief Tool handle for track extrapolation to vertex */   	   	 
  ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool; //AT Jan 2010

  void prepareMonitoringVars();

};
#endif

