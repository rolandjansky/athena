/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCImpl.h 597331 2014-05-15 22:07:54Z gemmeren $
#ifndef POOL_FCIMPL_H
#define POOL_FCIMPL_H

#include <bitset>
#include "FileCatalog/FCCommon.h"
#include "FileCatalog/FCBuf.h"

namespace pool {

  class FCImpl {
  public:  
    FCImpl(){}
    virtual ~FCImpl(){}
    virtual void registerPFN( const std::string& pfn, const std::string& filetype, FileCatalog::FileID& fid) const=0;  
    virtual void registerLFN( const std::string& pfn, const std::string& lfn ) const=0;      
    virtual void addReplicaPFN(const std::string& pf, const std::string& rpf) const=0;
    /*add a PFN to a fileID. If fileID exists, add a replica PFN; if fileID doesnot exist, add the pair. For performance reason, the implementation should NOT check the existance of the fileID explicitly. Use exception from the backend library whenever possible. For the moment the argument filetype is dummy, the backend will not insert the value into the catalog.
    **/
    virtual void addPFNtoGuid(const FileCatalog::FileID& fid, const std::string& pf, const std::string& filetype) const=0;
    virtual void renamePFN(const std::string& pf, const std::string& newpf) const=0;
    virtual void lookupFileByPFN(const std::string& pf, FileCatalog::FileID& fid, std::string& ftype) const=0;
    virtual void lookupFileByLFN(const std::string& lf, FileCatalog::FileID& fid) const=0;
    virtual void lookupBestPFN(const FileCatalog::FileID& fid, const FileCatalog::FileOpenMode& omode, 
      const FileCatalog::FileAccessPattern& amode, std::string& pf, std::string& filetype) const=0;
    virtual void insertPFN(PFNEntry& pentry) const=0;
    virtual void insertLFN(LFNEntry& lentry) const=0;
    virtual void deletePFN(const std::string&  pf) const=0;
    virtual void deleteLFN(const std::string&  lf) const=0;
    virtual void deleteEntry(const FileCatalog::FileID& guid) const=0;
    virtual bool isReadOnly() const=0;
    virtual bool retrievePFN(const std::string& query, FCBuf<PFNEntry>& buf, const size_t& start )=0;
    virtual bool retrieveLFN(const std::string& query, FCBuf<LFNEntry>& buf, const size_t& start )=0;
    virtual bool retrieveGuid(const std::string& query, FCBuf<FileCatalog::FileID>& buf, const size_t& start )=0;
    virtual void connect() = 0;
    virtual void disconnect() const = 0;
    virtual void start() const=0;
    virtual void commit(FileCatalog::CommitMode cm=FileCatalog::REFRESH) const=0;
    virtual void rollback() const=0;
    void setType( const std::string& type ){
      m_type=type;
    }
    void setURL(const std::string& url){
      m_url=url;
    }
    std::string url() const{
      return m_url;
    }
    std::string implType()const{
      return m_type;
    }

  protected:
    std::string m_url;
    std::string m_type;
    //boolean flags of catalog transaction states: 
    /**pos             1               0
       meaning  inConnection-flag inTransaction-flag 
       1 in-connection  1 in-transaction      
       0 out-connection 0 out-transaction 
       possible combinations of states:  
       0(00) out-connection; out-transaction
       1(01) out-connection; in-transaction(impossible)
       2(10) in-connection; out-transaction
       3(11) in-connection; in-transaction
    */ 
    mutable std::bitset<2> m_transactionsta;
  };
   

  class FCMetaImpl;

  namespace FCImplBuilder{
    FCImpl* BuildFCImpl(const std::string& contactstr);
    FCMetaImpl* BuildFCMetaImpl( FCImpl* impl );
  }
}//ns pool
#endif
