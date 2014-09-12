/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: implWrapper_test.cxx 457829 2011-09-08 13:13:25Z mnowak $
/*
  Author: Zhen Xie
*/
#include <string>
#include <iostream>
#include "FileCatalog/FCImpl.h"
#include <memory>
#include <cstdlib>
using namespace pool;
class myImplWrapper:public pool::FCImpl{
private:
  typedef FCImpl super;
  std::auto_ptr<super> me_;
  super& me(){return *me_;}
  super const& me() const{return *me_;}
public:
  myImplWrapper(const std::string& contact){
    me_.reset(FCImplBuilder::BuildFCImpl(contact));
  }
  void connect(){me().connect();}
  void disconnect() const {me().disconnect();}
  void start() const {me().start();}
  void commit(FileCatalog::CommitMode cm=FileCatalog::REFRESH) const{me().commit(cm);}
  void rollback() const{me().rollback();}
  void registerPFN( const std::string& pfn, const std::string& filetype, FileCatalog::FileID& fid ) const{me().registerPFN(pfn,filetype,fid);} 
  void registerLFN( const std::string& pfn, const std::string& lfn ) const{me().registerLFN(pfn,lfn);}  
  void addReplicaPFN(const std::string& pf, const std::string& rpf) const{me().addReplicaPFN(pf,rpf);}
  void addPFNtoGuid(const FileCatalog::FileID& guid, const std::string& pf, const std::string& filetype) const{me().addPFNtoGuid(guid,pf,filetype);}
  void renamePFN(const std::string& pf, const std::string& newpf) const{
    me().renamePFN(pf,newpf);}
  void lookupFileByPFN(const std::string& pf, FileCatalog::FileID& fid, std::string& ftype) const{me().lookupFileByPFN(pf,fid,ftype);}
  void lookupFileByLFN(const std::string& lf, FileCatalog::FileID& fid) const{
    me().lookupFileByLFN(lf,fid);}
  void lookupBestPFN(const FileCatalog::FileID& fid, const FileCatalog::FileOpenMode& omode,const FileCatalog::FileAccessPattern& amode, std::string& pf, std::string& filetype) const{
    me().lookupBestPFN(fid,omode,amode,pf,filetype);
  }
  void insertPFN(PFNEntry& pentry) const{me().insertPFN(pentry);}
  void insertLFN(LFNEntry& lentry) const{me().insertLFN(lentry);}
  void deletePFN(const std::string&  pf) const{me().deletePFN(pf);}
  void deleteLFN(const std::string&  lf) const{me().deleteLFN(lf);}
  void deleteEntry(const FileCatalog::FileID& guid) const{me().deleteEntry(guid);}
  bool isReadOnly() const{return me().isReadOnly();}
  bool retrievePFN(const std::string& query, FCBuf<PFNEntry>& buf, const size_t& start ){ return me().retrievePFN(query,buf,start);}
  bool retrieveLFN(const std::string& query, FCBuf<LFNEntry>& buf, const size_t& start ){ return me().retrieveLFN(query,buf,start);}
  bool retrieveGuid(const std::string& query, FCBuf<FileCatalog::FileID>& buf, const size_t& start ){return me().retrieveGuid(query,buf,start);}
};
 
int main(){
  std::string uri="mysqlcatalog_mysql://xiezhen@localhost/zhendb";
  try{
    myImplWrapper myimpl(uri);
    std::cout<<"before connect"<<std::endl;
    myimpl.connect();
    std::cout<<"after connect"<<std::endl;
    myimpl.disconnect();
  }catch(const std::exception& er){
    std::cerr << er.what() << std::endl;
    exit(1);
  }
}




