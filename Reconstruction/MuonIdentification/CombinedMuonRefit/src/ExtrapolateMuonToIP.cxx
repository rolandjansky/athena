/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolateMuonToIP.cxx
//   Implementation file for class ExtrapolateMuonToIP
///////////////////////////////////////////////////////////////////
// Niels van Eldik
///////////////////////////////////////////////////////////////////

#include "ExtrapolateMuonToIP.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkTrack/TrackCollection.h"

// Constructor with parameters:
ExtrapolateMuonToIP::ExtrapolateMuonToIP(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_backExtrapolator("ExtrapolateMuonToIPTool/ExtrapolateMuonToIPTool"),
  m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
{  
  // Get parameter values from jobOptions file
  declareProperty("BackExtrapolator",              m_backExtrapolator );
  declareProperty("MuonTrackLocation",             m_muonTrackLocation = "MooreTracks");
  declareProperty("ExtrapolatedMuonTrackLocation", m_extrapolateMuonTrackLocation = "MooreExtrapolatedTracks" );

}

// Initialize method:
StatusCode ExtrapolateMuonToIP::initialize()
{
  
  if( AthAlgorithm::initialize().isFailure() ){
    msg(MSG::ERROR) << "AthAlgorithm::ExtrapolateMuonToIP::initialize(), failed" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "ExtrapolateMuonToIP::initialize()" << endmsg;

  if(!m_backExtrapolator.empty()){
    if(m_backExtrapolator.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Failed to retrieve tool " << m_backExtrapolator << endmsg;
      return StatusCode::FAILURE;
    }else{
      msg(MSG::DEBUG) << "Retrieved tool " << m_backExtrapolator << endmsg;
    }
  }

  if( !m_printer.empty() ){
    if(m_printer.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Failed to retrieve tool " << m_printer << endmsg;
      return StatusCode::FAILURE;
    }else{
      msg(MSG::DEBUG) << "Retrieved tool " << m_printer << endmsg;
    }  
  }

  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode ExtrapolateMuonToIP::execute() 
{
  // retrieve muon tracks
  const TrackCollection* muonTracks = 0;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Retrieving "<<m_muonTrackLocation<<endmsg;
  StatusCode sc = evtStore()->retrieve(muonTracks, m_muonTrackLocation);
  if (sc.isFailure()){
    msg(MSG::INFO) << "Track collection named " << m_muonTrackLocation << " not found." << endmsg;
    return StatusCode::SUCCESS;
  }else{ 
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Muon Tracks found at " << m_muonTrackLocation << " size " << muonTracks->size() << endmsg; 
  }

  // use tool to perform back extrapolation
  TrackCollection* extrapolateTracks = m_backExtrapolator->extrapolate(*muonTracks);
  if( !extrapolateTracks) {
    msg(MSG::WARNING) << "BackExtrapolator returned zero pointer, creating empty collection " << endmsg;
    extrapolateTracks = new TrackCollection();
  }


if (evtStore()->contains<TrackCollection>(m_extrapolateMuonTrackLocation)) 
     { 
         const TrackCollection* extrapolatedTracks; 
         if (StatusCode::SUCCESS == evtStore()->retrieve(extrapolatedTracks, 
                                                         m_extrapolateMuonTrackLocation)) 
         { 
              
             if (StatusCode::SUCCESS == evtStore()->remove(extrapolatedTracks)) 
             { 
                 ATH_MSG_VERBOSE( " removed pre-existing extrapolated track collection" ); 
             } 
         } 
    } 


  sc = evtStore()->record(extrapolateTracks, m_extrapolateMuonTrackLocation);
  if (sc.isFailure()){
    msg(MSG::WARNING) <<"Could not record Track collection named " << m_extrapolateMuonTrackLocation << endmsg;
    return StatusCode::SUCCESS;
  }else{ 
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Extrapolated muons tracks recoreded at " << m_extrapolateMuonTrackLocation << " size " << extrapolateTracks->size() << endmsg; 
  }

  // now create track particles
  

  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode ExtrapolateMuonToIP::finalize() 
{
  if( AthAlgorithm::finalize().isFailure() ){
    msg(MSG::ERROR) << "AthAlgorithm::ExtrapolateMuonToIP::finalize(), failed" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "ExtrapolateMuonToIP::finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

