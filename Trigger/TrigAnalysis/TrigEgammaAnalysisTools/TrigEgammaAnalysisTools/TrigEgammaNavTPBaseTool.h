/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavTPBaseTool_H
#define TrigEgammaNavTPBaseTool_H

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
//#include "TrigNavigation/Navigation.h"

#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"
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
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
//#include "xAODCaloRings/RingSetContainer.h" 
//#include "xAODCaloRings/CaloRingsContainer.h"
//#include "xAODCaloRings/tools/getCaloRingsDecorator.h"
#include "egammaMVACalib/IegammaMVATool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

namespace Trig{
            class FeatureContainer;
}
class IegammaMVATool;

class TrigEgammaNavTPBaseTool
: public TrigEgammaAnalysisBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavTPBaseTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaNavTPBaseTool( const std::string& myname );
  ~TrigEgammaNavTPBaseTool() {};

  StatusCode childInitialize();
  StatusCode childBook();
  StatusCode childExecute();
  StatusCode childFinalize();
protected:
  /*! things like LAr-error, Tile-error, etc, should come here */
  bool EventWiseSelection(); 
  /*! at least one chain should pass. e28_tight_iloose? */
  bool MinimalTriggerRequirement ();
  /*! Tag and Probe method called by derived classes */
  StatusCode executeTandP(const std::string trigItem); 
  /*! Tag Electron selection */
  bool isTagElectron(const xAOD::Electron *el);
  /*! Rerun offline selection */
  bool ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname);
  /*! Probe selection */
  bool isGoodProbeElectron(const xAOD::Electron *el,const std::string,const float,const std::string); 
  /*! Event-wise trigger selection */
  bool passedTrigger(const HLT::TriggerElement* obj); 
  /*! Clears list of probes after each trigger item per event */
  void clearProbeList(); 
  /*! Dual-use tool for MVA calibration */
  ToolHandle<IegammaMVATool>  m_MVACalibTool; 
  /*! vector of offline electron probe and matched TE */
  std::vector<std::pair<const xAOD::Electron*,const HLT::TriggerElement*> > m_probeElectrons;
  /* vector of offline photon probe and matched TE */
  std::vector<std::pair<const xAOD::Photon*,const HLT::TriggerElement*> > m_probePhotons; 
  /* vector of offline TP pair mass */
  std::vector<float> m_mee;
  /*! List of triggers from menu */
  std::vector<std::string> m_trigInputList;
 /*! List of trigger categories for MaM */
  std::vector<std::string> m_categories; 
  /*! List of triggers to study */
  std::vector<std::string> m_trigList; 
  /*! To apply MVA calibration -- TBD */
  bool m_applyMVACalib; 
  /*! dR matching between TE and offline probe */
  float m_dR;
  /*! Remove crack region for Probe default True */
  bool m_rmCrack;
  /*! Directory name for each algorithm */
  std::string m_dir;
  // Additional monitoring function   
private:
  // ToolHandles
  //
  //Offline ++ selectors
  // In python order will matter. Should always be tight, medium, loose
  // Order no longer important since using a map
  ///*! Offline isEM Selectors */
  ToolHandleArray<IAsgElectronIsEMSelector> m_electronIsEMTool;
  /*! Offline LH Selectors */
  ToolHandleArray<IAsgElectronLikelihoodTool> m_electronLHTool;

  std::map< std::string, unsigned int > m_PidToolMap; /*! Pass a string to pick up correct selector */
  //std::map< std::string, std::string > m_PidMap; /*! Map trigger pid to selector pid */ 
  //
  /*! Event Wise offline ElectronContainer Access and end iterator */
  const xAOD::ElectronContainer* m_offElectrons;
  /*! Jet container for probe selection */
  const xAOD::JetContainer* m_jets;
  /* TP selection properties */
 
  /*! Zee lower mass cut */
  float m_ZeeMassMin;
  /*! Zee upper mass cut */
  float m_ZeeMassMax;
  /*! Not needed */
  unsigned int m_isEMoffTag;
  /*! Define the PID for tag electron */
  std::string m_offTagTightness;
  /*! define the Pid of Probe from the trigger name */ 
  std::string m_offProbeTightness;
  /*! define the Pid of Probe from the user */ 
  std::string m_defaultProbeTightness;
  /*! force user defined probe for pid triggers */
  bool m_forceProbePid;
  /*! Select opposite or same-sign pairs -- for background studies */
  bool m_oppositeCharge;
  /*! Minimum tag Et */
  float m_tagMinEt;
  /*! Minimum probe Et */
  float m_probeMinEt;
  /*! Trigger for tag and event wise selection */
  std::vector<std::string> m_tagTrigList;
  /*! Apply nearby jet selection */
  bool m_applyJetNearProbeSelection;
  /*! force probe isolation */
  bool m_forceProbeIsolation;
  /*! Define isolation working point for Probe electron */
  std::string m_offProbeIsolation;
};

#endif
