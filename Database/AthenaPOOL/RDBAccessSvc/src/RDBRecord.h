/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RDBRecord.h
 *
 * @brief Definition of RDBRecord class
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: RDBRecord.h,v 1.8 2006-10-23 15:24:06 tsulaia Exp $
 */

#ifndef _RDBRECORD_H_
#define _RDBRECORD_H_

#include "CoralBase/AttributeList.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include <map>
#include <iosfwd>

namespace coral
{
  class ICursor;
}

/**
 * @class RDBRecord
 *
 * @brief RDBRecord is one record in the RDBRecordset object
 */
class RDBRecord final : public IRDBRecord
{
  friend class RDBRecordset;

 public:

  RDBRecord (const RDBRecord&) = delete;
  RDBRecord& operator= (const RDBRecord&) = delete;

  /// Destructor
  ~RDBRecord() override;

  /// Check if the field value is NULL
  /// @param fieldName [IN] field name
  /// @retun TRUE if the field is NULL, FALSE otherwise
  bool isFieldNull(const std::string& fieldName) const override;

  /// Get int field value
  /// @param fieldName [IN] field name
  /// @return field value
  int getInt(const std::string& fieldName) const override;

  /// Get long field value
  /// @param fieldName [IN] field name
  /// @return field value
  long getLong(const std::string& fieldName) const override;

  /// Get double field value
  /// @param fieldName [IN] field name
  /// @return field value
  double getDouble(const std::string& fieldName) const override;

  /// Get float field value
  /// @param fieldName [IN] field name
  /// @return field value
  float getFloat(const std::string& fieldName) const override;

  /// Get string field value
  /// @param fieldName [IN] field name
  /// @return field value
  std::string getString(const std::string& fieldName) const override;

  // Access array values by index
  // arrays are implemented using the field with names like NAME_0, NAME_1 etc.

  /// Get array int field value
  /// @param fieldName [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  int getInt(const std::string& fieldName, unsigned int index) const override;

  /// Get array long field value
  /// @param fieldName [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  long getLong(const std::string& fieldName, unsigned int index) const override;

  /// Get array double field value
  /// @param fieldName [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  double getDouble(const std::string& fieldName, unsigned int index) const override;

  /// Get array float field value
  /// @param fieldName [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  float getFloat(const std::string& fieldName, unsigned int index) const override;

  /// Get array string field value
  /// @param fieldName [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  std::string getString(const std::string& fieldName, unsigned int index) const override;

  // Comparison operator
  bool operator!=(const RDBRecord& rhs) const;

  // Dump record to output stream
  std::ostream& toOutputStream(std::ostream& os) const;

 protected:
  /// Constructor used by RDBRecordset class
  /// @param attList [IN] pointer to the attribute list, source of the Record object
  /// @param cursor [IN] need to check if the field values are NULL
  /// @param tableName [IN] data table name in the database
  RDBRecord(const coral::AttributeList& attList, std::string tableName);

 private:
  typedef std::map<std::string, unsigned int, std::less<std::string> > FieldName2ListIndex;

  /// Empty private constructor
  RDBRecord(){}

  FieldName2ListIndex m_name2Index;
  coral::AttributeList* m_values;

  std::string   m_tableName;

};

inline std::ostream& operator << (std::ostream& os, const RDBRecord& x)
{
  x.toOutputStream(os);
  return os;
}


#endif

