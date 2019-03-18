// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPOBYTESTREAM_L1TOPOBYTESTREAMCNV_H
#define L1TOPOBYTESTREAM_L1TOPOBYTESTREAMCNV_H

// Gaudi/Athena include(s):
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"

#include "AthenaBaseComps/AthMessaging.h"

// Local include(s):
#include "L1TopoByteStreamTool.h"

// Forward declaration(s):
class L1TopoSrcIdMap;

/**
 *   @short ByteStream converter for the L1TopoRDOCollection
 *
 *          This is a quite standard BS converter for the output sent from
 *          L1Topo to its ROS. It uses a separate tool
 *          (L1TopoByteStreamTool) to do the actual work of the conversion,
 *          this converter is just to communicate with the framework.
 *          It was based on CTPByteStreamCnv by David Berge
 *          and the L1Calo converters by Alan Watson and Peter Faulkner.
 *
 *     @see L1TopoByteStreamTool
 *
 *  @author Simon George
 *    @date $Date: 2014-11-12 00:00:00 $
 */
class L1TopoByteStreamCnv : public Converter, public AthMessaging {

public:
  /// Standard constructor
  L1TopoByteStreamCnv(ISvcLocator* svcloc);

 public:
  /// Standard destructor
  ~L1TopoByteStreamCnv();

  /// Function connecting to all the needed services/tools
  virtual StatusCode initialize();
  /// Function creating the L1TopoRDOCollection object from a L1Topo ROB
  /// fragment
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
  /// Function creating the L1Topo ROB fragment from a L1TopoRDOCollection
  /// object
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  /// Function needed by the framework
  virtual long repSvcType() const { return i_repSvcType(); }
  /// Function needed by the framework
  static long storageType();
  /// Function needed by the framework
  static const CLID& classID();

 private:
  /// Tool doing the actual conversion
  ToolHandle<L1TopoByteStreamTool> m_tool;

  /// Object storing the various IDs of the CTP fragment
  L1TopoSrcIdMap* m_srcIdMap;

  /// Service used when reading the BS data
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  /// Service used when writing the BS data
  ServiceHandle<IByteStreamEventAccess> m_ByteStreamEventAccess;

};  // class L1TopoByteStreamCnv

#endif  // L1TOPOBYTESTREAM_L1TOPOBYTESTREAMCNV_H
