/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaMonitorElectronAlgorithm_H
#define TrigEgammaMonitorElectronAlgorithm_H


#include "TrigEgammaMonitorAnalysisAlgorithm.h"



class TrigEgammaMonitorElectronAlgorithm: public TrigEgammaMonitorAnalysisAlgorithm 
{

  public:

    TrigEgammaMonitorElectronAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~TrigEgammaMonitorElectronAlgorithm() override;

    virtual StatusCode initialize() override;
    
    virtual StatusCode fillHistograms( const EventContext& ctx) const override;

 
  
  private:
 

  
    /*! navigation method called by executeNavigation */
    StatusCode executeNavigation(const EventContext& ctx, const std::string trigItem,float,std::string,
                                 std::vector<std::pair<const xAOD::Egamma*, const TrigCompositeUtils::Decision*>> &) const;
    
    
    /*! List of triggers to study */
    std::vector<std::string> m_trigList;
    /*! List of triggers from menu */
    Gaudi::Property<std::vector<std::string>> m_trigInputList{this, "TriggerList", {}};
    /*! Directory name for each algorithm */
    Gaudi::Property<std::string> m_anatype{this, "Analysis", "Electron"};
    /*! Define isolation working point for Probe electron */
    Gaudi::Property<std::string> m_offProbeIsolation{ this, "OfflineProbeIsolation", "Loose"};
    /*! force probe isolation */
    Gaudi::Property<bool> m_forceProbeIsolation{this, "ForceProbeIsolation", false};
    /*! force pid and crack selection into electron navigation */
    Gaudi::Property<bool> m_forcePidSelection{ this, "ForcePidSelection", true};
    /* force et cluster cut*/
    Gaudi::Property<bool> m_forceEtThr{ this, "ForceEtThreshold", true};
    /* force experimentalSelection selection*/
    Gaudi::Property<bool> m_forceVetoVeryLoose{ this, "ForceVetoVeryLoose", false};
    /*! Remove crack region for Probe default True */
    Gaudi::Property<bool> m_rmCrack{ this, "RemoveCrack", true};
    /*! Event Wise offline ElectronContainer Access and end iterator */
    SG::ReadHandleKey<xAOD::ElectronContainer> m_offElectronKey{ this, "ElectronKey", "Electrons", ""};
    
};

#endif
