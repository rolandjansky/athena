/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCORALCLIENTUTILS_PCDKR_H
#define PIXELCORALCLIENTUTILS_PCDKR_H

#include "RelationalAccess/AccessMode.h"

#include <string>

namespace coral {
  class ISessionProxy;
}

class PCDkr
{
 public:
  PCDkr(const std::string& connString,
        const std::string& tableName,
        int verbose);
  ~PCDkr();

  void init(coral::AccessMode access_mode);
  void load();
  void save(cosnt std::string& tag, const std::string& cid);
  void validate(const std::string& tag);

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
