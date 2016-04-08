/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: IFCContainer.h 622332 2014-10-17 14:10:37Z ssnyder $
#ifndef POOL_IFCCONTAINER_H
#define POOL_IFCCONTAINER_H
#include <string>
#include "FileCatalog/FCBuf.h"
#include "FileCatalog/FCEntry.h"
#include "FileCatalog/FCLeaf.h" 
//#include "FileCatalog/FCImpl.h"

namespace pool{
  class IFileCatalog;
  class FCLeaf;
  //  class FCImpl;

#define FCDEFAULT_BUFSIZE 1000
 
  /** @class IFCContainer IFCContainer.h FileCatalog/IFCContainer.h
      File Catalog iterator
      @author Zhen Xie
  */
  template <class Item>
    class IFCContainer{
    public:
    enum QueryStatus{noResult=0, hasResult, lastResult};
    ///constructor
    IFCContainer(IFileCatalog* catalog, const size_t cachesize=FCDEFAULT_BUFSIZE): m_catalog(catalog), m_buf(cachesize), m_cachemax(cachesize), m_finish(0), m_current(0), m_nrcatalogs( m_catalog->nReadCatalogs() ),m_currentcatalog(0),m_status(noResult), m_wdone(false), m_rdone(false), m_me(0){
    }
    ///destructor
    virtual ~IFCContainer(){
      m_buf.clear();
    }
    
    /**tell if the container has next result
       @return true if has next result
    */
    public:
    virtual void setQuery( const std::string& query){
      m_wdone=false;
      m_query=query;
    }
    virtual bool hasNext( ){
      bool result=false;
      if(!m_wdone){
	m_me=static_cast<FCLeaf*>(m_catalog->getWriteCatalog());
	if( !m_me ) {
	  m_wdone=true;
	}else{
	  result=this->hasNext(m_me,m_query);
	  switch (m_status){
	  case noResult:
	    m_wdone=true;
	    break;
	  case lastResult:
	    if(!result){
	      m_wdone=true;
	    }else{
	      return result;
	    }
	    break;
	  case hasResult:
	    return result;
	    break;
	  }
	}
      }
      if(m_wdone){
	if ( m_currentcatalog==m_nrcatalogs ) return false;
	if (!m_rdone){
	  m_me=static_cast<FCLeaf*>(m_catalog->getReadCatalog(m_currentcatalog));
	  m_buf.clear();
	  m_current=0;
	  m_finish=0;
	  result=this->hasNext(m_me,m_query);
	  m_rdone=true;
	}else{
          if (!m_me) return false;
	  result=this->hasNext(m_me,m_query);
	}
	switch (m_status){
	case noResult:
	  if ( gotoNextCatalog() ){
	    return this->hasNext();
	  }else{
	    return false;
	  }
	  break;
	case lastResult: 
	  if(!result){
	    if( gotoNextCatalog() ){
	      return this->hasNext();
	    }else{
	      return false;
	    }
	  }else{
	    return result;
	  }
	  break;
	case hasResult:
	  return result;
	  break;
	}
	//	return false;
      }
      return false;
    }
    virtual bool hasNext( IFileCatalog* current, const std::string& query )=0;
    /**retrieve next entry in the buffer
       @return item retrieved from the buffer
    */
    const Item& Next(){
      size_t temppos=m_current-(m_buf.nBoundary(m_current))*m_cachemax;
      ++m_current;
      return m_buf[temppos];
    }
    /**maximum buffer capacity
       @return max buffer size
    */
    size_t maxsize() const {
      return m_cachemax;
    }
    void reset(){
      m_buf.clear();
      m_wdone=false;
      m_finish=0;
      m_current=0;
      m_currentcatalog=0;
      m_status=noResult; 
    }
    protected: 
    IFileCatalog* m_catalog;
    FCBuf<Item> m_buf; ///buffer
    size_t m_cachemax; ///max buffer size
    size_t m_finish;  ///iteration end position
    size_t m_current; ///iteration current position
    size_t m_nrcatalogs;
    size_t m_currentcatalog;
    QueryStatus m_status; //mark set by each catalog retrive
    std::string m_query;
    bool m_wdone;
    bool m_rdone;
    FCLeaf* m_me;
    ///default constructor
    IFCContainer(){}
    bool gotoNextCatalog(){
      if (m_currentcatalog==m_nrcatalogs) {
	return false;
      }else{
	++m_currentcatalog;
	m_buf.clear();
	m_current=0;
	m_finish=0;
	m_status=noResult;
	m_rdone=false;
      }
      return true;
    }
  };
}//ns pool
#include "FileCatalog/IFCContainer.inl"

#endif


