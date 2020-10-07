/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//	====================================================================
//
//	Guid.cpp
//	--------------------------------------------------------------------
//
//	Package   : Persistent Guid to identify objects in the persistent
//              world.
//
//	Author    : Markus Frank
//      
//	====================================================================
#define POOL_GUID_CPP 1
//$Id: v5_Guid.cxx 732100 2016-03-24 05:58:02Z salnikov $
#include "v5_Guid.h"
#include <cstdio>
#include <cstring>

#include "uuid/uuid.h"

static const char* const fmt_Guid = 
  "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX";

//{ 0x0,0x0,0x0,{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}};
static const offline_poolCopy_v5::Guid
  clid_null(std::string("00000000-0000-0000-0000-000000000000"));

const offline_poolCopy_v5::Guid& offline_poolCopy_v5::Guid::null()   {
  return clid_null;
}

/// Create a new Guid
void offline_poolCopy_v5::Guid::create(Guid& guid)   {
  uuid_t me_;
  ::uuid_generate_time(me_);
  unsigned int  *d1=(unsigned int*)me_;
  unsigned short *d2=(unsigned short*)(me_+4);
  unsigned short *d3=(unsigned short*)(me_+6);
  guid.Data1 = *d1;
  guid.Data2 = *d2;
  guid.Data3 = *d3;
  for (int i=0; i<8; i++){
	  guid.Data4[i]=me_[i+8];
  }
}

const std::string offline_poolCopy_v5::Guid::toString()   const   {
  char text[128];
  ::snprintf(text, 128, fmt_Guid,
            static_cast<long unsigned int>(Data1),
            Data2, Data3, 
            Data4[0], Data4[1], Data4[2], Data4[3], 
            Data4[4], Data4[5], Data4[6], Data4[7]);
  return text;
}

const offline_poolCopy_v5::Guid& offline_poolCopy_v5::Guid::fromString(const std::string& source)    {
  // Note: This looks funny, but the specs for sscanf formats say
  //       that the space of a pointer in the ellipsis may only be
  //       integer or short. Hence one has to reserve a bit more space
  //       otherwise the stack gets corrupted.
  unsigned char d[8];
  long unsigned int xData1;
  ::sscanf( source.c_str(), fmt_Guid, &xData1, &Data2, &Data3, 
            &Data4[0], &Data4[1], &Data4[2], &Data4[3], &d[0], &d[1], &d[2], &d[3]);
  Data1 = xData1;
  //*(int*)&Data4[4] = *(int*)d;
  ::memcpy(Data4+4, d, 4);
  return *this;
}

bool offline_poolCopy_v5::Guid::operator < (const Guid& g)  const  {
  return ::memcmp(&g.Data1, &Data1, 16) < 0;
}
