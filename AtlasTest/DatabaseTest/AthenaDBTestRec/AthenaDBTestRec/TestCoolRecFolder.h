/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENADBTESTREC_TESTCOOLRECFOLDER_H
#define ATHENADBTESTREC_TESTCOOLRECFOLDER_H
// TestCoolRecFolder.h - to keep details of a COOL folder being tested
// in readback mode
// Folders can be of different types:
// type=0 : CondAttrListCollection
// type=1 : TestCoolRecPoolDataColl (multichannel POOL objects)
// type=2 : CondAttrListVec (for CoraCool)
// type=3 : Generic objects (no callback, accessed via data proxy)
// >3 acts as type 0 with modulo run number

#include <string>
#include <fstream>
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/IIOVSvc.h"

class StoreGateSvc;
namespace SG {
  class DataProxy;
}
class IOVTime;

class TestCoolRecFolder {
 public:
  TestCoolRecFolder(const std::string key,const int type,
		    const SG::DataProxy* proxy,
		    const int dumpchan,std::ofstream* dumpf);
  StatusCode regCallBack(StoreGateSvc* detstore);
  StatusCode access(StoreGateSvc* detstore, const int run, const int event,
		    const IOVTime iovkey);
  StatusCode callBack( IOVSVC_CALLBACK_ARGS );
  std::string iovprint(const IOVTime& time, const bool isTS);

  const std::string name() const;
  int type() const;
  int nAccess() const;
  int nError() const;
  int nMisMatch() const;
  int nItems() const;
  int nCallBack() const;

 private:
  std::string m_key;
  int m_type;
  const SG::DataProxy* m_proxy;
  int m_dumpchan;
  std::ofstream* m_dumpf;
  int m_naccess;
  int m_nerror;
  int m_nmismatch;
  int m_nitems;
  int m_ncallback;
};

inline const std::string TestCoolRecFolder::name() const { return m_key;}
inline int TestCoolRecFolder::type() const { return m_type;}
inline int TestCoolRecFolder::nAccess() const { return m_naccess;}
inline int TestCoolRecFolder::nError() const { return m_nerror;}
inline int TestCoolRecFolder::nMisMatch() const { return m_nmismatch;}
inline int TestCoolRecFolder::nItems() const { return m_nitems;}
inline int TestCoolRecFolder::nCallBack() const { return m_ncallback;}

#endif // ATHENADBTESTREC_TESTCOOLRECFOLDER_H
