/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOL_CSCROD_DECODER_H
#define MUONCSC_CNVTOOL_CSCROD_DECODER_H

// MuonCSC_CnvTool includes  
#include "MuonCSC_CnvTools/ICSC_ROD_Decoder.h"        
#include "CscRODReadOut.h"
#include "CscRODReadOutV0.h"
#include "CscRODReadOutV1.h"
#include "CSC_Hid2RESrcID.h"

#include <inttypes.h>

#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/Identifier.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

#include "ByteStreamData/RawEvent.h" 
#include "eformat/SourceIdentifier.h"
#include "eformat/Version.h"

#include "CSCcabling/CSCcablingSvc.h"

class CscRawDataContainer;
class CscIdHelper;

//using namespace OFFLINE_FRAGMENTS_NAMESPACE ; 
//using eformat::helper::SourceIdentifier; 

namespace Muon {

/** This class provides conversion from ROD data to CSC RDO
   * Author: Ketevi A. Assamagan
   * BNL December 27 2003
   */

class CscROD_Decoder : virtual public ICSC_ROD_Decoder, public AthAlgTool  {

public: 
  
  /** constructor 
   */
  CscROD_Decoder(const std::string& type, const std::string& name,
		 const IInterface* parent ) ;

  /** destructor 
   */
  ~CscROD_Decoder(); 

  virtual StatusCode initialize();
  virtual StatusCode finalize() { return StatusCode::SUCCESS; }
  
  void setRobFragment(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* robFrag);
  void setEventInfo(const EventInfo* eventInfo);
  void setGeoVersion(const std::string geoVersion);

  void fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,  CscRawDataContainer& rdoIDC, MsgStream& mLog) ; 

  Identifier getChannelId(const uint32_t word, std::string detdesription);
  uint32_t getHashId(const uint32_t word, std::string detdesription);
  void getSamples(const std::vector<uint32_t>& words, std::vector<uint16_t>& samples );

  // put this in Interface header file...
  
private:

  /** the ROD version */
  void rodVersion0(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,  CscRawDataContainer& rdoIDC, MsgStream& mLog);

  void rodVersion1(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,  CscRawDataContainer& rdoIDC, MsgStream& mLog);

  void rodVersion2(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,  CscRawDataContainer& rdoIDC, MsgStream& mLog);

private:

  CSC_Hid2RESrcID                   m_hid2re;
  const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *               m_robFragment;
  const EventInfo   *               m_eventInfo;
  std::string                       m_geoVersion;
  const CscIdHelper *               m_cscHelper;
  ServiceHandle<CSCcablingSvc>      m_cabling;

  bool                              m_isCosmic;
  bool                              m_isOldCosmic;

}; 

/// set the ROD header
inline void CscROD_Decoder::setRobFragment(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* rob) {
   m_robFragment=rob;
}

inline void CscROD_Decoder::setEventInfo(const EventInfo* eventInfo) { m_eventInfo = eventInfo; }

inline void CscROD_Decoder::setGeoVersion(const std::string geoVersion) { m_geoVersion = geoVersion; }

}

#endif






