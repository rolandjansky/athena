/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_READMETAHANDLE_H
#define STOREGATE_READMETAHANDLE_H

#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/MetaCont.h"
#include "AthenaKernel/ExtendedEventContext.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadMetaHandleKey.h"
#include "CxxUtils/AthUnlikelyMacros.h"

#include "GaudiKernel/DataHandle.h"
#include "GaudiKernel/DataObjID.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include <string>
#include <stdexcept>

namespace SG {

  template <typename T>
    class ReadMetaHandle {
    
  public: 
    typedef T*               pointer_type; // FIXME: better handling of
    typedef const T*   const_pointer_type; //        qualified T type ?
    typedef T&             reference_type;
    typedef const T& const_reference_type;

  public:
    ReadMetaHandle(const SG::ReadMetaHandleKey<T>& key);
    ReadMetaHandle(const SG::ReadMetaHandleKey<T>& key
		   , const EventContext& ctx);
    
    ~ReadMetaHandle() {};
    
    const_pointer_type retrieve();
    const_pointer_type retrieve( const SG::SourceID& t);

    const_pointer_type  operator->()  { return  retrieve(); }
    const_pointer_type  operator*()   { return  retrieve(); }   
    
    bool isValid();

  private:

    bool initMetaHandle();
        
    SG::SourceID m_sid;
    const MetaCont<T>*  m_cont {nullptr};
    T* m_ent {nullptr};
    
    const SG::ReadMetaHandleKey<T>& m_hkey;
  };


  //------------------------------------------------------------------------

  template <typename T>
    ReadMetaHandle<T>::ReadMetaHandle(const SG::ReadMetaHandleKey<T>& key)
    : ReadMetaHandle(key, Gaudi::Hive::currentContext())
    { 
    }

  //------------------------------------------------------------------------

  template <typename T>
  ReadMetaHandle<T>::ReadMetaHandle(const SG::ReadMetaHandleKey<T>& key
				    , const EventContext& ctx)
    : m_sid( Atlas::getExtendedEventContext(ctx).proxy()->sourceID() )
    , m_cont( key.getContainer() )
    , m_hkey(key)
  {
    MsgStream msg(Athena::getMessageSvc(), "ReadMetaHandle");
    if (ATH_UNLIKELY(!m_hkey.isInit())) {
      msg << MSG::ERROR 
	  << "ReadMetaHandleKey " << key.objKey() << " was not initialized"
	  << endmsg;
      throw std::runtime_error("ReadMetaHandle: ReadMetaHandleKey was not initialized");
    }

    if (ATH_UNLIKELY(m_cont == 0)) {
      // Try to retrieve it
      StoreGateSvc* ms = m_hkey.getStore();
      MetaContBase* cb{nullptr};
      if(ms->retrieve(cb, m_hkey.key()).isFailure()) {
	msg << MSG::ERROR
	    << "can't retrieve " << m_hkey.fullKey()
            << " via base class" << endmsg;
        throw std::runtime_error("ReadCondHandle: ptr to CondCont<T> is zero");
      }
      else {
	m_cont = dynamic_cast< MetaCont<T>* > (cb);
	if(!m_cont) {
	  msg << MSG::ERROR
	      << "can't dcast MetaContBase to " << m_hkey.fullKey()
              << endmsg;
          throw std::runtime_error("ReadMetaHandle: ptr to MetaCont<T> is zero");
	}
      }
    }
  }

  //------------------------------------------------------------------------

  template <typename T>
  bool
  ReadMetaHandle<T>::initMetaHandle() 
  {

    if (m_ent) return true;

    // Initialize sid from dbKey found at ReadMetaHandleKey initialize
//    m_sid = m_hkey.dbKey();    ???? Do I need this ????

    if ( ATH_UNLIKELY(! m_cont->find(m_sid, m_ent)) ) {
      std::ostringstream ost;
      m_cont->list(ost);
      MsgStream msg(Athena::getMessageSvc(), "ReadMetaHandle");
      msg << MSG::ERROR 
          << "ReadMetaHandle:: could not find current SourceID " 
          << m_sid  << " for key " << m_hkey.objKey() << "\n"
          << ost.str() << endmsg;
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

    return m_ent;
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
          << sid  << " for key " << m_hkey.objKey() << "\n"
          << ost.str()
          << endmsg;
      return nullptr;
    }

    return obj;
  }

  //---------------------------------------------------------------------------

  template <typename T>
  bool 
  ReadMetaHandle<T>::isValid()  {

    return initMetaHandle();
  }

}

#endif
  
