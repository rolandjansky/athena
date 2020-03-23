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
    
    
    /*! List of triggers from menu */
    std::vector<std::string> m_trigInputList;
    /*! List of triggers to study */
    std::vector<std::string> m_trigList;
    /*! Directory name for each algorithm */
    std::string m_anatype;
    /*! Define isolation working point for Probe electron */
    std::string m_offProbeIsolation;
    /*! force probe isolation */
    bool m_forceProbeIsolation;
    /*! force pid and crack selection into electron navigation */
    bool m_forcePidSelection;
    /* force et cluster cut*/
    bool m_forceEtThr;
    /* force experimentalSelection selection*/
    bool m_forceVetoVeryLoose;
    /*! Remove crack region for Probe default True */
    bool m_rmCrack;
 
  
 
    // Containers 
    /*! Event Wise offline ElectronContainer Access and end iterator */
    SG::ReadHandleKey<xAOD::ElectronContainer> m_offElectronKey{ this, "ElectronKey", "Electrons", ""};
    
};

#endif
