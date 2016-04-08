/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_IFILECATALOG_H
#define POOL_IFILECATALOG_H

#include <vector>
#include "FileCatalog/FCCommon.h"

namespace pool{
#define FCDEFAULT_CACHE_SIZE 1000
  class IFCAction;
  class FCLeaf;

  class IFileCatalog{
  public:
    typedef std::vector<FCLeaf*> ReadCatalogs;
    typedef ReadCatalogs::iterator ReadCatalogIterator;
    typedef ReadCatalogs::reverse_iterator ReadCatalog_reverseIterator;
    ///Commit action
    IFileCatalog();
    virtual ~IFileCatalog();
    ///create write catalog, can only be one. 
    void setWriteCatalog( const std::string& contact );
    ///add read catalog, can be more than one. The search priority order is 
    ///the first added is searched first.
    void addReadCatalog( const std::string& contact );
    ///create write catalog, can only be one. 
    void setWriteCatalog( FCLeaf* w );
    ///add read catalog, can be more than one. The search priority order is 
    ///the first added is searched first.
    void addReadCatalog( FCLeaf* r );
    //iteration
    IFileCatalog* getReadCatalog( const unsigned long& idx );
    size_t nReadCatalogs() const;
    IFileCatalog* getWriteCatalog();
    void importCatalog(IFileCatalog* fc, 
		       const std::string& query,
		       size_t cacheSize=FCDEFAULT_CACHE_SIZE) const;
    
    //common interface to IFileCatalog and FCLeaf
    virtual bool isReadOnly() const;
    virtual void setAction( IFCAction& act );
    virtual void connect();
    virtual void disconnect();
    virtual void start();
    virtual void commit(FileCatalog::CommitMode cm=FileCatalog::REFRESH);
    virtual void rollback() ;
    //tell if metadata catalog implementation available
    bool hasMetaDataCatalog() const;
  private:
    FCLeaf* m_wchild;
    ReadCatalogs m_rchildren;
    friend class IFCAction;
  };
}
#endif
