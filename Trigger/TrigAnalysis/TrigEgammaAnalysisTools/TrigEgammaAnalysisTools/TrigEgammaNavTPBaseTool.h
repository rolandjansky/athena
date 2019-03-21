/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigEgammaNavTPBaseTool_H
#define TrigEgammaNavTPBaseTool_H

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
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"

namespace Trig{
            class FeatureContainer;
}

class TrigEgammaNavTPBaseTool: public TrigEgammaAnalysisBaseTool,
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
    /*! pass without detector errors */
    bool passesEventCleaning();
    /*! at least one chain should pass. e28_tight_iloose? */
    bool MinimalTriggerRequirement ();
    /*! Tag and Probe method called by derived classes */
    void executeTandP(); 
    /*! Match probes called by derived classes */
    void matchObjects(const std::string trigItem); 
    /*! Tag Electron selection */
    bool isTagElectron(const xAOD::Electron *el);
    /*! Rerun offline selection */
    bool ApplyElectronPid(const xAOD::Electron *eg,const std::string);
    /*! Rerun offline selection */
    void DressPid(const xAOD::Electron *eg);
    /*! Probe selection */
    bool isGoodProbeElectron(const xAOD::Electron *el);//,const std::string,const float,const std::string); 
    /*! Event-wise trigger selection */
    bool passedTrigger(const HLT::TriggerElement* obj); 
    /*! Clears list of probes after each trigger item per event */
    void clearProbeList(); 
    /*! Clears list of matched probes after each trigger item per event */
    void clearPairList();
    /* Clears decorations offline objects */
    void clearDecorations();
    /*! Return pseudo-lifetime of Jpsi */
    float GetPseudoLifetime(const xAOD::Electron*,const xAOD::Electron*);
    /*! Calculate the displacement of the Jpsi vertex w.r.t. the primary vertex in the transverse plane */
    double simple_lxy(int ,double, double, double , double , double , double , double, double );
    /*! get the W mass */
    float getWTransverseMass( const xAOD::Electron *el );
    /*! Return MissingEt obj */
    const xAOD::MissingET* getMET(){
      return (*m_METContainer)["FinalClus"];
    }

    /*! The pair index for each electron */
    std::vector<std::pair<unsigned,unsigned>> m_Z;
    /*! vector of electron probes */
    std::vector<const xAOD::Electron*> m_probeElectrons;
    /*! vector of photon probes */
    std::vector<const xAOD::Photon*> m_probePhotons;
    /*! vector of offline electron probe and matched TE */
    std::vector<std::pair<const xAOD::Egamma*,const HLT::TriggerElement*> > m_pairObj;
    /* vector of offline photon probe and matched TE */
    //std::vector<std::pair<const xAOD::Photon*,const HLT::TriggerElement*> > m_probePhotons; 
    /*! List of triggers from menu */
    std::vector<std::string> m_trigInputList;
   /*! List of trigger categories for MaM */
    std::vector<std::string> m_categories; 
    /*! List of triggers to study */
    std::vector<std::string> m_trigList; 
    /*! Zee lower mass cut */
    float m_ZeeMassMin;
    /*! Zee upper mass cut */
    float m_ZeeMassMax;
    /*! Remove crack region for Probe default True */
    bool m_rmCrack;
    /*! Enable the requirement of triggers */
    bool m_applyMinimalTrigger;
    /*! Directory name for each algorithm */
    std::string m_dir;
    /*! Directory name for each algorithm */
    std::string m_anatype;
    /*! Select opposite or same-sign pairs -- for background studies */
    bool m_oppositeCharge;
     /*! Minimum tag Et */
    float m_tagMinEt;
    /*! Minimum probe Et */
    float m_probeMinEt;
 
    // Containers 
    /*! Event Wise offline ElectronContainer Access and end iterator */
    const xAOD::ElectronContainer* m_offElectrons;
    /*! Jet container for probe selection */
    const xAOD::JetContainer* m_jets;
    /*! Missing Et container */
    const xAOD::MissingETContainer* m_METContainer;
    /*! EventInfo Container */
    const xAOD::EventInfo* m_eventInfo;
    /* Truth Container */
    const xAOD::TruthParticleContainer* m_truthContainer;
 
  private:

    // In python order will matter. Should always be tight, medium, loose
    // Order no longer important since using a map
    ///*! Offline isEM Selectors */
    ToolHandleArray<IAsgElectronIsEMSelector> m_electronIsEMTool;
    /*! Offline LH Selectors */
    ToolHandleArray<IAsgElectronLikelihoodTool> m_electronLHTool;
  
    std::map< std::string, unsigned int > m_PidToolMap; /*! Pass a string to pick up correct selector */
    //std::map< std::string, std::string > m_PidMap; /*! Map trigger pid to selector pid */ 
 
    /* TP selection properties */
    /*! Not needed */
    // unsigned int m_isEMoffTag;
    /*! Define the PID for tag electron */
    std::string m_offTagTightness;
    /*! define the Pid of Probe from the trigger name */ 
    std::string m_offProbeTightness;
    /*! define the Pid of Probe from the user */ 
    std::string m_defaultProbeTightness;
    /*! force user defined probe for pid triggers */
    bool m_forceProbePid;
    /*! Trigger for tag and event wise selection */
    std::vector<std::string> m_tagTrigList;
    /*! Apply nearby jet selection */
    bool m_applyJetNearProbeSelection; 
    /*! force probe isolation */
    bool m_forceProbeIsolation;
    /*! Define isolation working point for Probe electron */
    std::string m_offProbeIsolation;
    /*! skip trigger for offline Tag and Probe analysis */
    bool m_skipTrigger;
};

#endif
