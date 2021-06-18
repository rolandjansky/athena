/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigL0GepPerf/GepJetAlg.h"

// Base class
#include "TrigL0GepPerf/IJetMaker.h"

// *** Include derived jet algorithm classes ***



#include "xAODEventInfo/EventInfo.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "xAODTrigger/JetRoI.h"
#include "xAODTrigger/JetRoIAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODTrigger/JetRoIContainer.h"




GepJetAlg::GepJetAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAnalysisAlgorithm( name, pSvcLocator ),
  m_jetAlg ("JetAlg"),
  m_topoclLabel ("TopoclLabel")
{

  declareProperty("JetAlg", m_jetAlg, "Custom jet algorithm label. New custom jets will be named m_jetAlg + m_topoclLabel +\"Jets\".");
  declareProperty("TopoclLabel", m_topoclLabel, "Topoclusters label");

}



StatusCode GepJetAlg::execute() {
  ATH_MSG_DEBUG ("Executing " << name() << "...");


  // get clusters
  std::string topoclName = m_topoclLabel + "TopoClusters";
  const xAOD::CaloClusterContainer* clusters = nullptr;
  CHECK(evtStore()->retrieve(clusters,topoclName));


  std::vector<Gep::CustomTopoCluster> customClusters;
  for( auto iClus: *clusters){
    Gep::CustomTopoCluster clus;
    clus.vec.SetPxPyPzE(iClus->p4().Px(), iClus->p4().Py(),
                          iClus->p4().Pz(), iClus->e());
    customClusters.push_back(clus);
  }


  // run jet algorithm
  std::unique_ptr<Gep::IJetMaker> jetMaker{};


  // *** instantiate custom jet algorithm classes ***



  if( !jetMaker ){
    ATH_MSG_ERROR( "JetMaker is a null pointer." );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Running " << jetMaker->getName() << " jet algorithm." );

  std::vector<Gep::CustomJet> customJets = jetMaker->makeJet( customClusters );


  // create the new container and its auxiliary store
  auto athenaJets = std::make_unique<xAOD::JetContainer>();
  auto athenaJetsAux = std::make_unique<xAOD::JetAuxContainer>();

  athenaJets->setStore(athenaJetsAux.get()); //< connect the two


  // store custom jets in athena format
  for(const auto& iJet: customJets){

    std::unique_ptr<xAOD::Jet> athenaJet{new xAOD::Jet()};
    xAOD::Jet* ijet = athenaJet.get();
    athenaJets->push_back(std::move(athenaJet));

    xAOD::JetFourMom_t P4;
    P4.SetPt(iJet.vec.Pt());
    P4.SetEta(iJet.vec.Eta());
    P4.SetPhi(iJet.vec.Phi());
    P4.SetM(iJet.vec.M());

    ijet->setJetP4( P4 );

  }

  std::string jetsName = m_jetAlg + m_topoclLabel + "Jets";
  std::string jetsNameAux = jetsName + "Aux.";

  ATH_MSG_DEBUG( "Storing " << jetsName );
  CHECK( evtStore()->record(std::move(athenaJets),jetsName) );
  CHECK( evtStore()->record(std::move(athenaJetsAux),jetsNameAux) );


  return StatusCode::SUCCESS;
}
