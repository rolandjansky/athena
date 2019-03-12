/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigTrigFTKByteStreamCnv_h
#define TrigTrigFTKByteStreamCnv_h

/**********************************************************************************
 * @Project: FTK
 * @Package: TrigFTKByteStream
 * @class  : TrigFTKByteStreamCnv
 *
 * @brief  Gaudi bytestream Converter for the TrigFTK class
 *
 **********************************************************************************/

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

#include "TrigFTKByteStream/TrigFTKByteStreamTool.h"

// forward declarations
class IROBDataProviderSvc;
class IByteStreamEventAccess;

namespace FTK {

  class FTKByteStreamTool;


  /**
     @class TrigFTKByteStreamCnv
     @brief Gaudi bytestream Converter for the FTKAthTrackContainer class

     This class essentially provides two methods:
     1. createObj(IOpaqueAddress*,  DataObject*&)
     2. createRep(DataObject*, IOpaqueAddress*& )

     The first one creates a DataObject from persistent information, captured in
     IOpaqueAddress, and the second is supposed to write the DataObject to
     a persistent representation. Here, DataObject will be casted to TrigFTK

     see here for a general ByteStream converter description:
     http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/Event/ByteStreamCnvSvc/doc/ByteStream.txt

     beginning of this file:<br>
     ByteStream conversion Service (ByteStreamCnvSvc) provides a means to
     convert the raw data in the ByteStream format to and from the transient
     representation, Raw Data Objects (RDO).   In Gaudi's terminology, ByteStream
     can be considered as a persistency for reading and storing raw data. In
     practice, the source of the ByteStream can be a file, or the online data
     source (or its emulation) through network, for example.   Together with the
     IdentifiableContainer, used to contain the Raw Data Objects, ByteStreamCnvSvc
     provides a mechanism for creating the RDOs on demand.

  */
  class TrigFTKByteStreamCnv : public Converter {

  public:
    TrigFTKByteStreamCnv(ISvcLocator* svcloc); //!< std Gaudi converter constructor
    ~TrigFTKByteStreamCnv();


    virtual StatusCode initialize(); //!< Gaudi initialize
    virtual StatusCode finalize();   //!< Gaudi finalize

    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj); //!< create RDO (TrigFTK) from bytestream
    virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr); //!< create bytestream from RDOs (TrigFTK)

    // Storage type and class ID
    static unsigned char storageType()     { return ByteStreamAddress::storageType(); } //!< used for the converter
    static const CLID& classID(); //!< CLID

    long repSvcType() const { return i_repSvcType(); } //!< return repSvcType

  private:

    MsgStream* m_log; //!< std message stream

    ToolHandle< TrigFTKByteStreamTool > m_tool ; //!< tool for conversion work

    ServiceHandle< IByteStreamEventAccess > m_ByteStreamEventAccess; //!< Gaudi helper

    ServiceHandle< IROBDataProviderSvc > m_robDataProvider; //!< Gaudi ROB helper

  };
}

#endif
