/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMCNV_H
#define TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMCNV_H

// Local includes
#include "RoIBResultByteStreamTool.h"

// Athena includes
#include "AthenaBaseComps/AthMessaging.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

// Gaudi includes
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

/**
 *   @short ByteStream converter for the RoIBResult object
 *
 *          This is a bit of a tricky converter. It was first written quite
 *          early on, hence the implementation as a template class. The RoIBResult
 *          object is put together from a number of separate ROB fragments, as
 *          from eformat 3.0 onwards one ROB fragment can only hold one ROD
 *          fragment.
 *
 *     @see RoIBResultByteStreamTool
 *
 *  @author Tadasi Maeno
 *  @author Attila Krasznahorkay
 *    @date $Date: 2009-02-23 21:23:03 $
 */
template< class ROBF >
class RoIBResultByteStreamCnv : public Converter, public AthMessaging {

public:
  /// Standard constructor
  RoIBResultByteStreamCnv( ISvcLocator* svcloc );
  /// Function connecting to all the needed services/tools
  virtual StatusCode initialize();
  /// Function creating the RoIBResult object from the multiple ROB fragments
  virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );
  /// Function creating the multiple ROB fragments from the RoIBResult object
  virtual StatusCode createRep( DataObject* pObj, IOpaqueAddress*& pAddr );

  /// Function needed by the framework
  virtual long repSvcType() const { return i_repSvcType(); }
  /// Function needed by the framework
  static long storageType();
  /// Function needed by the framework
  static const CLID& classID();

private:
  /// Tool doing the actual conversion
  ToolHandle< RoIBResultByteStreamTool > m_tool;
  /// Service used when writing the BS data
  ServiceHandle< IByteStreamEventAccess > m_ByteStreamCnvSvc;
  /// Service used when reading the BS data
  ServiceHandle< IROBDataProviderSvc > m_robDataProviderSvc;
  /// Flag if running in athenaMT to prevent calling deprecated methods
  bool m_isMT{false};

}; // class RoIBResultByteStreamCnv

// Include the implementation:
#include "RoIBResultByteStreamCnv.icc"

#endif // TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMCNV_H
