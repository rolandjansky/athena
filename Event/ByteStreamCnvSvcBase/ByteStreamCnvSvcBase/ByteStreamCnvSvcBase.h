/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVCBASE_BYTESTREAMCNVSVCBASE_H
#define BYTESTREAMCNVSVCBASE_BYTESTREAMCNVSVCBASE_H

#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthCnvSvc.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamData/RawEvent.h"

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

public:
   /// Standard Service Constructor
   ByteStreamCnvSvcBase(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor

   virtual ~ByteStreamCnvSvcBase();
   /// Required of all Gaudi Services
   virtual StatusCode initialize() override;

   /// Required of all Gaudi services:  see Gaudi documentation for details
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

   /// Checks whether an IOpaqueAddress is a GenericAddress
   virtual StatusCode updateServiceState(IOpaqueAddress* pAddress) override;

   /// Implementation of IByteStreamEventAccess: Get RawEvent
   virtual RawEventWrite* getRawEvent() override { return m_rawEventWrite; }

   /// Implementation of IIncidentListener: Handle for EndEvent incidence
   virtual void handle(const Incident&) override;

protected: // data
   RawEventWrite* m_rawEventWrite;

   std::vector<std::string> m_initCnvs;
   std::vector<std::string> m_ROD2ROBmap;
};

#endif
