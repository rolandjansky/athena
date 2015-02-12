/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMBYTESTREAMAUXCNV_H
#define TRIGT1CALOBYTESTREAM_PPMBYTESTREAMXAUXCNV_H

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

// Externals
extern long ByteStream_StorageType;


namespace LVL1BS {
class L1CaloByteStreamReadTool;

/** ByteStream converter for Pre-processor Module DAQ data / TriggerTowers.
 *
 *  @author alexander.mazurov@cern.ch
 */

class PpmByteStreamAuxCnv: public Converter, public ::AthMessaging {

  friend class CnvFactory<PpmByteStreamAuxCnv>;

protected:

  PpmByteStreamAuxCnv(ISvcLocator* svcloc);

public:

  virtual ~PpmByteStreamAuxCnv(){};

  virtual StatusCode initialize();
  /// Create TriggerTowers from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
  /// Create ByteStream from TriggerTowers
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  //  Storage type and class ID
  virtual long repSvcType() const { return ByteStream_StorageType;}
  static  long storageType(){ return ByteStream_StorageType; }

  static const CLID& classID();
private:
 
private:

  /// Converter name
  std::string m_name;

  /// Do the main job - retrieve xAOD TriggerTowers from robs
  ToolHandle<L1CaloByteStreamReadTool> m_readTool;
};



} // end namespace
#endif
