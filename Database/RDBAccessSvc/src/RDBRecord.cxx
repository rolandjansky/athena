/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RDBRecord.cxx
 *
 * @brief Implementation of RDBRecord class
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: RDBRecord.cxx,v 1.11 2006-10-23 15:24:06 tsulaia Exp $
 */


#include "RDBRecord.h"
#include "RelationalAccess/ICursor.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeSpecification.h"

#include <stdexcept>
#include <sstream>
#include <string>

RDBRecord::RDBRecord(const coral::AttributeList& attList
		     , const std::string& tableName)
  : m_values(0)
  , m_tableName(tableName)
{
  // Copy attList.  Try to avoid sharing, for thread-safety.
  m_values = new coral::AttributeList(attList.specification(), false);
  m_values->fastCopyData (attList);

  for(unsigned int i=0; i<m_values->size(); i++) {
    std::string key = (*m_values)[i].specification().name();
    m_name2Index[key] = i;
  }
}

RDBRecord::~RDBRecord()
{
  delete m_values;
}

bool RDBRecord::isFieldNull(const std::string& fieldName) const 
{
  FieldName2ListIndex::const_iterator it = m_name2Index.find(m_tableName+"."+fieldName);
  if(it==m_name2Index.end()) {
    throw std::runtime_error( "Wrong name for the field " + m_tableName+"."+fieldName);
  }

  const coral::AttributeList& values = *m_values;
  return values[it->second].isNull();
}

int RDBRecord::getInt(const std::string& fieldName) const
{
  FieldName2ListIndex::const_iterator it = m_name2Index.find(m_tableName+"."+fieldName);
  if(it==m_name2Index.end()) {
    throw std::runtime_error( "Wrong name for the field " + m_tableName+"."+fieldName);
  }

  const coral::AttributeList& values = *m_values;
  if(values[it->second].specification().type()==typeid(int)) {
    return values[it->second].data<int>();
  }
  else if(values[it->second].specification().type()==typeid(long)) {
    return (int)values[it->second].data<long>();
  }
  else {
    throw std::runtime_error( "Field " + fieldName + " is NOT of integer type\n");
  }
}

long RDBRecord::getLong(const std::string& fieldName) const
{
  FieldName2ListIndex::const_iterator it = m_name2Index.find(m_tableName+"."+fieldName);
  if(it==m_name2Index.end()) {
    throw std::runtime_error( "Wrong name for the field " + m_tableName+"."+fieldName);
  }

  const coral::AttributeList& values = *m_values;
  if(values[it->second].specification().type()==typeid(long)) {
    return values[it->second].data<long>();
  }
  else if(values[it->second].specification().type()==typeid(int)) {
    return (long)values[it->second].data<int>();
  }
  else if(values[it->second].specification().type()==typeid(long long)) {
    return (long)values[it->second].data<long long>();
  }
  else {
    throw std::runtime_error( "Field " + fieldName + " is NOT of long type");
  }
}

double RDBRecord::getDouble(const std::string& fieldName) const
{
  FieldName2ListIndex::const_iterator it = m_name2Index.find(m_tableName+"."+fieldName);
  if(it==m_name2Index.end()) {
    throw std::runtime_error( "Wrong name for the field " + m_tableName+"."+fieldName);
  }

  const coral::AttributeList& values = *m_values;
  if(values[it->second].specification().type()==typeid(double)) {
    return values[it->second].data<double>();
  }
  else {
    throw std::runtime_error( "Field " + fieldName + " is NOT of double type");
  }
}

float RDBRecord::getFloat(const std::string& fieldName) const
{
  FieldName2ListIndex::const_iterator it = m_name2Index.find(m_tableName+"."+fieldName);
  if(it==m_name2Index.end()) {
    throw std::runtime_error( "Wrong name for the field " + m_tableName+"."+fieldName);
  }

  const coral::AttributeList& values = *m_values;
  if(values[it->second].specification().type()==typeid(float)) {
    return values[it->second].data<float>();
  }
  else {
    throw std::runtime_error( "Field " + fieldName + " is NOT of float type");
  }
}

std::string RDBRecord::getString(const std::string& fieldName) const
{
  FieldName2ListIndex::const_iterator it = m_name2Index.find(m_tableName+"."+fieldName);
  if(it==m_name2Index.end()) {
    throw std::runtime_error( "Wrong name for the field " + m_tableName+"."+fieldName);
  }

  const coral::AttributeList& values = *m_values;
  if(values[it->second].specification().type()==typeid(std::string)) {
    return values[it->second].data<std::string>();
  }
  else {
    throw std::runtime_error( "Field " + fieldName + " is NOT of string type");
  }
}

int RDBRecord::getInt(const std::string& fieldName, unsigned int index) const
{
  FieldName2ListIndex::const_iterator it = m_name2Index.find(m_tableName+"."+fieldName + "_" + std::to_string(index));
  if(it==m_name2Index.end()) {
    throw std::runtime_error("Wrong name for the array field " + m_tableName+"."+fieldName + " or index=" + std::to_string(index) + " is out of range");
  }

  const coral::AttributeList& values = *m_values;
  if(values[it->second].specification().type()==typeid(int)) {
    return values[it->second].data<int>();
  }
  else if(values[it->second].specification().type()==typeid(long)) {
    return (int)values[it->second].data<long>();
  }
  else {
    throw std::runtime_error( "Field " + fieldName + " is NOT of integer type\n");
  }
}

long RDBRecord::getLong(const std::string& fieldName, unsigned int index) const
{
  FieldName2ListIndex::const_iterator it = m_name2Index.find(m_tableName+"."+fieldName + "_" + std::to_string(index));
  if(it==m_name2Index.end()) {
    throw std::runtime_error("Wrong name for the array field " + m_tableName+"."+fieldName + " or index=" + std::to_string(index) + " is out of range");
  }

  const coral::AttributeList& values = *m_values;
  if(values[it->second].specification().type()==typeid(long)) {
    return values[it->second].data<long>();
  }
  else if(values[it->second].specification().type()==typeid(int)) {
    return (long)values[it->second].data<int>();
  }
  else {
    throw std::runtime_error( "Field " + fieldName + " is NOT of long type");
  }
}

double RDBRecord::getDouble(const std::string& fieldName, unsigned int index) const
{
  FieldName2ListIndex::const_iterator it = m_name2Index.find(m_tableName+"."+fieldName + "_" + std::to_string(index));
  if(it==m_name2Index.end()) {
    throw std::runtime_error("Wrong name for the array field " + m_tableName+"."+fieldName + " or index=" + std::to_string(index) + " is out of range");
  }

  const coral::AttributeList& values = *m_values;
  if(values[it->second].specification().type()==typeid(double)) {
    return values[it->second].data<double>();
  }
  else {
    throw std::runtime_error( "Field " + fieldName + " is NOT of double type");
  }
}

float RDBRecord::getFloat(const std::string& fieldName, unsigned int index) const
{
  FieldName2ListIndex::const_iterator it = m_name2Index.find(m_tableName+"."+fieldName + "_" + std::to_string(index));
  if(it==m_name2Index.end()) {
    throw std::runtime_error("Wrong name for the array field " + m_tableName+"."+fieldName + " or index=" + std::to_string(index) + " is out of range");
  }

  const coral::AttributeList& values = *m_values;
  if(values[it->second].specification().type()==typeid(float)) {
    return values[it->second].data<float>();
  }
  else {
    throw std::runtime_error( "Field " + fieldName + " is NOT of float type");
  }
}

std::string RDBRecord::getString(const std::string& fieldName, unsigned int index) const
{
  FieldName2ListIndex::const_iterator it = m_name2Index.find(m_tableName+"."+fieldName + "_" + std::to_string(index));
  if(it==m_name2Index.end()) {
    throw std::runtime_error("Wrong name for the array field " + m_tableName+"."+fieldName + " or index=" + std::to_string(index) + " is out of range");
  }

  const coral::AttributeList& values = *m_values;
  if(values[it->second].specification().type()==typeid(std::string)) {
    return values[it->second].data<std::string>();
  }
  else {
    throw std::runtime_error( "Field " + fieldName + " is NOT of string type");
  }
}

bool RDBRecord::operator!=(const RDBRecord& rhs) const
{
  const coral::AttributeList& myAttList = *m_values;
  const coral::AttributeList& rhsAttList = *rhs.m_values;

  if(myAttList.size()!=rhsAttList.size()) return true;

  for(size_t i(0); i<myAttList.size(); ++i) {
    const coral::Attribute& myAtt = myAttList[i];
    const std::string name = myAtt.specification().name();
    bool exists(false);
    for(size_t j(0); j<rhsAttList.size(); ++j) {
      const coral::Attribute& rhsAtt = rhsAttList[j];
      if(rhsAtt.specification().name()==name) {
	if(myAtt!=rhsAtt) {
	  return true;
	}
	exists = true;
	break;
      }
    }// Go through the attributes in the RHS list
    if(!exists)
      return true;
  }
  return false;
}

std::ostream& RDBRecord::toOutputStream(std::ostream& os) const
{
  m_values->toOutputStream(os);
  return os;
}
