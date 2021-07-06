/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: HLTResultByteStream
 * @class  : HLTResultByteStreamTool
 *
 * @brief  Gaudi bytestream Converter tool for the HLTResult class
 **********************************************************************************/

#ifndef TrigHLTResultByteStreamTool_h
#define TrigHLTResultByteStreamTool_h

/* general stuff */
#include <stdint.h>
#include <map>
#include <string>
#include "AthenaBaseComps/AthAlgTool.h"

/* To be able to use OFFLINE_FRAGMENTS_NAMESPACE */
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

/* the RDO structure */
#include "TrigSteeringEvent/HLTResult.h"
#include "HLTSrcIdMap.h"

#include "eformat/SourceIdentifier.h"

class IROBDataProviderSvc;

namespace HLT {

  /**
     @class HLTResultByteStreamCnv
     @brief An AlgTool to provide conversion of Run-1&2 HLTResult from/to ByteStream

     This tool is used by the Gaudi converter class: HLTResultByteStreamCnv.
  */
  class HLTResultByteStreamTool: public AthAlgTool {

  public:
    HLTResultByteStreamTool( const std::string& type, const std::string& name,
                             const IInterface* parent ) ; //!< std Gaudi tool constructor

    static const InterfaceID& interfaceID( ) ; //!< std Gaudi interface

    /** @brief convert ROBData to HLTResult this function
	should be called from createObj
     **/
    StatusCode convert(IROBDataProviderSvc& dataProvider, HLTResult*& result,
                       std::string objName);

    /** @brief convert HLTResult to ByteStream
	this function should be called from createRep
     **/
    StatusCode convert(HLTResult* result, RawEventWrite* re,
                       std::string objName);

  private:

    Gaudi::Property<std::map<std::string, uint32_t>> m_robIDMap{
      this, "HLTResultRobIdMap", {
        {"HLTResult_L2", eformat::helper::SourceIdentifier(eformat::TDAQ_LVL2, 0).code()},
        {"HLTResult_EF", eformat::helper::SourceIdentifier(eformat::TDAQ_EVENT_FILTER, 0).code()},
        {"HLTResult_HLT", eformat::helper::SourceIdentifier(eformat::TDAQ_HLT, 0).code()}
      }, "Map of HLTResult names to ROB IDs"};

    FullEventAssembler<HLTSrcIdMap> m_fea;
  };
}

inline const InterfaceID& HLT::HLTResultByteStreamTool::interfaceID()
{
  static const InterfaceID IID("HLTResultByteStreamTool", 1, 0);
  return IID;
}

#endif
