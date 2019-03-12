// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_CTPBYTESTREAMCNV_H
#define TRIGT1RESULTBYTESTREAM_CTPBYTESTREAMCNV_H

// Gaudi/Athena include(s):
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"

// Local include(s):
#include "TrigT1ResultByteStream/CTPByteStreamTool.h"

// Forward declaration(s):
class CTPSrcIdMap;

/**
 *   @short ByteStream converter for the CTP_RDO object
 *
 *          This is a quite standard BS converter for the output sent from the
 *          Central Trigger Processor to the DAQ. It uses an external tool
 *          (CTPByteStreamTool) to do the actual work of the conversion, this
 *          converter is "only" supposed to communicate with the framework.
 *
 *     @see CTPByteStreamTool
 *
 *  @author David Berge
 *    @date $Date: 2009-02-23 21:23:03 $
 */
class CTPByteStreamCnv : public Converter {

public:
  /// Standard constructor
  CTPByteStreamCnv( ISvcLocator* svcloc );

  /// Standard destructor
  ~CTPByteStreamCnv();

  /// Function connecting to all the needed services/tools
  virtual StatusCode initialize();
  /// Function creating the CTP_RDO object from a CTP ROB fragment
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
  /// Function creating the CTP ROB fragment from a CTP_RDO object
  virtual StatusCode createRep( DataObject* pObj, IOpaqueAddress*& pAddr );

  /// Function needed by the framework
  virtual long repSvcType() const { return i_repSvcType(); }
  /// Function needed by the framework
  static long storageType();
  /// Function needed by the framework
  static const CLID& classID();

private:
  /// Tool doing the actual conversion
  ToolHandle< CTPByteStreamTool > m_tool;

  /// Object storing the various IDs of the CTP fragment
  CTPSrcIdMap* m_srcIdMap;

  /// Service used when reading the BS data
  ServiceHandle< IROBDataProviderSvc >    m_robDataProvider;
  /// Service used when writing the BS data
  ServiceHandle< IByteStreamEventAccess > m_ByteStreamEventAccess;

}; // class CTPByteStreamCnv

#endif // TRIGT1RESULTBYTESTREAM_CTPBYTESTREAMCNV_H
