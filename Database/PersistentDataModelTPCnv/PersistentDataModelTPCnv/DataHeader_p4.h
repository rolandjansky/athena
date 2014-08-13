/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODELTPCNV_DATAHEADER_P4_H
#define PERSISTENTDATAMODELTPCNV_DATAHEADER_P4_H

/** @file DataHeader_p4.h
 *  @brief This file contains the class definition for the DataHeader_p4 and DataHeaderElement_p4 classes.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeader_p4.h,v 1.1 2009-04-21 21:48:34 gemmeren Exp $
 **/

#include <vector>
#include <string>
#include <stdint.h>

/** @class DataHeaderElement_p4
 *  @brief This class provides a persistent representation for the DataHeaderElement class.
 **/
class DataHeaderElement_p4 {
public: // Constructor and Destructor
   DataHeaderElement_p4();
   DataHeaderElement_p4(const DataHeaderElement_p4& rhs);
   virtual ~DataHeaderElement_p4();

   DataHeaderElement_p4& operator=(const DataHeaderElement_p4& rhs);

friend class DataHeaderCnv_p4;
friend class DataHeaderElementCnv_p4;

   unsigned int pClid() const;
   const std::vector<unsigned int>& clids() const;
   const std::string& key() const;
   const std::vector<std::string>& alias() const;
   const std::string& token() const;
   unsigned int oid1() const;
   unsigned int oid2() const;
   void overwriteOid2(unsigned int oid2);

private:
   std::vector<unsigned int> m_clids;
   std::string m_token;
   std::vector<std::string> m_alias;
   unsigned int m_technology, m_oid1, m_oid2;
   unsigned short m_dbGuidIdx, m_classIdIdx, m_prefixIdx;
   unsigned short m_keyPos;
   std::vector<uint64_t> m_hashes;
};

/** @class DataHeader_p4
 *  @brief This class provides a persistent representation for the DataHeader class.
 **/
class  DataHeader_p4 {
public: // Constructor and Destructor
   DataHeader_p4();
   DataHeader_p4(const DataHeader_p4& rhs);
   virtual ~DataHeader_p4();

   DataHeader_p4& operator=(const DataHeader_p4& rhs);

friend class DataHeaderCnv_p4;

   const std::vector<DataHeaderElement_p4>& elements() const;
   unsigned int provenanceSize() const;
   const std::vector<std::string>& guidMap() const;

private:
   std::vector<DataHeaderElement_p4> m_dataHeader;
   unsigned int m_provSize;
   std::vector<std::string> m_guidMap;
};

#endif
