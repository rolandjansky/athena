/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentTrackMaker/MuPatTrackBuilder.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"

#include "MuonRecToolInterfaces/IMuonTrackFinder.h"
#include "StoreGate/DataHandle.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "MuonSegment/MuonSegment.h"

#include "TrkTrack/TrackStateOnSurface.h"
#include "Particle/TrackParticleContainer.h"
#include <vector>

using namespace Muon;

MuPatTrackBuilder::MuPatTrackBuilder(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name,pSvcLocator),
     m_segmentKey("MooreSegments"),
     m_spectroTrackKey("MuonSpectrometerTracks"),
     m_spectroPartiKey("MuonSpectrometerParticles"),
     m_extrapPartiKey("ExtrapolatedMuonSpectrometerParticles"),
     m_trackMaker("Muon::MuonTrackFinder/MuonTrackSteering")
{
  // MoMu Key segments (per chamber)
  declareProperty("TrackSteering",m_trackMaker);
  declareProperty("MuonSegmentCollection", m_segmentKey);
  declareProperty("SpectrometerTrackOutputLocation", m_spectroTrackKey);
  declareProperty("SpectrometerParticleOutputLocation", m_spectroPartiKey);
  declareProperty("ExtrapolatedParticleOutputLocation", m_extrapPartiKey);
}

MuPatTrackBuilder::~MuPatTrackBuilder()
{;}

StatusCode MuPatTrackBuilder::initialize()
{
  if (m_trackMaker.retrieve().isFailure()){
    msg(MSG::FATAL) <<"Could not get " << m_trackMaker <<endmsg; 
    return StatusCode::FAILURE;
  }
  if (m_edmHelperSvc.retrieve().isFailure()){
    msg(MSG::FATAL) <<"Could not get " << m_edmHelperSvc <<endmsg; 
    return StatusCode::FAILURE;
  }
  if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Retrieved " << m_trackMaker << endmsg;
  
  ATH_CHECK( m_segmentKey.initialize() );
  ATH_CHECK( m_spectroTrackKey.initialize() );
  ATH_CHECK( m_spectroPartiKey.initialize() );
 
  // Check if the monitoring tool is not yet initialized
  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
  }
  
  return StatusCode::SUCCESS; 
}

StatusCode MuPatTrackBuilder::execute()
{
  typedef std::vector<const Muon::MuonSegment*> MuonSegmentCollection;

  SG::ReadHandle<Trk::SegmentCollection> segmentColl (m_segmentKey);
  if (!segmentColl.isValid() ) {
    msg(MSG::WARNING) << "Could not find MuonSegmentCollection at " << segmentColl.name() <<endmsg;
    return StatusCode::RECOVERABLE;
  }
    
  if( !segmentColl.cptr() ) {
    msg(MSG::WARNING) << "Obtained zero pointer for MuonSegmentCollection at " << segmentColl.name() <<endmsg;
    return StatusCode::RECOVERABLE;
  }
      
  if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Retrieved MuonSegmentCollection "  << segmentColl->size() << endmsg;

  MuonSegmentCollection msc;
  msc.reserve(segmentColl->size());
  for (unsigned int i=0;i<segmentColl->size();++i){
    if (!segmentColl->at(i)) continue;
    const Muon::MuonSegment * ms = dynamic_cast<const Muon::MuonSegment*>(segmentColl->at(i));
    if (ms) msc.push_back( ms );
  }

  if (msc.size() != segmentColl->size()){
    msg(MSG::WARNING) << "Input segment collection (size " << segmentColl->size() << ") and translated MuonSegment collection (size "
                      << msc.size() << ") are not the same size." << endmsg;
  }

  ATH_MSG_INFO("LAURYNAS msc size " << msc.size());
  TrackCollection * newtracks = m_trackMaker->find(msc);
  if (!newtracks) newtracks = new TrackCollection();

  SG::WriteHandle<TrackCollection> spectroTracks(m_spectroTrackKey); 	  
  if (spectroTracks.record(std::unique_ptr<TrackCollection>(newtracks)).isFailure()){    
      ATH_MSG_WARNING( "New Track Container " << spectroTracks.name() << " could not be recorded in StoreGate !");
      return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG ("TrackCollection '" << m_spectroTrackKey.key() << "' recorded in storegate, ntracks: " << newtracks->size());

  //---------------------------------------------------------------------------------------------------------------------//
  //------------                Monitoring of muon segments and tracks inside the trigger algs               ------------//
  //------------ Author:  Laurynas Mince                                                                     ------------//
  //------------ Created: 03/10/2019                                                                         ------------//
  //---------------------------------------------------------------------------------------------------------------------//

  // Only run monitoring for online algorithms
  if ( not m_monTool.name().empty() ) {
    std::vector<int>    ini_mstrksn(0);
    std::vector<double> ini_mstrkspt(0);
    std::vector<double> ini_mstrkseta(0);
    std::vector<double> ini_mstrksphi(0);
    auto mstrks_n     = Monitored::Collection("mstrks_n", ini_mstrksn);
    auto mstrks_pt    = Monitored::Collection("mstrks_pt", ini_mstrkspt);
    auto mstrks_eta   = Monitored::Collection("mstrks_eta", ini_mstrkseta);
    auto mstrks_phi   = Monitored::Collection("mstrks_phi", ini_mstrksphi);

    auto monitorIt = Monitored::Group(m_monTool, mstrks_n, mstrks_pt, mstrks_eta, mstrks_phi);

    // MS-only extrapolated tracks
    int count_mstrks = 0;
    for (auto const& mstrk : *newtracks) {
      count_mstrks++;
      ATH_MSG_INFO("LAURYNAS" << mstrk->trackParameters());
      const Trk::Perigee* perigee = mstrk->perigeeParameters();
      const Amg::Vector3D mom = perigee->momentum();
      ini_mstrkspt.push_back(mom.perp()/1000.0); // Converted to GeV
      //ini_mstrkspt.push_back(mstrk->pt()/1000.0); // converted to GeV
      // ini_mstrkseta.push_back(mstrk->eta());
      // ini_mstrksphi.push_back(mstrk->phi());
    }
    ini_mstrksn.push_back(count_mstrks);


  }



  return StatusCode::SUCCESS;
} // execute

StatusCode MuPatTrackBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

