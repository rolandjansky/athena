/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMBYTESTREAMAUXCNV_H
#define TRIGT1CALOBYTESTREAM_PPMBYTESTREAMAUXCNV_H

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



#include "ZdcByteStream/ZdcByteStreamReadV1V2Tool.h"

namespace LVL1BS {
class PpmByteStreamReadV1V2Tool;

/** ByteStream converter for Pre-processor Module DAQ data / TriggerTowers.
 *
 *  @author alexander.mazurov@cern.ch
 */

class PpmByteStreamAuxCnv: public Converter, public ::AthMessaging {

public:
  PpmByteStreamAuxCnv(ISvcLocator* svcloc);

  virtual ~PpmByteStreamAuxCnv(){};

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
 
private:

  /// Converter name
  std::string m_name;

  /// Do the main job - retrieve xAOD TriggerTowers from robs
  ToolHandle<PpmByteStreamReadV1V2Tool> m_readTool;
  ToolHandle<ZdcByteStreamReadV1V2Tool> m_zdcTool;
};



} // end namespace
#endif // not TRIGT1CALOBYTESTREAM_PPMBYTESTREAMAUXCNV_H
