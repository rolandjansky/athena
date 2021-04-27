/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMCNV_H
#define TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMCNV_H

// Local includes
#include "RecRoIBResultByteStreamTool.h"

// Athena includes
#include "AthenaBaseComps/AthMessaging.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

// Gaudi includes
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"


/**
 *   @short ByteStream converter for the RecRoIBResult object
 *
 *          This converter takes care of creating the RecRoIBResult objects
 *          from BS data. It's kind of tricky as multiple ROB fragments are
 *          needed to create a single RecRoIBResult object.
 *
 *     @see RecRoIBResultByteStreamTool
 *
 *  @author Tadashi Maeno
 *  @author Attila Krasznahorkay
 *    @date $Date: 2009-02-23 21:23:03 $
 */
template< class ROBF >
class RecRoIBResultByteStreamCnv : public Converter, public AthMessaging {

public:
  /// Standard constructor
  RecRoIBResultByteStreamCnv( ISvcLocator* svcloc );

  /// Function connecting to all the needed services/tools
  virtual StatusCode initialize();
  /// Function creating the RecRoIBResult object from the RoIB ROB fragments
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

  /// Function needed by the framework
  virtual long repSvcType() const { return i_repSvcType(); }
  /// Function needed by the framework
  static long storageType();
  /// Function needed by the framework
  static const CLID& classID();

private:
  /// Tool doing the actual conversion
  ToolHandle< RecRoIBResultByteStreamTool > m_tool;
  /// Service used when reading the BS data
  ServiceHandle< IROBDataProviderSvc > m_robDataProviderSvc;

}; // class RecRoIBResultByteStreamCnv

// Include the implementation:
#include "RecRoIBResultByteStreamCnv.icc"

#endif // TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMCNV_H
