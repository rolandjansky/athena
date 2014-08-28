/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbConn.h
// helper class for IOVDbSvc managing DB connections
// Richard Hawkings, started 24/11/08
#ifndef __IOVDBCONN_H__
#define __IOVDBCONN_H__

#include "CoolKernel/IDatabase.h"
#include "CoraCool/CoraCoolTypes.h"

class MsgStream;

class IOVDbConn {
 public:
  IOVDbConn(const std::string& dbname, const bool readOnly, MsgStream* msg);
  ~IOVDbConn();

  bool isActive() const;
  bool isReadOnly() const;
  bool aborted() const;
  unsigned int nFolders() const;
  const std::string& name() const;

  void incUsage();
  void decUsage();
  void summary(const float fread);
  cool::IDatabasePtr getCoolDb();
  CoraCoolDatabasePtr getCoraCoolDb();
  void setInactive();
  void setReadOnly(const bool readOnly);

 private:
  MsgStream* m_log;
  bool m_active;
  bool m_readonly;
  bool m_abort;
  unsigned int m_nconn;
  unsigned int m_nfolder;
  const std::string m_connstr;
  cool::IDatabasePtr m_coolDb;
  CoraCoolDatabasePtr m_coracoolDb;
};

inline bool IOVDbConn::isActive() const { return m_active; }

inline bool IOVDbConn::isReadOnly() const {return m_readonly; }

inline bool IOVDbConn::aborted() const {return m_abort; }

inline unsigned int IOVDbConn::nFolders() const {return m_nfolder; }

inline const std::string& IOVDbConn::name() const { return m_connstr; }

inline void IOVDbConn::incUsage() { ++m_nfolder; }

inline void IOVDbConn::decUsage() { --m_nfolder; }

#endif //  __IOVDBCONN_H__
