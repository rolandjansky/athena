/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModel/Guid.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "uuid/uuid.h"

static const char* const fmt_Guid = "%08X-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX";

//{ 0x0,0x0,0x0,{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}};
static const Guid clid_null(std::string("00000000-0000-0000-0000-000000000000"));

const Guid& Guid::null() {
   return clid_null;
}

const Guid::GuidGenMethod Guid::m_guidGenMethod = Guid::initGuidGenMethod();

Guid::GuidGenMethod Guid::initGuidGenMethod() {
   char* envv = getenv("POOL_GUID_TIME");
   if (envv != 0 && *envv) return GuidGenByTime;
   envv = getenv("POOL_GUID_RANDOM");
   if (envv != 0 && *envv) return GuidGenRandom;
   return GuidGenDefault;
}

/// Create a new Guid
void Guid::create(Guid& guid, GuidGenMethod method) {
   uuid_t me_;
   if (method == GuidGenDefault) method = m_guidGenMethod;
   switch(method) {
    case GuidGenRandom:
      ::uuid_generate(me_);
      break;
    case GuidGenByTime:
      ::uuid_generate_time(me_);
      break;
    default:
      ::uuid_generate(me_);
      break;
   }
   unsigned int *d1 = (unsigned int*)me_;
   unsigned short *d2 = (unsigned short*)(me_ + 4);
   unsigned short *d3 = (unsigned short*)(me_ + 6);
   guid.m_data1 = *d1;
   guid.m_data2 = *d2;
   guid.m_data3 = *d3;
   for (unsigned int i = 0; i < 8; i++) {
      guid.m_data4[i] = me_[i + 8];
   }
}

const std::string Guid::toString() const {
   char text[128];
   ::sprintf(text, fmt_Guid, m_data1, m_data2, m_data3,
	   m_data4[0], m_data4[1], m_data4[2], m_data4[3], m_data4[4], m_data4[5], m_data4[6], m_data4[7]);
   return std::string(text);
}

const Guid& Guid::fromString(const std::string& source) {
   if (::sscanf(source.c_str(), fmt_Guid, &m_data1, &m_data2, &m_data3,
	   	&m_data4[0], &m_data4[1], &m_data4[2], &m_data4[3], &m_data4[4], &m_data4[5], &m_data4[6], &m_data4[7]) != 11) {
      m_data1 = 0U;
      m_data2 = 0U;
      m_data3 = 0U;
      for (unsigned int i = 0; i < 8; i++) {
         m_data4[i] = '\0';
      }
   }
   return *this;
}

bool Guid::operator<(const Guid& g) const {
   return ::memcmp(&g.m_data1, &m_data1, 16) < 0;
}
