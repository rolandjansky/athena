/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavBaseTool_H
#define TrigEgammaNavBaseTool_H

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
//#include "TrigNavigation/Navigation.h"

#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "egammaMVACalib/IegammaMVATool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"


namespace Trig{
            class FeatureContainer;
}
class IegammaMVATool;

class TrigEgammaNavBaseTool
: public TrigEgammaAnalysisBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavBaseTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaNavBaseTool( const std::string& myname );
  ~TrigEgammaNavBaseTool() {};

  StatusCode childInitialize();
  StatusCode childBook();
  StatusCode childExecute();
  StatusCode childFinalize();
protected:
  /*! EventWise Selection */
  bool EventWiseSelection();
  /*! navigation method called by derived classes */
  StatusCode executeNavigation(const TrigInfo);

  /*! Tag Electron selection */
  bool passedTrigger(const HLT::TriggerElement* obj); 
  /*! Rerun offline selection */
  bool ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname);
  /*! Clears list of probes after each trigger item per event */
  void clearList(); 
  /*! vector of offline object and matched TE */
  std::vector<std::pair<const xAOD::Egamma*,const HLT::TriggerElement*> > m_objTEList;
  /*! List of triggers from menu */
  std::vector<std::string> m_trigInputList;
  /*! List of trigger categories for MaM */
  //std::vector<std::string> m_categories; 
  /*! List of triggers to study */
  std::vector<std::string> m_trigList; 
  /*! Base Directory name for each algorithm */
  std::string m_dir;
  /*! Directory name for each algorithm */
  std::string m_anatype;
  /*! Photon pid word */
  std::string m_photonPid;
  /*! doUnconverted analysis */
  bool m_doUnconverted;

  const xAOD::EventInfo* m_eventInfo;
  const xAOD::TruthParticleContainer* m_truthContainer;

private:
 
  /*! navigation method called by executeNavigation */
  StatusCode executeElectronNavigation(const std::string trigItem,float,std::string); 
  /*! navigation method called by executeNavigation */
  StatusCode executePhotonNavigation(const std::string trigItem,float); 
 
  // ToolHandles
  // Offline ++ selectors
  // In python order will matter. Should always be tight, medium, loose
  // Order no longer important since using a map
  ///*! Offline isEM Selectors */
  ToolHandleArray<IAsgElectronIsEMSelector> m_electronIsEMTool;
  /*! Offline LH Selectors */
  ToolHandleArray<IAsgElectronLikelihoodTool> m_electronLHTool; 
  /*! Event Wise offline ElectronContainer Access and end iterator */
  const xAOD::ElectronContainer* m_offElectrons;
  /*! Event Wise offline PhotonContainer Access and end iterator */
  const xAOD::PhotonContainer* m_offPhotons;
  /*! Jet container for probe selection */
  const xAOD::JetContainer* m_jets;
 
  /* TP selection properties */
 
  /*! Define the PID for tag electron */
  std::string m_offTagTightness;
  /*! define the Pid of Probe from the trigger name */ 
  std::string m_offProbeTightness;
  /*! Map of trigger name and L1 item */
  //std::map<std::string,std::string> m_triggerMap;
  /*! Define isolation working point for Probe electron */
  std::string m_offProbeIsolation;  
  /* Define the pidname for data collection using experimentalSelection methods*/
  std::string m_electronFilterType;
  /* Define the pidname for data collection using experimentalSelection methods*/
  std::string m_photonFilterType;

protected:/// Protect type is used here becouse this can be access by inheritance. Some analysis
  ///will change this flags. make everything auto by protect access!
  /*! force probe isolation */
  bool m_forceProbeIsolation;
  /*! force pid and crack selection into electron navigation */
  bool m_forcePidSelection;
  /* force experimentalSelection selection*/
  bool m_forceMCEnhancedBias;
  /* force et cluster cut*/
  bool m_forceEtThr;
   /*! Remove crack region for Probe default True */
  bool m_rmCrack; 


};

#endif
