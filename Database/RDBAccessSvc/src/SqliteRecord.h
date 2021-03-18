/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SqliteRecord.h
 *
 * @brief Declaration of the SqliteRecord class
 *
 */

#ifndef RDBACCESSSVC_SQLITERECORD_H
#define RDBACCESSSVC_SQLITERECORD_H

#include "RDBAccessSvc/IRDBRecord.h"
#include <map>
#include <memory>
#include <variant>
#include <string>
#include <tuple>

enum SqliteInpType
{
  SQLITEINP_INT
  , SQLITEINP_LONG
  , SQLITEINP_FLOAT
  , SQLITEINP_DOUBLE
  , SQLITEINP_STRING
  , SQLITEINP_UNDEF
};

typedef std::variant<int
                     , long
                     , float
                     , double
                     , std::string> SqliteInp;

typedef std::map<std::string,SqliteInpType> SqliteInpDef;
typedef std::shared_ptr<SqliteInpDef> SqliteInpDef_ptr;

/**
 * @class SqliteRecord
 *
 * @brief SqliteRecord is one record in the SqliteRecordset object
 */

class SqliteRecord final : public IRDBRecord
{
 public:
  SqliteRecord(SqliteInpDef_ptr def);
  SqliteRecord() = delete;
  SqliteRecord (const SqliteRecord&) = delete;
  SqliteRecord& operator= (const SqliteRecord&) = delete;

  /// Destructor
  ~SqliteRecord() override;

  /// Check if the field value is NULL
  /// @param field [IN] field name
  /// @retun TRUE if the field is NULL, FALSE otherwise
  bool isFieldNull(const std::string& field) const override;

  /// Get int field value
  /// @param field [IN] field name
  /// @return field value
  int getInt(const std::string& field) const override;

  /// Get long field value
  /// @param field [IN] field name
  /// @return field value
  long getLong(const std::string& field) const override;

  /// Get double field value
  /// @param field [IN] field name
  /// @return field value
  double getDouble(const std::string& field) const override;

  /// Get float field value
  /// @param field [IN] field name
  /// @return field value
  float getFloat(const std::string& field) const override;

  /// Get string field value
  /// @param field [IN] field name
  /// @return field value
  std::string getString(const std::string& field) const override;

  // Access array values by index
  // arrays are implemented using the field with names like NAME_0, NAME_1 etc.

  /// Get array int field value
  /// @param field [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  int getInt(const std::string& field, unsigned int index) const override;

  /// Get array long field value
  /// @param field [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  long getLong(const std::string& field, unsigned int index) const override;

  /// Get array double field value
  /// @param field [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  double getDouble(const std::string& field, unsigned int index) const override;

  /// Get array float field value
  /// @param field [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  float getFloat(const std::string& field, unsigned int index) const override;

  /// Get array string field value
  /// @param field [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  std::string getString(const std::string& field, unsigned int index) const override;

  /// Dump to cout
  void dump() const;

  void addValue(const std::string& field
		, SqliteInp value);

 private:
  typedef std::map<std::string,SqliteInp> Record;
  typedef Record::const_iterator RecordCIterator;
  enum FieldCheckCode
  {
    FIELD_CHECK_OK
    , FIELD_CHECK_BAD_NAME
    , FIELD_CHECK_BAD_TYPE
    , FIELD_CHECK_NULL_VAL
  };
  typedef std::tuple<RecordCIterator,FieldCheckCode> FieldCheckResult;

  SqliteInpDef_ptr m_def;
  Record           m_record;

  FieldCheckResult checkField(const std::string& field
			      , SqliteInpType fieldType) const;
  void             handleError(const std::string& field
			       ,FieldCheckCode checkCode) const;
};

inline SqliteRecord::FieldCheckResult SqliteRecord::checkField(const std::string& field
							       , SqliteInpType fieldType) const
{
  FieldCheckCode checkCode{FIELD_CHECK_OK};
  RecordCIterator checkIt{m_record.end()};

  auto defIt = m_def->find(field);
  if(defIt==m_def->end()) checkCode = FIELD_CHECK_BAD_NAME;
  if(defIt->second!=fieldType) checkCode = FIELD_CHECK_BAD_TYPE;

  if(checkCode==FIELD_CHECK_OK) {
    checkIt = m_record.find(field);
    if(checkIt==m_record.end()) {
      checkCode = FIELD_CHECK_NULL_VAL;
    }
  }
  return std::make_tuple(checkIt,checkCode);
}

inline void SqliteRecord::handleError(const std::string& field
				      ,FieldCheckCode checkCode) const
{
  switch(checkCode) {
  case FIELD_CHECK_BAD_NAME:
    throw std::runtime_error( "Wrong name for the field " + field);
  case FIELD_CHECK_BAD_TYPE:
    throw std::runtime_error( "Wrong data type requested for the field " + field);
  case FIELD_CHECK_NULL_VAL:
    throw std::runtime_error( field + " is NULL");
  default:
    break;
  }
  return;
}

#endif

