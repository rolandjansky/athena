/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKBYTESTREAMDECODERENCODERAUX_H
#define FTKBYTESTREAMDECODERENCODERAUX_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamData/RawEvent.h"

#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrigFTK_RawData/FTK_RawPixelCluster.h"
#include "TrigFTK_RawData/FTK_RawSCT_Cluster.h"
#include "TrigFTKByteStream/IFTKByteStreamDecoderEncoderTool.h"


namespace FTKByteStreamDecoderEncoderAux {

  const size_t TrackBlobSize = 18; // magic number from BS specification
  const size_t TrackParamsBlobSize = 2; // --||--
  const size_t PixHitParamsBlobSize = 2; // --||--
  const size_t SCTHitParamsBlobSize = 2; // --||--
  const size_t NPixLayers=4; // number of layers in FTK_RawTrack (not the no. layers in BS)
  const size_t NSCTLayers=8; // number of layers in FTK_RawTrack (not the no. layers in BS)

  const uint32_t headerMarker = 0xb0f00000; 
  const uint32_t headerSize   = 0x7;

}

namespace FTK {

  class FTKByteStreamDecoderEncoderAuxTool : public AthAlgTool, virtual public  IFTKByteStreamDecoderEncoderTool {
    
  public:
    FTKByteStreamDecoderEncoderAuxTool(const std::string& name, const std::string& type,
				    const IInterface* parent);
    virtual ~FTKByteStreamDecoderEncoderAuxTool();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    StatusCode decode(uint32_t nDataWords, OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result);
    StatusCode encode(const FTK_RawTrackContainer* result, std::vector<uint32_t>& payload);
    
    static const InterfaceID& interfaceID( ) ; 
    
  private:
    //to be removed
    void packNumberOfTracks(uint16_t nTracksLowPt, uint16_t nTracksHighPt, std::vector<uint32_t>& rod );
    void unpackNumberOfTracks(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, 
			      uint16_t& nTracksLowPt, uint16_t& nTracksHighPt, uint32_t& nTracks);

    //
    void packPixelCluster(const FTK_RawPixelCluster& cluster, std::vector<uint32_t>& payload);
    void unpackPixCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawPixelCluster& cluster);
    
    void packSCTCluster(const FTK_RawSCT_Cluster& cluster, std::vector<uint32_t>& payload);
    void unpackSCTCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawSCT_Cluster& cluster);
    
    void packTrack(const FTK_RawTrack* track, std::vector<uint32_t>& payload);
    FTK_RawTrack* unpackFTTrack( OFFLINE_FRAGMENTS_NAMESPACE::PointerType data);
    
    void packHeader(std::vector<uint32_t> &payload);
    void unpackHeader(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData);
    
    void packTrailer(std::vector<uint32_t> &payload);
    void unpackTrailer(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData);

    void unpackMonitoring(OFFLINE_FRAGMENTS_NAMESPACE::PointerType &rodData);

    bool m_doHeader;
    bool m_doTrailer;
    uint32_t m_towerID;
  };

}
#endif
