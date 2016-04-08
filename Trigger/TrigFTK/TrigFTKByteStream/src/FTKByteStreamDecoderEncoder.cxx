/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "FTKByteStreamDecoderEncoder.h"
using namespace FTKByteStreamDecoderEncoder;

namespace {
  void packNumberOfTracks(uint16_t nTracksLowPt, uint16_t nTracksHighPt, std::vector<uint32_t>& rod ) {
    uint32_t size_data = nTracksHighPt;
    size_data <<= 16;
    size_data |= nTracksLowPt;
    rod.push_back( size_data );
  }

  
  void unpackNumberOfTracks(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, 
			    uint16_t& nTracksLowPt, uint16_t& nTracksHighPt, uint32_t& nTracks) {
    nTracksLowPt  =  rodData[0] & 0xffff; 
    nTracksHighPt =  (rodData[0]>>16) & 0xffff; 
    
    nTracks = nTracksLowPt; // conversion to 32 + assignement
    nTracksLowPt += nTracksHighPt; // adding to more capable type
  }


  void packPixelCluster(const FTK_RawPixelCluster& cluster, std::vector<uint32_t>& payload) {
    payload.push_back( cluster.getWordA() );
    payload.push_back( cluster.getWordB() );
  }
  
  
  void unpackPixCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawPixelCluster& cluster) {
    cluster.setWordA(*data);
    cluster.setWordB(*(data+1));
  }

  void packSCTCluster(const FTK_RawSCT_Cluster& cluster, std::vector<uint32_t>& payload) {
    payload.push_back( cluster.getWord() );
  }

  void unpackSCTCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawSCT_Cluster& cluster) {
    cluster.setWord(*data);
  }


  void packTrack(const FTK_RawTrack* track, std::vector<uint32_t>& payload) {
    payload.push_back( track->getTH1() );
    payload.push_back( track->getTH2() );
    payload.push_back( track->getTH3() );
    payload.push_back( track->getTH4() );
    payload.push_back( track->getTH5() );
    payload.push_back( track->getTH6() );
    
    for ( std::vector<FTK_RawPixelCluster>::const_iterator it = track->getPixelClusters().begin();
	  it != track->getPixelClusters().end(); ++it ) {
      packPixelCluster(*it, payload);
    }
    
    for ( std::vector<FTK_RawSCT_Cluster>::const_iterator it = track->getSCTClusters().begin();
	  it != track->getSCTClusters().end(); ++it ) {
      packSCTCluster(*it, payload);
    }
  }



  FTK_RawTrack* unpackFTTrack( OFFLINE_FRAGMENTS_NAMESPACE::PointerType data) {  
    FTK_RawTrack* track = new FTK_RawTrack(data[0], data[1], data[2], data[3], data[4], data[5]); // first six words are track params
    data += TrackParamsBlobSize;
    
    // get pixel hits  
    track->getPixelClusters().resize(NPixLayers);
    for ( size_t i = 0; i < size_t(NPixLayers); ++i) {
      size_t offset = PixHitParamsBlobSize*i;
      unpackPixCluster(data+offset, track->getPixelCluster(i) );    
    }
    data += PixHitParamsBlobSize*NPixLayers;
    
    // gets SCT hits
    track->getSCTClusters().resize(NSCTLayers);
    for ( size_t i = 0; i < size_t(NSCTLayers); ++i) {
      size_t offset = SCTHitParamsBlobSize*i;
      unpackSCTCluster(data+offset, track->getSCTCluster(i) );    
    }
    // no more shifts needed
    return track;
  }  
} // eof internal namespace


namespace FTKByteStreamDecoderEncoder {
  StatusCode encode(const FTK_RawTrackContainer* container, std::vector<uint32_t>& payload,  
		    MsgStream& /*msg*/) {

    // We used to have a word defining the number of low/high pt tracks, that is no longer necessary
    // // do not know yet what tracks are high pT, so all will be low pT
    // payload.reserve( 1 + TrackParamsBlobSize * container->size() );
    // packNumberOfTracks( container->size(), 0, payload );  

    payload.reserve(TrackParamsBlobSize * container->size() );
    
    for ( FTK_RawTrackContainer::const_iterator track = container->begin(); 
	  track != container->end(); ++track ) {
      packTrack(*track, payload);
    }
    return StatusCode::SUCCESS;
  }  



  StatusCode decode(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result, 
		    MsgStream& msg) {
    
    uint16_t nTracksHighPt;
    uint16_t nTracksLowPt;
    uint32_t nTracks;
    unpackNumberOfTracks(rodData, nTracksLowPt, nTracksHighPt, nTracks);
    
    msg << MSG::DEBUG << "[convert to FTK_RawTrackContainer] ntracks All: " << nTracks 
	<< " Lo pT: " << nTracksLowPt 
	<< " Hi pT: " << nTracksHighPt << endreq;      

    rodData++;
    
    result->reserve(result->size() + nTracks);
    for ( size_t i = 0; i < nTracks; ++i ) {
      FTK_RawTrack* track = unpackFTTrack( rodData );
      rodData += TrackBlobSize;
      result->push_back(track);
    }
    return StatusCode::SUCCESS;
  }

  size_t decodeNumberOfTracks(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData) {
    uint16_t low, high;
    uint32_t all;
    unpackNumberOfTracks(rodData, low, high, all);
    return all;
  }

}
