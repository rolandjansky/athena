/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOBYTESTREAMCNV_H
#define EVENTINFOBYTESTREAMCNV_H

/** @file EventInfoByteStreamCnv.h
 *  @brief This file contains the class definition for the EventInfoByteStreamCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ByteStreamData/RawEvent.h"

#include <string>

class IOpaqueAddress;
class ByteStreamCnvSvc;
class IROBDataProviderSvc;
class StoreGateSvc;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


/** @class EventInfoByteStreamCnv
 *  @brief This class implements the ByteStream Converter for EventInfo.
 *  EventInfo is built from RawEvent when reading and does nothing when writing ByteStream
 **/
class EventInfoByteStreamCnv : public Converter {

public:
   EventInfoByteStreamCnv(ISvcLocator* svcloc);

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

private:
   const char* ascTime(unsigned int t);    //!< convert timestamp to ascii time.
   ByteStreamCnvSvc* m_ByteStreamCnvSvc;   //!< pointer to BS CnvSvc
   ServiceHandle<IROBDataProviderSvc> m_robDataProvider; //!< RODDataProviderSvc handle
   ServiceHandle<StoreGateSvc> m_mdSvc;                  //!< TDS handle

   // flags for EventType
   bool m_isSimulation;
   bool m_isTestbeam;
   bool m_isCalibration;

   std::string m_userType;
};

#endif
