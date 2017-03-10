// TrackCaloClusterRecAlgs includes
#include "TrackCaloClusterRecAlgs/TrackCaloClusterRecAlg.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODTrackCaloCluster/TrackCaloClusterAuxContainer.h"

// #include "xAODJet/JetContainer.h"
// #include "xAODJet/JetAuxContainer.h"

TrackCaloClusterRecAlg::TrackCaloClusterRecAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_trackCaloClusterWeightsTool("TrackCaloClusterRecTools/TrackCaloClusterWeightsTool"),
  m_trackCaloClusterCreatorTool("TrackCaloClusterRecTools/TrackCaloClusterCreatorTool") {
      
    declareProperty("TrackParticleContainerName"   ,    m_trkParticleName                 = "InDetTrackParticles"   );
    declareProperty("OutputCollectionPostFix"      ,    m_outputPostFix                   = ""                      );
    declareProperty("CaloClustersLocation"         ,    m_caloClusters                    = "CaloCalTopoClusters"   );
    declareProperty("TrackCaloClusterContainerName",    m_trackCaloClusterContainerName   = "TrackCaloClusters"     );
    declareProperty("TrackCaloClusterWeightsTool"  ,    m_trackCaloClusterWeightsTool                               );
    declareProperty("TrackCaloClusterCreatorTool"  ,    m_trackCaloClusterCreatorTool                               );
  }
  
TrackCaloClusterRecAlg::~TrackCaloClusterRecAlg() {}
  
StatusCode TrackCaloClusterRecAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");    
  ATH_CHECK(m_trackCaloClusterWeightsTool.retrieve());
  ATH_CHECK(m_trackCaloClusterCreatorTool.retrieve());
  return StatusCode::SUCCESS;
}
  
StatusCode TrackCaloClusterRecAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");  
  return StatusCode::SUCCESS;
}
  
StatusCode TrackCaloClusterRecAlg::execute() {  
    ATH_MSG_DEBUG ("Executing " << name() << "...");
    // Create cluster-to-tracks map
    std::multimap <const xAOD::CaloCluster*, const xAOD::TrackParticle*> clusterToTracksMap;
    std::map <const xAOD::TrackParticle*, FourMom_t> TrackTotalClusterPt;
    std::map <const xAOD::CaloCluster*, FourMom_t> clusterToTracksWeightMap;
    
    // Charged
    xAOD::TrackCaloClusterContainer* tccContainer = new xAOD::TrackCaloClusterContainer();
    ATH_CHECK( evtStore()->record( tccContainer, m_trackCaloClusterContainerName + "Charged" ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux, m_trackCaloClusterContainerName + "Charged" + "Aux." ) );
    tccContainer->setStore( tccContainerAux );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName + "Charged" );    

    // Look at associated clusters
    if ( !evtStore()->contains<xAOD::TrackParticleClusterAssociationContainer>( m_trkParticleName+"ClusterAssociations"+m_outputPostFix) ) {
                ATH_MSG_WARNING ("Couldn't retrieve TrackParticleClusterAssociations with key: " << m_trkParticleName+"ClusterAssociations"+m_outputPostFix );
                return StatusCode::SUCCESS;
        }
    
    const auto associatedClusters = getContainer<xAOD::TrackParticleClusterAssociationContainer>(m_trkParticleName+"ClusterAssociations"+m_outputPostFix);
    
    // Look at all clusters
    if ( !evtStore()->contains<xAOD::CaloClusterContainer>( m_caloClusters) ) {
                ATH_MSG_WARNING ("Couldn't retrieve CaloClusterContainer with key: " << m_caloClusters );
                return StatusCode::SUCCESS;
        }
    
    const auto allClusters = getContainer<xAOD::CaloClusterContainer>(m_caloClusters);
    
    // Look at all tracks
    if ( !evtStore()->contains<xAOD::TrackParticleContainer>( m_trkParticleName) ) {
                ATH_MSG_WARNING ("Couldn't retrieve TrackParticleContainer with key: " << m_trkParticleName );
                return StatusCode::SUCCESS;
        }
    
    const auto allTracks = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);
    
    // Create the weights
    m_trackCaloClusterWeightsTool->fillWeightMaps(associatedClusters, &clusterToTracksMap, &TrackTotalClusterPt, &clusterToTracksWeightMap);
    
    // Create charged xAOD::TCC
    m_trackCaloClusterCreatorTool->createChargedTCCs(tccContainer, associatedClusters, &TrackTotalClusterPt, &clusterToTracksWeightMap);
    
    // Charged + Neutral
    xAOD::TrackCaloClusterContainer* tccContainer1 = new xAOD::TrackCaloClusterContainer(*tccContainer);
    ATH_CHECK( evtStore()->record( tccContainer1, m_trackCaloClusterContainerName ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux1 = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux1, m_trackCaloClusterContainerName + "Aux." ) );
    tccContainer1->setStore( tccContainerAux1 );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName ); 
    
    // Create neutral xAOD::TCC
    m_trackCaloClusterCreatorTool->createNeutralTCCs(tccContainer1, allClusters, &clusterToTracksWeightMap);
    
    // Charged + Neutral + TrackOnly
    xAOD::TrackCaloClusterContainer* tccContainer2 = new xAOD::TrackCaloClusterContainer(*tccContainer1);
    ATH_CHECK( evtStore()->record( tccContainer2, m_trackCaloClusterContainerName + "All" ) );

    xAOD::TrackCaloClusterAuxContainer* tccContainerAux2 = new xAOD::TrackCaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( tccContainerAux2, m_trackCaloClusterContainerName + "All" + "Aux." ) );
    tccContainer2->setStore( tccContainerAux2 );
    ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_trackCaloClusterContainerName + "All" ); 
    
    // Create trackonly xAOD::TCC
    m_trackCaloClusterCreatorTool->createTrackOnlyTCCs(tccContainer2, allTracks, &TrackTotalClusterPt);
    
/* 
  // create a vector of PseudoJet (similar to 4-vector) that is used as input to fastjet to build the large-R jets
  std::vector<fastjet::PseudoJet> jet_clusters;
  for (auto const cluster : *tccContainer ){
    double pt_cluster = cluster->pt();
    double phi = cluster->phi();

    double px_cluster = pt_cluster*cos(phi);
    double py_cluster = pt_cluster*sin(phi);
    double pz_cluster = pt_cluster*sinh(cluster->eta());
    double e_cluster = sqrt(pt_cluster*pt_cluster + pz_cluster*pz_cluster + cluster->m()*cluster->m()); //clusters are massless

    if(e_cluster > 0. && pt_cluster > 1.){
      fastjet::PseudoJet this_cluster(px_cluster, py_cluster, pz_cluster, e_cluster);
      jet_clusters.push_back(this_cluster);
    }
  }

  fastjet::Strategy strategy = fastjet::Best;
  fastjet::RecombinationScheme recomb_scheme = fastjet::E_scheme;

  //ATLAS specific variables for fastjet
  double m_ghostMaxrap = 6.0;
  unsigned int m_ghostRepeat = 1;
  double m_ghostArea = 0.01;
  double m_gridScatter = 1.0;
  double ptmin = 5000.; // we don't want to store jets below 5 GeV                                                                                                               
  
  // Which algorithm and jet radius??
  fastjet::JetAlgorithm jet_alg = fastjet::antikt_algorithm;
  fastjet::JetDefinition jd = fastjet::JetDefinition(jet_alg, 1.0, recomb_scheme, strategy);
  // Some fastjet definitions
  fastjet::GhostedAreaSpec area_spec(m_ghostMaxrap, m_ghostRepeat, m_ghostArea, m_gridScatter);
  fastjet::AreaDefinition area_def(fastjet::active_area_explicit_ghosts, area_spec);
  fastjet::ClusterSequenceArea cs_clusters(jet_clusters, jd, area_def); // input here are the clusters in the event
  
  //Let's get the ungroomed jets:
  std::vector<fastjet::PseudoJet> sorted_jets = fastjet::sorted_by_pt(cs_clusters.inclusive_jets(ptmin));
  
  xAOD::JetContainer* xaodJets = new xAOD::JetContainer();
  ATH_CHECK( evtStore()->record(xaodJets, m_jetContainerName) );

  xAOD::JetAuxContainer* xaodJetsAux = new xAOD::JetAuxContainer();
  ATH_CHECK( evtStore()->record( xaodJetsAux, m_jetContainerName + "Aux." ) );
  xaodJets->setStore( xaodJetsAux );
  ATH_MSG_DEBUG( "Recorded JetContainer with key: " << m_jetContainerName ); 
  
  //Let's define the parameters of the trimming algorithm
  float ptfrac = 0.05;
  float Rtrim = 0.2;

  fastjet::JetAlgorithm subjet_alg = fastjet::kt_algorithm; //subjets are built with the kt algorithm and then removed if they have a certain pt fraction
  //fastjet definitions for the trimmer
  fastjet::JetDefinition sjd = fastjet::JetDefinition(subjet_alg, Rtrim, recomb_scheme, strategy);
  //Trimming
  fastjet::Filter actrimmer(sjd, fastjet::SelectorPtFractionMin(ptfrac));
  
  xAOD::JetContainer* xaodJetsTrimmed = new xAOD::JetContainer();
  ATH_CHECK( evtStore()->record( xaodJetsTrimmed, m_jetContainerName + "Trimmed" ) );

  xAOD::JetAuxContainer* xaodJetsTrimmedAux = new xAOD::JetAuxContainer();
  ATH_CHECK( evtStore()->record( xaodJetsTrimmedAux, m_jetContainerName + "Trimmed" + "Aux." ) );
  xaodJetsTrimmed->setStore( xaodJetsTrimmedAux );
  ATH_MSG_DEBUG( "Recorded JetContainer with key: " << m_jetContainerName + "Trimmed" ); 
  
  const xAOD::Jet* pparent = nullptr;
  // Loop over the ungroomed jets and save them into xAOD::Jets
  for(unsigned int ij = 0; ij < sorted_jets.size(); ij++){
    xAOD::Jet* pjet = m_bld->add(sorted_jets[ij], *xaodJets, pparent);
    fastjet::PseudoJet trimmed_jet = actrimmer(sorted_jets[ij]);
    xAOD::Jet* pjet1 = m_bld->add(trimmed_jet, *xaodJetsTrimmed, pparent);
  }
   */
  return StatusCode::SUCCESS;
}
