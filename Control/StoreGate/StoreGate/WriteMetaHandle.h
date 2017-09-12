/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_WRITEMETAHANDLE_H
#define STOREGATE_WRITEMETAHANDLE_H 1

#include "AthenaKernel/MetaCont.h"
#include "AthenaKernel/getMessageSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/VarHandleBase.h"
#include "StoreGate/WriteMetaHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/DataHandle.h"
#include "GaudiKernel/DataObjID.h"

#include <string>
#include <stdexcept>

namespace SG {

  template <typename T>
  class WriteMetaHandle : public SG::VarHandleBase {

  public: 
    typedef T*               pointer_type; // FIXME: better handling of
    typedef const T*   const_pointer_type; //        qualified T type ?
    typedef T&             reference_type;
    typedef const T& const_reference_type;

  public:
    WriteMetaHandle(const WriteMetaHandleKey<T>& key);
    WriteMetaHandle(const WriteMetaHandleKey<T>& key, const EventContext& ctx);
    
    virtual ~WriteMetaHandle() override {};   

    virtual bool isValid() override;
    bool isValid(const MetaContBase::SourceID& t) const;

    StatusCode record(const MetaContBase::SourceID& range, T* t);
    void updateStore();
    
    const std::string& dbKey() const { return m_hkey.dbKey(); }
    
  private:

    const MetaContBase::SourceID& m_sid;
    MetaCont<T>* m_cont {nullptr};
    StoreGateSvc* m_cs {nullptr};

    const SG::WriteMetaHandleKey<T>& m_hkey;
    
  };
  

  //---------------------------------------------------------------------------

  template <typename T>
  WriteMetaHandle<T>::WriteMetaHandle( const SG::WriteMetaHandleKey<T>& key ) :
    WriteMetaHandle( key, Gaudi::Hive::currentContext() ) {}

  //------------------------------------------------------------------------

  template <typename T>
  WriteMetaHandle<T>::WriteMetaHandle( const SG::WriteMetaHandleKey<T>& key,
                                       const EventContext& ctx) :
    SG::VarHandleBase( key, &ctx ),
    m_sid( MetaContBase::SourceID("") ),
    m_cont( key.getContainer() ),
    m_cs( key.getStore() ),
    m_hkey(key)
  {
    if (m_cont == 0) {
      MsgStream msg(Athena::getMessageSvc(), "WriteMetaHandle");
      msg << MSG::ERROR 
          << "WriteMetaHandle : ptr to MetaCont<T> is zero"
          << endmsg;
    }

    if (! m_hkey.isInit()) {
      MsgStream msg(Athena::getMessageSvc(), "WriteMetaHandle");
      msg << MSG::ERROR 
          << "WriteMetaHandleKey " << key.objKey() << " was not initialized"
          << endmsg;
      throw std::runtime_error("WriteMetaHandle: WriteMetaHandleKey was not initialized");

    }
    
  }


  //------------------------------------------------------------------------

  template <typename T>
  StatusCode
  WriteMetaHandle<T>::record(const MetaContBase::SourceID& r, T* t) {

    if (!m_cont->insert(r, t)) {
      MsgStream msg(Athena::getMessageSvc(), "WriteMetaHandle");
      msg << MSG::ERROR 
          << "WriteMetaHandle::record() : unable to insert obj in MetaCont<T>"
          << endmsg;
      return StatusCode::FAILURE;
    }
 

    MsgStream msg(Athena::getMessageSvc(), "WriteMetaHandle");
    msg << MSG::DEBUG
        << "WriteMetaHandle::record() : obj at: " << t << "  range: " << r 
        << endmsg;

    updateStore();
 
    return StatusCode::SUCCESS;
  }

  //------------------------------------------------------------------------

  template <typename T>
  void
  WriteMetaHandle<T>::updateStore() {
    m_cs->addedNewTransObject( fullKey().clid(), fullKey().key() );
  }


  //------------------------------------------------------------------------

  template <typename T>
  bool 
  WriteMetaHandle<T>::isValid(const MetaContBase::SourceID& t) const {

    return (m_cont->valid(t));
  }


  //------------------------------------------------------------------------

  template <typename T>
  bool 
  WriteMetaHandle<T>::isValid() {

    return (m_cont->valid(m_sid));
  }

}

#endif
  
