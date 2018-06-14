/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IFTKBYTESTREAMDECODERENCODERTOOL_H
#define IFTKBYTESTREAMDECODERENCODERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h"

#include "TrigFTK_RawData/FTK_RawTrackContainer.h"


static const InterfaceID IID_IFTKByteStreamDecoderEncoderTool("IFTKByteStreamDecoderEncoderTool",1,0);

namespace FTK {

  class IFTKByteStreamDecoderEncoderTool : virtual public IAlgTool {
    
  public:
    static const InterfaceID& interfaceID ()   
      {  return IID_IFTKByteStreamDecoderEncoderTool; } 

    
    virtual StatusCode decode(const uint32_t nDataWords, OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result)=0;
    virtual StatusCode encode(const FTK_RawTrackContainer* result, std::vector<uint32_t>& payload)=0;
    
    
  };

}
#endif
