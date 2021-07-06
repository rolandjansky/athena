// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_RECMUCTPIBYTESTREAMCNV_H
#define TRIGT1RESULTBYTESTREAM_RECMUCTPIBYTESTREAMCNV_H

// Gaudi/Athena include(s):
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"

// Local include(s):
#include "RecMuCTPIByteStreamTool.h"
#include "MuCTPISrcIdMap.h"


/**
 *   @short ByteStream converter for the MuCTPI_RIO object
 *
 *          This is a quite standard BS converter for the output sent from the
 *          Muon to Central Trigger Processor Interface to the DAQ. It uses an
 *          external tool (RecMuCTPIByteStreamTool) to do the actual work of
 *          the conversion, this converter is "only" supposed to communicate
 *          with the framework.
 *
 *     @see RecMuCTPIByteStreamTool
 *
 *  @author Tadashi Maeno
 *  @author Attila Krasznahorkay
 */
class RecMuCTPIByteStreamCnv : public Converter {

public:
  /// Standard constructor
  RecMuCTPIByteStreamCnv( ISvcLocator* svcloc );

  /// Function connecting to all the needed services/tools
  virtual StatusCode initialize() override;
  /// Function creating the MuCTPI_RIO object from a MuCTPI ROB fragment
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) override;

  /// Function needed by the framework
  virtual long repSvcType() const override { return i_repSvcType(); }
  /// Function needed by the framework
  static long storageType();
  /// Function needed by the framework
  static const CLID& classID();

private:
  /// Tool doing the actual conversion
  ToolHandle< RecMuCTPIByteStreamTool > m_tool;

  /// Object storing the various IDs of the MuCTPI fragment
  MuCTPISrcIdMap m_srcIdMap;

  /// Service used when reading the BS data
  ServiceHandle< IROBDataProviderSvc >    m_robDataProvider;

}; // class RecMuCTPIByteStreamCnv

#endif // TRIGT1RESULTBYTESTREAM_RECMUCTPIBYTESTREAMCNV_H
