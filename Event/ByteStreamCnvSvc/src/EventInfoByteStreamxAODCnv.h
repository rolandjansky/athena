/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOBYTESTREAMXAODCNV_H
#define EVENTINFOBYTESTREAMXAODCNV_H

/**
 * @file EventInfoByteStreamxAODCnv.h
 *
 * @class EventInfoByteStreamxAODCnv
 *
 * @brief This is the class definition of ByteStream converter for xAOD::EventInfo
 * Event Info is built from RawEvent when reading. Nothing is done when writing ByteStream
 *
 */ 

#include "GaudiKernel/Converter.h"
#include "AthenaBaseComps/AthMessaging.h"

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class EventInfoByteStreamxAODCnv : public Converter, public AthMessaging
{
 public:
  EventInfoByteStreamxAODCnv(ISvcLocator* svcloc);
  virtual ~EventInfoByteStreamxAODCnv() override {}

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  /// converter method to create object
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;
  /// converter method to write object
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType();
  static const CLID& classID();

};

#endif
