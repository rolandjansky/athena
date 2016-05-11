/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* file contains the implementation for the AthenaRootSerializeSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaRootSerializeSvc.h"
#include "DataModelRoot/RootType.h"

#include "StorageSvc/DbReflex.h"
#include "TBufferFile.h"

//___________________________________________________________________________
AthenaRootSerializeSvc::AthenaRootSerializeSvc(const std::string& name,
	ISvcLocator* pSvcLocator) : AthService(name, pSvcLocator) {
}

//___________________________________________________________________________
AthenaRootSerializeSvc::~AthenaRootSerializeSvc() {
}

//___________________________________________________________________________
StatusCode AthenaRootSerializeSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaRootSerializeSvc::finalize() {
   ATH_MSG_INFO("in finalize()");
   return(::AthService::finalize());
}

//___________________________________________________________________________
StatusCode AthenaRootSerializeSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if ( IAthenaSerializeSvc::interfaceID().versionMatch(riid) ) {
      *ppvInterface = (IAthenaSerializeSvc*)this;
   } else {
      // Interface is not directly available: try out a base class
      return(AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
void* AthenaRootSerializeSvc::serialize(const void* /*object*/, const std::string& /*name*/, size_t& /*nbytes*/) {
   return(0);
}

//___________________________________________________________________________
void* AthenaRootSerializeSvc::serialize(const void* object, const Guid& id, size_t& nbytes) {
   RootType cltype(pool::DbReflex::forGuid(id));
   TBufferFile writeBuffer(TBuffer::kWrite);
   writeBuffer.WriteObjectAny(object, cltype);
   void* buffer = writeBuffer.Buffer();
   nbytes = writeBuffer.Length();
   writeBuffer.ResetBit(TBuffer::kIsOwner); writeBuffer.SetBuffer(0);
   return(buffer);
}

//___________________________________________________________________________
void* AthenaRootSerializeSvc::deserialize(void* /*buffer*/, size_t /*nbytes*/, const std::string& /*name*/) {
   return(0);
}

//___________________________________________________________________________
void* AthenaRootSerializeSvc::deserialize(void* buffer, size_t nbytes, const Guid& id) {
   RootType cltype(pool::DbReflex::forGuid(id));
   TBufferFile readBuffer(TBuffer::kRead, nbytes, buffer, kTRUE);
   void* obj = readBuffer.ReadObjectAny(cltype);
   return(obj);
}
