/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_FCMETAIMPL_H
#define POOL_FCMETAIMPL_H

#include "FileCatalog/FCCommon.h"
#include "FileCatalog/FCBuf.h"

namespace pool {
  class FCImpl;
   
  class FCMetaImpl{
  public:  
    FCMetaImpl( FCImpl* fc ): m_fc(fc){}
    virtual ~FCMetaImpl(){}
    virtual void insertMetaData(MetaDataEntry& mentry) const=0;
    virtual void createMetaDataSpec( MetaDataEntry& schemadef) const=0;
    virtual void updateMetaDataSpec( MetaDataEntry& schemadef, const FileCatalog::MetaUpdateMode metamode=FileCatalog::NO_DELETE) const=0;
    virtual void getMetaDataSpec( MetaDataEntry& schemadef) const=0;
    virtual void dropMetaDataSpec() const=0;
    virtual void deleteMetaData(const FileCatalog::FileID& guid) const=0;
    //functions for bulk query
    virtual bool retrievePFN(const std::string& query, FCBuf<PFNEntry>& buf, const size_t& start )=0;
    virtual bool retrieveLFN(const std::string& query, FCBuf<LFNEntry>& buf, const size_t& start )=0;
    virtual bool retrieveMetaData(const std::string& query, FCBuf<MetaDataEntry>& buf, const size_t& start )=0;
    virtual bool retrieveGuid(const std::string& query, FCBuf<FileCatalog::FileID>& buf, const size_t& start )=0;

 protected:
    FCImpl* m_fc;
  };
}//ns pool
#endif



