/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: FTK
 * @Package: TrigFTKByteStream
 * @Class  : TrigFTKByteStreamTool
 *
 * @brief  Gaudi bytestream Converter tool for the TrigFTK class
 *
 *
 * File and Version Information:
 * $Id:
 **********************************************************************************/

#ifndef TrigTrigFTKByteStreamTool_h
#define TrigTrigFTKByteStreamTool_h

#include "AthenaBaseComps/AthAlgTool.h"


/* To be able to use OFFLINE_FRAGMENTS_NAMESPACE */
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

/* the RDO structure */
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"

#include "TrigFTKByteStream/IFTKByteStreamDecoderEncoderTool.h"

#include "eformat/SourceIdentifier.h"

// forward declarations
class IROBDataProviderSvc;

namespace FTK {

  //  class IFTKByteStreamDecoderEncoderTool;
  /**
     @class TrigFTKByteStreamTool
     @brief An AlgTool class to provide conversion from TrigFTK to ByteStream, and fill it in RawEvent

     This tool is used by the Gaudi converter class: TrigFTKByteStreamCnv.
  */

  class TrigFTKByteStreamTool: public AthAlgTool {
    
  public:

    TrigFTKByteStreamTool( const std::string& type, const std::string& name,
           const IInterface* parent ) ; //!< std Gaudi tool constructor

    virtual ~TrigFTKByteStreamTool() ; //!< virtual destructor

    static const InterfaceID& interfaceID( ) ; //!< std Gaudi interface

    virtual StatusCode initialize(); //!< std Gaudi initialize -> call initialize from base class
    virtual StatusCode finalize();   //!< std Gaudi finalize   -> call finalize from base class


    /// @brief convert ROBData to FTK_RawTrackContainer this function should be called from createObj
    StatusCode convert(IROBDataProviderSvc& dataProvider, FTK_RawTrackContainer*& result,
                       const std::string& objName);

    /// @brief convert FTK_RawTrackContainer to ByteStream this function should be called from createRep
    StatusCode convert( const FTK_RawTrackContainer* result, RawEventWrite* re,
			const std::string& objName);
  private:
    FullEventAssembler<> m_fea;
    ToolHandle <IFTKByteStreamDecoderEncoderTool> m_decoder;
    bool m_isAuxFormat;
    std::vector<uint32_t> m_robID;

  };
}

#endif
