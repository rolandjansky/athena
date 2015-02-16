/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavZeeTPBaseTool_H
#define TrigEgammaNavZeeTPBaseTool_H

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
//#include "TrigNavigation/Navigation.h"

#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "egammaMVACalib/IegammaMVATool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

namespace Trig{
            class FeatureContainer;
}
class IegammaMVATool;

class TrigEgammaNavZeeTPBaseTool
: public TrigEgammaAnalysisBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavZeeTPBaseTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaNavZeeTPBaseTool( const std::string& myname );
  ~TrigEgammaNavZeeTPBaseTool() {};

  StatusCode childInitialize();
  StatusCode childExecute();
  StatusCode childFinalize();
  template<class T> const T* getFeature(const HLT::TriggerElement* te);
  template<class T> bool ancestorPassed(const HLT::TriggerElement* te);
protected:
  /*! things like LAr-error, Tile-error, etc, should come here */
  bool EventWiseSelection(); 
  /*! at least one chain should pass. e28_tight_iloose? */
  bool MinimalTriggerRequirement ();
  /*! Tag and Probe method called by derived classes */
  StatusCode executeTandP(const std::string trigItem); 
  /*! Tag Electron selection */
  bool isTagElectron(const xAOD::Electron *el);
  /*! Simple setter to pick up correct probe PID for given trigger */
  void setProbePid(const std::string);
  /*! Probe selection */
  bool isGoodProbeElectron(const xAOD::Electron *el,const std::string); 
  /*! Creates pairs of probes and TE */
  bool isProbeElectron(const xAOD::Electron *el,
          const std::string trigItem,
          const HLT::TriggerElement*& finalFC); 
  /*! Event-wise trigger selection */
  bool passedTrigger(const HLT::TriggerElement* obj); 
  /*! Clears list of probes after each trigger item per event */
  void clearProbeList(); 
  // ToolHandles
  ToolHandle<Trig::TrigDecisionTool> m_trigdec;
  /*! Dual-use tool for MVA calibration */
  ToolHandle<IegammaMVATool>  m_MVACalibTool; 
  /*! vector of offline electron probe and matched TE */
  std::vector<std::pair<const xAOD::Electron*,const HLT::TriggerElement*> > m_probeElectrons;
  /* vector of offline photon probe and matched TE */
  std::vector<std::pair<const xAOD::Photon*,const HLT::TriggerElement*> > m_probePhotons; 
  /*! List of triggers to study */
  std::vector<std::string> m_probeTrigList; 
  /*! To apply MVA calibration -- TBD */
  bool m_applyMVACalib; 
  /*! Directory name for each algorithm */
  std::string m_dir; 

  
private:
  // ToolHandles
  //
  //Offline ++ selectors
  // In python order will matter. Should always be tight, medium, loose
  // Order no longer important since using a map
  ///*! Offline isEM Selectors */
  ToolHandleArray<IAsgElectronIsEMSelector> m_electronPPCutIDTool;
  /*! Offline LH Selectors */
  ToolHandleArray<IAsgElectronLikelihoodTool> m_electronLHTool; 

  /*! String for offline container key */
  std::string m_offElContKey;
  std::map< std::string, unsigned int > m_PidMap; /*! Pass a string to pick up correct selector */
  //
  /*! Event Wise offline ElectronContainer Access and end iterator */
  const xAOD::ElectronContainer* m_offElectrons;
  /*! Jet container for probe selection */
  const xAOD::JetContainer* m_jets;
  /* TP selection properties */
  std::vector< std::string > m_minTrig; /* Event-wise trigger list */
  /*! Zee lower mass cut */
  float m_ZeeMassMin;
  /*! Zee upper mass cut */
  float m_ZeeMassMax;
  /*! Not needed */
  unsigned int m_isEMoffTag;
  /*! Define the PID for tag electron */
  std::string m_offTagTightness;
  /*! define the Pid of Probe from the trigger */ 
  std::string m_offProbeTightness;
  /*! Select opposite or same-sign pairs -- for background studies */
  bool m_oppositeCharge;
  /*! Minimum tag Et */
  float m_tagMinEt;
  /*! Trigger for tag */
  std::string m_tagTrigItem; 
  
};

/** Main feature of the code 
 * templates the TE to get a feature
 * or find the passing TE
 * **********************/
template<class T>
const T*
TrigEgammaNavZeeTPBaseTool::getFeature(const HLT::TriggerElement* te){
    if ( te == NULL ) return NULL;
    if ( (m_trigdec->ancestor<T>(te)).te() == NULL )
        return NULL;
    return ( (m_trigdec->ancestor<T>(te)).cptr() );
}

template<class T>
bool
TrigEgammaNavZeeTPBaseTool::ancestorPassed(const HLT::TriggerElement* te){
    if ( te == NULL ) return NULL;
    if ( (m_trigdec->ancestor<T>(te)).te() == NULL )
        return false;
    return ( (m_trigdec->ancestor<T>(te)).te()->getActiveState());
}
#endif
