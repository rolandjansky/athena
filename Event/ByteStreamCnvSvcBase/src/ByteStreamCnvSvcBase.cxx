/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "AthenaKernel/IClassIDSvc.h"
#include "StoreGate/StoreGate.h" 

//______________________________________________________________________________
ByteStreamCnvSvcBase::ByteStreamCnvSvcBase(const std::string& name, ISvcLocator* pSvcLocator) :
   ::AthCnvSvc(name, pSvcLocator, ByteStreamAddress::storageType()),
	m_rawEventWrite(0) {
   declareProperty("InitCnvs", m_initCnvs); 
   // This property is used by Tile BS converter, not by this class.
   declareProperty("ROD2ROBmap", m_ROD2ROBmap);
}
//______________________________________________________________________________
/// Standard Destructor
ByteStreamCnvSvcBase::~ByteStreamCnvSvcBase()   {
}
//______________________________________________________________________________
/// Initialize the service.
StatusCode ByteStreamCnvSvcBase::initialize()     {
   if (!::AthCnvSvc::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthCnvSvc base class.");
      return(StatusCode::FAILURE);
   }

   ServiceHandle<IIncidentSvc> incsvc("IncidentSvc", this->name());
   if (!incsvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get IncidentSvc.");
      return(StatusCode::FAILURE);
   }
   incsvc->addListener(this, "BeginRun", 0, false, true); // true for singleshot
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
StatusCode ByteStreamCnvSvcBase::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (IByteStreamEventAccess::interfaceID().versionMatch(riid)) {
      *ppvInterface = dynamic_cast<IByteStreamEventAccess*>(this);
   } else {
      // Interface is not directly available: try out a base class
      return(::AthCnvSvc::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode ByteStreamCnvSvcBase::updateServiceState(IOpaqueAddress* pAddress) {
   if (pAddress != 0) {
      GenericAddress* pAddr = dynamic_cast<GenericAddress*>(pAddress);
      if (pAddr != 0) {
         return(StatusCode::SUCCESS);
      }
   }
   return(StatusCode::FAILURE);
}
//______________________________________________________________________________
void ByteStreamCnvSvcBase::handle(const Incident& /*incident*/) {
   ServiceHandle<IClassIDSvc> clidSvc("ClassIDSvc", name());
   if (!clidSvc.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Cannot get ClassIDSvc.");
      return;
   }
   // Initialize the converters
   for (std::vector<std::string>::const_iterator it = m_initCnvs.begin(), it_e = m_initCnvs.end();
		   it != it_e; it++) {
      ATH_MSG_DEBUG("Accessing Converter for " << *it);
      CLID id;
      if (!clidSvc->getIDOfTypeName(*it, id).isSuccess()) {
         ATH_MSG_WARNING("Cannot get CLID for " << *it);
      } else {
         IConverter* cnv = converter(id);
         if (cnv == 0) {
	    ATH_MSG_WARNING("Cannot get converter for " << *it);
         } 
      }
   } 
   return;
}
