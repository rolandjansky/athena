/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
//class DataObject;
//class StatusCode;
//class IAddressCreator;
class ByteStreamCnvSvc;
class IROBDataProviderSvc;
class StoreGateSvc;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

// Externals
extern long ByteStream_StorageType;

/** @class EventInfoByteStreamCnv
 *  @brief This class implements the ByteStream Converter for EventInfo.
 *  EventInfo is built from RawEvent when reading and does nothing when writing ByteStream
 **/
class EventInfoByteStreamCnv : public Converter {
   friend class CnvFactory<EventInfoByteStreamCnv>;

protected:
   /// Constructor
   EventInfoByteStreamCnv(ISvcLocator* svcloc);

public:
   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// converter method to create object
   virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
   /// converter method to write object
   virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

   /// Storage type and class ID
   virtual long repSvcType() const    { return ByteStream_StorageType; }
   static long storageType()          { return ByteStream_StorageType; }
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
