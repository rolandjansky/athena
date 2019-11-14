/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "topoEgammaBuilder.h"
#include "smallChrono.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/EventContext.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

// INCLUDE GAUDI HEADER FILES:
#include <algorithm> 
#include <cmath>

namespace {
static const SG::AuxElement::Accessor < std::vector< 
  ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
}

topoEgammaBuilder::topoEgammaBuilder(const std::string& name, 
                                     ISvcLocator* pSvcLocator): 
  AthReentrantAlgorithm(name, pSvcLocator),
  m_timingProfile("ChronoStatSvc", name)
{
}

// =================================================================
StatusCode topoEgammaBuilder::initialize()
{
  // initialize method

  ATH_MSG_DEBUG("Initializing topoEgammaBuilder");

  // the data handle keys
  ATH_CHECK(m_electronOutputKey.initialize(m_doElectrons));
  ATH_CHECK(m_electronSuperClusterRecContainerKey.initialize(m_doElectrons));
  ATH_CHECK(m_photonOutputKey.initialize(m_doPhotons));
  ATH_CHECK(m_photonSuperClusterRecContainerKey.initialize(m_doPhotons));
  
  //////////////////////////////////////////////////
  // retrieve tools
  ATH_MSG_DEBUG("Retrieving " << m_egammaTools.size() << " tools for egamma objects");
  ATH_CHECK( m_clusterTool.retrieve() );
  ATH_CHECK( m_egammaTools.retrieve() );

  if ( m_doElectrons ){
    ATH_MSG_DEBUG("Retrieving " << m_electronTools.size() << " tools for electrons");
    ATH_CHECK( m_electronTools.retrieve() );
  }
  if ( m_doPhotons ){
    ATH_MSG_DEBUG("Retrieving " << m_photonTools.size() << " tools for photons");
    ATH_CHECK( m_photonTools.retrieve() );
  }

  if ( m_doElectrons && m_doPhotons ){
    ATH_CHECK( m_ambiguityTool.retrieve() );
  }
 
  // retrieve timing profile
  if (m_doChrono) CHECK( m_timingProfile.retrieve() );

  ATH_MSG_DEBUG("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode topoEgammaBuilder::finalize(){
  // finalize method
  return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode topoEgammaBuilder::execute(const EventContext& ctx) const{
  // athena execute method

  ATH_MSG_DEBUG("Executing topoEgammaBuilder");

  // Chrono name for each Tool
  std::string chronoName;

  const EgammaRecContainer* inputElRecs = nullptr;
  const EgammaRecContainer* inputPhRecs = nullptr;
  xAOD::ElectronContainer* electrons = nullptr;
  xAOD::PhotonContainer* photons = nullptr;
  /* From here one if a Read/Write handle is retrieved the above will be != nullptr
   * for electron or photons or both 
   */
  if (m_doElectrons){
    SG::ReadHandle<EgammaRecContainer> electronSuperRecs(m_electronSuperClusterRecContainerKey, ctx);
    // the output handles
    SG::WriteHandle<xAOD::ElectronContainer> electronContainer(m_electronOutputKey, ctx);
    ATH_CHECK(electronContainer.record(std::make_unique<xAOD::ElectronContainer>(),
                                       std::make_unique<xAOD::ElectronAuxContainer>()));

    inputElRecs=electronSuperRecs.ptr();
    electrons=electronContainer.ptr();
  }
  if (m_doPhotons){
    SG::ReadHandle<EgammaRecContainer> photonSuperRecs(m_photonSuperClusterRecContainerKey, ctx);
    SG::WriteHandle<xAOD::PhotonContainer> photonContainer(m_photonOutputKey, ctx);
    ATH_CHECK(photonContainer.record(std::make_unique<xAOD::PhotonContainer>(),
                                     std::make_unique<xAOD::PhotonAuxContainer>()));

    inputPhRecs=photonSuperRecs.ptr();
    photons=photonContainer.ptr();
  }

  if (inputElRecs&&electrons){
    for (const egammaRec* electronRec : *inputElRecs) { 
      unsigned int author = xAOD::EgammaParameters::AuthorElectron;
      xAOD::AmbiguityTool::AmbiguityType type= xAOD::AmbiguityTool::electron;
      if(inputPhRecs){
        // get the hottest cell
        const xAOD::CaloCluster *const elClus = electronRec->caloCluster();
        const auto elEta0 = elClus->eta0();
        const auto elPhi0 = elClus->phi0();
        for (const egammaRec* photonRec : *inputPhRecs) {
          const xAOD::CaloCluster *const phClus = photonRec->caloCluster();
          //See if they have the same hottest cell
          if (elEta0 == phClus->eta0() && elPhi0 == phClus->phi0()) {
            ATH_MSG_DEBUG("Running AmbiguityTool for electron");
            author = m_ambiguityTool->ambiguityResolve(elClus,
                                                       photonRec->vertex(),
                                                       electronRec->trackParticle(),
                                                       type);
            break;
          }
        }
      }
      //Create Electron xAOD objects
      if (author == xAOD::EgammaParameters::AuthorElectron || 
          author == xAOD::EgammaParameters::AuthorAmbiguous){
        ATH_MSG_DEBUG("getElectron");
        if ( !getElectron(electronRec, electrons, author,type) ){
          return StatusCode::FAILURE;
        }
      }
    }
  }

  if (inputPhRecs&&photons){
    for (const auto& photonRec : *inputPhRecs) {
      unsigned int author = xAOD::EgammaParameters::AuthorPhoton;
      xAOD::AmbiguityTool::AmbiguityType type= xAOD::AmbiguityTool::photon;
      if (inputElRecs){
        // get the hottest cell
        const xAOD::CaloCluster *const phClus = photonRec->caloCluster();
        const auto phEta0 = phClus->eta0();
        const auto phPhi0 = phClus->phi0();
        for (const auto& electronRec : *inputElRecs) {
          const xAOD::CaloCluster *const elClus = electronRec->caloCluster();
          //See if they have the same hottest cell
          if (phEta0 == elClus->eta0() && phPhi0 == elClus->phi0()) {
            ATH_MSG_DEBUG("Running AmbiguityTool for photon");
            author = m_ambiguityTool->ambiguityResolve(elClus,
                                                       photonRec->vertex(),
                                                       electronRec->trackParticle(),
                                                       type);
            break;
          }
        }
      }
      //Create Photon xAOD objects
      if (author == xAOD::EgammaParameters::AuthorPhoton || 
          author == xAOD::EgammaParameters::AuthorAmbiguous){
        ATH_MSG_DEBUG("getPhoton");
        if ( !getPhoton(photonRec, photons, author,type) ){
          return StatusCode::FAILURE;
        }
      }
    }
  }

  ATH_MSG_DEBUG("Executing : " << m_clusterTool);  
  if ( m_clusterTool->contExecute(ctx, electrons , photons).isFailure() ){
    ATH_MSG_ERROR("Problem executing the " << m_clusterTool<<" tool");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Calling egammaTools: " );
  for (auto& tool : m_egammaTools){
    ATH_MSG_DEBUG("Calling tool " << tool );
    ATH_CHECK( CallTool(ctx, tool, electrons,photons) );
  }

  if(electrons){
    ATH_MSG_DEBUG("Calling Electron tools: ");
    for (auto& tool : m_electronTools){
      ATH_MSG_DEBUG("Calling tool " << tool );
      ATH_CHECK( CallTool(ctx, tool, electrons, nullptr) );
    }
  }
  if(photons){
    ATH_MSG_DEBUG("Calling Photon tools: ");
    for (auto& tool : m_photonTools){
      ATH_MSG_DEBUG("Calling tool " << tool );
      CHECK( CallTool(ctx, tool, nullptr, photons) );
    }
  }
  //Do the ambiguity Links
  if (electrons && photons ){
    ATH_CHECK(doAmbiguityLinks (electrons,photons));
  }

  return StatusCode::SUCCESS;
}


StatusCode topoEgammaBuilder::doAmbiguityLinks(xAOD::ElectronContainer *electronContainer, 
                                               xAOD::PhotonContainer *photonContainer) const {

  ///Needs the same logic as the ambiguity after building the objects (make sure they are all valid)
  static const SG::AuxElement::Accessor<ElementLink<xAOD::EgammaContainer> > ELink ("ambiguityLink");
  ElementLink<xAOD::EgammaContainer> dummylink;
  for (size_t photonIndex=0; photonIndex < photonContainer->size() ; ++photonIndex) {    

    xAOD::Photon* photon = photonContainer->at(photonIndex); 
    ELink(*photon)=dummylink;

    if(photon->author()!= xAOD::EgammaParameters::AuthorAmbiguous){
      continue;
    }      

    for (size_t electronIndex=0; electronIndex < electronContainer->size() ; ++electronIndex) {

      xAOD::Electron* electron = electronContainer->at(electronIndex); 
      if(electron->author()!= xAOD::EgammaParameters::AuthorAmbiguous){
        continue;
      }

      if(caloClusterLinks(*(electron->caloCluster())).at(0) ==
         caloClusterLinks(*(photon->caloCluster())).at(0)){
        ElementLink<xAOD::EgammaContainer> link (*electronContainer,electronIndex);
        ELink(*photon)=link;
        break;
      }
    }
  }
  for (size_t electronIndex=0; electronIndex < electronContainer->size() ; ++electronIndex) {    

    xAOD::Electron* electron = electronContainer->at(electronIndex); 
    ELink(*electron)=dummylink;
    if(electron->author()!= xAOD::EgammaParameters::AuthorAmbiguous){
      continue;
    }      
    for (size_t photonIndex=0; photonIndex < photonContainer->size() ; ++photonIndex) {

      xAOD::Photon* photon = photonContainer->at(photonIndex); 
      if(photon->author()!= xAOD::EgammaParameters::AuthorAmbiguous){
        continue;
      }

      if(caloClusterLinks(*(electron->caloCluster())).at(0) ==
         caloClusterLinks(*(photon->caloCluster())).at(0)){
        ElementLink<xAOD::EgammaContainer> link (*photonContainer,photonIndex);
        ELink(*electron)=link;
        break;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode topoEgammaBuilder::CallTool(const EventContext& ctx, 
                                       const ToolHandle<IegammaBaseTool>& tool, 
                                       xAOD::ElectronContainer *electronContainer /* = 0*/, 
                                       xAOD::PhotonContainer *photonContainer /* = 0*/) const{


  smallChrono timer(*m_timingProfile,this->name()+"_"+tool->name(), m_doChrono);  

  if (electronContainer){    
    ATH_MSG_DEBUG("Executing tool on electrons: " << tool );
    for (const auto& electron : *electronContainer){
      if (tool->execute(ctx, electron).isFailure() ){
        ATH_MSG_ERROR("Problem executing tool on electrons: " << tool);
        return StatusCode::FAILURE;
      }
    }
  }
  if (photonContainer){
    ATH_MSG_DEBUG("Executing tool on photons: " << tool );
    for (const auto& photon : *photonContainer){
      if (tool->execute(ctx, photon).isFailure() ){
        ATH_MSG_ERROR("Problem executing tool on photons: " << tool);
        return StatusCode::FAILURE;
      }
    }
  }  
  return StatusCode::SUCCESS;
}

bool topoEgammaBuilder::getElectron(const egammaRec* egRec, 
                                    xAOD::ElectronContainer *electronContainer,
                                    const unsigned int author,
                                    const uint8_t type) const{  

  if (!egRec || !electronContainer) return false;

  xAOD::Electron *electron = new xAOD::Electron();
  electronContainer->push_back( electron );
  electron->setAuthor( author );

  static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
  acc(*electron) = type;

  std::vector< ElementLink< xAOD::CaloClusterContainer > > clusterLinks;
  for (size_t i = 0 ; i < egRec->getNumberOfClusters(); ++i){
    clusterLinks.push_back( egRec->caloClusterElementLink(i) );
  }
  electron->setCaloClusterLinks( clusterLinks );

  std::vector< ElementLink< xAOD::TrackParticleContainer > > trackLinks;
  for (size_t i = 0 ; i < egRec->getNumberOfTrackParticles(); ++i){
    trackLinks.push_back( egRec->trackParticleElementLink(i) );
  }
  electron->setTrackParticleLinks( trackLinks );

  electron->setCharge(electron->trackParticle()->charge());
  //Set DeltaEta, DeltaPhi , DeltaPhiRescaled
  float deltaEta = static_cast<float>(egRec->deltaEta(0));
  float deltaPhi = static_cast<float>(egRec->deltaPhi(0));
  float deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(0));
  electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta0 );
  electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi0 );
  electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled0 );

  deltaEta = static_cast<float>(egRec->deltaEta(1));
  deltaPhi = static_cast<float>(egRec->deltaPhi(1));
  deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(1));
  electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta1 );
  electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi1 );
  electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled1);

  deltaEta = static_cast<float>(egRec->deltaEta(2));
  deltaPhi = static_cast<float>(egRec->deltaPhi(2));
  deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(2));
  electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta2 );
  electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi2 );
  electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled2);

  deltaEta = static_cast<float>(egRec->deltaEta(3));
  deltaPhi = static_cast<float>(egRec->deltaPhi(3));
  deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(3));
  electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta3 );
  electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi3 );
  electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled3);

  float deltaPhiLast = static_cast<float>(egRec->deltaPhiLast ());
  electron->setTrackCaloMatchValue(deltaPhiLast,xAOD::EgammaParameters::deltaPhiFromLastMeasurement );

  return true;
}

bool topoEgammaBuilder::getPhoton(const egammaRec* egRec,
                                  xAOD::PhotonContainer *photonContainer,
                                  const unsigned int author,
                                  const uint8_t type) const{
  if (!egRec || !photonContainer) return false;

  xAOD::Photon *photon = new xAOD::Photon();
  photonContainer->push_back( photon );
  photon->setAuthor( author );
  static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
  acc(*photon)=type;

  // Transfer the links to the clusters
  std::vector< ElementLink< xAOD::CaloClusterContainer > > clusterLinks;
  for (size_t i = 0 ; i < egRec->getNumberOfClusters(); ++i){
    clusterLinks.push_back( egRec->caloClusterElementLink(i) );
  }
  photon->setCaloClusterLinks( clusterLinks );

  // Transfer the links to the vertices  
  std::vector< ElementLink< xAOD::VertexContainer > > vertexLinks;
  for (size_t i = 0 ; i < egRec->getNumberOfVertices(); ++i){
    vertexLinks.push_back( egRec->vertexElementLink(i) );
  }
  photon->setVertexLinks( vertexLinks );

  // Transfer deltaEta/Phi info
  float deltaEta = egRec->deltaEtaVtx(), deltaPhi = egRec->deltaPhiVtx();
  if (!photon->setVertexCaloMatchValue( deltaEta,
                                        xAOD::EgammaParameters::convMatchDeltaEta1) ){
    ATH_MSG_WARNING("Could not transfer deltaEta to photon");
    return false;
  }

  if (!photon->setVertexCaloMatchValue( deltaPhi,
                                        xAOD::EgammaParameters::convMatchDeltaPhi1) ){
    ATH_MSG_WARNING("Could not transfer deltaPhi to photon");
    return false;
  }
  return true;
}


