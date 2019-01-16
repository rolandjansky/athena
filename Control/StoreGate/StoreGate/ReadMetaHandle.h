/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_READMETAHANDLE_H
#define STOREGATE_READMETAHANDLE_H 1

#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/MetaCont.h"
#include "AthenaKernel/ExtendedEventContext.h"

#include "StoreGate/VarHandleBase.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadMetaHandleKey.h"

#include "GaudiKernel/DataHandle.h"
#include "GaudiKernel/DataObjID.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include <string>
#include <stdexcept>


namespace SG {

  template <typename T>
  class ReadMetaHandle : public SG::VarHandleBase {

  public: 
    typedef T*               pointer_type; // FIXME: better handling of
    typedef const T*   const_pointer_type; //        qualified T type ?
    typedef T&             reference_type;
    typedef const T& const_reference_type;

  public:
    ReadMetaHandle(const SG::ReadMetaHandleKey<T>& key);
    ReadMetaHandle(const SG::ReadMetaHandleKey<T>& key, 
                   const EventContext& ctx);
    
    virtual ~ReadMetaHandle() override {};
    
    // retrieve <T> for current SID in data header
    const_pointer_type retrieve();
    // retrieve <T> for specified SID
    const_pointer_type retrieve( const SG::SourceID& t);

    const_pointer_type  operator->()  { return  retrieve(); }
    const_pointer_type  operator*()   { return  retrieve(); }   

    
    virtual bool isValid() override;

  private:

    bool initMetaHandle();
        
    // current SID
    SG::SourceID m_sid;
    // pinter to container
    const MetaCont<T>*  m_cont {nullptr};
    // pointer to object in container for current SID
    T* m_ent {nullptr};
    
    const SG::ReadMetaHandleKey<T>& m_hkey;
  };


  //------------------------------------------------------------------------

  template <typename T>
  ReadMetaHandle<T>::ReadMetaHandle(const SG::ReadMetaHandleKey<T>& key):
  ReadMetaHandle(key, Gaudi::Hive::currentContext())
  { 
  }

  //------------------------------------------------------------------------

  template <typename T>
  ReadMetaHandle<T>::ReadMetaHandle(const SG::ReadMetaHandleKey<T>& key,
                                    const EventContext& ctx):
    SG::VarHandleBase( key, &ctx ),
    m_sid( ctx.getExtension<Atlas::ExtendedEventContext>().proxy()->sourceID() ),
    m_cont( key.getContainer() ),
    m_hkey(key)
  {
    MsgStream msg(Athena::getMessageSvc(), "ReadMetaHandle");
    if (! m_hkey.isInit()) {
      msg << MSG::ERROR 
          << "ReadMetaHandleKey " << key.objKey() << " was not initialized"
          << endmsg;
      throw std::runtime_error("ReadMetaHandle: ReadMetaHandleKey was not initialized");
    }

    if (m_cont == 0) {
      MsgStream msg(Athena::getMessageSvc(), "ReadMetaHandle");
      msg << MSG::ERROR
          << "ReadMetaHandle : ptr to MetaCont<T> is zero"
          << endmsg;
      throw std::runtime_error("ReadMetaHandle: ptr to MetaCont<T> is zero");
    }
  
  }

  //------------------------------------------------------------------------

  template <typename T>
  bool
  ReadMetaHandle<T>::initMetaHandle() 
  {

    if (m_ent != 0) return true;

    // Initialize sid from dbKey found at ReadMetaHandleKey initialize
    m_sid = m_hkey.dbKey();

    if ( ! m_cont->find(m_sid, m_ent) ) {
      std::ostringstream ost;
      MsgStream msg(Athena::getMessageSvc(), "ReadMetaHandle");
      msg << MSG::ERROR 
          << "ReadMetaHandle: could not find current SourceID " 
          << m_sid  << " for key " << objKey() << "\n"
          << endmsg;
      m_ent = nullptr;
      return false;
    }

    return true;
  }

  //------------------------------------------------------------------------

  template <typename T>
  const T*
  ReadMetaHandle<T>::retrieve() 
  {
    
    if (m_ent == 0) {
      if (!initMetaHandle()) {
        return nullptr;
      }
    }

    const_pointer_type cobj = 
      const_cast<const_pointer_type>( m_ent );      
    return cobj;
  }

  //------------------------------------------------------------------------

  template <typename T>
  const T*
  ReadMetaHandle<T>::retrieve(const SG::SourceID& sid) 
  {
    if (sid == m_sid) {
      return retrieve();
    }

    pointer_type obj(0);
    if (! (m_cont->find(sid, obj) ) ) {
      std::ostringstream ost;
      m_cont->list(ost);
      MsgStream msg(Athena::getMessageSvc(), "ReadMetaHandle");
      msg << MSG::ERROR 
          << "ReadMetaHandle::retrieve() could not find SourceID " 
          << sid  << " for key " << objKey() << "\n"
          << "Container contents: \n" 
          << ost.str()
          << "\n Done" 
          << endmsg;
      return nullptr;
    }
  
    const_pointer_type cobj = const_cast<const_pointer_type>( obj );

    return cobj;
  }

  //---------------------------------------------------------------------------

  template <typename T>
  bool 
  ReadMetaHandle<T>::isValid()  {

    return initMetaHandle();
  }

}

#endif
  
