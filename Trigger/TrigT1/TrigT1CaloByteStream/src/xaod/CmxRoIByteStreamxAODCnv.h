/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CMXROIBYTESTREAMXAODCNV_H
#define TRIGT1CALOBYTESTREAM_CMXROIBYTESTREAMXAODCNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthMessaging.h"

class DataObject;
class IOpaqueAddress;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;
class StoreGateSvc;

namespace LVL1BS {
/** ByteStream converter for CmxTob.
 *
 *  @author alexander.mazurov@cern.ch
 */

class CmxRoIByteStreamxAODCnv: public Converter, public ::AthMessaging {

public:
  CmxRoIByteStreamxAODCnv(ISvcLocator* svcloc);

  virtual ~CmxRoIByteStreamxAODCnv(){};

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
};

} // end namespace

#endif
