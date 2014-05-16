/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _RDBQUERY_H_
#define _RDBQUERY_H_

#include "RDBAccessSvc/IRDBQuery.h"
#include "boost/shared_ptr.hpp"
#include <vector>
#include <string>

class RDBVersionAccessor;
typedef boost::shared_ptr<RDBVersionAccessor> RDBVersionAccessor_ptr;

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

#include "RDBVersionAccessor.h" 

class RDBQuery : public IRDBQuery
{
 public:
  RDBQuery(RDBAccessSvc* _accessSvc,
	   coral::ISessionProxy* _session,
	   const std::string& nodeName,
	   const std::string& tagId);

  virtual ~RDBQuery();
  
  void execute();
  long size();
  void finalize();
  void setOrder(const std::string&);
  void addToOutput(const std::string&);

  bool next();

 private:
  RDBQuery() {}

  coral::IQuery*           m_query;
  coral::IQuery*           m_queryCount;
  RDBAccessSvc*            m_accessSvc;
  coral::ISessionProxy*    m_session;
  std::string              m_nodeName;
  std::string              m_tagId;
  long                     m_size;
  std::vector<std::string> m_fields;
  coral::ICursor*          m_cursor;
  std::string              m_orderField;
};

#endif
