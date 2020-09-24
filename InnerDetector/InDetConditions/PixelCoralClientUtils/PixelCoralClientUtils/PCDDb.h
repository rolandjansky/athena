/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCORALCLIENTUTILS_PCDDB_H
#define PIXELCORALCLIENTUTILS_PCDDB_H

#include <string>

namespace PixelCoralClientUtils {
  class PixelCalibData;
}

namespace coral {
  class ISessionProxy;
  class IQuery;
  class ICursor;
}

class PCDDb
{
 public:
  PCDDb(const std::string& connString,
        const std::string& tableName,
        bool verbose, bool load_text);
  ~PCDDb();

  bool init(const std::string& tag, int revision);
  int next();
  bool set(int idmod_cur, PixelCoralClientUtils::PixelCalibData& pcd);

 private:
  bool m_verbose;
  coral::ISessionProxy *m_session;
  std::string m_connString;
  std::string m_pixeltable;

  coral::IQuery* m_query;
  coral::IQuery* m_query_2;
  coral::ICursor* m_cursor;

  void transactionStartReadOnly();
  void transactionStartUpdate();
  void transactionCommit();
};

#endif
