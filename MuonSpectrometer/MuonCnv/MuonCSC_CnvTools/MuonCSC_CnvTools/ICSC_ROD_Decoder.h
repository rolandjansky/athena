/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOL_ICSC_ROD_Decoder_H
#define MUONCSC_CNVTOOL_ICSC_ROD_Decoder_H

#include "GaudiKernel/IAlgTool.h"
#include <string>
#include "ByteStreamData/RawEvent.h" 
#include <vector>

class CscRawDataContainer;
class MsgStream;
class EventInfo;
class Identifier;

//using eformat::helper::SourceIdentifier; 

static const InterfaceID IID_ICSC_ROD_Decoder("Muon::ICSC_ROD_Decoder", 1, 0);


namespace Muon {
  
  /** This class provides conversion from ROD data to CSC RDO
   * Author: Ketevi A. Assamagan
   * BNL December 27 2003
   */
  
  class ICSC_ROD_Decoder : virtual public IAlgTool {
    
  public: 
    static const InterfaceID& interfaceID( ) { return IID_ICSC_ROD_Decoder; };
    
    virtual void setRobFragment(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* robFrag)=0;
    virtual void setEventInfo(const EventInfo* eventInfo)=0;
    virtual void setGeoVersion(const std::string geoVersion)=0;
    virtual void fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,  CscRawDataContainer& rdoIDC, MsgStream& mLog) =0; 
    
    virtual Identifier getChannelId(const uint32_t word, std::string detdesription) =0;
    virtual uint32_t getHashId(const uint32_t word, std::string detdesription) =0;
    virtual void getSamples(const std::vector<uint32_t>& words, std::vector<uint16_t>& samples ) =0;
  }; 
  
}

#endif

