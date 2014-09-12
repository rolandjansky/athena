/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_IFILECATALOG_H
#include "FileCatalog/IFileCatalog.h"
#endif
#include <string>
#include "FileCatalog/FCLeaf.h"
#include "FileCatalog/FCException.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/FCBuf.h"
#include "FileCatalog/FCEntry.h"
#include "FileCatalog/FCImpl.h"
#include "FileCatalog/FCMetaImpl.h"
#include "CoralBase/MessageStream.h"
namespace pool{

  IFileCatalog::IFileCatalog():m_wchild(0){
  }
  
  IFileCatalog::~IFileCatalog(){
    if( m_wchild !=0 ){
      delete m_wchild;
    }
    if( m_rchildren.size()!=0 ){
      IFileCatalog::ReadCatalogIterator it;
      for(it=m_rchildren.begin();it!=m_rchildren.end();++it){
        delete *it;
      }
      m_rchildren.clear();
    }
  }

  void IFileCatalog::setWriteCatalog( const std::string& contact ){
    if( m_wchild !=0 ){
      throw FCTransactionException ( "IFileCatalog::setWriteCatalog","ERROR: writeCatalog already exists" );
    }
    m_wchild=new FCLeaf( contact );
  }

  void IFileCatalog::addReadCatalog( const std::string& contact ){
    m_rchildren.push_back(new FCLeaf(contact) );
  }
  
  void IFileCatalog::setWriteCatalog( FCLeaf* w ){
    if( m_wchild !=0 ){
      throw FCTransactionException ( "IFileCatalog::setWriteCatalog","ERROR: writeCatalog already exists" );
    }
    m_wchild=w;
  }

  void IFileCatalog::addReadCatalog( FCLeaf* r ){
    m_rchildren.push_back( r );
  }
  
  IFileCatalog* IFileCatalog::getReadCatalog( const unsigned long& idx ){
    return static_cast<IFileCatalog*>(m_rchildren[idx]); 
  }
  
  size_t IFileCatalog::nReadCatalogs() const {
    return m_rchildren.size();
  }

  IFileCatalog* IFileCatalog::getWriteCatalog(){
    return static_cast<IFileCatalog*>(m_wchild);
  }

  void IFileCatalog::importCatalog(IFileCatalog* fc, 
                                   const std::string& query,
                                   size_t cacheSize) const{
    //warning: cannot do cross table query!
    if( m_wchild->isReadOnly() ){
      throw FCTransactionException ( "IFileCatalog::importCatalog","ERROR: the catalog is readonly" );
    }
    coral::MessageStream fclog( "IFileCatalog");
    bool importMeta=true;
    MetaDataEntry m;
    FCMetaImpl* meta=m_wchild->getMetaImpl();
    bool metasource=fc->hasMetaDataCatalog();
    if(!meta||!metasource){
      importMeta=false;
    }else{
      meta->getMetaDataSpec(m);
      if(m.size()<=1) importMeta=false;
    }
    size_t n=fc->nReadCatalogs();
    FCLeaf* cat=0;
    for(size_t i=0; i<n; ++i){
      FCBuf<PFNEntry> pbuf(cacheSize);
      FCBuf<LFNEntry> lbuf(cacheSize);
      FCBuf<MetaDataEntry> mbuf(cacheSize);
      cat=static_cast<FCLeaf*> (fc->getReadCatalog(i));
      size_t pcounter=0;
      size_t lcounter=0;
      size_t mcounter=0;
      if(importMeta){
	while( cat->getMetaImpl()->retrievePFN(query,pbuf,pcounter) ){
	  for(size_t i=0; i<pbuf.size(); ++i){
	    PFNEntry pentry=const_cast<PFNEntry&>(pbuf[i]);
	    try{
	      m_wchild->getImpl()->insertPFN( pentry );
	    }catch(const FCduplicatePFNException& dup){
	      //
	      //donot insert duplicated entries
	      //
	      fclog<<coral::Warning<<"skip importing entry: "<<pentry.pfname()<<" "<<dup.what()<<coral::MessageStream::endmsg;
	    }
	  }
	  pcounter+=pbuf.maxsize();
	}
      }else{
	while( cat->getImpl()->retrievePFN(query,pbuf,pcounter) ){
	  for(size_t i=0; i<pbuf.size(); ++i){
	    PFNEntry pentry=const_cast<PFNEntry&>(pbuf[i]);
	    try{
	      m_wchild->getImpl()->insertPFN( pentry );
	    }catch(const FCduplicatePFNException& dup){
	      //
	      //donot insert duplicated entries
	      //
	      fclog<<coral::Warning<<"skip importing entry: "<<pentry.pfname()<<" "<<dup.what()<<coral::MessageStream::endmsg;
	    }
	  }
	  pcounter+=pbuf.maxsize();
	}
      }
      if(importMeta){
	while( cat->getMetaImpl()->retrieveLFN(query,lbuf,lcounter) ){
	  for(size_t i=0; i<lbuf.size(); ++i){
	    LFNEntry lentry=const_cast<LFNEntry&>(lbuf[i]);
	    try{
	      m_wchild->getImpl()->insertLFN( lentry );
	    }catch(const FCduplicateLFNException& dup){
	      //
	      //donot insert duplicated entries
	      //
	      fclog<<coral::Warning<<"skip importing entry: "<<lentry.lfname()<<" "<<dup.what()<<coral::MessageStream::endmsg;
	    }
	  }
	  lcounter+=lbuf.maxsize();
	}
      }else{
	while( cat->getImpl()->retrieveLFN(query,lbuf,lcounter) ){
	  for(size_t i=0; i<lbuf.size(); ++i){
	    LFNEntry lentry=const_cast<LFNEntry&>(lbuf[i]);
	    try{
	      m_wchild->getImpl()->insertLFN( lentry );
	    }catch(const FCduplicateLFNException& dup){
	      //
	      //donot insert duplicated entries
	      //
	      fclog<<coral::Warning<<"skip importing entry: "<<lentry.lfname()<<" "<<dup.what()<<coral::MessageStream::endmsg;
	    }
	  }
	  lcounter+=lbuf.maxsize();
	}
      }
      if(importMeta){
	while( cat->getMetaImpl()->retrieveMetaData(query,mbuf,mcounter) ){
	  for(size_t i=0; i<mbuf.size(); ++i){
	    std::string myg=mbuf[i].guid();
	    try{
	      m_wchild->getMetaImpl()->insertMetaData( const_cast<MetaDataEntry&>(mbuf[i]) );
	    }catch(const FCduplicatemetadataException& dup){
	      //
	      //donot insert duplicated entries
	      //
	      fclog<<coral::Warning<<"skip importing entry: "<<myg<<" "<<dup.what()<<coral::MessageStream::endmsg;
	    }
	  }
	  mcounter+=mbuf.maxsize();
	}
      }
    }
    cat=static_cast<FCLeaf*> ( fc->getWriteCatalog() );
    if( cat!=0 ){
      FCBuf<PFNEntry> pbuf(cacheSize);
      FCBuf<LFNEntry> lbuf(cacheSize);
      FCBuf<MetaDataEntry> mbuf(cacheSize);
      size_t pcounter=0;
      while( cat->getImpl()->retrievePFN(query,pbuf,pcounter) ){
        size_t bufsize=pbuf.size();
        for(size_t i=0; i<bufsize; ++i){
	  PFNEntry pentry=const_cast<PFNEntry&>(pbuf[i]);
	  try{
	    m_wchild->getImpl()->insertPFN( pentry );
	  }catch(const FCduplicatePFNException& dup){
	    //
	    //donot insert duplicated entries
	    //
	    fclog<<coral::Warning<<"skip importing entry: "<<pentry.pfname()<<" "<<dup.what()<<coral::MessageStream::endmsg;
	  }
        }
        if( bufsize >= cacheSize){
          pcounter+=cacheSize;
        }else{
          break;
        }
      }
      size_t lcounter=0;
      while( cat->getImpl()->retrieveLFN(query,lbuf,lcounter) ){
        size_t bufsize=lbuf.size();
        for(size_t i=0; i<bufsize; ++i){
	  LFNEntry lentry=const_cast<LFNEntry&>(lbuf[i]);
	  try{
	    m_wchild->getImpl()->insertLFN( lentry );
	  }catch(const FCduplicateLFNException& dup){
	    //
	    //donot insert duplicated entries
	    //
	    fclog<<coral::Warning<<"skip importing entry: "<<lentry.lfname()<<" "<<dup.what()<<coral::MessageStream::endmsg;
	  }
        }
        if( bufsize >= cacheSize){
          lcounter+=cacheSize;
        }else{
          break;
        }
      }
      if(importMeta){
	size_t mcounter=0;
	while( cat->getMetaImpl()->retrieveMetaData(query,mbuf,mcounter) ){
	  size_t bufsize=mbuf.size();
	  for(size_t i=0; i<bufsize; ++i){
	    std::string myg=mbuf[i].guid();
	    try{
	      m_wchild->getMetaImpl()->insertMetaData(const_cast<MetaDataEntry&>(mbuf[i]));
	    }catch(const FCduplicatemetadataException& dup){
	      //
	      //donot insert duplicated entries
	      //
	      fclog<<coral::Warning<<"skip importing entry: "<<myg<<" "<<dup.what()<<coral::MessageStream::endmsg;
	    }
	  }
	  if( bufsize >= cacheSize){
	    mcounter+=cacheSize;
	  }else{
	    break;
	  }
	}
      }
    }
  }
  
  bool IFileCatalog::isReadOnly() const{
    return m_wchild==0;
  }

  void IFileCatalog::setAction( IFCAction& act ){
    act.visitFCComposite(this);
  }

   
  void IFileCatalog::connect(){
    coral::MessageStream fclog( "IFileCatalog");
    if(m_wchild) m_wchild->connect();
    if( m_rchildren.size()!=0 ) {
       ReadCatalogs	goodRCatalogs;
       std::string 	bad_catalogs_string;
       IFileCatalog::ReadCatalogIterator i;

       for(i=m_rchildren.begin(); i!=m_rchildren.end(); ++i) {
	  try {
	     ((FCLeaf*)(*i))->connect();
	     goodRCatalogs.push_back(*i);
	  }
	  catch(...) { 
	     fclog << coral::Warning << "Problem with the catalog: " 
		   << ((FCLeaf*)(*i))->getImpl()->url() << coral::MessageStream::endmsg;
	      bad_catalogs_string = bad_catalogs_string + ((FCLeaf*)(*i))->getImpl()->url() + " ";
	      delete *i;
	  }
       }
       m_rchildren.swap( goodRCatalogs );
       if( m_rchildren.size() != goodRCatalogs.size() ) {
	  bad_catalogs_string = " The following ReadCatalogs: " + bad_catalogs_string 
	    + "could have not been connected (probably they are non-existing). They have been removed from the list.";
	  throw pool::FCTransactionException( "IFileCatalog::start",
					      bad_catalogs_string );
       } 
    }
  }

   
  void IFileCatalog::disconnect() {
    if( m_rchildren.size()!=0 ){
      ReadCatalog_reverseIterator i;
      for(i=m_rchildren.rbegin();i!=m_rchildren.rend();++i){
        ((FCLeaf*)(*i))->disconnect();
      }
    }
    if(m_wchild) m_wchild->disconnect();
  }

   
  void IFileCatalog::start() {
    coral::MessageStream fclog( "IFileCatalog");
    if(m_wchild) m_wchild->start();
    if( m_rchildren.size()!=0 ) {
       ReadCatalogs	goodRCatalogs;
       std::string 	bad_catalogs_string;
       IFileCatalog::ReadCatalogIterator i;

       for( i=m_rchildren.begin(); i!=m_rchildren.end(); ++i) {
	  try {
	     ((FCLeaf*)(*i))->start();
	     goodRCatalogs.push_back(*i);
	  }
	  catch(...) { 
	     fclog << coral::Warning << "Problem with the catalog: " 
		   << ((FCLeaf*)(*i))->getImpl()->url() << coral::MessageStream::endmsg;
	     bad_catalogs_string = bad_catalogs_string + ((FCLeaf*)(*i))->getImpl()->url() + " ";
	     delete *i;
	  }
       }
       m_rchildren.swap( goodRCatalogs );
       if( m_rchildren.size() != goodRCatalogs.size() ) {
	  bad_catalogs_string = " The following ReadCatalogs: " + bad_catalogs_string 
	     + "could have not been started (probably they are non-existing). They have been removed from the list.";
	  fclog << coral::Info << "Some read catalogs could not be started. Remaining good catalogs are: ";
	  for( i=m_rchildren.begin(); i!=m_rchildren.end(); ++i) {
	     fclog << ((FCLeaf*)(*i))->getImpl()->url() << "  ";
	  }
	  fclog << coral::MessageStream::endmsg;
	  throw pool::FCTransactionException( "IFileCatalog::start",
					      bad_catalogs_string );
       } 
    }
  }
  void IFileCatalog::commit( FileCatalog::CommitMode cm ) {
    if( m_rchildren.size()!=0 ){
      ReadCatalog_reverseIterator i;
      for(i=m_rchildren.rbegin();i!=m_rchildren.rend();++i){
        ((FCLeaf*)(*i))->commit(cm);
      }
    }
    if(m_wchild) m_wchild->commit( cm );
  }
  void IFileCatalog::rollback() {
    if( m_rchildren.size()!=0 ){
      ReadCatalog_reverseIterator i;
      for(i=m_rchildren.rbegin();i!=m_rchildren.rend();++i){
        ((FCLeaf*)(*i))->rollback();
      }
    }
    if(m_wchild) m_wchild->rollback();
  }
  bool IFileCatalog::hasMetaDataCatalog() const{
    bool hasmeta=true;
    if( m_wchild ){
      if(!m_wchild->getMetaImpl() ){
	hasmeta=false;
      }
    }
    if( m_rchildren.size()!=0 ){
      if(!m_rchildren[0]->getMetaImpl() ){
	hasmeta=false;
      }
    } 
    return hasmeta;
  }
}//ns pool
