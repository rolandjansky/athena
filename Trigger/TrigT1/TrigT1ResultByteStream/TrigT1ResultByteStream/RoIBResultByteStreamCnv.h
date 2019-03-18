// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMCNV_H
#define TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMCNV_H

// Gaudi/Athena include(s):
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"

// Local include(s):
#include "TrigT1ResultByteStream/RoIBResultByteStreamTool.h"

/**
 *   @short ByteStream converter for the RoIBResult object
 *
 *          This is a bit of a tricky converter. It was first written quite
 *          early on, hence the implementation as a template class. The RoIBResult
 *          object is put together from a number of separate ROB fragments, as
 *          from eformat 3.0 onwards one ROB fragment can only hold one ROD
 *          fragment. For this reason this converter is treated in a special way
 *          at ByteStream conversion.
 *
 *     @see RoIBResultByteStreamTool
 *
 *  @author Tadasi Maeno
 *  @author Attila Krasznahorkay
 *    @date $Date: 2009-02-23 21:23:03 $
 */
template< class ROBF >
class RoIBResultByteStreamCnv : public Converter {

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

}; // class RoIBResultByteStreamCnv

// Include the implementation:
#include "TrigT1ResultByteStream/RoIBResultByteStreamCnv.icc"

#endif // TRIGT1RESULTBYTESTREAM_ROIBRESULTBYTESTREAMCNV_H
