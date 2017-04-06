/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENASERIALIZESVC_H
#define ATHENAKERNEL_IATHENASERIALIZESVC_H

#include "GaudiKernel/IService.h"
#include "DataModelRoot/RootType.h"

class Guid;

static const InterfaceID IID_IAthenaSerializeSvc( "IAthenaSerializeSvc", 1, 0 );

class IAthenaSerializeSvc : virtual public ::IService {
public:
   static const InterfaceID& interfaceID() { return IID_IAthenaSerializeSvc; }
 
   virtual void* serialize(const void* object, const std::string& name, size_t& nbytes) const = 0;
   virtual void* serialize(const void* object, const Guid& id, size_t& nbytes) const = 0;
   virtual void* serialize(const void* object, const RootType& cltype, size_t& nbytes) const = 0;

   virtual void* deserialize(void* buffer, size_t& nbytes, const std::string& name) const = 0;
   virtual void* deserialize(void* buffer, size_t& nbytes, const Guid& id) const = 0;
   virtual void* deserialize(void* buffer, size_t& nbytes, const RootType& cltype) const = 0;
};

#endif
