/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVCBASE_BYTESTREAMCNVSVCBASE_H
#define BYTESTREAMCNVSVCBASE_BYTESTREAMCNVSVCBASE_H

#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthCnvSvc.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamData/RawEvent.h"

template <class TYPE> class SvcFactory;

/**
  @class ByteStreamCnvSvcBase
  @brief base class for ByteStream conversion service.

  description
         This class is used as a conversion service in online HLT
	 and it is the base class for offline bytestream conversion service.
*/
class ByteStreamCnvSvcBase : public ::AthCnvSvc,
		public virtual IIncidentListener,
		public virtual IByteStreamEventAccess {
   // Allow the factory class access to the constructor
   friend class SvcFactory<ByteStreamCnvSvcBase>;

public:
   /// Required of all Gaudi Services
   StatusCode initialize();

   /// Required of all Gaudi services:  see Gaudi documentation for details
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Checks whether an IOpaqueAddress is a GenericAddress
   virtual StatusCode updateServiceState(IOpaqueAddress* pAddress);

   /// Implementation of IByteStreamEventAccess: Get RawEvent
   virtual RawEventWrite* getRawEvent() { return m_rawEventWrite; }

   /// Implementation of IIncidentListener: Handle for EndEvent incidence
   virtual void handle(const Incident&);

protected: // constructor and destructor
   /// Standard Service Constructor
   ByteStreamCnvSvcBase(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~ByteStreamCnvSvcBase();

protected: // data
   RawEventWrite* m_rawEventWrite;

   std::vector<std::string> m_initCnvs;
   std::vector<std::string> m_ROD2ROBmap;
};

#endif
