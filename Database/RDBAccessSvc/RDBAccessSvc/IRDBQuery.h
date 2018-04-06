/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _IRDBQUERY_H_
#define _IRDBQUERY_H_

#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include <string>

class IRDBQuery
{
 public:
  IRDBQuery():m_attrList(0) {}
  virtual ~IRDBQuery() {}

  virtual void execute() = 0;
  virtual long size() = 0;
  virtual void finalize() = 0;
  virtual void setOrder(const std::string&) = 0;
  virtual void addToOutput(const std::string&) = 0;

  virtual bool next() = 0;

  template<typename T> const T& data(const std::string&);
  template<typename T> const T& data(unsigned int);
  bool isNull(const std::string&);
  bool isNull(unsigned int);

 protected:
  const coral::AttributeList* m_attrList;
};

template<typename T> const T& IRDBQuery::data(const std::string& field)
{
  return (*m_attrList)[field].data<T>();
}

template<typename T> const T& IRDBQuery::data(unsigned int fieldInd)
{
  return (*m_attrList)[fieldInd].data<T>();
}

inline bool IRDBQuery::isNull(const std::string& field)
{
  return (*m_attrList)[field].isNull();
}

inline bool IRDBQuery::isNull(unsigned int fieldInd)
{
  return (*m_attrList)[fieldInd].isNull();
}

#endif
