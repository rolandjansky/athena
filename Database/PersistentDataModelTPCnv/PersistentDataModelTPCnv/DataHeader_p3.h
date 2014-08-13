/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODELTPCNV_DATAHEADER_P3_H
#define PERSISTENTDATAMODELTPCNV_DATAHEADER_P3_H

/** @file DataHeader_p3.h
 *  @brief This file contains the class definition for the DataHeader_p3 and DataHeaderElement_p3 classes.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeader_p3.h,v 1.4 2009-04-08 19:13:19 gemmeren Exp $
 **/

#include <vector>
#include <string>
#include <stdint.h>

/** @class DataHeaderElement_p3
 *  @brief This class provides a persistent representation for the DataHeaderElement class.
 **/
class DataHeaderElement_p3 {
public: // Constructor and Destructor
   DataHeaderElement_p3();
   DataHeaderElement_p3(const DataHeaderElement_p3& rhs);
   virtual ~DataHeaderElement_p3();

   DataHeaderElement_p3& operator=(const DataHeaderElement_p3& rhs);

friend class DataHeaderCnv_p3;
friend class DataHeaderElementCnv_p3;

   const std::string& key() const;
   unsigned int pClid() const;
   const std::vector<unsigned int>& clids() const;
   const std::string& token() const;
   const std::vector<std::string> alias() const;
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

/** @class DataHeader_p3
 *  @brief This class provides a persistent representation for the DataHeader class.
 **/
class  DataHeader_p3 {
public: // Constructor and Destructor
   DataHeader_p3();
   DataHeader_p3(const DataHeader_p3& rhs);
   virtual ~DataHeader_p3();

   DataHeader_p3& operator=(const DataHeader_p3& rhs);

friend class DataHeaderCnv_p3;

   const std::vector<DataHeaderElement_p3>& elements() const;
   const std::vector<DataHeaderElement_p3>& inputElements() const;
   const std::vector<std::string>& GuidMap() const;

private:
   std::vector<DataHeaderElement_p3> m_DataHeader;
   std::vector<DataHeaderElement_p3> m_InputDataHeader;
   std::vector<std::string> m_GuidMap;
};

#endif
