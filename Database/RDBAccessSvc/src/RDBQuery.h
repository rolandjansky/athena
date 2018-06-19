/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RDBACCESSSVC_RDBQUERY_H
#define RDBACCESSSVC_RDBQUERY_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>
#include <vector>
#include <string>

namespace coral
{
  class IQuery;
  class ISessionProxy;
  class ICursor;
}

/**
 * @class RDBQuery
 *
 * @brief RDBQuery is an implementation of IRDBQuery interface
 *
 */

class RDBAccessSvc;

class RDBQuery final : public IRDBQuery
{
 public:
  RDBQuery(RDBAccessSvc* accessSvc
	   , coral::ISessionProxy* session
	   , const std::string& nodeName
	   , const std::string& tagId
	   , const std::string& connName);

  virtual ~RDBQuery() override;
  
  virtual void execute() override;
  virtual long size() override;
  virtual void finalize() override;
  virtual void setOrder(const std::string&) override;
  virtual void addToOutput(const std::string&) override;

  virtual bool next() override;

 private:
  RDBQuery() {}

  coral::IQuery*           m_query;
  coral::IQuery*           m_queryCount;
  RDBAccessSvc*            m_accessSvc;
  coral::ISessionProxy*    m_session;
  std::string              m_nodeName;
  std::string              m_tagId;
  std::string              m_connName;
  long                     m_size;
  std::vector<std::string> m_fields;
  coral::ICursor*          m_cursor;
  std::string              m_orderField;
};

#endif
