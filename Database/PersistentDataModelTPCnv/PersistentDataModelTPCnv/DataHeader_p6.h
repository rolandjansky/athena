/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODELTPCNV_DATAHEADER_P6_H
#define PERSISTENTDATAMODELTPCNV_DATAHEADER_P6_H

/** @file DataHeader_p6.h
 *  @brief This file contains the class definition for the DataHeader_p6,
 *  DataHeaderForm_p6 and DataHeaderElement_p6 classes.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeader_p6.h,v 1.1 2009-04-21 21:48:34 gemmeren Exp $
 **/

#include "PersistentDataModel/Guid.h"

#include <vector>
#include <set>
#include <string>

/** @class DataHeaderElement_p6
 *  @brief This class provides a persistent representation for the DataHeaderElement class.
 **/
class DataHeaderElement_p6 {
public: // Constructor and Destructor
   DataHeaderElement_p6();
   DataHeaderElement_p6(const DataHeaderElement_p6& rhs);
   virtual ~DataHeaderElement_p6();

   DataHeaderElement_p6& operator=(const DataHeaderElement_p6& rhs);

friend class DataHeaderCnv_p6;
friend class DataHeaderElementCnv_p6;

   const std::string& token() const;
   unsigned long long oid1() const;
   unsigned long long oid2() const;
   void overwriteOid2(unsigned long long oid2);

private:
   std::string m_token;
   unsigned long long m_oid1;
   unsigned long long m_oid2;

   unsigned int m_dbIdx; // entry with GUID & technology
   unsigned int m_objIdx; // Will get entry with ClassId and transient CLID/key, direct clid for now...
};

/** @class DataHeaderForm_p6
 *  @brief This class provides storage for the constant fields of the persistent DataHeader class.
 **/
class  DataHeaderForm_p6 {
public:
   typedef std::pair<Guid, unsigned int> DbRecord;
   typedef std::pair<std::string, unsigned int> SgRecord;
   typedef std::pair<Guid, SgRecord> ObjRecord;
public: // Constructor and Destructor
   DataHeaderForm_p6();
   DataHeaderForm_p6(const DataHeaderForm_p6& rhs);
   virtual ~DataHeaderForm_p6();

   DataHeaderForm_p6& operator=(const DataHeaderForm_p6& rhs);

friend class DataHeaderCnv_p6;

   unsigned int insertDb(const DbRecord& rec);
   std::size_t sizeDb() const;
   Guid getDbGuid(unsigned int index) const;
   unsigned int getDbTech(unsigned int index) const;

   unsigned int insertObj(const ObjRecord& rec,
	   const std::set<std::string>& alias = std::set<std::string>(),
	   const std::set<unsigned int>& symLinks = std::set<unsigned int>(),
	   const std::vector<unsigned int>& hashes = std::vector<unsigned int>());
   std::size_t sizeObj() const;
   std::string getObjKey(unsigned int index) const;
   unsigned int getObjType(unsigned int index) const;
   Guid getObjClassId(unsigned int index) const;
   std::set<std::string> getObjAlias(unsigned int index) const;
   std::set<unsigned int> getObjSymLinks(unsigned int index) const;
   std::vector<unsigned int> getObjHashes(unsigned int index) const;

   const std::vector<unsigned int>& params() const;
   void insertParam(unsigned int param);

   unsigned int entry() const;
   void start() const;
   void next() const;
   unsigned int size() const;
   void resize(unsigned int size);

private:
   std::vector<std::vector<unsigned int> > m_uints;
   std::vector<DbRecord> m_dbRecords;
   std::vector<ObjRecord> m_objRecords;
   std::vector<std::vector<std::string> > m_objAlias;
   std::vector<std::vector<unsigned int> > m_objSymLinks;
   std::vector<std::vector<unsigned int> > m_objHashes;
   mutable unsigned int m_entry;
};

/** @class DataHeader_p6
 *  @brief This class provides a persistent representation for the DataHeader class.
 **/
class  DataHeader_p6 {
public: // Constructor and Destructor
   DataHeader_p6();
   DataHeader_p6(const DataHeader_p6& rhs);
   virtual ~DataHeader_p6();

   DataHeader_p6& operator=(const DataHeader_p6& rhs);

friend class DataHeaderCnv_p6;

   const std::vector<DataHeaderElement_p6>& elements() const;
   const DataHeaderForm_p6& dhForm() const;
   void setDhForm(const DataHeaderForm_p6& form);
   const std::string& dhFormToken() const;
   void setDhFormToken(const std::string& formToken);
   void calculateDhFormMdx();
   const std::string& dhFormMdx() const;

private:
   std::vector<DataHeaderElement_p6> m_dataHeader;
   unsigned int m_provenanceSize;
   DataHeaderForm_p6 m_dhForm;
   std::string m_dhFormToken;
   std::string m_dhFormMdx;
};

#endif
