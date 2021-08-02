/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */


#ifndef TrigEgammaEmulationToolMT_H
#define TrigEgammaEmulationToolMT_H

// Core staff
#include "TrigEgammaMatchingTool/TrigEgammaMatchingToolMT.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationChain.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include "AsgTools/AsgTool.h"
// Selector includes
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronSelectorTool.h"
#include "RingerSelectorTools/IAsgRingerSelectorTool.h"
// EDM includes
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"




namespace Trig{
  

class TrigData{

  public: 
    TrigData( std::string );
    ~TrigData()=default;

    void clear();
    bool isValid() const;
  

    bool isPassed( const xAOD::Photon *ph , std::string pidname ) const;
    bool isPassed( const xAOD::Electron *el , float avgmu, std::string pidname ) const;
    bool isPassed( const xAOD::TrigRingerRings *rings , float avgmu, std::string pidname ) const;


  public:

    // chain name
    std::string trigger;
    // signature slice type: electron or photon only
    std::string signature;
    
    // L1 object
    const TrigRoiDescriptor *roi;
    // L1 legacy object
    const xAOD::EmTauRoI *l1;
    // Step 1 cluster
    const xAOD::TrigEMCluster *emCluster;
    // Step 1 rings
    const xAOD::TrigRingerRings *rings;
    // Step 2 photon object
    const xAOD::TrigPhoton* trig_photon;
    // Step 3 clusters
    std::vector<const xAOD::CaloCluster*> clusters;
    // Step 2 electrons
    std::vector<const xAOD::TrigElectron*> trig_electrons;
    // Step 4 electrons
    std::vector<const xAOD::Electron*> electrons;
    // Step 4 photons
    std::vector<const xAOD::Photon*> photons;
   

    /* Step 4 photon selectors */
    ToolHandleArray< IAsgPhotonIsEMSelector > egammaPhotonCBTools; 
    /* Step 4 electron selectors */    
    ToolHandleArray<AsgElectronSelectorTool>    egammaElectronDNNTools;
    ToolHandleArray<IAsgElectronIsEMSelector>   egammaElectronCBTools;
    ToolHandleArray<IAsgElectronLikelihoodTool> egammaElectronLHTools;
    // Step 1 electron ringer selectors
    ToolHandleArray<Ringer::IAsgRingerSelectorTool> ringerTools;
};


class TrigEgammaEmulationToolMT: public asg::AsgTool 
{

  ASG_TOOL_INTERFACE( TrigEgammaEmulationToolMT )
  ASG_TOOL_CLASS0( TrigEgammaEmulationToolMT )

  public:

    TrigEgammaEmulationToolMT(const std::string& myname);

    ~TrigEgammaEmulationToolMT()=default;

    StatusCode initialize();

    asg::AcceptData emulate(const TrigCompositeUtils::Decision *dec, std::string trigger) const;
    
    bool match( const TrigCompositeUtils::Decision *dec , Trig::TrigData &, unsigned int condition=TrigDefs::includeFailedDecisions ) const;


  private:

    /* hypo tools */
    ToolHandleArray<Trig::ITrigEgammaEmulationChain> m_hypoTools{ this, "HypoTools", {}, "Hypo tools" };
    
    /*! Trigger decision tool */
    ToolHandle<Trig::TrigDecisionTool> m_trigdec;
    /* Trigger e/g matching tool */
    ToolHandle<TrigEgammaMatchingToolMT> m_matchTool;
    /*! Accept */
    asg::AcceptInfo m_accept;

    Gaudi::Property<std::vector<std::string>> m_electronTrigList{ this, "ElectronTriggerList", {}};
    Gaudi::Property<std::vector<std::string>> m_photonTrigList{ this, "PhotonTriggerList", {}};    

    const ToolHandle<TrigEgammaMatchingToolMT>& match() const {return m_matchTool;}


    /*
     * e/g selectors
     */


    /* photon selectors */
    ToolHandleArray< IAsgPhotonIsEMSelector > m_egammaPhotonCBTools { this, "PhotonCBSelectorTools", {} , "Cut-based tools"};

    /* electron selectors */    
    ToolHandleArray<AsgElectronSelectorTool>    m_egammaElectronDNNTools{ this, "ElectronDNNSelectorTools", {},"DNN tools" };
    ToolHandleArray<IAsgElectronIsEMSelector>   m_egammaElectronCBTools{ this, "ElectronCBSelectorTools", {},"Cut-based tools" };
    ToolHandleArray<IAsgElectronLikelihoodTool> m_egammaElectronLHTools{ this, "ElectronLHSelectorTools", {},"Likelihood tools" };
    
    // electron ringer selectors
    ToolHandleArray<Ringer::IAsgRingerSelectorTool> m_ringerTools{ this, "RingerTools", {}, "Ringer tools" };

};

}


#endif
