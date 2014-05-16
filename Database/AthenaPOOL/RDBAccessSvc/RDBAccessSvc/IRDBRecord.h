/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IRDBRecord.h
 *
 * @brief Definition of the abstract IRDBRecord interface
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: IRDBRecord.h,v 1.8 2006-10-23 15:24:05 tsulaia Exp $
 */


#ifndef _IRDBRECORD_H_
#define _IRDBRECORD_H_


#include <string>


/**
 * @class IRDBRecord
 *
 * @brief IRDBRecord is one record in the IRDBRecordset object
 */

class IRDBRecord
{
 public:
  virtual ~IRDBRecord() {}

  /// Check if the field value is NULL
  /// @param fieldName [IN] field name
  /// @retun TRUE if the field is NULL, FALSE otherwise
  virtual bool isFieldNull(const std::string& fieldName) const = 0;

  /// Get int field value
  /// @parem fieldName [IN] field name
  /// @return field value
  virtual int getInt(const std::string& fieldName) const = 0;

  /// Get long field value
  /// @parem fieldName [IN] field name
  /// @return field value
  virtual long getLong(const std::string& fieldName) const = 0;

  /// Get double field value
  /// @parem fieldName [IN] field name
  /// @return field value
  virtual double getDouble(const std::string& fieldName) const = 0;

  /// Get float field value
  /// @parem fieldName [IN] field name
  /// @return field value
  virtual float getFloat(const std::string& fieldName) const = 0;

  /// Get string field value
  /// @parem fieldName [IN] field name
  /// @return field value
  virtual std::string getString(const std::string& fieldName) const = 0;


  // Access array values by index
  // arrays are implemented using the field with names like NAME_0, NAME_1 etc.

  /// Get array int field value
  /// @parem fieldName [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  virtual int getInt(const std::string& fieldName, unsigned int index) const = 0;

  /// Get array long field value
  /// @parem fieldName [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  virtual long getLong(const std::string& fieldName, unsigned int index) const = 0;

  /// Get array double field value
  /// @parem fieldName [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  virtual double getDouble(const std::string& fieldName, unsigned int index) const = 0;

  /// Get array float field value
  /// @parem fieldName [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  virtual float getFloat(const std::string& fieldName, unsigned int index) const = 0;

  /// Get array string field value
  /// @parem fieldName [IN] field name
  /// @param index [IN] index in the array
  /// @return field value
  virtual std::string getString(const std::string& fieldName, unsigned int index) const = 0;

};

#endif

