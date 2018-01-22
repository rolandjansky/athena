/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModel/Placement.h"

#include <cstdio>
#include <cstring>

static const char* fmt_tech = "[TECH=%08X]";

Placement::Placement() : m_technology(0L), m_fileName(""), m_containerName("") {
}

const std::string Placement::toString() const {
   char text[128];
   std::string str = "[FILE=" + m_fileName + "][CONT=" + m_containerName + "]";
   sprintf(text, fmt_tech, m_technology);
   str += text;
   str += m_auxString;
   return str;
}

Placement& Placement::fromString(const std::string& source) {
   for (const char* p1 = source.c_str(); p1; p1 = ::strchr(++p1,'[')) {
      const char* p2 = ::strchr(p1, '=');
      const char* p3 = ::strchr(p1, ']');
      if (p2 != 0 && p3 != 0) {
         if (::strncmp("[FILE=", p1, 6) == 0) {
            char* p3mod = const_cast<char*>(p3);
            *p3mod = 0;
            m_fileName = p2 + 1;
            *p3mod = ']';
         } else if (::strncmp("[CONT=", p1, 6) == 0) {
            char* p3mod = const_cast<char*>(p3);
            *p3mod = 0;
            m_containerName = p2 + 1;
            *p3mod = ']';
         } else if (::strncmp(fmt_tech, p1, 6) == 0) {
            ::sscanf(p1, fmt_tech, &m_technology);
         } else {
            while (*(p2 + 1) == '[' && *(++p3) != 0 && *p3 != ']') {
               p3 = ::strchr(p3, ']');
            }
            char* p3mod = const_cast<char*>(p3);
            *p3mod = 0;
            m_auxString += p1;
            m_auxString += "]";
            *p3mod = ']';
         }
      }
   }
   return *this;
}
