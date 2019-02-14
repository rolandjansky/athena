/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: HLTResultByteStream
 * @class  : HLTResultByteStreamTool
 *
 * @brief  Gaudi bytestream Converter tool for the HLTResult class
 *
 *
 * File and Version Information:
 * $Id: HLTResultByteStreamTool.h,v 1.4 2007-07-05 13:40:31 eifert Exp $
 **********************************************************************************/

#ifndef TrigHLTResultByteStreamTool_h
#define TrigHLTResultByteStreamTool_h

/* general stuff */
#include <stdint.h>
#include "AthenaBaseComps/AthAlgTool.h"

/* Needed to extract the evnum */
//#include "EventInfo/EventInfo.h"
//#include "EventInfo/EventID.h"
//#include "StoreGate/StoreGateSvc.h"

/* To be able to use OFFLINE_FRAGMENTS_NAMESPACE */
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

/* the RDO structure */
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigHLTResultByteStream/HLTSrcIdMap.h"

#include "eformat/SourceIdentifier.h"

class IROBDataProviderSvc;

namespace HLT {

  /**
     @class HLTResultByteStreamCnv
     @brief An AlgTool class to provide conversion from HLTResult to ByteStream, and fill it in RawEvent

     This tool is used by the Gaudi converter class: HLTResultByteStreamCnv.

  */


  class HLTResultByteStreamTool: public AthAlgTool {

  public:

    HLTResultByteStreamTool( const std::string& type, const std::string& name,
			     const IInterface* parent ) ; //!< std Gaudi tool constructor

    virtual ~HLTResultByteStreamTool() ; //!< virtual destructor

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

    eformat::SubDetector byteStreamLocation(std::string objName); //!< helper method
    FullEventAssembler<HLTSrcIdMap>* eventAssembler(std::string objName); //!< helper method

  private:
    static const std::string s_l2ResultName, s_efResultName, s_hltResultName, s_dataScoutingResultName; //!< LVL2 and EF StoreGate keys

    FullEventAssembler<HLTSrcIdMap> m_feaL2, m_feaEF; //!< helper for reading
  };
}

inline const InterfaceID& HLT::HLTResultByteStreamTool::interfaceID()
{
  static const InterfaceID IID("HLTResultByteStreamTool", 1, 0);
  return IID;
}

#endif
