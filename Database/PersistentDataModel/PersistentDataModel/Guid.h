/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODEL_GUID_H
#define PERSISTENTDATAMODEL_GUID_H

/** @file Guid.h
 *  @brief This file contains the class definition for the Guid class (migrated from POOL).
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include <iostream> // for std::ostream
#include <string>

/** @class Guid
 *  @brief This class provides a encapsulation of a GUID/UUID/CLSID/IID data structure (128 bit number).
 **/
class Guid {
public:
   /// Standard constructor
   Guid() : m_data1(0U), m_data2(0U), m_data3(0U), m_data4() {}
   /// Standard constructor (With possible initialization)
   explicit Guid(bool assign) : Guid() { if (assign) create(*this); }
   /// Constructor for Guid from char*
   explicit Guid(const char* s) { fromString(s); }
   /// Constructor for Guid from string
   explicit Guid(const std::string& s) { fromString(s); }
   /// Copy constructor
   Guid(const Guid& c) { *this = c; }
   /// Assignment operator
   Guid& operator=(const Guid& g) {
      if (this != &g) {
         m_data1 = g.m_data1;
         m_data2 = g.m_data2;
         m_data3 = g.m_data3;
         unsigned int* p = (unsigned int*)m_data4;
         const unsigned int* q = (const unsigned int*)g.m_data4;
         *p++ = *q++; *p = *q;
      }
      return *this;
   }
   /// Smaller operator
   bool operator<(const Guid& g) const;
   /// Equality operator
   bool operator==(const Guid& g) const {
      if (this != & g)  {
         if (m_data1 != g.m_data1 ) return false;
         if (m_data2 != g.m_data2 ) return false;
         if (m_data3 != g.m_data3 ) return false;
         const unsigned int* p = (const unsigned int*)m_data4, *q = (const unsigned int*)g.m_data4;
         return *p++ == *q++ && *p == *q;
      }
      return true;
   }
   bool operator==(const std::string& g) const { return (this->toString() == g); }
   /// Non-equality operator
   bool operator!=(const Guid& g) const { return !(this->operator==(g)); }
   bool operator!=(const std::string& g) const { return !(this->operator==(g)); }

   /// Automatic conversion to string representation
   const std::string toString() const;
   /// Automatic conversion from string representation 
   const Guid& fromString(const std::string& s);
   /// NULL-Guid: static class method
   static const Guid& null();

   enum GuidGenMethod { GuidGenDefault, GuidGenRandom, GuidGenByTime };
   static GuidGenMethod m_guidGenMethod;
   /// Checks for POOL_GUID_TIME or POOL_GUID_RANDOM env variables
   static GuidGenMethod initGuidGenMethod();
   /// Set method of GUID generation to time or random based
   static void setGuidGeneration(GuidGenMethod method) { m_guidGenMethod = method; }

   /// Create a new Guid
   /// default method is currently Random, can be changed by param, API or environment
   static void create(Guid& guid, GuidGenMethod method = GuidGenDefault);

   /// Allow accessors to member data
   unsigned int data1() const { return m_data1; }
   unsigned short data2() const { return m_data2; }
   unsigned short data3() const { return m_data3; }
   unsigned char data4(unsigned int i) const { if (i < 8) return m_data4[i]; return 0; }

   /// Allow modifiers for member data
   void setData1(unsigned int data) { m_data1 = data; }
   void setData2(unsigned short data) { m_data2 = data; }
   void setData3(unsigned short data) { m_data3 = data; }
   void setData4(unsigned char data, unsigned int i) { if (i < 8) m_data4[i] = data; }

   /// Equality operator
   friend bool operator==(const std::string& str, const Guid& rhs) { return (rhs.operator==(str)); }
   /// Non-equality operator
   friend bool operator!=(const std::string& str, const Guid& rhs) { return !(rhs.operator==(str)); }
   /// Extraction operators
   friend std::ostream& operator<<(std::ostream& os, const Guid& rhs) { os << rhs.toString(); return os; }

private:
   unsigned int m_data1;
   unsigned short m_data2;
   unsigned short m_data3;
   unsigned char m_data4[8];
};

#endif
