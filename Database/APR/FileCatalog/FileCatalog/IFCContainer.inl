#include "FileCatalog/FCImpl.h"
#include "FileCatalog/FCMetaImpl.h"
#include "FileCatalog/FCQueryParser.h"
#include "FileCatalog/FCException.h"
/**
   @class IFCContainer.inl IFCContainer.inl
   implementation of container iterators. TO BE CLEANED UP!
   @Author: Z.Xie
*/
namespace pool{
  typedef IFCContainer<PFNEntry> IPFNContainer;
  typedef IFCContainer<LFNEntry> ILFNContainer;
  typedef IFCContainer<MetaDataEntry> IMetaDataContainer;
  typedef IFCContainer<FileCatalog::FileID> IGuidContainer;
  class PFNContainer: public IPFNContainer{
  public:
    PFNContainer(IFileCatalog* catalog, const size_t cachesize=FCDEFAULT_BUFSIZE):IPFNContainer(catalog,cachesize) {
    }
    virtual bool hasNext(){
     return IPFNContainer::hasNext();
    }
    virtual void setQuery( const std::string& query){
       IPFNContainer::setQuery(query);
    }
    virtual bool hasNext( IFileCatalog* c, const std::string& query ){
      FCMetaImpl* m=0;
      FCImpl* i=0;
      FCQueryParser parser(query);
      parser.parse();
      if(parser.isMetaQuery()){
         m=static_cast<FCLeaf*>(c)->getMetaImpl();
         if(!m){
           throw FCnonexistentMetaDataException("PFNContainer" );
         }
      }else{
         i=static_cast<FCLeaf*>(c)->getImpl();
      }
      if(  m_buf.isBoundary(m_current) ){
	m_buf.clear();
	bool result=false;
	if(i){ 
           result=i->retrievePFN(query, m_buf, m_current);
        }else if(m) {	
           result=m->retrievePFN(query, m_buf, m_current);
        }
	if(result){
	  m_finish=m_buf.size();
	  if(m_finish<m_buf.maxsize()){
            m_status=IPFNContainer::lastResult;  	
	  }else{
            m_status=IPFNContainer::hasResult;
          }
	  return true;
	}else{
          m_status=IPFNContainer::noResult;
	  return false;
	}
      }else{
	size_t temppos=m_current-(m_buf.nBoundary(m_current))*m_cachemax;
	return temppos!=m_finish;
      }
    }
  };

  class LFNContainer: public ILFNContainer{
  public:
    LFNContainer(IFileCatalog* catalog, const size_t cachesize=FCDEFAULT_BUFSIZE):ILFNContainer(catalog,cachesize){
    }
    virtual void setQuery( const std::string& query){
       ILFNContainer::setQuery(query);
    }
    virtual bool hasNext(){
       return ILFNContainer::hasNext();
    }
    virtual bool hasNext( IFileCatalog* c, const std::string& query ){
      FCMetaImpl* m=0;
      FCImpl* i=0;
      FCQueryParser parser(query);
      parser.parse();
      if(parser.isMetaQuery()){
         m=static_cast<FCLeaf*>(c)->getMetaImpl();
         if(!m){
           throw FCnonexistentMetaDataException("LFNContainer" );
         }
      }else{
         i=static_cast<FCLeaf*>(c)->getImpl();
      }
      if(  m_buf.isBoundary(m_current) ){
        m_buf.clear();
        bool result=false;
	if(i){ 
           result=i->retrieveLFN(query, m_buf, m_current);
        }else if(m) {	
           result=m->retrieveLFN(query, m_buf, m_current);
        }
	if(result){
	  m_finish=m_buf.size();
	  if( m_finish<m_buf.maxsize() ){
            m_status=ILFNContainer::lastResult;  	
	  }else{
            m_status=ILFNContainer::hasResult;
          }
	  return true;
	}else{
          m_status=ILFNContainer::noResult;
	  return false;
	}
      }else{
	size_t temppos=m_current-(m_buf.nBoundary(m_current))*m_cachemax;
	return temppos!=m_finish;
      }
    }
  };
  
  class MetaDataContainer: public IMetaDataContainer{
  public:
    MetaDataContainer(IFileCatalog* catalog, const size_t cachesize=FCDEFAULT_BUFSIZE):IMetaDataContainer(catalog, cachesize){
    }
    virtual void setQuery( const std::string& query){
       IMetaDataContainer::setQuery(query);
    }
    virtual bool hasNext(){
     return IMetaDataContainer::hasNext();
    }
    virtual bool hasNext( IFileCatalog* c, const std::string& query ){
       FCMetaImpl* i=static_cast<FCLeaf*>(c)->getMetaImpl();
       if(!i){
           throw FCnonexistentMetaDataException("MetaDataContainer" );
       }
       if(  m_buf.isBoundary(m_current) ){
        m_buf.clear();
        if( i->retrieveMetaData(query, m_buf, m_current) ){
          m_finish=m_buf.size();
          if( m_finish<m_buf.maxsize() ){
            m_status=IMetaDataContainer::lastResult;  
          }else{
             m_status=IMetaDataContainer::hasResult;
          }
          return true;
        }else{
          m_status=IMetaDataContainer::noResult;
          return false;
        }
      }else{
        size_t temppos=m_current-(m_buf.nBoundary(m_current))*m_cachemax;
        return temppos!=m_finish;
      }
    }
  };

  class GuidContainer: public IGuidContainer{
    public:
    GuidContainer(IFileCatalog* catalog, const size_t cachesize=FCDEFAULT_BUFSIZE):IGuidContainer(catalog,cachesize){
    }
    virtual void setQuery( const std::string& query){
       IGuidContainer::setQuery(query);
    }
    virtual bool hasNext(){
     return IGuidContainer::hasNext();
    }
    virtual bool hasNext( IFileCatalog* c, const std::string& query ){
      FCMetaImpl* m=0;
      FCImpl* i=0;
      FCQueryParser parser(query);
      parser.parse();
      if(parser.isMetaQuery()){
         m=static_cast<FCLeaf*>(c)->getMetaImpl();
         if(!m){
           throw FCnonexistentMetaDataException("GuidContainer" );
         }
      }else{
         i=static_cast<FCLeaf*>(c)->getImpl();
      }
      if(m_buf.isBoundary(m_current)){
	m_buf.clear();
        bool result=false;
        if(i){
          result=i->retrieveGuid(query, m_buf, m_current);
        }else if(m) {	
          result=m->retrieveGuid(query, m_buf, m_current);
        }
	if( result ){
	  m_finish=m_buf.size();
	  if( m_finish<m_buf.maxsize() ){
            m_status=IGuidContainer::lastResult;  	
	  }else{
            m_status=IGuidContainer::hasResult;
          }
	  return true;
	}else{
          m_status=IGuidContainer::noResult;
	  return false;
	}
      }else{
	size_t temppos=m_current-(m_buf.nBoundary(m_current))*m_cachemax;
	return temppos!=m_finish;
      }
    }
  };
}
//ns pool

