// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_RECCTPBYTESTREAMCNV_H
#define TRIGT1RESULTBYTESTREAM_RECCTPBYTESTREAMCNV_H

// Gaudi/Athena include(s):
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"

// Local include(s):
#include "TrigT1ResultByteStream/RecCTPByteStreamTool.h"

// Forward declaration(s):
class CTPSrcIdMap;

/**
 *   @short ByteStream converter for the CTP_RIO object
 *
 *          This is a quite standard BS converter for the output sent from the
 *          Central Trigger Processor to the DAQ. It uses an external tool
 *          (RecCTPByteStreamTool) to do the actual work of the conversion, this
 *          converter is "only" supposed to communicate with the framework.
 *
 *     @see RecCTPByteStreamTool
 *
 *  @author David Berge
 *    @date $Date: 2009-02-23 21:23:03 $
 */
class RecCTPByteStreamCnv : public Converter {

public:
  /// Standard constructor
  RecCTPByteStreamCnv(ISvcLocator* svcloc);

  /// Standard destructor
  ~RecCTPByteStreamCnv();

  /// Function connecting to all the needed services/tools
  virtual StatusCode initialize();
  /// Function creating the CTP_RIO object from a CTP ROB fragment
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

  /// Function needed by the framework
  virtual long repSvcType() const { return i_repSvcType(); }
  /// Function needed by the framework
  static long storageType();
  /// Function needed by the framework
  static const CLID& classID();

private:
  /// Tool doing the actual conversion
  ToolHandle< RecCTPByteStreamTool > m_tool;

  /// Object storing the various IDs of the CTP fragment
  CTPSrcIdMap* m_srcIdMap;

  /// Service used when reading the BS data
  ServiceHandle< IROBDataProviderSvc > m_robDataProvider;

}; // class RecCTPByteStreamCnv

#endif // TRIGT1RESULTBYTESTREAM_MUCTPIBYTESTREAMCNV_H
