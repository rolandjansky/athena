/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* file contains the implementation for the AthenaRootSerializeSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaRootSerializeSvc.h"
#include "DataModelRoot/RootType.h"

#include "StorageSvc/DbReflex.h"
#include "TBufferFile.h"
#include "TClass.h"

//___________________________________________________________________________
AthenaRootSerializeSvc::AthenaRootSerializeSvc(const std::string& name,
	ISvcLocator* pSvcLocator) : AthService(name, pSvcLocator) {
}

//___________________________________________________________________________
AthenaRootSerializeSvc::~AthenaRootSerializeSvc() {
}

//___________________________________________________________________________
StatusCode AthenaRootSerializeSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name());

   // Load message handler dictionary to avoid crashes when reporting
   // possible errors during event processing (ATR-25049)
   TClass::GetClass("TMessageHandler");

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
void* AthenaRootSerializeSvc::serialize(const void* /*object*/, const std::string& /*name*/, size_t& /*nbytes*/) const {
   return(nullptr);
}

//___________________________________________________________________________
void* AthenaRootSerializeSvc::serialize(const void* object, const Guid& id, size_t& nbytes) const {
   return(this->serialize(object, pool::DbReflex::forGuid(id), nbytes));
}

//___________________________________________________________________________
void* AthenaRootSerializeSvc::serialize(const void* object, const RootType& cltype, size_t& nbytes) const {
   TBufferFile writeBuffer(TBuffer::kWrite);
   writeBuffer.WriteObjectAny(object, cltype);
   void* buffer = writeBuffer.Buffer();
   nbytes = writeBuffer.Length();
   writeBuffer.ResetBit(TBuffer::kIsOwner); writeBuffer.SetBuffer(nullptr);
   return(buffer);
}

//___________________________________________________________________________
void* AthenaRootSerializeSvc::deserialize(void* /*buffer*/, size_t& /*nbytes*/, const std::string& /*name*/) const {
   return(nullptr);
}

//___________________________________________________________________________
void* AthenaRootSerializeSvc::deserialize(void* buffer, size_t& nbytes, const Guid& id) const {
   return(this->deserialize(buffer, nbytes, pool::DbReflex::forGuid(id)));
}

//___________________________________________________________________________
void* AthenaRootSerializeSvc::deserialize(void* buffer, size_t& nbytes, const RootType& cltype) const {
   void* obj = nullptr;
   if (cltype.IsFundamental()) {
      obj = new char[nbytes];
      std::memcpy(obj, buffer, nbytes);
   } else if (cltype.Name() == "Token") { // Token is C string
      obj = new char[nbytes];
      std::memcpy(obj, buffer, nbytes); static_cast<char*>(obj)[nbytes - 1] = 0;
   } else {
      TBufferFile readBuffer(TBuffer::kRead, nbytes, buffer, kTRUE);
      obj = readBuffer.ReadObjectAny(cltype);
      nbytes = readBuffer.Length();
      readBuffer.ResetBit(TBuffer::kIsOwner); readBuffer.SetBuffer(nullptr);
   }
   return(obj);
}
