/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SqliteRecord.cxx
 *
 * @brief Implementation of the SqliteRecord class
 *
 */


#include "SqliteRecord.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sstream>

SqliteRecord::SqliteRecord(SqliteInpDef_ptr def)
  : m_def(def)
{
}

SqliteRecord::~SqliteRecord()
{
}

bool SqliteRecord::isFieldNull(const std::string& field) const 
{
  if(m_record.find(field)!=m_record.end()) return false;
  if(m_def->find(field)==m_def->end()) throw std::runtime_error( "Wrong name for the field "+ field);
  return true;
}

int SqliteRecord::getInt(const std::string& field) const
{
  auto [recIt,checkCode] = checkField(field,SQLITEINP_INT);
  if(checkCode==FIELD_CHECK_OK) {
    return std::get<int>(recIt->second);
  }
  handleError(field,checkCode);
  return 0;
}

long SqliteRecord::getLong(const std::string& field) const
{
  // Our database does not support LONG data types at this moment
  return (long)getInt(field);
}

double SqliteRecord::getDouble(const std::string& field) const
{
  auto [recIt,checkCode] = checkField(field,SQLITEINP_DOUBLE);
  if(checkCode==FIELD_CHECK_OK) {
    return std::get<double>(recIt->second);
  }
  handleError(field,checkCode);
  return 0.;
}

float SqliteRecord::getFloat(const std::string& field) const
{
  // SQLite stores REAL as double
  return (float)getDouble(field);
}

std::string SqliteRecord::getString(const std::string& field) const
{
  auto [recIt,checkCode] = checkField(field,SQLITEINP_STRING);
  if(checkCode==FIELD_CHECK_OK) {
    return std::get<std::string>(recIt->second);
  }
  handleError(field,checkCode);
  return std::string();
}

int SqliteRecord::getInt(const std::string& field, unsigned int index) const
{
  return getInt(field + "_" + std::to_string(index));
}

long SqliteRecord::getLong(const std::string& field, unsigned int index) const
{
  return getLong(field + "_" + std::to_string(index));
}

double SqliteRecord::getDouble(const std::string& field, unsigned int index) const
{
  return getDouble(field + "_" + std::to_string(index));
}

float SqliteRecord::getFloat(const std::string& field, unsigned int index) const
{
  return getFloat(field + "_" + std::to_string(index));
}

std::string SqliteRecord::getString(const std::string& field, unsigned int index) const
{
  return getString(field + "_" + std::to_string(index));
}

void SqliteRecord::addValue(const std::string& field
			    , SqliteInp value)
{
  auto [it,result] = m_record.insert(std::pair(field,value));
  if(!result) throw std::runtime_error("Unexpected error when adding new value for the field " + 
				       field + ". Duplicate field name?");
}

void SqliteRecord::dump() const
{
  for(const auto& element : m_record) {
    auto defIt = m_def->find(element.first);
    std::cout << "[\"" << element.first << "\" :" << std::setprecision(10);
    switch(defIt->second) {
    case SQLITEINP_INT:
      std::cout << "INT " << std::get<int>(element.second) << "], ";
      break;
    case SQLITEINP_LONG:
      std::cout << "LONG " << std::get<long>(element.second) << "], ";
      break;
    case SQLITEINP_FLOAT:
      std::cout << "FLOAT " << std::get<float>(element.second) << "], ";
      break; 
    case SQLITEINP_DOUBLE:
      std::cout << "DOUBLE " << std::get<double>(element.second) << "], ";
      break;
    case SQLITEINP_STRING:
      std::cout << "STRING " << std::get<std::string>(element.second) << "], ";
      break;
    default:
      std::cout << "ERROR], ";
    }
  }
  std::cout << std::endl;
}
