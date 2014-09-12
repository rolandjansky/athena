/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_IFCACTION_H
#define POOL_IFCACTION_H
#include <vector>
#include "FileCatalog/FCCommon.h"
#include "FileCatalog/IFileCatalog.h"
namespace pool{
  class FCLeaf;
  class MetaDataEntry;
  class IFCAction{
  public:
    IFCAction();
    virtual ~IFCAction();
    virtual void visitFCLeaf(IFileCatalog* cat);
    virtual void visitFCComposite(IFileCatalog* cat);
    bool isWritableEntry( FileCatalog::FileID& fid );
    void registerPFN( const std::string& pfn, const std::string& filetype, FileCatalog::FileID& fid );
    void lookupFileByPFN(const std::string& pf, FileCatalog::FileID& fid, std::string& ftype);
    void lookupFileByLFN(const std::string& lf, FileCatalog::FileID& fid);
    void lookupBestPFN(const FileCatalog::FileID& fid, const FileCatalog::FileOpenMode& omode, 
		       const FileCatalog::FileAccessPattern& amode, std::string& pf, std::string& filetype) ;
    void getMetaDataSpec( MetaDataEntry& spec); 
  protected:
    FCLeaf* m_write;
    std::vector<FCLeaf*> m_read;
  };
  
  class FCregister: public  IFCAction{
  public:
    FCregister(){}
    ~FCregister(){}
    void registerLFN( const std::string& pfn, const std::string& lfn );
    void addReplicaPFN(const std::string& pf, const std::string& rpf);
    void renamePFN(const std::string& pf, const std::string& newpf);
    void registerMetaData( const FileCatalog::FileID& fid, MetaDataEntry& meta);
  };

  template <class Item>
    class IFCContainer;
  class PFNContainer;
  class LFNContainer;
  class MetaDataContainer;
  class GuidContainer;
  class AttributeListSpecification;
  class FClookup: public IFCAction{
  public:
    FClookup(){}
    ~FClookup(){}
    void lookupPFN(const FileCatalog::FileID& fid, PFNContainer& pfs);
    void lookupLFN(const FileCatalog::FileID& fid, LFNContainer& lfs);
    void lookupPFNByQuery(const std::string& query, PFNContainer& pfs);
    void lookupLFNByQuery(const std::string& query, LFNContainer& pfs);
    void lookupPFNByLFN(const std::string& lfn, PFNContainer& pfs);
    void lookupLFNByPFN(const std::string& pfn, LFNContainer& lfs);
    void lookupMetaDataByQuery(const std::string& query, MetaDataContainer& metas);
    void lookupFileByQuery(const std::string& query, GuidContainer& fids);
  };
  
  class FCAdmin: public IFCAction{
  public:
    FCAdmin(){}
    ~FCAdmin(){}
    void createMetaDataSpec( MetaDataEntry& schemadef);
    void updateMetaDataSpec( MetaDataEntry& schemadef, const FileCatalog::MetaUpdateMode metamode=FileCatalog::NO_DELETE );
    void dropMetaDataSpec();
    void deletePFN(const std::string&  pf);
    void deleteLFN(const std::string& lf);
    void deleteMetaData(const FileCatalog::FileID& fid);
    void deleteEntry( const FileCatalog::FileID& fid );
  };
}//ns pool
#endif
