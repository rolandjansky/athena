/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_L1CALOBYTESTREAMXAODCNV_H
#define TRIGT1CALOBYTESTREAM_L1CALOBYTESTREAMXAODCNV_H

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
class L1CaloByteStreamReadTool;

/** ByteStream converter for Pre-processor Module DAQ data / TriggerTowers.
 *
 *  @author alexander.mazurov@cern.ch
 */

template<typename ContainerT, typename AuxContainerT>
class L1CaloByteStreamAuxCnv: public Converter, public ::AthMessaging {

public:
  L1CaloByteStreamAuxCnv(ISvcLocator* svcloc);

  virtual ~L1CaloByteStreamAuxCnv(){};

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

  /// Do the main job - retrieve xAOD TriggerTowers from robs
  ToolHandle<L1CaloByteStreamReadTool> m_readTool;
};



} // end namespace

#include "L1CaloByteStreamAuxCnv.icc"

#endif
