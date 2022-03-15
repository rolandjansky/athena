/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/JetPFlowSelectionAlg.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/WriteHandle.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODPFlow/FlowElementAuxContainer.h"


StatusCode JetPFlowSelectionAlg::initialize() {
  ATH_MSG_DEBUG("Initializing  " );

  ATH_CHECK(m_ChargedPFlowContainerKey.initialize());
  ATH_CHECK(m_NeutralPFlowContainerKey.initialize());
  ATH_CHECK(m_outputChargedPFlowHandleKey.initialize());
  ATH_CHECK(m_outputNeutralPFlowHandleKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode JetPFlowSelectionAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG(" execute() ... ");

  SG::ReadHandle<xAOD::FlowElementContainer> ChargedPFlowObjects(m_ChargedPFlowContainerKey, ctx);
  if (! ChargedPFlowObjects.isValid()){
    ATH_MSG_ERROR("Can't retrieve input container "<< m_ChargedPFlowContainerKey);                  
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::FlowElementContainer> NeutralPFlowObjects(m_NeutralPFlowContainerKey, ctx);
  if (! NeutralPFlowObjects.isValid()){
    ATH_MSG_ERROR("Can't retrieve input container "<< m_NeutralPFlowContainerKey);                  
    return StatusCode::FAILURE;
  }

  auto selectedChargedPFlowObjects = std::make_unique<xAOD::FlowElementContainer>(); // SG::VIEW_ELEMENTS
  auto selectedChargedPFlowObjectsAux = std::make_unique<xAOD::FlowElementAuxContainer>();
  selectedChargedPFlowObjects->setStore(selectedChargedPFlowObjectsAux.get());

  auto selectedNeutralPFlowObjects = std::make_unique<xAOD::FlowElementContainer>();
  auto selectedNeutralPFlowObjectsAux = std::make_unique<xAOD::FlowElementAuxContainer>();
  selectedNeutralPFlowObjects->setStore(selectedNeutralPFlowObjectsAux.get());

  // To store the charged FE objects matched to an electron/muon
  std::vector< const xAOD::FlowElement* > ChargedPFlowObjects_matched;

  // Loop over Charged FE objects
  for ( const xAOD::FlowElement* fe : *ChargedPFlowObjects ) {

    // Select FE object if not matched to an electron or muon via links
    if ( !checkLeptonLinks(fe) ){
      xAOD::FlowElement* selectedFE = new xAOD::FlowElement();
      selectedChargedPFlowObjects->push_back(selectedFE);
      *selectedFE = *fe; // copies auxdata
    }
    else { // Use the matched object to put back its energy later
      ChargedPFlowObjects_matched.push_back(fe);
    }

  } // End loop over Charged FE Objects


  // Loop over Neutral FE objects
  for ( const xAOD::FlowElement* fe : *NeutralPFlowObjects ) {

    xAOD::FlowElement* selectedFE = new xAOD::FlowElement();
    selectedNeutralPFlowObjects->push_back(selectedFE);
    *selectedFE = *fe;

  } // End loop over Neutral FE Objects


  // Add the energy from removed charged FE clusters to neutral FE object 
  // if shared clusters exist, create the new neutral FE object otherwise
  for ( const xAOD::FlowElement* chargedFE : ChargedPFlowObjects_matched ){
      
    // Get charged FE topoclusters and weights
    std::vector<std::pair<const xAOD::IParticle*,float> > theOtherPairs_charged = chargedFE->otherObjectsAndWeights();
    std::vector<ElementLink<xAOD::IParticleContainer>> theOtherLinks_charged = chargedFE->otherObjectLinks();

    // Loop over charged FE topoclusters
    for (unsigned int iCluster = 0; iCluster < chargedFE->nOtherObjects(); ++iCluster){

      bool thisCluster_matched = false;

      std::pair<const xAOD::IParticle*,float> theOtherPair_charged = theOtherPairs_charged[iCluster];
      const xAOD::IParticle* theCluster_charged = theOtherPair_charged.first;
      float theClusterWeight_charged = theOtherPair_charged.second;

      // Loop over neutral FE objects
      for ( xAOD::FlowElement* neutralFE : *selectedNeutralPFlowObjects ) {
        if (thisCluster_matched) continue;

        // Loop over neutral FE topoclusters
        std::vector<std::pair<const xAOD::IParticle*,float> > theOtherPairs_neutral = neutralFE->otherObjectsAndWeights();
        for (auto& [theCluster_neutral, theClusterWeight_neutral] : theOtherPairs_neutral){

          // If topoclusters are matched, add the energy to the neutral FE object
          if (theCluster_charged == theCluster_neutral){

            // Add the energy to the neutral FE object
            float newEnergy = neutralFE->e() + theClusterWeight_charged;
            neutralFE->setP4(newEnergy/cosh(neutralFE->eta()), 
                            neutralFE->eta(),
                            neutralFE->phi(),
                            neutralFE->m());

            ATH_MSG_DEBUG("Updated neutral FlowElement with E, pt, eta and phi: "
                    << neutralFE->e() << ", " << neutralFE->pt() << ", "
                    << neutralFE->eta() << " and " << neutralFE->phi());

            thisCluster_matched = true;
          }

        } // End loop over neutral FE clusters
      } // End loop over neutral FE objects

      // If a topocluster is left unmatched, create a neutral FE object.
      // Ignore topoclusters with nullptr
      if ( !thisCluster_matched && theCluster_charged ){

        xAOD::FlowElement* newFE = new xAOD::FlowElement();
        selectedNeutralPFlowObjects->push_back(newFE);

        newFE->setP4(theClusterWeight_charged / cosh(theCluster_charged->eta()),  // using energy from charged FE weight, not cluster->e()
                    theCluster_charged->eta(),
                    theCluster_charged->phi(),
                    theCluster_charged->m());
        newFE->setCharge(0);
        newFE->setSignalType(xAOD::FlowElement::SignalType::NeutralPFlow);

        ATH_MSG_DEBUG("Created neutral FlowElement with E, pt, eta and phi: "
                  << newFE->e() << ", " << newFE->pt() << ", "
                  << newFE->eta() << " and " << newFE->phi());

        std::vector<ElementLink<xAOD::IParticleContainer>> theClusters;
        ElementLink< xAOD::IParticleContainer > theIParticleLink;
        theIParticleLink.resetWithKeyAndIndex(theOtherLinks_charged[iCluster].persKey(), theOtherLinks_charged[iCluster].persIndex()); 

        theClusters.push_back(theIParticleLink);
        newFE->setOtherObjectLinks(theClusters);
      }
        
    } // End loop over topoclusters of removed charged FE objects
  } // End loop over removed charged FE objects


  auto handle_ChargedPFlow_out =  SG::makeHandle(m_outputChargedPFlowHandleKey, ctx);
  if (!handle_ChargedPFlow_out.record(std::move(selectedChargedPFlowObjects), std::move(selectedChargedPFlowObjectsAux)) ){
    ATH_MSG_ERROR("Can't record output PFlow container "<< m_outputChargedPFlowHandleKey);
    return StatusCode::FAILURE;
  }

  auto handle_NeutralPFlow_out =  SG::makeHandle(m_outputNeutralPFlowHandleKey, ctx);
  if (!handle_NeutralPFlow_out.record(std::move(selectedNeutralPFlowObjects), std::move(selectedNeutralPFlowObjectsAux)) ){
    ATH_MSG_ERROR("Can't record output PFlow container "<< m_outputNeutralPFlowHandleKey);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

bool JetPFlowSelectionAlg::checkLeptonLinks(const xAOD::FlowElement* fe) const {

  // Links to electrons
  static const SG::AuxElement::ConstAccessor< std::vector < ElementLink< xAOD::ElectronContainer > > > acc_electron_FE_Link("FE_ElectronLinks");
  if( acc_electron_FE_Link.isAvailable(*fe) ){
    std::vector< ElementLink< xAOD::ElectronContainer > > ElectronLinks=acc_electron_FE_Link(*fe);

    for (ElementLink<xAOD::ElectronContainer>& ElectronLink: ElectronLinks){
      if (!ElectronLink.isValid()){
        ATH_MSG_WARNING("JetPFlowSelectionAlg encountered an invalid electron element link. Skipping. ");
        continue; 
      }

      const xAOD::Electron* electron = *ElectronLink;
      bool passElectronID = false;
      bool gotID = electron->passSelection(passElectronID, m_electronID);
      if (!gotID) {
	      ATH_MSG_WARNING("Could not get Electron ID");
	      continue;
      }

      if( electron->pt() > 10000 && passElectronID){
        return true;
      }
	  }
  }

  // Links to muons
  SG::AuxElement::ConstAccessor< std::vector < ElementLink< xAOD::MuonContainer > > > acc_muon_FE_Link("FE_MuonLinks");
  if( acc_muon_FE_Link.isAvailable(*fe) ){
    std::vector< ElementLink< xAOD::MuonContainer > > MuonLinks=acc_muon_FE_Link(*fe);

    for (ElementLink<xAOD::MuonContainer>& MuonLink: MuonLinks){
      if (!MuonLink.isValid()){
        ATH_MSG_WARNING("JetPFlowSelectionAlg encountered an invalid muon element link. Skipping. ");
        continue; 
      }

      //Details of medium muons are here:
      //https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSelectionTool
      const xAOD::Muon* muon = *MuonLink;
      if ( muon->quality() <= xAOD::Muon::Medium && muon->muonType() == xAOD::Muon::Combined ){
        return true;
      }

    }
  }

  return false;
}