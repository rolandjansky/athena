/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    
   
    

    /*! List of triggers from menu after filter*/
    std::vector<std::string> m_trigList;
    

  private:

    /*! at least one chain should pass. e28_tight_iloose? */
    bool minimalTriggerRequirement () const;
    /*! Tag Electron selection */
    bool isTagElectron( ToolHandle<GenericMonitoringTool> monGroup, const xAOD::Electron *el) const;
    /*! Probe selection */
    bool isGoodProbeElectron( ToolHandle<GenericMonitoringTool> monGroup, const xAOD::Electron *el, const xAOD::JetContainer *) const;
    /*! Return pseudo-lifetime of Jpsi */
    float getPseudoLifetime(const xAOD::Electron*,const xAOD::Electron*) const;
    /*! Calculate the displacement of the Jpsi vertex w.r.t. the primary vertex in the transverse plane */
    double simple_lxy(int ,double, double, double , double , double , double , double, double ) const;
    /*! Rerun offline selection */
    void dressPid(const xAOD::Electron *eg) const;
    

    /** Properties **/


    /*! List of triggers from menu */
    Gaudi::Property<std::vector<std::string>> m_trigInputList{this, "TriggerList", {}};
    /*! Tag trigger list */
    Gaudi::Property<std::vector<std::string>> m_tagTrigList{ this, "TagTriggerList", {}};
    /*! Zee lower mass cut */
    Gaudi::Property<float> m_ZeeMassMin{ this, "ZeeLowerMass", 80};
    /*! Zee upper mass cut */
    Gaudi::Property<float> m_ZeeMassMax{ this, "ZeeUpperMass", 100};
    /*! Define the PID for tag electron */
    Gaudi::Property<std::string> m_offTagTightness{ this, "OfflineTagSelector", "Tight"};
    /*! define the Pid of Probe from the user */
    Gaudi::Property<std::string> m_offProbeTightness{ this, "OfflineProbeSelector", "Loose"};
    /*! Select opposite or same-sign pairs -- for background studies */
    Gaudi::Property<bool> m_oppositeCharge{ this, "OppositeCharge", true};
    /*! Minimum tag Et */
    Gaudi::Property<float> m_tagMinEt{ this, "OfflineTagMinEt", 25};
    /*! Minimum probe Et */
    Gaudi::Property<float> m_probeMinEt{this, "OfflineProbeMinEt", 4};
    /*! Probe isolation */
    Gaudi::Property<std::string> m_offProbeIsolation{ this, "OfflineProbeIsolation", "Loose"};
    /*! Remove crack region for Probe default True */
    Gaudi::Property<bool> m_rmCrack{this, "RemoveCrack", true};
    /*! Enable the requirement of triggers */
    Gaudi::Property<bool> m_applyMinimalTrigger{this, "ApplyMinimalTrigger", true};
    /*! Apply nearby jet selection */
    Gaudi::Property<bool> m_applyJetNearProbeSelection{this, "ApplyJetNearProbeSelection", true};
    /*! do jpsiee tag and probe */
    Gaudi::Property<bool> m_doJpsiee{this,"DoJpsiee", false};
    /*! analysis name */
    Gaudi::Property<std::string> m_anatype{ this, "Analysis","Zee"};
    /*! Event Wise offline ElectronContainer Access and end iterator */
    SG::ReadHandleKey<xAOD::ElectronContainer> m_offElectronKey{ this, "ElectronKey", "Electrons", ""};
    /*! Jet container for probe selection */
    SG::ReadHandleKey<xAOD::JetContainer> m_jetKey{ this, "JetKey" , "AntiKt4LCTopoJets", ""};
};

#endif
