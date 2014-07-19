/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _PCDkr_h_
#define _PCDkr_h_

#include "RelationalAccess/AccessMode.h"

#include <string>

namespace coral {
  class ISessionProxy;
}

class PCDkr
{
 public:
  PCDkr(std::string connString, std::string tableName, int verbose);
  ~PCDkr();

  void init(coral::AccessMode access_mode);
  void load();
  void save(std::string tag, std::string cid);
  void validate(std::string tag);

 private:
  std::string m_connString;
  std::string m_pixeltable;
  int m_verbose;
  coral::ISessionProxy *m_session;

  void createTable();

  void transactionStartReadOnly();
  void transactionStartUpdate();
  void transactionCommit();
};

#endif
