/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_WRITECONDHANDLE_H
#define STOREGATE_WRITECONDHANDLE_H 1

#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/getMessageSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/DataHandle.h"
#include "GaudiKernel/DataObjID.h"
#include "GaudiKernel/EventIDBase.h"

#include <string>
#include <stdexcept>

namespace SG {

  template <typename T>
  class WriteCondHandle {

  public: 
    typedef T*               pointer_type; // FIXME: better handling of
    typedef const T*   const_pointer_type; //        qualified T type ?
    typedef T&             reference_type;
    typedef const T& const_reference_type;

  public:
    WriteCondHandle(const WriteCondHandleKey<T>& key);
    WriteCondHandle(const WriteCondHandleKey<T>& key, const EventContext& ctx);
    
    ~WriteCondHandle() {};   

    const std::string& key() const { return m_hkey.key(); }
    const DataObjID& fullKey() const { return m_hkey.fullKey(); }

    bool isValid();
    bool isValid(const EventIDBase& t) const;

    StatusCode record(const EventIDRange& range, T* t);
    StatusCode record(const EventIDRange& range, std::unique_ptr<T> t);
    void updateStore();
    
    const std::string& dbKey() const { return m_hkey.dbKey(); }
    
  private:

    const EventIDBase& m_eid;
    CondCont<T>* m_cc {nullptr};
    StoreGateSvc* m_cs {nullptr};

    const SG::WriteCondHandleKey<T>& m_hkey;
    
  };
  

  //---------------------------------------------------------------------------

  template <typename T>
  WriteCondHandle<T>::WriteCondHandle( const SG::WriteCondHandleKey<T>& key ) :
    WriteCondHandle( key, Gaudi::Hive::currentContext() ) {}

  //---------------------------------------------------------------------------

  template <typename T>
  WriteCondHandle<T>::WriteCondHandle( const SG::WriteCondHandleKey<T>& key,
                                       const EventContext& ctx) :
    m_eid(ctx.eventID()),
    m_cc( key.getCC() ),
    m_cs( key.getCS() ),
    m_hkey(key)
  {
    if (m_cc == 0) {
      MsgStream msg(Athena::getMessageSvc(), "WriteCondHandle");
      msg << MSG::ERROR 
          << "WriteCondHandle : ptr to CondCont<T> is zero"
          << endmsg;
    }

    if (! m_hkey.isInit()) {
      MsgStream msg(Athena::getMessageSvc(), "WriteCondHandle");
      msg << MSG::ERROR 
          << "WriteCondHandleKey " << key.objKey() << " was not initialized"
          << endmsg;
      throw std::runtime_error("WriteCondHandle: WriteCondHandleKey was not initialized");

    }
    
  }


  //---------------------------------------------------------------------------

  template <typename T>
  StatusCode
  WriteCondHandle<T>::record(const EventIDRange& r, std::unique_ptr<T> t)
  {
    MsgStream msg(Athena::getMessageSvc(), "WriteCondHandle");
    msg << MSG::DEBUG
        << "WriteCondHandle::record() : obj at: " << t.get() << "  range: " << r 
        << endmsg;

    if (!m_cc->insert(r, std::move(t))) {
      msg << MSG::ERROR 
          << "WriteCondHandle::record() : unable to insert obj in CondCont<T>"
          << endmsg;
      return StatusCode::FAILURE;
    }
 
    updateStore();
 
    return StatusCode::SUCCESS;
  }

  
  template <typename T>
  StatusCode
  WriteCondHandle<T>::record(const EventIDRange& r, T* t)
  {
    return record (r, std::unique_ptr<T> (t));
  }

  //---------------------------------------------------------------------------

  template <typename T>
  void
  WriteCondHandle<T>::updateStore() {
    m_cs->addedNewTransObject( m_hkey.fullKey().clid(), m_hkey.fullKey().key() );
  }


  //---------------------------------------------------------------------------

  template <typename T>
  bool 
  WriteCondHandle<T>::isValid(const EventIDBase& t) const {

    return (m_cc->valid(t));
  }


  //---------------------------------------------------------------------------

  template <typename T>
  bool 
  WriteCondHandle<T>::isValid() {

    return (m_cc->valid(m_eid));
  }

}

#endif
  
