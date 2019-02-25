/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_RODHEADERBYTESTREAMAUXCNV_H
#define TRIGT1CALOBYTESTREAM_RODHEADERBYTESTREAMAUXCNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthMessaging.h"

class DataObject;
class IByteStreamEventAccess;
class IOpaqueAddress;
class IROBDataProviderSvc;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;



namespace LVL1BS {
class RodHeaderByteStreamTool;

/** ByteStream converter for RodHeader.
 *
 *  @author alexander.mazurov@cern.ch
 */

class RodHeaderByteStreamAuxCnv: public Converter, public ::AthMessaging {

public:
  RodHeaderByteStreamAuxCnv(ISvcLocator* svcloc);

  virtual ~RodHeaderByteStreamAuxCnv(){};

  virtual StatusCode initialize();
  /// Create TriggerTowers from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
  /// Create ByteStream from TriggerTowers
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  //  Storage type and class ID
  virtual long repSvcType() const { return i_repSvcType(); }
  static  long storageType();

  static const CLID& classID();
private:
  /// Converter name
  std::string m_name;

  /// Do the main job - retrieve objects from robs
  ToolHandle<RodHeaderByteStreamTool> m_readTool;
};



} // end namespace
#endif
