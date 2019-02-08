/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOL_ICSC_ROD_Decoder_H
#define MUONCSC_CNVTOOL_ICSC_ROD_Decoder_H

#include "GaudiKernel/IAlgTool.h"
#include <string>
#include "xAODEventInfo/EventInfo.h"
#include "ByteStreamData/RawEvent.h" 
#include <vector>

class CscRawDataContainer;
class MsgStream;
class EventInfo;
class Identifier;



namespace Muon {
  
  /** This class provides conversion from ROD data to CSC RDO
   * Author: Ketevi A. Assamagan
   * BNL December 27 2003
   */
  
  class ICSC_ROD_Decoder : virtual public IAlgTool {
    
  public: 
    DeclareInterfaceID (ICSC_ROD_Decoder, 1, 0);
    
    virtual void fillCollection(const xAOD::EventInfo& eventInfo,
                                const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                CscRawDataContainer& rdoIDC) const =0; 
    
    virtual Identifier getChannelId(const uint32_t word, std::string detdesription) const =0;
    virtual uint32_t getHashId(const uint32_t word, std::string detdesription) const =0;
    virtual void getSamples(const std::vector<uint32_t>& words, std::vector<uint16_t>& samples ) const =0;
  }; 
  
}

#endif

