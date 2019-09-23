/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbConn.h
// helper class for IOVDbSvc managing DB connections
// Richard Hawkings, started 24/11/08
#ifndef IOVDbSvc_IOVDbConn_h
#define IOVDbSvc_IOVDbConn_h

#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoraCool/CoraCoolTypes.h"
#include "CoraCool/CoraCoolDatabase.h"

class MsgStream;

class IOVDbConn {
 public:
  IOVDbConn(const std::string& dbname, const bool readOnly, MsgStream & msg);
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
  bool open();
  void close();
  bool dropAndReconnect();
  bool valid() const;
  
  template<typename T=cool::IFolderPtr> 
  T
  getFolderPtr(const std::string &folderName){
    if (not m_coolDb.get()) return nullptr;
    return m_coolDb->getFolder(folderName);
  }
  
 private:
  MsgStream & m_log;
  bool m_active;
  bool m_readonly;
  bool m_abort;
  unsigned int m_nconn;
  unsigned int m_nfolder;
  const std::string m_connstr;
  cool::IDatabasePtr m_coolDb;
  CoraCoolDatabasePtr m_coracoolDb;
};

inline bool IOVDbConn::valid() const { return m_coolDb.get()!=0; }

inline bool IOVDbConn::isActive() const { return m_active; }

inline bool IOVDbConn::isReadOnly() const {return m_readonly; }

inline bool IOVDbConn::aborted() const {return m_abort; }

inline unsigned int IOVDbConn::nFolders() const {return m_nfolder; }

inline const std::string& IOVDbConn::name() const { return m_connstr; }

inline void IOVDbConn::incUsage() { ++m_nfolder; }

inline void IOVDbConn::decUsage() { --m_nfolder; }

template<> 
inline CoraCoolFolderPtr
IOVDbConn::getFolderPtr(const std::string &folderName){
  if (not m_coracoolDb.get()) return nullptr;
  return m_coracoolDb->getFolder(folderName);
}

#endif //  IOVDbSvc_IOVDbConn_h
