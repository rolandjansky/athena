/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENADBTESTREC_TESTCOOLRECWRITER_H
#define ATHENADBTESTREC_TESTCOOLRECWRITER_H

#include "FolderInfo.h"
#include "DBConnection.h"
#include "boost/random.hpp"
#include "CoralBase/Attribute.h"
#include "CoolKernel/Record.h"

namespace pool {
class IFileCatalog;
class IPersistencySvc;
}

class TestCoolRecWriter {
 public:
  TestCoolRecWriter(const std::string& cooldb, const int mode,
		    const std::string& configfile,
                    const int nrun, const int run0, const int nevent, 
                    const int time0, const int timerun);
  int execute();
  int fillDB();
  int checkFolders();
  bool readConfig();
  void setPayload(const FolderInfo* folderi,cool::Record& payload,
		  const int irun, const int ichan, const int iarray=0);
  void setCoraPayload(const FolderInfo* folderi, 
		      const cool::IRecordSpecification& rspec,
		      std::vector<coral::AttributeList>& payloads,
		      const int irun,const int ichan);
  void setCoolVecPayload(const FolderInfo* folderi, 
			 const cool::IRecordSpecification& rspec,
			 std::vector<cool::IRecordPtr>& payloads,
			 const int irun,const int ichan);
  int genJobOpts(const std::string& file);
  bool setupPool();
  bool finalizePool();
  void setPoolPayload(const FolderInfo* folderi, cool::Record& payload,
		      const int irun, const int ichan);

 private:
  // parameters
  std::string m_coolstr;
  int m_mode;
  std::string m_configfile;
  int m_nrun;
  int m_run0;
  int m_nevent;
  int m_time0;
  int m_timerun;
  int m_poolmode;
  std::string m_poolstem;
  std::string m_tagsuffix;

  bool m_usepool;
  bool m_usecoracool;

  std::vector<FolderInfo*> m_folders;
  std::vector<DBConnection*> m_dbconn;

  pool::IFileCatalog* m_poolcat;
  pool::IPersistencySvc* m_persistencySvc;

};

#endif // ATHENADBTESTREC_TESTCOOLRECWRITER_H
