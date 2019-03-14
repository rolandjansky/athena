/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModel/Token.h"

#include <cstdio>
#include <cstring>
#include <climits>
#include <atomic>

static const char* const fmt_clid = "[CLID=";
static const char* const fmt_tech = "[TECH=%08X]";
static const char* const fmt_oid  = "[OID=%016llX-%016llX]";
static const char* const fmt_oid_old  = "[OID=%08llX-%08llX]";
static const int KEY_MASK = (~0u) << CHAR_BIT;
static std::atomic<int> s_numCount { 0 };

int Token::numInstances() { return s_numCount; }

/// Standard Constructor(1): Empty constructor
Token::Token() : m_refCount(1),
	m_technology(0),
	m_dbID(Guid::null()),
	m_classID(Guid::null()),
	m_oid(OID_t(~0x0LL, ~0x0LL)),
	m_type(0) {
   s_numCount++;
}

/// Copy constructor
Token::Token(const Token& copy) : m_refCount(1),
	m_technology(copy.m_technology),
	m_dbID(copy.m_dbID),
	m_classID(copy.m_classID),
	m_oid(copy.m_oid),
	m_type(0) {
   copy.setData(this);
   s_numCount++;
}

/// Copy constructor
Token::Token(const Token* source) : m_refCount(1),
	m_technology(0),
	m_dbID(Guid::null()),
	m_classID(Guid::null()),
	m_oid(OID_t(~0x0LL, ~0x0LL)),
	m_type(0) {
   if (source != 0) {
      source->setData(this);
   }
   s_numCount++;
}

Token::~Token() {
   s_numCount--;
}

/// Release token: Decrease reference count and eventually delete.
int Token::release() {
   int cnt = --m_refCount;
   if (0 >= cnt) {
      delete this;
   }
   return cnt;
}

/// Assignment operator
Token& Token::operator=(const Token& copy) {
  if (&copy != this) {
     copy.setData(this);
  }
  return *this;
}

/// Equality operator
bool Token::equal(const Token& copy) const {
   if (&copy != this) {
      if (m_oid.second == copy.m_oid.second) {
         if (m_classID == copy.m_classID) {
            if (m_dbID == copy.m_dbID) {
               if (m_cntID == copy.m_cntID) {
                  return true;
               }
            }
         }
      }
      return false;
   }
   return true;
}

/// Operator to allow ordering
bool Token::less(const Token& copy) const {
   if (&copy != this) {
      if (m_oid.second < copy.m_oid.second)
         return true;
      else if (m_oid.second > copy.m_oid.second)
         return false;
      if (!(m_classID == copy.m_classID)) {
         return (m_classID < copy.m_classID);
      }
      if (!(m_dbID == copy.m_dbID)) {
         return (m_dbID < copy.m_dbID);
      }
      int res = m_cntID.compare(copy.m_cntID);
      if (res != 0) {
         return (res < 0);
      }
   }
   return false;
}

const std::string Token::toString() const {
   char text1[128];
   sprintf(text1, fmt_tech, m_technology);
   char text2[128];
   sprintf(text2, fmt_oid, m_oid.first, m_oid.second);
   std::string str = "[DB=" + m_dbID.toString() + "][CNT=" + m_cntID + "][CLID=" + m_classID.toString() + "]" + text1 + text2 + m_auxString;
   return str;
}

Token& Token::fromString(const std::string& source)    {
   m_auxString.clear();
   for (const char* p1 = source.c_str(); p1; p1 = ::strchr(++p1,'[')) {
      const char* p2 = ::strchr(p1, '=');
      const char* p3 = ::strchr(p1, ']');
      if (p2 != 0 && p3 != 0) {
         if (::strncmp("[DB=", p1, 4) == 0)  {
            m_dbID.fromString(p1 + 4);
         } else if (::strncmp("[CNT=", p1, 5) == 0) {
            m_cntID.assign (p2+1, p3-p2-1);
         } else if (::strncmp(fmt_oid, p1, 5) == 0) {
            long long unsigned int first, second;
            if (::strncmp("]", p1 + 22, 1) == 0) { // 5 + 8(int) + 1(minus) + 8(int) = 22
               ::sscanf(p1, fmt_oid_old, &first, &second);
               if (int(first) == ~0x0) first = ~0x0LL;
               if (int(second) == ~0x0) second = ~0x0LL;
            } else {
               ::sscanf(p1, fmt_oid, &first, &second);
            }
            m_oid.first = first;
            m_oid.second = second;
         } else if (::strncmp(fmt_clid, p1, 6) == 0) {
            m_classID.fromString(p1 + 6);
         } else if (::strncmp(fmt_tech, p1, 6) == 0) {
            ::sscanf(p1, fmt_tech, &m_technology);
         } else {
            while (*(p2 + 1) == '[' && p3 && *(++p3) != 0 && *p3 != ']') {
               p3 = ::strchr(p3, ']');
            }
            if (!p3) p3 = source.c_str() + source.size();
            m_auxString.append (p1, p3-p1);
            m_auxString += "]";
         }
      }
   }
   return *this;
}

/// Retrieve the string representation of the token.
const std::string Token::key() const {
   char text[1024];
   ::sprintf(text, "][TECH=%08X]", m_technology & KEY_MASK);
   std::string k = "[DB=" + m_dbID.toString() + "][CNT=" + m_cntID + "][CLID=" + m_classID.toString() + text;
   return k;
}

const Token& Token::set(Token* pToken) const {
   pToken->m_technology = m_technology;
   pToken->m_dbID = m_dbID;
   pToken->m_cntID = m_cntID;
   pToken->m_classID = m_classID;
   pToken->m_oid.first = m_oid.first;
   return *this;
}

const Token& Token::setData(Token* pToken) const {
   this->set(pToken);
   pToken->m_oid.second = m_oid.second;
   return *this;
}
