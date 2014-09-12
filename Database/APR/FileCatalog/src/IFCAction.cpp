/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_IFCACTION_H
#include "FileCatalog/IFCAction.h" 
#endif
#include "FileCatalog/FCLeaf.h"
#include "FileCatalog/FCException.h"
#include "FileCatalog/FCImpl.h"
#include "FileCatalog/FCMetaImpl.h"
#include "FileCatalog/FCEntry.h"
#include "FileCatalog/IFCContainer.h"
#include "FileCatalog/FCQueryParser.h"
//#include <iostream>
namespace pool{
  IFCAction::IFCAction():m_write(0){}
  IFCAction::~IFCAction(){
    m_read.clear();
  }
  void IFCAction::visitFCLeaf( IFileCatalog* cat){
    FCLeaf* me=static_cast<FCLeaf*>(cat);
    //m_read.push_back(me);
    m_write=me;
  }
  bool IFCAction::isWritableEntry( FileCatalog::FileID& fid ){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	return false;
      }else{
	std::string mypf; 
	std::string myftype;
	m_write->getImpl()->lookupBestPFN(fid,FileCatalog::WRITE,FileCatalog::SEQUENTIAL,mypf,myftype);
	if( mypf.empty() ){
	  return false; 
	}else{
	  return true;
	}
      }
    }else{
      return false;
    }
  }
  void IFCAction::visitFCComposite(IFileCatalog* cat){
    m_write=cat->m_wchild;
    m_read=cat->m_rchildren;
  }

  void IFCAction::registerPFN( const std::string& pfn, const std::string& filetype, FileCatalog::FileID& fid ){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "IFCAction::registerPFN","ERROR: the catalog is readonly" );
      }
      m_write->getImpl()->registerPFN(pfn,filetype,fid);
    }else{
      return ;
    }
  }
  void IFCAction::lookupFileByPFN(const std::string& pf, FileCatalog::FileID& fid, std::string& ftype) {
    if( m_write!=0 ){
      m_write->getImpl()->lookupFileByPFN(pf,fid,ftype);
      if( !fid.empty() ) return;
    }
    std::vector<FCLeaf*>::iterator i;
    for(i=m_read.begin(); i!=m_read.end(); ++i){
      (*i)->getImpl()->lookupFileByPFN(pf,fid,ftype);
      if( !fid.empty() ) return;
    }
  }
  void IFCAction::lookupFileByLFN(const std::string& lf, FileCatalog::FileID& fid){
    if( m_write!=0 ){
      m_write->getImpl()->lookupFileByLFN(lf,fid);
      if( !fid.empty() ) return;
    }
    std::vector<FCLeaf*>::iterator i;
    for(i=m_read.begin(); i!=m_read.end(); ++i){
      (*i)->getImpl()->lookupFileByLFN(lf,fid);
      if( !fid.empty() ) return;
    }
  }
  void IFCAction::lookupBestPFN(const FileCatalog::FileID& fid, const FileCatalog::FileOpenMode& omode, const FileCatalog::FileAccessPattern& amode, std::string& pf, std::string& filetype) {
    if( m_write!=0 ){
      m_write->getImpl()->lookupBestPFN(fid,omode,amode,pf,filetype);
      if( !pf.empty() ) return;
    }
    std::vector<FCLeaf*>::iterator i;
    for(i=m_read.begin(); i!=m_read.end(); ++i){
      (*i)->getImpl()->lookupBestPFN(fid, omode, amode, pf, filetype);
      if( !pf.empty() ) return;
    }
  }
  void IFCAction::getMetaDataSpec(MetaDataEntry& spec){
    if( m_write!=0 ){
      FCMetaImpl* pmeta=m_write->getMetaImpl();
      if(pmeta){
	pmeta->getMetaDataSpec(spec);
      }else{
	throw FCnonexistentMetaDataException("IFCAction::getMetaDataSpec" );
      }
      return;
    }
    if( m_read.size() !=0 ){
      std::vector<FCLeaf*>::iterator i;
      i=m_read.begin();
      FCMetaImpl* pmeta=(*i)->getMetaImpl();
      if(pmeta){
	pmeta->getMetaDataSpec(spec);
      }else{
	throw FCnonexistentMetaDataException("IFCAction::getMetaDataSpec" );
      }
      return;
    }
  }
  void FCregister::registerLFN( const std::string& pfn, const std::string& lfn ){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "FCregister::registerLFN","ERROR: the catalog is readonly" );
      }
      m_write->getImpl()->registerLFN(pfn,lfn);
    }else{
      return ;
    }
  }
  void FCregister::addReplicaPFN( const std::string& pfn, const std::string& lfn ){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "FCregister::addReplicaPFN","ERROR: the catalog is readonly" );
      }
      m_write->getImpl()->addReplicaPFN(pfn,lfn);
    }else{
      return ;
    }
  }
  void FCregister::renamePFN(const std::string& pf, const std::string& newpf){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "FCregister::renamePFN","ERROR: the catalog is readonly" );
      }
      m_write->getImpl()->renamePFN(pf,newpf);
    }else{
      return ;
    }
  }
  void FCregister::registerMetaData( const FileCatalog::FileID& fid, MetaDataEntry& meta){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "FCregister::registerMetaData","ERROR: the catalog is readonly" );
      }
      meta.setGuidValue(fid);
      FCMetaImpl* pmeta=m_write->getMetaImpl();
      if(pmeta){
	pmeta->insertMetaData(meta);
      }else{
	throw FCnonexistentMetaDataException("FCAdmin::registerMetaData" );
      }
    }else{
      return ;
    }
  }
  //////FClookup////
  void FClookup::lookupPFN(const FileCatalog::FileID& fid, PFNContainer& pfs){
    if(fid.empty()) return;
    std::string myfid(fid);
    myfid.insert(0,"'");myfid.append("'");
    pfs.setQuery( std::string("guid=")+myfid );
    return;
  }
  void FClookup::lookupLFN(const FileCatalog::FileID& fid, LFNContainer& lfs){
    if(fid.empty()) return;
    std::string myfid(fid);
    myfid.insert(0,"'");myfid.append("'");
    lfs.setQuery( std::string("guid=")+myfid );
    return;
  }
  void FClookup::lookupPFNByQuery(const std::string& query, PFNContainer& pfs){
    
    pfs.setQuery( query );
    return;
  }
  void FClookup::lookupLFNByQuery(const std::string& query, LFNContainer& lfs){
    lfs.setQuery( query );
    return;
  }
  void FClookup::lookupPFNByLFN(const std::string& lfn, PFNContainer& pfs){
    if(lfn.empty()) return;
    std::string mylfn(lfn);
    mylfn.insert(0,"'");mylfn.append("'");
    pfs.setQuery( std::string("lfname=")+mylfn );
    return;
  }
  void FClookup::lookupLFNByPFN(const std::string& pfn, LFNContainer& lfs){
    if(pfn.empty()) return;
    std::string mypfn(pfn);
    mypfn.insert(0,"'");mypfn.append("'");
    lfs.setQuery( std::string("pfname=")+mypfn );
    return;
  }
  void FClookup::lookupMetaDataByQuery(const std::string& query, MetaDataContainer& metas){
    metas.setQuery( query );
  }
  void FClookup::lookupFileByQuery(const std::string& query, GuidContainer& fids){
    fids.setQuery( query );
  }
  void FCAdmin::createMetaDataSpec( MetaDataEntry& schemadef){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "FCAdmin::createMetaDataSpec","ERROR: the catalog is readonly" );
      }
      FCMetaImpl* pmeta=m_write->getMetaImpl();
      if(pmeta){
	pmeta->createMetaDataSpec(schemadef);
      }else{
	throw FCnonexistentMetaDataException("FCAdmin::createMetaDataSpec" );
      }
    }else{
      return ;
    }
  }
  void FCAdmin::updateMetaDataSpec( MetaDataEntry& schemadef, const FileCatalog::MetaUpdateMode metamode){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "FCAdmin::updateMetaDataSpec","ERROR: the catalog is readonly" );
      }
      FCMetaImpl* pmeta=m_write->getMetaImpl();
      if(pmeta){
	pmeta->updateMetaDataSpec(schemadef,metamode);
      }else{
	throw FCnonexistentMetaDataException("FCAdmin::updateMetaDataSpec" );
      }
    }else{
      return ;
    }
  }
  void FCAdmin::dropMetaDataSpec(){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "FCAdmin::dropMetaDataSpec","ERROR: the catalog is readonly" );
      }
      FCMetaImpl* pmeta=m_write->getMetaImpl();
      if(pmeta){
	pmeta->dropMetaDataSpec();
      }else{
	throw FCnonexistentMetaDataException("FCAdmin::dropMetaDataSpec" );
      }
    }else{
      return ;
    }
  }
  void FCAdmin::deletePFN(const std::string&  pf){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "FCAdmin::deletePFN","ERROR: the catalog is readonly" );
      }
      m_write->getImpl()->deletePFN(pf);
    }else{
      return ;
    }
  }
  void FCAdmin::deleteLFN(const std::string& lf){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "FCAdmin::deleteLFN","ERROR: the catalog is readonly" );
      }
      m_write->getImpl()->deleteLFN(lf);
    }else{
      return ;
    }
  }
  void FCAdmin::deleteMetaData(const FileCatalog::FileID& fid){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "FCAdmin::deleteMetaData","ERROR: the catalog is readonly" );
      }
      FCMetaImpl* pmeta=m_write->getMetaImpl();
      if(pmeta){
	pmeta->deleteMetaData(fid);
      }else{
	throw FCnonexistentMetaDataException("FCAdmin::deleteMetaData" );
      }
    }else{
      return ;
    }
  }
  void FCAdmin::deleteEntry( const FileCatalog::FileID& fid ){
    if( m_write!=0 ){
      if( m_write->isReadOnly() ){
	throw FCTransactionException ( "FCAdmin::deleteEntry","ERROR: the catalog is readonly" );
      }
      m_write->getImpl()->deleteEntry(fid);
      FCMetaImpl* pmeta=m_write->getMetaImpl();
      if(pmeta){
	pmeta->deleteMetaData(fid);
      }else{
	throw FCnonexistentMetaDataException("FCAdmin::deleteEntry" );
      }
    }else{
      return ;
    }
  }
}//ns pool
