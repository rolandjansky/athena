/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_FCLEAF_H
#define POOL_FCLEAF_H
#include "FileCatalog/FCCommon.h"
#include "FileCatalog/IFileCatalog.h"
namespace pool{
  class FCImpl;
  class FCMetaImpl;
  class IFCAction;
  class IFileCatalog;
  class FCLeaf: public IFileCatalog{
  public:
    FCLeaf( const std::string& contact);
    explicit FCLeaf( FCImpl* impl, FCMetaImpl* metaimpl=0); //accept external wrappers
    virtual ~FCLeaf();    
    virtual void setAction( IFCAction& act );
    virtual void connect();
    virtual void disconnect();
    virtual void start();
    virtual void commit(FileCatalog::CommitMode cm=FileCatalog::REFRESH);
    virtual void rollback();
    virtual bool isReadOnly() const;
    FCImpl* getImpl() const;
    FCMetaImpl* getMetaImpl() const;
  private:
    mutable bool m_isreadonly;
    FCImpl* m_impl;
    FCMetaImpl* m_metaimpl;
  };
}
#endif
