/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
     
///////////////////////////////////////////////////////////////////
//   Implementation file for class VrtSecDecay
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "VrtSecDecay/VrtSecDecay.h"
//#include "AthContainers/ConstDataVector.h"
#include "GaudiKernel/MsgStream.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkMeasurementBase/MeasurementBase.h"

/*
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexAuxContainer.h"
*/

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

VKalVrtAthena::VrtSecDecay::VrtSecDecay (const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_iVertexFitter ( "Trk::TrkVKalVrtFitter")

{

  //
  m_doTrkTrackMethod = false;

  //
  declareProperty("VertexFitterTool",m_iVertexFitter);
  declareProperty("DoTrkTrackMethod",m_doTrkTrackMethod);



}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

VKalVrtAthena::VrtSecDecay::~VrtSecDecay()
{

}

///////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////

StatusCode VKalVrtAthena::VrtSecDecay::initialize()
{

  /* Get the right vertex fitting tool from ToolSvc */
  if ( m_iVertexFitter.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVertexFitter << endreq;
    return StatusCode::FAILURE;
  }
   else {
    msg(MSG::INFO) << "Retrieved tool " << m_iVertexFitter << endreq;
  }

  
  // Return gracefully 
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode VKalVrtAthena::VrtSecDecay::execute()
{

  //
  if( m_doTrkTrackMethod ) return doTrkTrackMethod();
  else                     return doTrackParticleMethod();
  //

}
///////////////////////////////////////////////////////////////////
// Execute strategy using Trk::Track
///////////////////////////////////////////////////////////////////
StatusCode VKalVrtAthena::VrtSecDecay::doTrkTrackMethod()
{

  //
  const TrackCollection* ptracks = 0;
  if( evtStore()->retrieve ( ptracks, "ResolvedPixelThreeLayerTracks" ).isFailure() ){
    ATH_MSG_WARNING( "Could not find ResolvedPixelThreeLayerTracks in StoreGate. Note that this algorithm will not create any seeds!");
    return StatusCode::FAILURE;
	  
  }

  //
  const TrackCollection* stracks = 0;
  if( evtStore()->retrieve ( stracks, "SiSpSeededLargeD0Tracks" ).isFailure() ){
    ATH_MSG_WARNING( "Could not find SiSpSeededLargeD0Tracks in StoreGate." );
    return StatusCode::FAILURE;
  }

  // Pre-select soft-tracks so we don't have to 
  // apply this pre-selection for N tracklets in the loop below
  std::vector<const Trk::Track*> preselected_tracks = PreSelectTracks(stracks);

  // Selected tracks for vertex fits
  // Cleared up below, here so we only declare it once
  std::vector<const Trk::Track*> selectedTracks; 
  selectedTracks.clear();


  // Loop over primary tracks
  for( const auto& ptrack : *ptracks ){

    const Trk::TrackSummary* trkSum = ptrack->trackSummary();
    if( !trkSum ){
      ATH_MSG_WARNING("Track summary doesn't exist!!!!!!" );
      continue;
    }

    if( trkSum->get( Trk::SummaryType::numberOfSCTHits )>0   ) continue;
    if( trkSum->get( Trk::SummaryType::numberOfPixelHits )<3 ) continue;

    const Trk::Perigee* perigee = ptrack->perigeeParameters();
    if( !perigee ){
      ATH_MSG_WARNING("NO VALID perigee. Ignoring this track...." );
      continue;
    }

    // Momentum cut
    if( perigee->momentum().perp()<20e3 ) continue;

    // Tracklet measurements
    const DataVector<const Trk::MeasurementBase>* tps = ptrack->measurementsOnTrack();

    // Find the maximum radius hit
    const Trk::MeasurementBase* maxTR = 0;
    float max_r = 0.0;
    for( auto& tp : *tps ){
      float _r = sqrt( pow(tp->globalPosition().x(),2) + pow(tp->globalPosition().y(),2) ) ;
      if( _r > max_r ){
	max_r = _r;
	maxTR = tp;
      }
    }

    //
    if( !maxTR ) continue;

    // Loop over secondary tracks
    for(const auto& strack : preselected_tracks ){

      const DataVector<const Trk::MeasurementBase>* smb = strack->measurementsOnTrack();
      
      // Require all hits along this track to have radius > maxTR 

      bool hasHitBeforeLastPrimary = false;
      for( auto& tp : *smb ){
	float _r = sqrt( pow(tp->globalPosition().x(),2) + pow(tp->globalPosition().y(),2) ) ;
	if( _r < max_r ){
	  std::cout << "Found a hit before the last pixel hit: " << _r << " compared to the pixel : " << max_r << std::endl;
	  hasHitBeforeLastPrimary = true;
	  break;
	}
      }

      //
      if( hasHitBeforeLastPrimary ) continue;
      
      // Now, Vertex fitting time!
      
      selectedTracks.clear();
      selectedTracks.push_back(ptrack);
      selectedTracks.push_back(strack);
      

      // Inital position estimation
      Amg::Vector3D initPos(0.,0.,0.);
      StatusCode sc = m_iVertexFitter->VKalVrtFitFast( selectedTracks, initPos);
      if( sc.isFailure() ){
        ATH_MSG_INFO("FAILED TO ESTIMATE VTX INITIAL POSITION ");
        continue;
      }
 
      xAOD::Vertex* myVertex = 0;
      myVertex = m_iVertexFitter->fit(selectedTracks, initPos);
      
      // Pass through Vtx selection tools



    } // Loop over secondary tracks
  } // Loop over primary tracks

  return StatusCode::SUCCESS;


}
///////////////////////////////////////////////////////////////////
// Execute strategy using xAOD::TrackParticles
///////////////////////////////////////////////////////////////////
StatusCode VKalVrtAthena::VrtSecDecay::doTrackParticleMethod()
{

  //
  // Pixel tracklet container
  //
  const xAOD::TrackParticleContainer* tracklets(0);
  if ( evtStore()->retrieve ( tracklets, "InDetPixelThreeLayerTrackParticles" ).isFailure() )
    {
      ATH_MSG_WARNING( "Could not find xAOD::TrackParticleContainer InDetPixelPrdAssociationTrackParticles in StoreGate.");
      return StatusCode::SUCCESS;
    }
  
  //
  // Soft-pions, before ambiguity resolving
  //
  const xAOD::TrackParticleContainer* tracks(0);
  if ( evtStore()->retrieve ( tracks, "InDetTrackParticles" ).isFailure() )
    {
      ATH_MSG_ERROR( "Could not find xAOD::TrackParticleContainer tracks in StoreGate.");
      return StatusCode::FAILURE;
    }
  
  // Pre-select soft-tracks so we don't have to 
  // apply this pre-selection for N tracklets in the loop below
  std::vector<const xAOD::TrackParticle*> preselected_tracks = PreSelectTracks(tracks);

  //
  // ----- Track selection -------
  //
  std::vector<const xAOD::TrackParticle*> selectedTracks; 
  selectedTracks.clear();

  // 
  // ----- Output vertex containers -------
  ///
  xAOD::VertexContainer* VtxContainer = new xAOD::VertexContainer();
  xAOD::VertexAuxContainer* VtxContainerAux = new xAOD::VertexAuxContainer();
  VtxContainer->setStore( VtxContainerAux );

  bool m_doHitPatternMatching = true;
  bool m_doAngularMatching = true;
  bool m_doRadiusSorting = true;
  std::vector<VtxObj> initialVertices;

  // Select the tracks we want
  for( const auto& tracklet : *tracklets ){

    //  pT cut on the tracklet 
    if( tracklet->pt() < 20e3 ) continue;

    //
    initialVertices.clear();

    //
    for( const auto& pion : preselected_tracks ){

      // Consistency checks
      if( m_doAngularMatching && tracklet->p4().DeltaR( pion->p4() ) > 1.5 ) continue;
      if( m_doHitPatternMatching &&  !consistentHitPattern(tracklet,pion) ) continue;
      selectedTracks.clear();
      selectedTracks.push_back(tracklet);
      selectedTracks.push_back(pion);

      // Inital position estimation
      Amg::Vector3D initPos(0.,0.,0.);
      StatusCode sc = m_iVertexFitter->VKalVrtFitFast( selectedTracks, initPos);
      if( sc.isFailure() ){
        ATH_MSG_INFO("FAILED TO ESTIMATE VTX INITIAL POSITION ");
        continue;
      }
 
      xAOD::Vertex* myVertex = 0;
      myVertex = m_iVertexFitter->fit(selectedTracks, initPos);
      
      
      ////// 
      // TODO: Move to a separate method 
      // so we can call this from fitting method
      // with Trk::Track or xAOD::TrackParticle 
      // fitting methods...
      if( myVertex ){

        // chi-squared cut
        if( (myVertex->chiSquared() / myVertex->numberDoF() ) > 10.0 ){
          delete myVertex;
          continue;
        }

        // displacement
        double VtxR = TMath::Sqrt( pow(myVertex->x(),2) + pow(myVertex->y(),2));
        if( VtxR < 85.0 || VtxR>300.0 ){
          delete myVertex;
          continue;
        }

        // Vertex type
        myVertex->setVertexType(xAOD::VxType::SecVtx);

        // Links to the track particles in the vertex fit
        std::vector<ElementLink<xAOD::TrackParticleContainer>> trackLinks;
        trackLinks.push_back( ElementLink<xAOD::TrackParticleContainer>(pion, *tracks));
        trackLinks.push_back( ElementLink<xAOD::TrackParticleContainer>(tracklet, *tracklets));
        myVertex->setTrackParticleLinks( trackLinks );

        // Save for now..
        VtxContainer->push_back(myVertex);

        // Cache
        //initialVertices.push_back( VtxObj(myVertex,selectedTracks) );

      } // MyVtx exists
    } // First iteration of vertex fits

    // Radius sorting method 
    //if( m_doRadiusSorting ){
    //  std::sort( initialVertices.begin(), initialVertices.end(), rank_radius );
    //}

    //std::cout << "Listing information for vertices with tracklet  : " << tracklet->pt() << " and eta : " << tracklet->eta() << std::endl;
    //std::cout << "  ----> Number of vertices : " << initialVertices.size() << std::endl;

    // So we should write out the vertices at this point
    // and study what the environment looks like so we can decide on
    // how to cluster the vertices and what's the optimal 
    // clustering cuts
    //
    // Note that flexiblable should be kept so that other 
    // analyses with a different signature can tune 
    // the algorithm for their purposes!

    // Now lets try to piece common vertices together
    // This loop will ultimately establiosh which 
    // vertices we are writing out
    //for( auto& pV : initialVertices ){
    //  std::cout << "-----> Vertex with radius : " << pV.vtx->position().perp() << " and x: " << pV.vtx->x() << " and y : " << pV.vtx->y() << " and z : " << pV.vtx->z()  << std::endl;
    //}


/*
      // Only allowed to use this track once!
      //auto pVit = usedTracks.find(pV.second);
      //if( pVit!= usedTracks.end() ) continue;

      // Grouping of tracks
      selectedTracks.clear();
      selectedTracks = pV.inputTrks;
      
      xAOD::Vertex* finalVertex = 0;
      for( auto& cV : initialVertices ){
         
        if( pV.vtx==cV.vtx ) continue;

	// Only allowed to use this track once!
	//auto cVit = usedTracks.find(cV.second);
	//if( cVit!= usedTracks.end() ) continue;

        // Check if these verices are compatiable 
        // within uncertainties in both x,y and z, directions
	auto pV_covPos = pV.vtx->covariancePosition();
	auto cV_covPos = cV.vtx->covariancePosition();

	// x-position
	float xDiff = fabs(pV.vtx->position().x()-cV.vtx->position().x());
	float dx = 5.0 * sqrt( pV_covPos(0,0) + cV_covPos(0,0) );
	if( xDiff>dx ) continue;

      }
*/
/*
	// y-position 
	float yDiff = fabs(pV.first->position().y()-cV.first->position().y());
	float dy = 5.0 * sqrt( pV_covPos(1,1) + cV_covPos(1,1) );
	if( yDiff>dy) continue;

	// z-position
	float zDiff = fabs(pV.first->position().z()-cV.first->position().z());
	float dz = 10.0 * sqrt( pV_covPos(2,2) + cV_covPos(2,2) );
	if( zDiff>dz) continue;

	// Seed position
	Amg::Vector3D seedPos(0.,0.,0.);

	// Grab the initial position from the last fit
	if( finalVertex ){
	  seedPos = finalVertex->position();
	}
	// Otherwise from the fit of the two track vertices
	else{
	  seedPos = pV.first->position();
	}

	// Point to latest copy
	std::vector<const xAOD::TrackParticle*> vtxFitTracks = selectedTracks;
	
	// Fit !
	xAOD::Vertex* myVertex = m_iVertexFitter->fit(selectedTracks, seedPos);
      
	// Failed
	if( !myVertex ){
	  continue;
	}

	  
	// chi-squared cut
	if( (myVertex->chiSquared() / myVertex->numberDoF() ) > 10.0 ){
	  delete myVertex;
	  continue;
	}
	
	// displacement
	double VtxR = TMath::Sqrt( pow(myVertex->x(),2) + pow(myVertex->y(),2));
	if( VtxR < 85.0 ){
	  delete myVertex;
	  continue;
	}
	
	if( finalVertex ) delete finalVertex;
	finalVertex = myVertex;

	// Save !
	selectedTracks.push_back( cV.second ); 
	
      } // Loop over radius sorted 

      // Couldn't combine any vertices
      // Just point to the parent vertex
      if( !finalVertex ){
	finalVertex = pV.first;
      }

      // Write these guys out
      finalVertex->setVertexType(xAOD::VxType::SecVtx);
 
      // Links to the track particles in the vertex fit
      //std::vector<ElementLink<xAOD::TrackParticleContainer>> trackLinks;
      //trackLinks.push_back( ElementLink<xAOD::TrackParticleContainer>(pion, *tracks));
      //trackLinks.push_back( ElementLink<xAOD::TrackParticleContainer>(tracklet, *tracklets));
      //myVertex->setTrackParticleLinks( trackLinks );
      
      // Save!
      VtxContainer->push_back(finalVertex);

    } // Loop over radius sorted    
*/

  } //  Loop over tracklets 

  // Write them to store gate  
  if( evtStore()->record(VtxContainer, "VrtSecDecay").isFailure() ){
      ATH_MSG_WARNING( "Could not write SecVertices to StoreGate.");
      return StatusCode::SUCCESS;
  }
  
  if( evtStore()->record(VtxContainerAux, "VrtSecDecayAux.").isFailure() ){
      ATH_MSG_WARNING( "Could not write SecVerticesAux. to StoreGate.");
      return StatusCode::SUCCESS;
  }
  //
  return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////
// Check hit patterns
///////////////////////////////////////////////////////////////////
bool VKalVrtAthena::VrtSecDecay::passVtxSelection(xAOD::Vertex*& vtx)
{

  // Chi-squared cut over nDOF cut
  // Intentionally very loose right now
  if( (vtx->chiSquared() / vtx->numberDoF() ) > 10.0 ){
    delete vtx;
    return false;
  }
  
  // Transverse displacement cut,
  // Currently hardcoded to be between the 3rd pixel layer and the first SCT layer
  double VtxR = TMath::Sqrt( pow(vtx->x(),2) + pow(vtx->y(),2));
  if( VtxR < 85.0 || VtxR>300.0 ){
    delete vtx;
    return false;
  }
  
  // Any other cuts?
  //
  return true;

}
///////////////////////////////////////////////////////////////////
// Check hit patterns
///////////////////////////////////////////////////////////////////

bool VKalVrtAthena::VrtSecDecay::consistentHitPattern(const xAOD::TrackParticle* tracklet, const xAOD::TrackParticle* track)
{

  /*
enum DetectorType {
        pixelBarrel0 = 0, //!< there are three or four pixel barrel layers (R1/R2)
        pixelBarrel1 = 1,
        pixelBarrel2 = 2,
	pixelBarrel3 = 3,

        pixelEndCap0 = 4, //!< three pixel discs (on each side)
        pixelEndCap1 = 5,
        pixelEndCap2 = 6,

        sctBarrel0   = 7, //!< four sct barrel layers
        sctBarrel1   = 8,
        sctBarrel2   = 9,
        sctBarrel3   = 10,

        sctEndCap0   = 11, //!< and 9 sct discs (on each side)
        sctEndCap1   = 12,
        sctEndCap2   = 13,
        sctEndCap3   = 14,
        sctEndCap4   = 15,
        sctEndCap5   = 16,
        sctEndCap6   = 17,
        sctEndCap7   = 18,
        sctEndCap8   = 19,

        trtBarrel    = 20,
        trtEndCap    = 21,

...

  */
  
  
  // TODO: Completely hardcoded hit patterns right now
  // This will have to become a bit more flexible....
  // But for now lets see what we can do by selecting ideal tracklets and pions
  
  // Three or four later tracklets, with no hits other than in the 
  // pixl barrel layers
  if( tracklet->hitPattern()!=7 && tracklet->hitPattern()!=15 ) return false;

  // Soft pions (TODO: THESE IGNORE END CAP)
  // 896 --> Hits in sctBarrel 0,1,2
  // 1792 -> Hits in sctBarrel 1,2,3
  // 1928 -> Hits in pixelBarrel3 and sctBarrel 0,1,2,3
  /*
  if( track->hitPattern()!=896 && 
      track->hitPattern()!=1792 && 
      track->hitPattern()!=1928    ) return false;

  */
  // DECODE
  // std::string trackletp;
  // for (unsigned int mask=0x80000000; mask; mask>>=1) {
  //   trackletp += (tracklet->hitPattern() & mask) ? "1" : "0";
  // }


  return true;

}

///////////////////////////////////////////////////////////////////
// Pre-selection of tracks
// These methods will be transitions into a common track
// selection tool, eventually..
///////////////////////////////////////////////////////////////////

std::vector<const Trk::Track*> VKalVrtAthena::VrtSecDecay::PreSelectTracks(const TrackCollection* tracks)
{


  std::vector<const Trk::Track*> preselected_tracks;
  return preselected_tracks;






}

std::vector<const xAOD::TrackParticle*> VKalVrtAthena::VrtSecDecay::PreSelectTracks(const xAOD::TrackParticleContainer* tracks)
{

  std::vector<const xAOD::TrackParticle*> preselected_tracks;
  preselected_tracks.clear();

  // TODO: This will all be configurables in 
  // the future....
  for( const auto& track : * tracks ){

    // pT threshold
    if( track->pt()<200.0 || track->pt() > 1500.0 ) continue;

    // Ignore tracks with B-layer hits
    uint8_t nBLayerHits;
    track->summaryValue(nBLayerHits, xAOD::numberOfBLayerHits);
    if( nBLayerHits>0 ) continue;

    // Ignore tracks with hits on the second pixel layer
    uint8_t numberOfNextToInnermostPixelLayerHits;
    track->summaryValue(numberOfNextToInnermostPixelLayerHits, xAOD::numberOfNextToInnermostPixelLayerHits);
    if( numberOfNextToInnermostPixelLayerHits>0 ) continue;

    uint8_t numberOfPixelHits;
    track->summaryValue(numberOfPixelHits, xAOD::numberOfPixelHits);
    if( numberOfPixelHits>1 ) continue;

    // Require hits in the SCT
    uint8_t nSCTHits;
    track->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
    if( nSCTHits<6 ) continue;

    // GOOD TRACK!
    preselected_tracks.push_back(track);
   

  } // Loop over track container

  return preselected_tracks;

}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode VKalVrtAthena::VrtSecDecay::finalize()
{
 
  // Return gracefully
  return StatusCode::SUCCESS;
}
