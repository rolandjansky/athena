/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODELTPCNV_DATAHEADER_P6_H
#define PERSISTENTDATAMODELTPCNV_DATAHEADER_P6_H

/** @file DataHeader_p6.h
 *  @brief This file contains the class definitions for the
 *  DataHeader_p6 and DataHeaderForm_p6 
 *  @author Peter van Gemmeren <gemmeren@anl.gov>, Marcin Nowak
 **/

#include "PersistentDataModel/Guid.h"

#include <vector>
#include <set>
#include <string>

class Token;


/** @class DataHeaderForm_p6
 *  @brief This class provides storage for the constant fields of the persistent DataHeader class.
 **/
class  DataHeaderForm_p6 {
friend class DataHeaderCnv_p6;

public:
  struct DbRecord {
    Guid fid; unsigned tech;
    DbRecord() {}
    DbRecord( const Guid& f, unsigned t) : fid(f), tech(t) {}
    bool operator==(const DbRecord& rhs) const { return fid==rhs.fid && tech==rhs.tech; }
  };
  struct ObjRecord {
     Guid guid;
     std::string cont, key;
     unsigned clid; long long oid1;
    ObjRecord() {}
    ObjRecord( const Guid& g, const std::string& c, const std::string& k, unsigned id, long long o)
       : guid(g), cont(c), key(k), clid(id), oid1(o) {}
    bool operator==(const ObjRecord& rhs) const
      { return clid == rhs.clid && cont == rhs.cont && key == rhs.key && oid1 == rhs.oid1; }
  };

public: // Constructor and Destructor
   DataHeaderForm_p6() {}
   DataHeaderForm_p6(const DataHeaderForm_p6& rhs);
   DataHeaderForm_p6& operator=(const DataHeaderForm_p6& rhs);
   ~DataHeaderForm_p6();

   unsigned int insertDb(const DbRecord& rec);
   std::size_t sizeDb() const;
   Guid getDbGuid(unsigned int index) const;
   unsigned int getDbTech(unsigned int index) const;

   unsigned int insertObj(const ObjRecord& rec,
	   const std::set<std::string>& alias = std::set<std::string>(),
	   const std::set<unsigned int>& symLinks = std::set<unsigned int>(),
	   const std::vector<unsigned int>& hashes = std::vector<unsigned int>());
   std::size_t sizeObj() const;
   std::string getObjContainer(unsigned int index) const;
   std::string getObjKey(unsigned int index) const;
   unsigned int getObjType(unsigned int index) const;
   Guid getObjClassId(unsigned int index) const;
   long long getObjOid1(unsigned int index) const { return m_objRecords[index].oid1; }
   std::set<std::string>        getObjAlias(unsigned int index) const;
   std::set<unsigned int>       getObjSymLinks(unsigned int index) const;
   std::vector<unsigned int>    getObjHashes(unsigned int index) const;
   std::string          calculateMdx();
   bool                 wasModified() const;
   void                 clearModified();
   void                 setToken(const std::string& tok);
   const std::string&   getToken() const;
   void                 resize(unsigned int size);

private:
   std::vector<DbRecord> m_dbRecords;
   std::vector<ObjRecord> m_objRecords;
   std::vector<std::vector<std::string> > m_objAlias;
   std::vector<std::vector<unsigned int> > m_objSymLinks;
   std::vector<std::vector<unsigned int> > m_objHashes;

   /// In case we need ot handle encoding changes later
   unsigned             m_version { 600 };
  
   // transient members
   /// indicates that the last event was somehow different and a new DHForm needs to be written
   bool                 m_modified { true };
   /// Reference to self in the persistent storage
   std::string          m_token;
};


/** @class DataHeader_p6
 *  @brief This class provides a persistent representation for the DataHeader class.

    Version P6 optimized FOR references pointing to the same OID2 in the same Database
 **/
class  DataHeader_p6
{
  friend class DataHeaderCnv_p6;

public: 
  struct FullElement {
    FullElement() : oid2(0), dbIdx(0), objIdx(0) {}
    FullElement(unsigned long long o2, unsigned db, unsigned obj) : oid2(o2), dbIdx(db), objIdx(obj) {}
    unsigned long long oid2;
    unsigned int dbIdx;  // index to DHForm entry with DB GUID & technology
    unsigned int objIdx; // index to DHForm entry with object data
  };

   DataHeader_p6() {};
   const std::string& dhFormToken() const;
   void setDhFormToken(const std::string& formToken);
  
private:
   /// common DB entry index used by all short DH elements
   unsigned                     m_commonDbIndex;
   unsigned long long           m_commonOID2;
  
   std::vector<int>             m_shortElements;
   std::vector<FullElement>     m_fullElements;
   unsigned int                 m_provenanceSize;

   std::string                  m_dhFormToken;
};

#endif





