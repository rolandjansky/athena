/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileCatalog/FCCommon.h"
#include "FileCatalog/FCLeaf.h"
//#include "FileCatalog/IFileCatalogPluginFactory.h"
#include "FileCatalog/FCImpl.h"
#include "FileCatalog/FCMetaImpl.h"
//#include "FileCatalog/FCEntry.h"
//#include "FileCatalog/URIParser.h"
#include "FileCatalog/IFCAction.h"
//#include "CoralBase/MessageStream.h"

namespace pool{
  FCLeaf::FCLeaf( const std::string& contact):m_isreadonly(true){
    try{
      m_impl=FCImplBuilder::BuildFCImpl(contact);
      //assert m_impl not 0
      m_metaimpl=FCImplBuilder::BuildFCMetaImpl(m_impl);
    }catch(...){
      throw;
    }
  }
  FCLeaf::FCLeaf( FCImpl* impl, FCMetaImpl* metaimpl): m_isreadonly(true),m_impl(impl), m_metaimpl(metaimpl){
  }
  FCLeaf::~FCLeaf(){
    if (m_impl !=0 ) delete m_impl;
    if (m_metaimpl !=0 ) delete m_metaimpl;
  }
  void FCLeaf::setAction( IFCAction& act ){
    act.visitFCLeaf(this);
  }
  void FCLeaf::connect(){
    m_impl->connect();
  }
  void FCLeaf::disconnect(){
    m_impl->disconnect(); 
  }
  void FCLeaf::start(){
    m_impl->start();
  }
  void FCLeaf::commit(FileCatalog::CommitMode cm){
    m_impl->commit(cm);
  }
  void FCLeaf::rollback(){
    m_impl->rollback();
  }
  FCImpl* FCLeaf::getImpl() const{
    return m_impl;
  }
  FCMetaImpl* FCLeaf::getMetaImpl() const{
    return m_metaimpl;
  }
  bool FCLeaf::isReadOnly() const{
    m_isreadonly=m_impl->isReadOnly();
    return m_isreadonly;
  }
}//ns pool
