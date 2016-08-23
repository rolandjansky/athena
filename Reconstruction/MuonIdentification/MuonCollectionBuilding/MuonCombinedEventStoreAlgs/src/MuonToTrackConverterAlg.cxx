/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonToTrackConverterAlg.h"
#include "muonEvent/MuonContainer.h"

#include "GaudiKernel/MsgStream.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "muonEvent/MuonContainer.h"

#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"





/////////////////////////////////////////////////////////////////
// Constructor

MuonToTrackConverterAlg::MuonToTrackConverterAlg(const std::string& name, 
			 ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_useMuonHitsOnly(false), 
  m_storeGate("StoreGateSvc", name),
  m_edmPrinter("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
  m_edmHelper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
  m_propagator("Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"),
  m_muonHoleRecoverTool("")
{
  m_muonContainerNames.push_back( "MuidMuonCollection" ); // set default to muid AOD container
  /** input collections from the combined reconstructions */
  declareProperty("MuonContainerNames", m_muonContainerNames );
  declareProperty("MuonTrackLocation",  m_muonTrackLocation  = "MuidMuonTracks" );
  declareProperty("HoleRecoveryTool",   m_muonHoleRecoverTool);
  declareProperty("UseMuonHitsOnly",    m_useMuonHitsOnly    );
}

/////////////////////////////////////////////////////////////////
// Destructor

MuonToTrackConverterAlg::~MuonToTrackConverterAlg() {}

/////////////////////////////////////////////////////////////////
// Initialize

StatusCode MuonToTrackConverterAlg::initialize() {

  if (m_edmHelper.retrieve().isFailure()){
    ATH_MSG_ERROR("Could not get " << m_edmHelper); 
    return StatusCode::FAILURE;
  }

  if( msgLvl(MSG::DEBUG) ){
    if (m_edmPrinter.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_edmPrinter); 
      return StatusCode::FAILURE;
    }
    
    if (m_edmHelper.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_edmHelper); 
      return StatusCode::FAILURE;
    }
  }

  
  if( !m_muonHoleRecoverTool.empty() ){
    if (m_muonHoleRecoverTool.retrieve().isFailure()) {
      ATH_MSG_ERROR("Could not find refit tool "<<m_muonHoleRecoverTool<<". Exiting.");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////
// Finalize

StatusCode MuonToTrackConverterAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode MuonToTrackConverterAlg::execute() {

  std::vector<const Analysis::Muon*> muons;
  retrieveMuons(m_muonContainerNames,muons);
  
  if( msgLvl(MSG::DEBUG) ) ATH_MSG_DEBUG("Retrieved " << muons.size() << " muons from " << m_muonContainerNames.size());

  // convert muons into tracks
  TrackCollection* tracks = convertToTrack( muons );
  if( !tracks ) return StatusCode::SUCCESS;

  if (evtStore()->record(tracks,m_muonTrackLocation).isFailure()) {
    ATH_MSG_WARNING("could not store the Muons at " << m_muonTrackLocation);
  }else{
    ATH_MSG_DEBUG(" recorded track collection: " << tracks->size() << " at " << m_muonTrackLocation);
  }
  return StatusCode::SUCCESS;
}

TrackCollection* MuonToTrackConverterAlg::convertToTrack( const std::vector<const Analysis::Muon*>& muons ) const {
  
  TrackCollection* tracks = new TrackCollection();
  tracks->reserve(muons.size());


  // loop over muons
  std::vector<const Analysis::Muon*>::const_iterator mit = muons.begin();
  std::vector<const Analysis::Muon*>::const_iterator mit_end = muons.end();
  for( ;mit!=mit_end;++mit ){
    const Analysis::Muon& muon = **mit;
    
    bool wasFitted = true;

    Trk::Track* track = 0;
    if (!m_useMuonHitsOnly && muon.hasCombinedMuonTrackParticle() && (muon.combinedTrackLink()).isValid() ) {

      const Rec::TrackParticle* cbtp = muon.combinedMuonTrackParticle();
      const Trk::Track* cbtr = cbtp ? cbtp->originalTrack() : 0;
      if( cbtr ) {
        track = new Trk::Track(*cbtr); // copy track as ownership is passed to DataVector
        ATH_MSG_VERBOSE(" Combined Muon " << track->info().dumpInfo() << " aod author " << muon.author());
      } 
    }else if( muon.hasMuonExtrapolatedTrackParticle() && (muon.muonExtrapTrackLink()).isValid() ){

      const Rec::TrackParticle* satp = muon.muonExtrapolatedTrackParticle();
      const Trk::Track* satr = satp ? satp->originalTrack() : 0;
      if( satr ) {
	track = new Trk::Track(*satr); // copy track as ownership is passed to DataVector
	ATH_MSG_VERBOSE(" StandAlone Muon " << track->info().dumpInfo() <<" aod author " << muon.author() );
      }

    }else if (muon.hasInDetTrackParticle() && (muon.inDetTrackLink()).isValid() ) {

      if (muon.hasInnerExtrapolatedTrackParticle()) {
	const Trk::Track* tr = muon.innerExtrapolatedTrkTrack();
	if( tr ) {
	  track = new Trk::Track(*tr); // copy track as ownership is passed to DataVector
	  ATH_MSG_VERBOSE(" StandAlone Muon " << track->info().dumpInfo() <<" aod author " << muon.author() );
	}
      }else{
	wasFitted = false;

	track = createTaggedMuonTrack(muon);
	if (track) ATH_MSG_VERBOSE(" Tagged Muon " << track->info().dumpInfo() << " aod author " << muon.author());
      }
    }else{
      ATH_MSG_WARNING("Unexpected Muon type ");
    }

    if( !track ) {
      ATH_MSG_WARNING("Could not get track for Muon ");
      continue;
    }
 
    if( wasFitted && !m_muonHoleRecoverTool.empty() ){
      Trk::Track* recoveredTrack = m_muonHoleRecoverTool->recover(*track);
      if( !recoveredTrack ) {
	ATH_MSG_WARNING(" final track lost, this should not happen ");
      }else{
	if( recoveredTrack != track ) delete track;
	track = recoveredTrack;
      }
    }

    tracks->push_back( track );
  }
  return tracks;
}

Trk::Track* MuonToTrackConverterAlg::createTaggedMuonTrack( const Analysis::Muon& muon ) const {

  MsgStream log( msgSvc(), name() );
  const Rec::TrackParticle* idtp = muon.inDetTrackParticle();
  const Trk::Track* track = idtp ? idtp->originalTrack() : 0;
  if (!track) return 0;
  ATH_MSG_VERBOSE(" makeSegmentTrack " );
  
  if( !track->trackStateOnSurfaces() || track->trackStateOnSurfaces()->empty() ) return 0;

  Trk::TrackInfo::TrackPatternRecoInfo author =  Trk::TrackInfo::MuGirlUnrefitted;
  if (muon.author() == MuonParameters::MuTag) author =  Trk::TrackInfo::StacoLowPt;
  if (muon.author() == MuonParameters::MuTagIMO) author =  Trk::TrackInfo::StacoLowPt;
  ATH_MSG_VERBOSE(" author " << muon.author() );
  DataVector<const Trk::TrackStateOnSurface>*  trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

  //  Copy ID track
  if(!m_useMuonHitsOnly){
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it  = track->trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end = track->trackStateOnSurfaces()->end(); 
    for ( ; it!=it_end; ++it) trackStateOnSurfaces->push_back( (*it)->clone() );
  }

  //  Loop over segments   
  int nseg = muon.numberOfSegments();
  for (int i = 0; i < nseg; ++i) {
    const Trk::Segment* tseg = muon.muonSegment(i);
    const Muon::MuonSegment* seg  = dynamic_cast<const  Muon::MuonSegment* > (tseg);
    if( !seg ) continue;

    // create pars for muon and loop over hits
    double momentum = 1e8;
    double charge   = 0.;
    const Trk::TrackParameters* pars = m_edmHelper->createTrackParameters( *seg, momentum, charge );
    std::vector<const Trk::MeasurementBase*>::const_iterator mit = seg->containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = seg->containedMeasurements().end();
    for( ;mit!=mit_end;++mit ){

      const Trk::MeasurementBase& meas = **mit;

      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
      typePattern.set(Trk::TrackStateOnSurface::Measurement);

      const Trk::TrackParameters* exPars = m_propagator->propagateParameters(*pars,meas.associatedSurface(),
									     Trk::anyDirection, false, Trk::MagneticFieldProperties(Trk::NoField));
      if(!exPars){
	ATH_MSG_VERBOSE("Could not propagate Track to segment surface");
      }

      const Trk::TrackStateOnSurface* trackState = new Trk::TrackStateOnSurface( meas.clone(), exPars, 0, 0, typePattern );
      trackStateOnSurfaces->push_back( trackState ); 
    } // end segment loop
    delete pars;
  }
  Trk::TrackInfo info(Trk::TrackInfo::Unknown,Trk::muon);
  info.setPatternRecognitionInfo(author);
  Trk::Track* newtrack=  new Trk::Track(info, trackStateOnSurfaces,(track->fitQuality())->clone());
  return newtrack;
}

void MuonToTrackConverterAlg::retrieveMuons( const std::vector<std::string>& keys, std::vector<const Analysis::Muon*>& muons ) const {

  MsgStream log( msgSvc(), name() );
  // loop over keys
  std::vector<std::string>::const_iterator kit = keys.begin();
  std::vector<std::string>::const_iterator kit_end = keys.end();
  for( ;kit!=kit_end;++kit ){

    // retrieve muon container
    const Analysis::MuonContainer * muonContainer = 0;
    StatusCode sc = evtStore()->retrieve(muonContainer, *kit);
    if (sc.isFailure() || !muonContainer) {
      ATH_MSG_WARNING("Could not retrieve MuonContainer: " << *kit);
      continue;
    }

    // insert the muons in to the vector
    muons.insert(muons.end(),muonContainer->begin(),muonContainer->end());
  }
}

