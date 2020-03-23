/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaMonitorPhotonAlgorithm_H
#define TrigEgammaMonitorPhotonAlgorithm_H


#include "TrigEgammaMonitorAnalysisAlgorithm.h"




class TrigEgammaMonitorPhotonAlgorithm: public TrigEgammaMonitorAnalysisAlgorithm 
{

  public:

    TrigEgammaMonitorPhotonAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~TrigEgammaMonitorPhotonAlgorithm() override;

    virtual StatusCode initialize() override;
    
    virtual StatusCode fillHistograms( const EventContext& ctx) const override;

 
  
  private:
   
  
    
    /*! navigation method called by executeNavigation */
    StatusCode executeNavigation(const EventContext& ctx, const std::string trigItem,float,
                                 std::vector<std::pair<const xAOD::Egamma*, const TrigCompositeUtils::Decision*>> &) const;
  
    /*! Photon pid word */
    std::string m_photonPid;
    /*! doUnconverted analysis */
    bool m_doUnconverted;
    /*! List of triggers from menu */
    std::vector<std::string> m_trigInputList;
    /*! List of triggers to study */
    std::vector<std::string> m_trigList;
    /*! Directory name for each algorithm */
    std::string m_anatype;
    /*! The basepath */
    std::string m_dir;
    
  
   
 
    // Containers 
    /*! Event Wise offline ElectronContainer Access and end iterator */
    SG::ReadHandleKey<xAOD::PhotonContainer> m_offPhotonKey{ this, "PhotonKey", "Photons", ""};
    
};

#endif
