/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCORALCLIENTUTILS_PIXCALIBKNOWLEDGEDB_H
#define PIXELCORALCLIENTUTILS_PIXCALIBKNOWLEDGEDB_H

#include "RelationalAccess/AccessMode.h"

#include <string>

namespace coral {
  class ISessionProxy;
}

class PixCalibKnowledgeDb
{
 public:
  PixCalibKnowledgeDb(const std::string& connString,
                      const std::string& tableName,
                      int verbose);
  ~PixCalibKnowledgeDb();

  void init(coral::AccessMode access_mode);
  //long long findFK(std::string tag);

  void saveCorrespondingConfig(long int UnixTimeInSeconds, long int RunNumber, const std::string& calibtags_in_string, const std::string& idTag, const std::string& connTag, const std::string& cfgTag, const std::string& cfgModTag );

  void readCorrespondingCalibTag(long int Utime);

 private:
  std::string m_connString;
  std::string m_pixeltable;
  int m_verbose;
  coral::ISessionProxy *m_session;
  //std::string findTag(long long fk);

  void createTable();
  void createAuxTables();
  //  void createHeaderTable();
  void createConfigurationTable();
  //  void createDataTable();
  long long updateKey();

  void transactionStartReadOnly();
  void transactionStartUpdate();
  void transactionCommit();
};

#endif
