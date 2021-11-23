//Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 

#include "FlavourTaggingTests/PhysicsVariablePlots.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingUtilities.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

#include "CxxUtils/phihelper.h"

#include "FlavorTagDiscriminants/BTagTrackIpAccessor.h"

namespace FTAGValidation {

  PhysicsVariablePlots::PhysicsVariablePlots( const std::string& name, 
					     ISvcLocator* pSvcLocator ) :
    FTAGValidationAlgorithm( name, pSvcLocator ) {}

  PhysicsVariablePlots::~PhysicsVariablePlots() {}
 
  /* ===================================================================== */

  StatusCode PhysicsVariablePlots::initialize() {
    ATH_MSG_INFO( "Inizializing " << name() << " ..." );
    ATH_CHECK( FTAGValidationAlgorithm::initialize() );

    ATH_MSG_INFO( "Properties declaration:" );
    ATH_MSG_INFO( "   -- " << m_jetKey      );
    ATH_MSG_INFO( "   -- " << m_vertexKey   );
    ATH_MSG_INFO( "   -- " << m_trackKey    );

    ATH_MSG_INFO( "   -- " << m_minJetEta );
    ATH_MSG_INFO( "   -- " << m_maxJetEta );
    ATH_MSG_INFO( "   -- " << m_minJetPt  );

    ATH_CHECK( m_jetKey.initialize() );
    ATH_CHECK( m_vertexKey.initialize() );
    ATH_CHECK( m_trackKey.initialize() );

    ATH_CHECK( m_histSvc.retrieve() );
    ATH_CHECK( initializeHistograms() );

    return StatusCode::SUCCESS;
  }

  StatusCode PhysicsVariablePlots::execute() {
    ATH_MSG_DEBUG( "Executing " << name() << " ..." );

    const EventContext context = getContext();

    /* =========================================================================================================================================== */
    /* ==== Retrieve Collections */
    /* =========================================================================================================================================== */

    // Event Info
    SG::ReadHandle< xAOD::EventInfo > eventInfoHandle = SG::makeHandle( m_eventInfoKey,context );
    ATH_CHECK( eventInfoHandle.isValid() );
    const xAOD::EventInfo *eventInfo = eventInfoHandle.get();

    // Retrieve Jet Collection
    const xAOD::JetContainer *jetCollection = nullptr;
    ATH_CHECK( retrieveCollectionfromStoreGate( context,jetCollection,m_jetKey ) );

    // Retrieve Vertex Collection
    const xAOD::VertexContainer *vertexCollection = nullptr;
    ATH_CHECK( retrieveCollectionfromStoreGate( context,vertexCollection,m_vertexKey ) );
    ATH_CHECK( fillHistogram( "nPrimVtx",vertexCollection->size() ) );

    const xAOD::Vertex *primaryVertex = getPrimaryVertex( vertexCollection );
    if ( primaryVertex == nullptr ) {
      ATH_MSG_ERROR( "Could not retrieve Primary Vertex" );
      return StatusCode::FAILURE;
    }

    ATH_CHECK( fillHistogram( "nTracksPrimVtx",primaryVertex->nTrackParticles() ) );
    ATH_CHECK( fillHistogram( "PV_x"  ,primaryVertex->x() ) );
    ATH_CHECK( fillHistogram( "PV_y"  ,primaryVertex->y() ) );
    ATH_CHECK( fillHistogram( "PV_z"  ,primaryVertex->z() ) );

    // Retrieve TrackParticleCollection
    const xAOD::TrackParticleContainer *trackParticleCollection = nullptr;
    ATH_CHECK( retrieveCollectionfromStoreGate( context,trackParticleCollection,m_trackKey) );
    ATH_CHECK( fillHistogram( "nTracks",trackParticleCollection->size() ) );

    /* =========================================================================================================================================== */
    /* ==== ShortList jets */ 
    /* =========================================================================================================================================== */

    std::vector< const xAOD::Jet* > qualityJetCollection;

    for ( const xAOD::Jet *jet : *jetCollection ) {
      if ( not passJetQualitySelection( jet ) ) continue;
      if ( not passJetKinematicSelection( jet ) ) continue;
      if ( not passJetJVTSelection( jet,m_jetKey.key() )  ) continue;
      qualityJetCollection.push_back( jet );
    }

    ATH_CHECK( fillHistogram( "nJets",qualityJetCollection.size() ) );

    /* =========================================================================================================================================== */
    /* ==== Run on Jets */
    /* =========================================================================================================================================== */

    int nJetsWithMuon = 0;
    int nJetsWithSV   = 0;

    for ( const xAOD::Jet *jet : qualityJetCollection )  {

      ATH_MSG_DEBUG( "JET KINEMATICS:" );
      ATH_MSG_DEBUG( "  ** pt=" << jet->p4().Et() / Gaudi::Units::GeV <<
		     " eta=" << jet->eta() <<
		     " phi=" << jet->phi() );

      ATH_CHECK( fillHistogram( "jet_E"   , jet->e()  / Gaudi::Units::GeV ) );
      ATH_CHECK( fillHistogram( "jet_pT"  , jet->pt() / Gaudi::Units::GeV ) );
      ATH_CHECK( fillHistogram( "jet_eta" , jet->eta() ) );
      ATH_CHECK( fillHistogram( "jet_phi" , jet->phi() ) );
      //nMuons and nSV1
      const xAOD::BTagging* bTag = xAOD::BTaggingUtilities::getBTagging( *jet );
      std::vector< ElementLink< xAOD::VertexContainer > > SV1_vertex = bTag->auxdata<std::vector< ElementLink< xAOD::VertexContainer > > >("SV1_vertices");
      if(SV1_vertex.size() >= 1) nJetsWithSV++;

      if (bTag->isAvailable< ElementLink<xAOD::MuonContainer> >("SMT_mu_link")) {
        ElementLink<xAOD::MuonContainer> muonLink = bTag->auxdata< ElementLink<xAOD::MuonContainer> >("SMT_mu_link");
        if ( muonLink.isValid() ) {
          const xAOD::Muon* muon=(*muonLink);
          if ( muon != 0 ) {
            nJetsWithMuon++;
            ATH_CHECK( fillHistogram( "muon_pT_frac" , muon->pt() / jet->pt() ) );
          }
        }
      }
      else {
        ATH_MSG_DEBUG( "SMT_mu_link is not available." );
      }

      // Retrieve jet flavour
      std::string flavour = "DATA_";
      if ( eventInfo->eventType(xAOD::EventInfo::EventType::IS_SIMULATION) ) {
	
	if ( jet->isAvailable< int >( "HadronConeExclTruthLabelID" ) == false ) {
	  ATH_MSG_ERROR( "Input sample is MC but jet has no 'HadronConeExclTruthLabelID' aux data. Something is wrong!" );
	  return StatusCode::FAILURE;
	}
	
	int HadronConeExclTruthLabelID = jet->auxdata< int >( "HadronConeExclTruthLabelID");
	ATH_MSG_DEBUG( "  ** 'HadronConeExclTruthLabelID' is " << HadronConeExclTruthLabelID );
        ATH_CHECK( fillHistogram( "truth_label" , HadronConeExclTruthLabelID ) );
	
	if ( HadronConeExclTruthLabelID == 0 ) flavour = "U_";
	else if ( HadronConeExclTruthLabelID == 4 ) flavour = "C_";
	else if ( HadronConeExclTruthLabelID == 5 ) flavour = "B_";
	else continue;
      }

      ATH_CHECK( analyseTrackParticleObjects( jet,primaryVertex,flavour ) );
      ATH_CHECK( analyseBTaggingObject( jet,flavour ) );
    }

    ATH_CHECK( fillHistogram( "nJetsWithMuon" , nJetsWithMuon ) );
    ATH_CHECK( fillHistogram( "fracJetsWithMuon" , double(nJetsWithMuon)/double(qualityJetCollection.size()) ) );
    ATH_CHECK( fillHistogram( "nJetsWithSV"   , nJetsWithSV ) );
    ATH_CHECK( fillHistogram( "fracJetsWithSV" , double(nJetsWithSV)/double(qualityJetCollection.size()) ) );

    return StatusCode::SUCCESS;
  }

  StatusCode PhysicsVariablePlots::finalize() {
    ATH_MSG_INFO( "Finalizing " << name() << " ..." );
    return StatusCode::SUCCESS;
  }

  /* ===================================================================== */

  StatusCode PhysicsVariablePlots::initializeHistograms() {

    // 1D histograms
    ATH_MSG_DEBUG( "Defining 1D histograms ..." );
    for ( const std::vector< std::string >& definitions : m_Histograms1Ddefinitions ) {
      ATH_MSG_DEBUG( "  ** " << definitions.at( HISTO::NAME ) << " of type " << definitions.at( HISTO::TYPE ) );
      
      if ( definitions.at( HISTO::TYPE ) == "TH1I" )
      	ATH_CHECK( defineHistogram< TH1I >( definitions.at( HISTO::NAME ),
					    definitions.at( HISTO::TITLE ),
					    std::stoi( definitions.at( HISTO::XBINS ) ),
					    std::stoi( definitions.at( HISTO::XMIN ) ),
					    std::stoi( definitions.at( HISTO::XMAX ) ),
					    definitions.at( HISTO::PATH ) ) );
      else if ( definitions.at( HISTO::TYPE ) == "TH1F" )
	ATH_CHECK( defineHistogram< TH1F >( definitions.at( HISTO::NAME ),
					    definitions.at( HISTO::TITLE ),
                                            std::stoi( definitions.at( HISTO::XBINS ) ),
                                            std::stof( definitions.at( HISTO::XMIN ) ),
                                            std::stof( definitions.at( HISTO::XMAX ) ),
					    definitions.at( HISTO::PATH )) );
      else if ( definitions.at( HISTO::TYPE ) == "TH1D" )
	ATH_CHECK( defineHistogram< TH1D >( definitions.at( HISTO::NAME ),
					    definitions.at( HISTO::TITLE ),
                                            std::stoi( definitions.at( HISTO::XBINS ) ),
                                            std::stof( definitions.at( HISTO::XMIN ) ),
                                            std::stof( definitions.at( HISTO::XMAX ) ),
					    definitions.at( HISTO::PATH )) );
    }

    // 2D histograms
    ATH_MSG_DEBUG( "Defining 2D histograms ..." );
    for ( const std::vector< std::string >& definitions : m_Histograms2Ddefinitions ) {
      ATH_MSG_DEBUG( "  ** " << definitions.at( HISTO::NAME ) << " of type " << definitions.at( HISTO::TYPE ) );

      if ( definitions.at( HISTO::TYPE ) == "TH2I" )
        ATH_CHECK( defineHistogram< TH2I >( definitions.at( HISTO::NAME ),
                                            definitions.at( HISTO::TITLE ),
                                            std::stoi( definitions.at( HISTO::XBINS ) ),
                                            std::stoi( definitions.at( HISTO::XMIN ) ),
                                            std::stoi( definitions.at( HISTO::XMAX ) ),
					    std::stoi( definitions.at( HISTO::YBINS ) ),
                                            std::stoi( definitions.at( HISTO::YMIN ) ),
                                            std::stoi( definitions.at( HISTO::YMAX ) ),
                                            definitions.at( HISTO::PATH ) ) );
      else if ( definitions.at( HISTO::TYPE ) == "TH2F" )
        ATH_CHECK( defineHistogram< TH2F >( definitions.at( HISTO::NAME ),
                                            definitions.at( HISTO::TITLE ),
                                            std::stoi( definitions.at( HISTO::XBINS ) ),
                                            std::stof( definitions.at( HISTO::XMIN ) ),
                                            std::stof( definitions.at( HISTO::XMAX ) ),
					    std::stoi( definitions.at( HISTO::YBINS ) ),
                                            std::stof( definitions.at( HISTO::YMIN ) ),
                                            std::stof( definitions.at( HISTO::YMAX ) ),
                                            definitions.at( HISTO::PATH )) );
      else if ( definitions.at( HISTO::TYPE ) == "TH2D" )
        ATH_CHECK( defineHistogram< TH2D >( definitions.at( HISTO::NAME ),
                                            definitions.at( HISTO::TITLE ),
                                            std::stoi( definitions.at( HISTO::XBINS ) ),
                                            std::stof( definitions.at( HISTO::XMIN ) ),
                                            std::stof( definitions.at( HISTO::XMAX ) ),
					    std::stoi( definitions.at( HISTO::YBINS ) ),
                                            std::stof( definitions.at( HISTO::YMIN ) ),
                                            std::stof( definitions.at( HISTO::YMAX ) ),
                                            definitions.at( HISTO::PATH )) );
    }


    return StatusCode::SUCCESS;
  }
  
  StatusCode PhysicsVariablePlots::analyseTrackParticleObjects( const xAOD::Jet *jet,
								const xAOD::Vertex *primaryVertex,
								const std::string& flavour,
								std::string chain ) {

    if( chain != "" ) chain = chain + "_";

    /* =========================================================================================================================================== */
    /* ==== Ghost Tracks */
    /* =========================================================================================================================================== */
/*
    if( chain == "" ) { //No ghost tracks at trigger level
      ATH_MSG_DEBUG( "GHOST TRACKS:" );
    
      std::vector< const xAOD::IParticle* > trackVector = jet->getAssociatedObjects< xAOD::IParticle >( xAOD::JetAttribute::GhostTrack );
      ATH_CHECK( fillHistogram( chain+flavour+"nGhostTracksInJet",trackVector.size() ) );
      ATH_MSG_DEBUG( "  ** nGhostTracks: " << trackVector.size() );

      for ( const xAOD::IParticle* iparticle : trackVector ) {
        const xAOD::TrackParticle* trackParticle = dynamic_cast< const xAOD::TrackParticle* >( iparticle );
        if( not trackParticle ) continue;

        CHECK( computeAndStoreTrackVariables( flavour,jet,
					      "ghost",trackParticle,
					      primaryVertex ) );
      }
    }
*/

    /* =========================================================================================================================================== */
    /* ==== BTagTrackToJetAssociator Tracks */
    /* =========================================================================================================================================== */

    ATH_MSG_DEBUG( "BTagTrackToJetAssociator TRACKS:" );
    const xAOD::BTagging* bTaggingObject = xAOD::BTaggingUtilities::getBTagging( *jet );

    if ( !bTaggingObject ) {
      ATH_MSG_ERROR( "Could not retrieve b-tagging object from selected jet." );
      return StatusCode::FAILURE;
    }

    if ( not bTaggingObject->isAvailable< std::vector< ElementLink<xAOD::TrackParticleContainer > > >("BTagTrackToJetAssociator") ) {
      ATH_MSG_ERROR( "Cannot retrieve 'BTagTrackToJetAssociator' auxdata from b-tagging object!" );
      return StatusCode::FAILURE;
    }

    std::vector< ElementLink< xAOD::TrackParticleContainer > > assocTracks = bTaggingObject->auxdata< std::vector< ElementLink<xAOD::TrackParticleContainer > > >("BTagTrackToJetAssociator");
    ATH_CHECK( fillHistogram( chain+flavour+"numTracks_perJet",assocTracks.size() ) );
    ATH_MSG_DEBUG( "  ** nBTagTracksInJet: " << assocTracks.size() );

    for ( const ElementLink< xAOD::TrackParticleContainer >& trackEL : assocTracks ) {
      if ( not trackEL.isValid() ) continue;

      const xAOD::TrackParticle* trackParticle = *trackEL;

      CHECK( computeAndStoreTrackVariables( flavour,jet,
					    "",trackParticle, //Only supporting one track type to simplify merging with PhysVal definitions
					    primaryVertex,chain ) );      
    }



    return StatusCode::SUCCESS;
  }

  StatusCode PhysicsVariablePlots::computeAndStoreTrackVariables( const std::string& jetType, const xAOD::Jet* jet,
								  const std::string& trackType, const xAOD::TrackParticle *trackParticle,
								  const xAOD::Vertex* primaryVertex, const std::string chain) {

    std::string flavour = jetType + trackType;

    // Jet Variables
    float jet_pt = jet->pt();
    float jet_eta = jet->eta();
    float jet_phi = jet->phi();

    // track Variables
    float track_pt = trackParticle->pt();
    float fracEt = track_pt / jet_pt;

    ATH_CHECK( fillHistogram( chain+flavour+"track_pT_frac",fracEt ) );
    ATH_MSG_DEBUG( "  ** pTfrac = " << fracEt );

    float track_eta = trackParticle->eta();
    float track_phi = trackParticle->phi();

    float deltaEta = fabs( jet_eta - track_eta );
    float deltaPhi = CxxUtils::wrapToPi( jet_phi - track_phi );
    float deltaR = sqrt( deltaEta*deltaEta + deltaPhi*deltaPhi );

    ATH_CHECK( fillHistogram( chain+flavour+"DeltaR_jet_track",deltaR ) );
    ATH_MSG_DEBUG( "  ** deltaR = " << deltaR );

    float d0 = trackParticle->d0();
    float sigmad0 = sqrt( trackParticle->definingParametersCovMatrix()( Trk::d0, Trk::d0 ) );
    float z0 = trackParticle->z0() + trackParticle->vz() - primaryVertex->z();
    float sigmaz0 = sqrt( trackParticle->definingParametersCovMatrix()( Trk::z0, Trk::z0 ) );

    ATH_CHECK( fillHistogram( chain+flavour+"track_d0",d0 ) );
    ATH_CHECK( fillHistogram( chain+flavour+"track_sigd0",d0/sigmad0 ) );

    ATH_CHECK( fillHistogram( chain+flavour+"track_z0",z0 ) );
    ATH_CHECK( fillHistogram( chain+flavour+"track_sigz0",z0/sigmaz0 ) );

    ATH_MSG_DEBUG( "  ** d0 +/- err [significance] = " << d0 << " +/- " << sigmad0 << " [ " << d0/sigmad0 << " ]" );
    ATH_MSG_DEBUG( "  ** z0 +/- err [significance] = " << z0 << " +/- " << sigmaz0 << " [ " << z0/sigmaz0 << " ]" );

    if (trackType == "bTag" || trackType == "") {

      if (chain != "") {
        BTagTrackIpAccessor accessor("btagIp_");
        float d0_signed_sig = accessor.getSignedIp(*trackParticle, *jet).ip3d_signed_d0_significance;
        float z0_signed_sig = accessor.getSignedIp(*trackParticle, *jet).ip3d_signed_z0_significance;
        ATH_CHECK( fillHistogram( chain+flavour+"track_sigd0_signed", d0_signed_sig) );
        ATH_CHECK( fillHistogram( chain+flavour+"track_sigz0_signed", z0_signed_sig) );
      }

      uint8_t nInnHits       = -1;
      uint8_t nNextToInnHits = -1;
      uint8_t nBLHits        = -1;
      uint8_t nsharedBLHits  = -1;
      uint8_t nsplitBLHits   = -1;
      uint8_t nPixHits       = -1;
      uint8_t nPixHoles      = -1;
      uint8_t nsharedPixHits = -1;
      uint8_t nsplitPixHits  = -1;
      uint8_t nSCTHits       = -1;
      uint8_t nSCTHoles      = -1;
      uint8_t nsharedSCTHits = -1;

      ATH_CHECK( trackParticle->summaryValue(nInnHits,xAOD::numberOfInnermostPixelLayerHits) );
      ATH_CHECK( trackParticle->summaryValue(nNextToInnHits,xAOD::numberOfNextToInnermostPixelLayerHits) );
      ATH_CHECK( trackParticle->summaryValue(nBLHits,xAOD::numberOfBLayerHits) );
      ATH_CHECK( trackParticle->summaryValue(nsharedBLHits,xAOD::numberOfBLayerSharedHits) );
      ATH_CHECK( trackParticle->summaryValue(nsplitBLHits,xAOD::numberOfBLayerSplitHits) );
      ATH_CHECK( trackParticle->summaryValue(nPixHits,xAOD::numberOfPixelHits) );
      ATH_CHECK( trackParticle->summaryValue(nPixHoles,xAOD::numberOfPixelHoles) );
      ATH_CHECK( trackParticle->summaryValue(nsharedPixHits,xAOD::numberOfPixelSharedHits) );
      ATH_CHECK( trackParticle->summaryValue(nsplitPixHits,xAOD::numberOfPixelSplitHits) );
      ATH_CHECK( trackParticle->summaryValue(nSCTHits,xAOD::numberOfSCTHits) );
      ATH_CHECK( trackParticle->summaryValue(nSCTHoles,xAOD::numberOfSCTHoles) );
      ATH_CHECK( trackParticle->summaryValue(nsharedSCTHits,xAOD::numberOfSCTSharedHits) );

      ATH_CHECK( fillHistogram( chain+flavour+"nInnHits", nInnHits) );
      ATH_CHECK( fillHistogram( chain+flavour+"nNextToInnHits", nNextToInnHits) );
      ATH_CHECK( fillHistogram( chain+flavour+"nBLHits", nBLHits) );
      ATH_CHECK( fillHistogram( chain+flavour+"nsharedBLHits", nsharedBLHits) );
      ATH_CHECK( fillHistogram( chain+flavour+"nsplitBLHits", nsplitBLHits) );
      ATH_CHECK( fillHistogram( chain+flavour+"nPixHits", nPixHits) );
      ATH_CHECK( fillHistogram( chain+flavour+"nPixHoles", nPixHoles) );
      ATH_CHECK( fillHistogram( chain+flavour+"nsharedPixHits", nsharedPixHits) );
      ATH_CHECK( fillHistogram( chain+flavour+"nsplitPixHits", nsplitPixHits) );
      ATH_CHECK( fillHistogram( chain+flavour+"nSCTHits", nSCTHits) );
      ATH_CHECK( fillHistogram( chain+flavour+"nSCTHoles", nSCTHoles) );
      ATH_CHECK( fillHistogram( chain+flavour+"nsharedSCTHits", nsharedSCTHits) );
    }

    return StatusCode::SUCCESS;
  }

  StatusCode PhysicsVariablePlots::analyseBTaggingObject( const xAOD::Jet* jet,
							  const std::string& flavour,
                                                          std::string chain) {

    if( chain != "" ) chain = chain + "_";

    float jetPt = jet->p4().Et() / Gaudi::Units::GeV;
    float jetEta = jet->eta();

    // Retrieve b-tagging object    
    const xAOD::BTagging *bTaggingObject = xAOD::BTaggingUtilities::getBTagging( *jet );
    if ( !bTaggingObject ) {
      ATH_MSG_ERROR( "Could not retrieve b-tagging object from selected jet." );
      return StatusCode::FAILURE;
    }
    
    // Available variables listed in PhysicsAnalysis/DerivationFramework/DerivationFrameworkFlavourTag/python/BTaggingContent.py
    ATH_MSG_DEBUG( "BTAGGING:" );

    // IP2D
    double IP2D_pu = -1;
    double IP2D_pb = -1;
    double IP2D_pc = -1;
//    double IP2D_LLR = bTaggingObject->IP2D_loglikelihoodratio();
    bTaggingObject->pu( "IP2D",IP2D_pu );
    bTaggingObject->pb( "IP2D",IP2D_pb );
    bTaggingObject->pc( "IP2D",IP2D_pc );

    ATH_CHECK( fillHistogram( chain+flavour+"IP2D_pu",IP2D_pu ) );
    ATH_CHECK( fillHistogram( chain+flavour+"IP2D_pb",IP2D_pb ) );
    ATH_CHECK( fillHistogram( chain+flavour+"IP2D_pc",IP2D_pc ) );
//    ATH_CHECK( fillHistogram( chain+flavour+"IP2D_LLR",IP2D_LLR ) );
    ATH_MSG_DEBUG( "  ** IP2D [pu,pb,pc] = [" << IP2D_pu <<"," << IP2D_pb << "," << IP2D_pc << "]" );


    int IP2D_nTrks = -1;
    std::vector< float > IP2D_weightBOfTracks;
    std::vector< float > IP2D_weightCOfTracks;
    std::vector< float > IP2D_weightUOfTracks;
    std::vector< int > IP2D_gradeOfTracks;
//    std::vector< float > IP2D_valD0wrtPVOfTracks;
//    std::vector< float > IP2D_sigD0wrtPVOfTracks;
//    std::vector< float > IP2D_valZ0wrtPVOfTracks;
//    std::vector< float > IP2D_sigZ0wrtPVOfTracks;

    bTaggingObject->variable< int >( "IP2D", "nTrks", IP2D_nTrks );
    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP2D_weightBOfTracks" ) )
      IP2D_weightBOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP2D_weightBOfTracks" );
    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP2D_weightCOfTracks" ) )
      IP2D_weightCOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP2D_weightCOfTracks" );
    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP2D_weightUOfTracks" ) )
      IP2D_weightUOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP2D_weightUOfTracks" );
    if ( bTaggingObject->isAvailable< std::vector< int > >( "IP2D_gradeOfTracks" ) )
      IP2D_gradeOfTracks = bTaggingObject->auxdata< std::vector< int > >( "IP2D_gradeOfTracks" );
//    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP2D_valD0wrtPVOfTracks" ) )
//      IP2D_valD0wrtPVOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP2D_valD0wrtPVOfTracks" );
//    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP2D_valZ0wrtPVOfTracks" ) )
//      IP2D_valZ0wrtPVOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP2D_valZ0wrtPVOfTracks" );

//    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP2D_sigD0wrtPVOfTracks" ) )
//      IP2D_sigD0wrtPVOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP2D_sigD0wrtPVOfTracks" );
//    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP2D_sigZ0wrtPVOfTracks" ) )
//      IP2D_sigZ0wrtPVOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP2D_sigZ0wrtPVOfTracks" );

    ATH_CHECK( fillHistogram( chain+flavour+"IP2D_nTracks",IP2D_nTrks ) );
    for ( unsigned int i(0); i < IP2D_gradeOfTracks.size(); i++ )
      ATH_CHECK( fillHistogram( chain+flavour+"IP2D_gradeOfTracks", IP2D_gradeOfTracks.at(i) ) );
    for ( unsigned int i(0); i < IP2D_weightBOfTracks.size(); i++ )
      ATH_CHECK( fillHistogram( chain+flavour+"IP2D_weightBOfTracks", IP2D_weightBOfTracks.at(i) ) );
    for ( unsigned int i(0); i < IP2D_weightCOfTracks.size(); i++ )
      ATH_CHECK( fillHistogram( chain+flavour+"IP2D_weightCOfTracks", IP2D_weightCOfTracks.at(i) ) );
    for ( unsigned int i(0); i < IP2D_weightUOfTracks.size(); i++ )
      ATH_CHECK( fillHistogram( chain+flavour+"IP2D_weightUOfTracks", IP2D_weightUOfTracks.at(i) ) );
//    for ( unsigned int i(0); i < IP2D_valD0wrtPVOfTracks.size(); i++ )
//      ATH_CHECK( fillHistogram( chain+flavour+"IP2D_valD0wrtPVOfTracks", IP2D_valD0wrtPVOfTracks.at(i) ) ); //value
//    for ( unsigned int i(0); i < IP2D_sigD0wrtPVOfTracks.size(); i++ )
//      ATH_CHECK( fillHistogram( chain+flavour+"IP2D_sigD0wrtPVOfTracks", IP2D_sigD0wrtPVOfTracks.at(i) ) ); //significance
//    for ( unsigned int i(0); i < IP2D_valZ0wrtPVOfTracks.size(); i++ )
//      ATH_CHECK( fillHistogram( chain+flavour+"IP2D_valZ0wrtPVOfTracks", IP2D_valZ0wrtPVOfTracks.at(i) ) ); //value
//    for ( unsigned int i(0); i < IP2D_sigZ0wrtPVOfTracks.size(); i++ )
//      ATH_CHECK( fillHistogram( chain+flavour+"IP2D_sigZ0wrtPVOfTracks", IP2D_sigZ0wrtPVOfTracks.at(i) ) ); //significance
    

    // IP3D
    double IP3D_pu = -1;
    double IP3D_pb = -1;
    double IP3D_pc = -1;
    double IP3D_LLR = bTaggingObject->IP3D_loglikelihoodratio();
    bTaggingObject->pu( "IP3D",IP3D_pu );
    bTaggingObject->pb( "IP3D",IP3D_pb );
    bTaggingObject->pc( "IP3D",IP3D_pc );

    ATH_CHECK( fillHistogram( chain+flavour+"IP3D_pu",IP3D_pu ) );
    ATH_CHECK( fillHistogram( chain+flavour+"IP3D_pb",IP3D_pb ) );
    ATH_CHECK( fillHistogram( chain+flavour+"IP3D_pc",IP3D_pc ) );
    ATH_CHECK( fillHistogram( chain+flavour+"IP3D_LLR",IP3D_LLR ) );
    ATH_MSG_DEBUG( "  ** IP3D [pu,pb,pc] = [" << IP3D_pu <<"," << IP3D_pb << "," << IP3D_pc << "]" );

    int IP3D_nTrks = -1;
    std::vector< float > IP3D_weightBOfTracks;
    std::vector< float > IP3D_weightCOfTracks;
    std::vector< float > IP3D_weightUOfTracks;
    std::vector< int > IP3D_gradeOfTracks;
    std::vector< float > IP3D_D0wrtPVOfTracks;
    std::vector< float > IP3D_sigD0wrtPVOfTracks;
    std::vector< float > IP3D_Z0wrtPVOfTracks;
    std::vector< float > IP3D_sigZ0wrtPVOfTracks;

    bTaggingObject->variable< int >( "IP3D", "nTrks", IP3D_nTrks );
    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP3D_weightBOfTracks" ) )
      IP3D_weightBOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP3D_weightBOfTracks" );
    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP3D_weightCOfTracks" ) )
      IP3D_weightCOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP3D_weightCOfTracks" );
    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP3D_weightUOfTracks" ) )
      IP3D_weightUOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP3D_weightUOfTracks" );
    if ( bTaggingObject->isAvailable< std::vector< int > >( "IP3D_gradeOfTracks" ) )
      IP3D_gradeOfTracks = bTaggingObject->auxdata< std::vector< int > >( "IP3D_gradeOfTracks" );

    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP3D_D0wrtPVOfTracks" ) )
      IP3D_D0wrtPVOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP3D_D0wrtPVOfTracks" );
    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP3D_Z0wrtPVOfTracks" ) )
      IP3D_Z0wrtPVOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP3D_Z0wrtPVOfTracks" );
    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP3D_sigD0wrtPVOfTracks" ) )
      IP3D_sigD0wrtPVOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP3D_sigD0wrtPVOfTracks" );
    if ( bTaggingObject->isAvailable< std::vector< float > >( "IP3D_sigZ0wrtPVOfTracks" ) )
      IP3D_sigZ0wrtPVOfTracks = bTaggingObject->auxdata< std::vector< float > >( "IP3D_sigZ0wrtPVOfTracks" );

    ATH_MSG_DEBUG("Size IP3D_gradeOfTracks/_weightBOfTracks/_weightUOfTracks/_D0wrtPVOfTracks/_sigD0wrtPVOfTracks/_Z0wrtPVOfTracks/_sigZ0wrtPVOfTracks: " << IP3D_gradeOfTracks.size() <<", "<< IP3D_weightBOfTracks.size() << ", " << IP3D_weightUOfTracks.size() << ", " << IP3D_D0wrtPVOfTracks.size() << ", " << IP3D_sigD0wrtPVOfTracks.size() << ", " << IP3D_Z0wrtPVOfTracks.size() << ", " << IP3D_sigZ0wrtPVOfTracks.size() );
    ATH_CHECK( fillHistogram( chain+flavour+"IP3D_nTracks",IP3D_nTrks ) );
    for ( unsigned int i(0); i < IP3D_gradeOfTracks.size(); i++ )
      ATH_CHECK( fillHistogram( chain+flavour+"IP3D_gradeOfTracks", IP3D_gradeOfTracks.at(i) ) );
    for ( unsigned int i(0); i < IP3D_weightBOfTracks.size(); i++ ) 
      ATH_CHECK( fillHistogram( chain+flavour+"IP3D_weightBOfTracks", IP3D_weightBOfTracks.at(i) ) );
    for ( unsigned int i(0); i < IP3D_weightCOfTracks.size(); i++ ) 
      ATH_CHECK( fillHistogram( chain+flavour+"IP3D_weightCOfTracks", IP3D_weightCOfTracks.at(i) ) );
    for ( unsigned int i(0); i < IP3D_weightUOfTracks.size(); i++ ) 
      ATH_CHECK( fillHistogram( chain+flavour+"IP3D_weightUOfTracks", IP3D_weightUOfTracks.at(i) ) );
    for ( unsigned int i(0); i < IP3D_D0wrtPVOfTracks.size(); i++ ) 
      ATH_CHECK( fillHistogram( chain+flavour+"IP3D_D0wrtPVOfTracks", IP3D_D0wrtPVOfTracks.at(i) ) ); //value
    for ( unsigned int i(0); i < IP3D_sigD0wrtPVOfTracks.size(); i++ ) 
      ATH_CHECK( fillHistogram( chain+flavour+"IP3D_sigD0wrtPVOfTracks", IP3D_sigD0wrtPVOfTracks.at(i) ) ); //significance
    for ( unsigned int i(0); i < IP3D_Z0wrtPVOfTracks.size(); i++ ) 
      ATH_CHECK( fillHistogram( chain+flavour+"IP3D_Z0wrtPVOfTracks", IP3D_Z0wrtPVOfTracks.at(i) ) ); //value
    for ( unsigned int i(0); i < IP3D_sigZ0wrtPVOfTracks.size(); i++ ) 
      ATH_CHECK( fillHistogram( chain+flavour+"IP3D_sigZ0wrtPVOfTracks", IP3D_sigZ0wrtPVOfTracks.at(i) ) ); //significance

    // SV1
    double SV1_pu = -1;
    double SV1_pb = -1;
    double SV1_pc = -1;
//    double SV1_LLR = bTaggingObject->SV1_loglikelihoodratio();

    bTaggingObject->pu( "SV1",SV1_pu );
    bTaggingObject->pb( "SV1",SV1_pb );
    bTaggingObject->pc( "SV1",SV1_pc );

    ATH_CHECK( fillHistogram( chain+flavour+"SV1_pu",SV1_pu ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_pb",SV1_pb ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_pc",SV1_pc ) );
//    ATH_CHECK( fillHistogram( chain+flavour+"SV1_LLR",SV1_LLR ));
    ATH_MSG_DEBUG( "  ** SV1 [pu,pb,pc] = [" << SV1_pu <<"," << SV1_pb << "," << SV1_pc << "]" );

    float SV1_masssvx = -1;
    float SV1_efracsvx = -1;
    float SV1_deltaR = -1;
    int SV1_N2Tpair = -1;
    float SV1_significance3d = -1;
    float SV1_energyTrkInJet = -1;
    int SV1_NGTinSvx = -1;
    float SV1_Lxy = -1;

    bTaggingObject->variable< float >( "SV1", "masssvx", SV1_masssvx );
    bTaggingObject->variable< float >( "SV1", "efracsvx", SV1_efracsvx );
    bTaggingObject->variable< float >( "SV1", "deltaR", SV1_deltaR );
    bTaggingObject->variable< int >( "SV1", "N2Tpair", SV1_N2Tpair );
    bTaggingObject->variable< float >( "SV1", "significance3d", SV1_significance3d );
    bTaggingObject->variable< float >( "SV1", "energyTrkInJet", SV1_energyTrkInJet );
    bTaggingObject->variable< int >( "SV1", "NGTinSvx", SV1_NGTinSvx );
    bTaggingObject->variable< float >( "SV1", "Lxy", SV1_Lxy );

    ATH_CHECK( fillHistogram( chain+flavour+"SV1_masssvx",SV1_masssvx ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_efracsvx",SV1_efracsvx ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_deltaR",SV1_deltaR ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_N2Tpair",SV1_N2Tpair ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_significance3d",SV1_significance3d ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_energyTrkInJet",SV1_energyTrkInJet ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_NGTinSvx",SV1_NGTinSvx ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_Lxy",SV1_Lxy ) );

    ATH_CHECK( fillHistogram( chain+flavour+"SV1_masssvx_vs_pT_2D",jetPt,SV1_masssvx ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_N2Tpair_vs_pT_2D",jetPt,SV1_N2Tpair ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_efracsvx_vs_pT_2D",jetPt,SV1_efracsvx ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_deltaR_vs_pT_2D",jetPt,SV1_deltaR ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_masssvx_vs_eta_2D",jetEta,SV1_masssvx ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_N2Tpair_vs_eta_2D",jetEta,SV1_N2Tpair ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_efracsvx_vs_eta_2D",jetEta,SV1_efracsvx ) );
    ATH_CHECK( fillHistogram( chain+flavour+"SV1_deltaR_vs_eta_2D",jetEta,SV1_deltaR ) );


    ATH_MSG_DEBUG( "  ** SV1 [masssvx,efracsvx,deltaR,N2Tpair] = [" << SV1_masssvx << 
		   "," << SV1_efracsvx << 
		   "," << SV1_deltaR << 
		   "," << SV1_N2Tpair << "]" );

    // Jet Fitter
    int JetFitter_N2Tpair = -1;
    int JetFitter_nVTX = -1;
    int JetFitter_nSingleTracks = -1;
    int JetFitter_nTracksAtVtx = -1;
    float JetFitter_mass = -1;
    float JetFitter_energyFraction = -1;
    float JetFitter_significance3d = -1;
    bTaggingObject->variable< int >( "JetFitter","N2Tpair",JetFitter_N2Tpair );
    bTaggingObject->variable< int >( "JetFitter","nVTX",JetFitter_nVTX );
    bTaggingObject->variable< int >( "JetFitter","nSingleTracks",JetFitter_nSingleTracks );
    bTaggingObject->variable< int >( "JetFitter","nTracksAtVtx",JetFitter_nTracksAtVtx );
    bTaggingObject->variable< float >( "JetFitter","mass",JetFitter_mass );
    bTaggingObject->variable< float >( "JetFitter","energyFraction",JetFitter_energyFraction );
    bTaggingObject->variable< float >( "JetFitter","significance3d",JetFitter_significance3d );

    ATH_CHECK( fillHistogram( chain+flavour+"JetFitter_N2Tpair",JetFitter_N2Tpair ) );
    ATH_CHECK( fillHistogram( chain+flavour+"JetFitter_nVTX",JetFitter_nVTX ) );
    ATH_CHECK( fillHistogram( chain+flavour+"JetFitter_nSingleTracks",JetFitter_nSingleTracks ) );
    ATH_CHECK( fillHistogram( chain+flavour+"JetFitter_nTracksAtVtx",JetFitter_nTracksAtVtx ) );
    ATH_CHECK( fillHistogram( chain+flavour+"JetFitter_mass",JetFitter_mass ) );
    ATH_CHECK( fillHistogram( chain+flavour+"JetFitter_energyFraction",JetFitter_energyFraction ) );
    ATH_CHECK( fillHistogram( chain+flavour+"JetFitter_significance3d",JetFitter_significance3d ) );
    ATH_MSG_DEBUG( "  ** JetFitter [mass] = [" << JetFitter_mass <<"]" );

//    double JetFitter_pu = bTaggingObject->JetFitter_pu();
//    double JetFitter_pb = bTaggingObject->JetFitter_pb();
//    double JetFitter_pc = bTaggingObject->JetFitter_pc();
//    double JetFitter_LLR = bTaggingObject->JetFitter_loglikelihoodratio();

//    ATH_CHECK( fillHistogram( chain+flavour+"JetFitter_pu",JetFitter_pu ) );
//    ATH_CHECK( fillHistogram( chain+flavour+"JetFitter_pb",JetFitter_pb ) );
//    ATH_CHECK( fillHistogram( chain+flavour+"JetFitter_pc",JetFitter_pc ) );
//    ATH_CHECK( fillHistogram( chain+flavour+"JetFitter_LLR",JetFitter_LLR ) );
      
    // RNNIP
    double RNNIP_pu = -1;
    double RNNIP_pb = -1;
    double RNNIP_pc = -1;
    double RNNIP_LLR = -1;
    bTaggingObject->pu( "rnnip",RNNIP_pu );
    bTaggingObject->pb( "rnnip",RNNIP_pb );
    bTaggingObject->pc( "rnnip",RNNIP_pc );
    bTaggingObject->loglikelihoodratio( "rnnip", RNNIP_LLR);

    ATH_CHECK( fillHistogram( chain+flavour+"RNNIP_pu",RNNIP_pu ) );
    ATH_CHECK( fillHistogram( chain+flavour+"RNNIP_pb",RNNIP_pb ) );
    ATH_CHECK( fillHistogram( chain+flavour+"RNNIP_pc",RNNIP_pc ) );
    ATH_CHECK( fillHistogram( chain+flavour+"RNNIP_LLR",RNNIP_LLR ) );
    ATH_MSG_DEBUG( "  ** RNNIP [pu,pb,pc] = [" << RNNIP_pu <<"," << RNNIP_pb << "," << RNNIP_pc << "]" );

    // DL1
    double DL1_pu = -1;
    double DL1_pb = -1;
    double DL1_pc = -1;
    bTaggingObject->pu( "DL1",DL1_pu );
    bTaggingObject->pb( "DL1",DL1_pb );
    bTaggingObject->pc( "DL1",DL1_pc );
    ATH_CHECK( fillHistogram( chain+flavour+"DL1_pu",DL1_pu ) );
    ATH_CHECK( fillHistogram( chain+flavour+"DL1_pb",DL1_pb ) );
    ATH_CHECK( fillHistogram( chain+flavour+"DL1_pc",DL1_pc ) );

    // DL1r
    double DL1r_pu = -1;
    double DL1r_pb = -1;
    double DL1r_pc = -1;
    bTaggingObject->pu( "DL1r",DL1r_pu );
    bTaggingObject->pb( "DL1r",DL1r_pb );
    bTaggingObject->pc( "DL1r",DL1r_pc );

    if ( DL1r_pu == -1 &&
	 DL1r_pb == -1 &&
	 DL1r_pc == -1 ) {
      bTaggingObject->pu( "DL1rnn",DL1r_pu );
      bTaggingObject->pb( "DL1rnn",DL1r_pb );
      bTaggingObject->pc( "DL1rnn",DL1r_pc );
    }

    float cFraction = 0.03;
    float DL1r = 0;
    if ( DL1r_pu != -1 && DL1r_pb != -1 && DL1r_pc != -1 )
      DL1r = log( DL1r_pb / ( DL1r_pu * ( 1 - cFraction ) + DL1r_pc * cFraction ) );

    float bFraction = 0.03;
    float DL1rc = 0;
    if ( DL1r_pu != -1 && DL1r_pb != -1 && DL1r_pc != -1 )
      DL1rc = log( DL1r_pb / ( DL1r_pu * ( 1 - bFraction ) + DL1r_pc * bFraction ) );

    ATH_CHECK( fillHistogram( chain+flavour+"DL1r_LLR",DL1r ) );
    ATH_CHECK( fillHistogram( chain+flavour+"DL1rc",DL1rc ) );
    ATH_CHECK( fillHistogram( chain+flavour+"DL1r_pu",DL1r_pu ) );
    ATH_CHECK( fillHistogram( chain+flavour+"DL1r_pb",DL1r_pb ) );
    ATH_CHECK( fillHistogram( chain+flavour+"DL1r_pc",DL1r_pc ) );
    ATH_MSG_DEBUG( "  ** DL1r = " << DL1r );
    ATH_MSG_DEBUG( "  ** DL1r [pu,pb,pc] = [" << DL1r_pu <<"," << DL1r_pb << "," << DL1r_pc << "]" );

    // MV2
    double mv2c10_discriminant = -2;
    bTaggingObject->MVx_discriminant( "MV2c10",mv2c10_discriminant );

    ATH_CHECK( fillHistogram( chain+flavour+"MV2c10_discriminant",mv2c10_discriminant ) );
    ATH_MSG_DEBUG( "  ** MV2c10 = " << mv2c10_discriminant );

    return StatusCode::SUCCESS;
  }

}



