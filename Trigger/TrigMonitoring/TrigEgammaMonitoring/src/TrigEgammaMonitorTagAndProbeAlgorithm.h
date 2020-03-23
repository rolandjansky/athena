/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigEgammaMonitorTagAndProbeAlgorithm_H
#define TrigEgammaMonitorTagAndProbeAlgorithm_H

#include "TrigEgammaMonitorAnalysisAlgorithm.h"




class TrigEgammaMonitorTagAndProbeAlgorithm: public TrigEgammaMonitorAnalysisAlgorithm 
{

  public:

    TrigEgammaMonitorTagAndProbeAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~TrigEgammaMonitorTagAndProbeAlgorithm() override;

    virtual StatusCode initialize() override;
    
    virtual StatusCode fillHistograms( const EventContext& ctx) const override;


  protected:

    /*! Tag and Probe method called by derived classes */
    bool executeTandP( const EventContext& ctx, std::vector<const xAOD::Electron*> & ) const;

    /*! Match probes called by derived classes */
    void matchObjects(const std::string trigItem,  std::vector<const xAOD::Electron*>, 
                      std::vector<std::pair<const xAOD::Egamma*, const TrigCompositeUtils::Decision*>> &) const; 
    
   
    
    /*! Trigger for tag and event wise selection */
    std::vector<std::string> m_tagTrigList;
    /*! List of triggers from menu */
    std::vector<std::string> m_trigInputList;
    /*! List of triggers from menu after filter*/
    std::vector<std::string> m_trigList;
    

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
    /*! analysis name */
    std::string m_anatype;

     // Containers 
    /*! Event Wise offline ElectronContainer Access and end iterator */
    SG::ReadHandleKey<xAOD::ElectronContainer> m_offElectronKey{ this, "ElectronKey", "Electrons", ""};
    /*! Jet container for probe selection */
    SG::ReadHandleKey<xAOD::JetContainer> m_jetKey{ this, "JetKey" , "AntiKt4LCTopoJets", ""};
    /*! Event Info key */
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{ this, "EventInfoKey", "EventInfo", "" };
 

    
};

#endif
