/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODEL_TOKEN_H
#define PERSISTENTDATAMODEL_TOKEN_H

/** @file Token.h
 *  @brief This file contains the class definition for the Token class (migrated from POOL).
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "PersistentDataModel/Guid.h"

#include <string>
#include <utility>

/** @class Token
 *  @brief This class provides a token that identifies in a unique way objects on the persistent storage.
 **/
class Token   {
public:
   // typedef std::pair<long long int, long long int> OID_t;
   struct OID_t : public std::pair<long long int, long long int> {
     OID_t() {}
     OID_t(long long int i1, long long int i2) : std::pair<long long int, long long int> (i1,i2) {}
   };

private:
   /// No copy allowed: put prototype to disable bit-wise copy.
   Token(const Token& copy);
   /// No assignment allowed: put prototype to disable bit-wise assignment.
   Token& operator=(const Token& copy);

public:
   /// Standard Constructor
   Token();
   /// Constructor with data assignment
   explicit Token(const Token* source);
   /// Standard destructor: release all allocated resources.
   virtual ~Token();
   /// Operator < to allow ordering
   bool operator<(const Token& refToken) const { return less(refToken); }
   /// Operator > to allow ordering
   bool operator>(const Token& refToken) const { 
      if (less(refToken)) return false;
      return !equal(refToken);
   }
   /// Equality operator
   bool operator==(const Token& refToken) const { return equal(refToken); }
   /// Equality operator
   bool operator!=(const Token& refToken) const { return !equal(refToken); }
   /// Fast token comparison: operator less
   virtual bool less(const Token& pTok) const;
   /// Fast token comparison: operator equals
   virtual bool equal(const Token& pTok) const;
   /// Release token: Decrease reference count and eventually delete.
   int release();
   /// Increase reference count
   int addRef() { return ++m_refCount; }
   /// Access database identifier
   const Guid& dbID() const { return m_dbID; }
   /// Set database name
   Token& setDb(const Guid& db) { m_dbID = db; return *this; }
   Token& setDb(const std::string& db) { m_dbID.fromString(db); return *this; }
   /// Access container identifier
   const std::string& contID() const { return m_cntID; }
   /// Set container name
   Token& setCont(const std::string& cnt) { m_cntID = cnt; return *this; }
   /// Access database identifier
   const Guid& classID() const { return m_classID; }
   /// Access database identifier
   Token& setClassID(const Guid& cl_id) { m_classID = cl_id; return *this; }
   /// Access technology type
   int technology() const { return m_technology; }
   /// Set technology type
   Token& setTechnology(int t) { m_technology = t; return *this; }
   /// Access object identifier
   const OID_t& oid() const { return m_oid; }
   /// Access object identifier
   OID_t& oid() { return m_oid; }
   /// Set object identifier
   Token& setOid(const OID_t& oid) { m_oid = oid; return *this; }
   /// Access token type
   int type() const { return m_type; }
   /// Set token type
   Token& setType(int t) { m_type = t; return *this; }
   /// Access auxiliary string
   const std::string& auxString() const { return m_auxString; }
   /// Set auxiliary string
   Token& setAuxString(const std::string& auxString) { m_auxString = auxString; return *this; }

   /// Retrieve the string representation of the token.
   virtual const std::string toString() const;
   /// Retrieve token key
   virtual const std::string key() const;
   /// Build from the string representation of a token.
   Token& fromString(const std::string& from);
   /// Set token information
   const Token& set(Token* pToken) const;
   /// Set all the data part of the token
   const Token& setData(Token* pToken) const;

   /// expose Token instance counter for debugging
   static int numInstances();

private:
   /// Reference count
   int m_refCount;
   /// Technology identifier
   unsigned int m_technology;
   /// Database identifier
   Guid m_dbID;
   /// Container identifier
   std::string m_cntID;
   /// Object global identifier
   Guid m_classID;
   /// Persistent object identifier
   OID_t m_oid;
   /// Token type
   int m_type;
   /// Auxiliary string
   std::string m_auxString;
};

#endif
