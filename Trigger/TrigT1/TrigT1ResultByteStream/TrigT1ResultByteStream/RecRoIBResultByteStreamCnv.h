// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMCNV_H
#define TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMCNV_H

// Gaudi/Athena include(s):
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "TrigT1ResultByteStream/RecRoIBResultByteStreamTool.h"

/**
 *   @short ByteStream converter for the RecRoIBResult object
 *
 *          This converter takes care of creating the RecRoIBResult objects
 *          from BS data. It's kind of tricky as multiple ROB fragments are
 *          needed to create a single RecRoIBResult object, but the BS
 *          converter framework handles the RoIB result as a special case...
 *
 *     @see RecRoIBResultByteStreamTool
 *
 *  @author Tadashi Maeno
 *  @author Attila Krasznahorkay
 *    @date $Date: 2009-02-23 21:23:03 $
 */
template< class ROBF >
class RecRoIBResultByteStreamCnv : public Converter {

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

}; // class RecRoIBResultByteStreamCnv

// Include the implementation:
#include "TrigT1ResultByteStream/RecRoIBResultByteStreamCnv.icc"

#endif // TRIGT1RESULTBYTESTREAM_RECROIBRESULTBYTESTREAMCNV_H
