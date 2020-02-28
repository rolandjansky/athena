/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavBaseTool_H
#define TrigEgammaNavBaseTool_H

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
#include "xAODTrigger/EmTauRoIContainer.h"


namespace Trig{
  class FeatureContainer;
}

class TrigEgammaNavBaseTool:  public TrigEgammaAnalysisBaseTool,
                              virtual public ITrigEgammaAnalysisBaseTool 

{
  ASG_TOOL_CLASS(TrigEgammaNavBaseTool, ITrigEgammaAnalysisBaseTool)

  public:
  
    TrigEgammaNavBaseTool( const std::string& myname );
    virtual ~TrigEgammaNavBaseTool() {};
    virtual StatusCode childInitialize() override;
    virtual StatusCode childFinalize() override;
  
  
  protected:
    /*! navigation method called by derived classes */
    StatusCode executeNavigation(const EventContext& ctx, const TrigInfo,
                                 std::vector<std::pair<const xAOD::Egamma*, const TrigEgammaMatchingUtils::Element>> & )  const;
    /*! Rerun offline selection */
    bool ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname) const;
    
    
    /*! List of triggers from menu */
    std::vector<std::string> m_trigInputList;
    /*! List of triggers to study */
    std::vector<std::string> m_trigList;
    /*! Directory name for each algorithm */
    std::string m_anatype;
    /*! The basepath */
    std::string m_dir;
    
  
  private:
  
    /*! navigation method called by executeNavigation */
    StatusCode executeElectronNavigation(const EventContext& ctx, const std::string trigItem,float,std::string,
                                         std::vector<std::pair<const xAOD::Egamma*, const TrigEgammaMatchingUtils::Element>> &) const;
    
    /*! navigation method called by executeNavigation */
    StatusCode executePhotonNavigation(const EventContext& ctx, const std::string trigItem,float,
                                       std::vector<std::pair<const xAOD::Egamma*, const TrigEgammaMatchingUtils::Element>> &) const;
  
    /*! Photon pid word */
    std::string m_photonPid;
    /*! doUnconverted analysis */
    bool m_doUnconverted;
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
    SG::ReadHandleKey<xAOD::PhotonContainer> m_offPhotonKey{ this, "PhotonKey", "Photons", ""};
    
};

#endif
