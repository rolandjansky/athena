/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RDBRecordset.h
 *
 * @brief Definition of RDBRecordset class
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: RDBRecordset.h,v 1.11 2006-11-28 22:39:26 tsulaia Exp $
 */


#ifndef _RDBRECORDSET_H_
#define _RDBRECORDSET_H_

#include "RDBAccessSvc/IRDBRecordset.h"
#include <string>

class IRDBQuery;

namespace coral
{
  class ISessionProxy;
}

/**
 * @class RDBRecordset
 *
 * @brief RDBRecordset is an implementation of IRDBRecordset interface
 *
 */

class RDBAccessSvc;

#include "RDBVersionAccessor.h" 

class RDBRecordset final: public IRDBRecordset
{
 public:
  /// Construct empty recordset
  RDBRecordset(RDBAccessSvc* accessSvc);
  
  /// Destructor, deletes all records
  ~RDBRecordset() override;

  /// Constructs SQL query and retrieves the data from DB
  /// @param session [IN] active relational session
  /// @param taginfo [IN] object holding information about the node tag
  void getData(coral::ISessionProxy* session,
	       const std::string& nodeName,
	       const std::string& tagName,
	       const std::string& tagId);

  /// @return number of records
  unsigned int size() const override;

  /// @return node name
  std::string nodeName() const override;

  /// @return tag name
  std::string tagName() const override;

  /// @param index [IN] index of the record
  /// @return RDBRecord by index
  const IRDBRecord* operator[](unsigned int index) const override;

  /// @return begin iterator
  IRDBRecordset::const_iterator begin() const override;

  /// @return end iterator
  IRDBRecordset::const_iterator end() const override;

  // Comparison operator
  bool operator!=(const RDBRecordset& rhs) const;

  // Comparison results with other recordset
  void compare(const RDBRecordset& rec, std::ostream& os) const;

  // Set table name only when retrieval of data has failed and
  // an empty recordset needs to be returned to the client
  void setNodeName(const std::string& nodeName);

 private:
  std::string                m_tableName;
  std::string                m_tagName;
  RecordsVector              m_records;
  RDBAccessSvc*              m_accessSvc;
};

#endif
