// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_MUCTPIBYTESTREAMCNV_H
#define TRIGT1RESULTBYTESTREAM_MUCTPIBYTESTREAMCNV_H

//#define CTP_MUCTPI_HAVE_SAME_ROS

// Gaudi/Athena include(s):
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#ifdef CTP_MUCTPI_HAVE_SAME_ROS
#  include "StoreGate/StoreGateSvc.h"
#endif

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"

// Local include(s):
#include "TrigT1ResultByteStream/MuCTPIByteStreamTool.h"
#ifdef CTP_MUCTPI_HAVE_SAME_ROS
#  include "TrigT1ResultByteStream/CTPByteStreamTool.h"
#  include "TrigT1ResultByteStream/RecCTPByteStreamTool.h"
#endif

// Forward declaration(s):
class MuCTPISrcIdMap;

/**
 *   @short ByteStream converter for the MuCTPI_RDO object
 *
 *          This is a quite standard BS converter for the output sent from the
 *          "Muon to Central Trigger Processor Interface" to the DAQ. It uses
 *          an external tool (MuCTPIByteStreamTool) to do the actual work of
 *          the conversion, this converter is "only" supposed to communicate
 *          with the framework.
 *
 *     @see MuCTPIByteStreamTool
 *
 *  @author Tadashi Maeno
 *  @author Attila Krasznahorkay
 *    @date $Date: 2009-02-23 21:23:03 $
 */
class MuCTPIByteStreamCnv : public Converter {

public:
  /// Standard constructor
  MuCTPIByteStreamCnv( ISvcLocator* svcloc );

  /// Standard destructor
  ~MuCTPIByteStreamCnv();

  /// Function connecting to all the needed services/tools
  virtual StatusCode initialize();
  /// Function creating the MuCTPI_RDO object from a MuCTPI ROB fragment
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
  /// Function creating the MuCTPI ROB fragment from a MuCTPI_RDO object
  virtual StatusCode createRep( DataObject* pObj, IOpaqueAddress*& pAddr );

  /// Function needed by the framework
  virtual long repSvcType() const { return i_repSvcType(); }
  /// Function needed by the framework
  static long storageType();
  /// Function needed by the framework
  static const CLID& classID();

private:
  /// Tool doing the actual conversion
  ToolHandle< MuCTPIByteStreamTool > m_tool;
  /// Object storing the various IDs of the MuCTPI fragment
  MuCTPISrcIdMap* m_srcIdMap;

#ifdef CTP_MUCTPI_HAVE_SAME_ROS
  ToolHandle< CTPByteStreamTool >    m_ctp_tool;
  ToolHandle< RecCTPByteStreamTool > m_ctp_rec_tool;
  ServiceHandle< StoreGateSvc >      m_storeGate;
#endif

  /// Service used when reading the BS data
  ServiceHandle< IROBDataProviderSvc >    m_robDataProvider;
  /// Service used when writing the BS data
  ServiceHandle< IByteStreamEventAccess > m_ByteStreamEventAccess;

}; // class MuCTPIByteStreamCnv

#endif // TRIGT1RESULTBYTESTREAM_MUCTPIBYTESTREAMCNV_H
