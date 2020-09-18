/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCORALCLIENTUTILS_PCDIO_H
#define PIXELCORALCLIENTUTILS_PCDIO_H

#include "CxxUtils/checker_macros.h"
#include "RelationalAccess/AccessMode.h"

#include <string>

namespace coral {
  class ISessionProxy;
}

class PCDio
{
 public:
  PCDio(const std::string& connString,
        const std::string& tableName,
        int verbose);
  ~PCDio();

  void init(coral::AccessMode access_mode);
  void load(const std::string& tag, int revision);
  void save(const std::string& tag, int revision, const std::string& sources);

 private:
  std::string m_connString;
  std::string m_pixeltable;
  int m_verbose;
  coral::ISessionProxy *m_session;

  void createTable();
  void createAuxTables();
  void createHeaderTable();
  void createDataTable();
  long long updateKey();

  void transactionStartReadOnly();
  void transactionStartUpdate();
  void transactionCommit();
};

#endif
