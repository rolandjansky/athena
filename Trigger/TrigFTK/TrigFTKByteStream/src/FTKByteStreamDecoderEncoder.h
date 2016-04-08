/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKBYTESTREAMDECODERENCODER_H
#define FTKBYTESTREAMDECODERENCODER_H

#include "ByteStreamData/RawEvent.h"

#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrigFTK_RawData/FTK_RawPixelCluster.h"
#include "TrigFTK_RawData/FTK_RawSCT_Cluster.h"

namespace FTKByteStreamDecoderEncoder {
  const size_t TrackBlobSize = 22; // magic number from BS specification
  const size_t TrackParamsBlobSize = 6; // --||--
  const size_t PixHitParamsBlobSize = 2; // --||--
  const size_t SCTHitParamsBlobSize = 1; // --||--
  const size_t NPixLayers=4;
  const size_t NSCTLayers=8;

  StatusCode decode(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result,
  		    MsgStream& msg);

  size_t decodeNumberOfTracks(OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData);

  StatusCode encode(const FTK_RawTrackContainer* result, std::vector<uint32_t>& payload,
  		    MsgStream& msg);
}

#endif
