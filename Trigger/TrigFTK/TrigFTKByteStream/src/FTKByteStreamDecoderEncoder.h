/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKBYTESTREAMDECODERENCODER_H
#define FTKBYTESTREAMDECODERENCODER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamData/RawEvent.h"

#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrigFTK_RawData/FTK_RawPixelCluster.h"
#include "TrigFTK_RawData/FTK_RawSCT_Cluster.h"


namespace FTKByteStreamDecoderEncoder {
  const uint32_t headerMarker = 0xee1234ee; 
  const uint32_t headerSize   = 0x9;
  const size_t TrackBlobSize = 22; // magic number from BS specification
  const size_t TrackParamsBlobSize = 6; // --||--
  const size_t PixHitParamsBlobSize = 2; // --||--
  const size_t SCTHitParamsBlobSize = 1; // --||--
  const size_t NPixLayers=4;
  const size_t NSCTLayers=8;


  /*
  StatusCode decode(uint32_t nTracks, OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result,
  		    MsgStream& msg);

  size_t decodeNumberOfTracks(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData);

  StatusCode encode(const FTK_RawTrackContainer* result, std::vector<uint32_t>& payload,
  		    MsgStream& msg);
  */

}

namespace FTK {

  class FTKByteStreamDecoderEncoderTool : public AthAlgTool {
    
  public:
    FTKByteStreamDecoderEncoderTool(const std::string& name, const std::string& type,
				    const IInterface* parent);
    virtual ~FTKByteStreamDecoderEncoderTool();
    
    StatusCode initialize();
    StatusCode finalize();
    
    StatusCode decode(uint32_t nTracks, OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result);
    StatusCode encode(const FTK_RawTrackContainer* result, std::vector<uint32_t>& payload);
    
    static const InterfaceID& interfaceID( ) ; 
    
  private:
    void packNumberOfTracks(uint16_t nTracksLowPt, uint16_t nTracksHighPt, std::vector<uint32_t>& rod );
    void unpackNumberOfTracks(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, 
			      uint16_t& nTracksLowPt, uint16_t& nTracksHighPt, uint32_t& nTracks);
    void packPixelCluster(const FTK_RawPixelCluster& cluster, std::vector<uint32_t>& payload);
    void unpackPixCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawPixelCluster& cluster);
    
    void packSCTCluster(const FTK_RawSCT_Cluster& cluster, std::vector<uint32_t>& payload);
    void unpackSCTCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawSCT_Cluster& cluster);
    
    void packTrack(const FTK_RawTrack* track, std::vector<uint32_t>& payload);
    FTK_RawTrack* unpackFTTrack( OFFLINE_FRAGMENTS_NAMESPACE::PointerType data);
    
    void packHeader(std::vector<uint32_t> &payload);
    void unpackHeader(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData);
    
    void packTrailer();
    void unpackTrailer(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData);

    void unpackMonitoring(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData);
  };

}
#endif
