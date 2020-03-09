/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigEgammaNavTPBaseTool_H
#define TrigEgammaNavTPBaseTool_H

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
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
#include "StoreGate/ReadHandleKey.h"


namespace Trig{
    class FeatureContainer;
}




class TrigEgammaNavTPBaseTool: public TrigEgammaAnalysisBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavTPBaseTool, ITrigEgammaAnalysisBaseTool)

  public:

    TrigEgammaNavTPBaseTool( const std::string& myname );
    virtual ~TrigEgammaNavTPBaseTool() {};
    virtual StatusCode childInitialize() override;
    virtual StatusCode childFinalize() override;


  protected:

    /*! Tag and Probe method called by derived classes */
    bool executeTandP( const EventContext& ctx, std::vector<const xAOD::Electron*> & ) const;

    /*! Match probes called by derived classes */
    void matchObjects(const std::string trigItem,  std::vector<const xAOD::Electron*>, 
                      std::vector<std::pair<const xAOD::Egamma*, const TrigEgammaMatchingUtils::Element>> &) const; 
    
    /*! Rerun offline selection */
    bool ApplyElectronPid(const xAOD::Electron *eg,const std::string) const;
    
    
    /*! Trigger for tag and event wise selection */
    std::vector<std::string> m_tagTrigList;
    /*! List of triggers from menu */
    std::vector<std::string> m_trigInputList;
    /*! List of triggers from menu after filter*/
    std::vector<std::string> m_trigList;
    
    
    /*! The basepath */
    std::string m_dir;
    /*! Directory name for each algorithm */
    std::string m_anatype;


  private:

    /*! at least one chain should pass. e28_tight_iloose? */
    bool MinimalTriggerRequirement () const;
    /*! Tag Electron selection */
    bool isTagElectron( ToolHandle<GenericMonitoringTool> monGroup, const xAOD::Electron *el) const;
    /*! Probe selection */
    bool isGoodProbeElectron( ToolHandle<GenericMonitoringTool> monGroup, const xAOD::Electron *el, const xAOD::JetContainer *) const;
    /*! Return pseudo-lifetime of Jpsi */
    float GetPseudoLifetime(const xAOD::Electron*,const xAOD::Electron*) const;
    /*! Calculate the displacement of the Jpsi vertex w.r.t. the primary vertex in the transverse plane */
    double simple_lxy(int ,double, double, double , double , double , double , double, double ) const;
    /*! Rerun offline selection */
    void DressPid(const xAOD::Electron *eg) const;
    

    /** Properties **/

    /*! Zee lower mass cut */
    float m_ZeeMassMin;
    /*! Zee upper mass cut */
    float m_ZeeMassMax;
    /*! Define the PID for tag electron */
    std::string m_offTagTightness;
    /*! define the Pid of Probe from the user */
    std::string m_offProbeTightness;
    /*! Select opposite or same-sign pairs -- for background studies */
    bool m_oppositeCharge;
    /*! Minimum tag Et */
    float m_tagMinEt;
    /*! Minimum probe Et */
    float m_probeMinEt;
    /*! Probe isolation */
    std::string m_offProbeIsolation;
    /*! Remove crack region for Probe default True */
    bool m_rmCrack;
    /*! Enable the requirement of triggers */
    bool m_applyMinimalTrigger;
    /*! Apply nearby jet selection */
    bool m_applyJetNearProbeSelection;
    /*! do jpsiee tag and probe */
    bool m_doJpsiee;
       
     // Containers 
    /*! Event Wise offline ElectronContainer Access and end iterator */
    SG::ReadHandleKey<xAOD::ElectronContainer> m_offElectronKey{ this, "ElectronKey", "Electrons", ""};
    /*! Jet container for probe selection */
    SG::ReadHandleKey<xAOD::JetContainer> m_jetKey{ this, "JetKey" , "AntiKt4LCTopoJets", ""};
    /*! Event Info key */
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{ this, "EventInfoKey", "EventInfo", "" };
 

    /*! Pass a string to pick up correct selector */
    std::map< std::string, unsigned int > m_PidToolMap; 
   
    
};

#endif
