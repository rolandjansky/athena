/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_WRITECONDHANDLE_H
#define STOREGATE_WRITECONDHANDLE_H 1

#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/getMessageSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/ReadCondHandle.h"

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

    bool isValid() const;
    bool isValid(const EventIDBase& t) const;

    bool isValid(EventIDRange& range) const;
    bool isValid(const EventIDBase& t, EventIDRange& range) const;

    void addDependency(const EventIDRange& range);

    template <typename R>
    void addDependency(SG::ReadCondHandle<R>& rch);

    template <typename R, typename... Args>
    void addDependency(ReadCondHandle<R>& rch, Args... args);
    


    /**
     * @brief record handle, with explicit range   DEPRECATED
     * @param range IOVRange of handle
     * @param t unique_ptr to handle
     */
    StatusCode record(const EventIDRange& range, T* t);
    StatusCode record(const EventIDRange& range, std::unique_ptr<T> t);
    
    /**
     * @brief record handle, range must have been set by addDependency(...)
     * @param t unique_ptr to handle
     */
    StatusCode record(std::unique_ptr<T> t);
    StatusCode record(T* t);

    /**
     * @brief Extend the range of the last IOV.
     * @param range New validity range.
     * @param ctx Event context.
     *
     * Returns failure if the start time of @c range does not match the start time
     * of the last IOV in the container.  Otherwise, the end time for the last
     * IOV is changed to the end time for @c range.  (If the end time for @c range
     * is before the end of the last IOV, then nothing is changed.)
     */
    StatusCode extendLastRange(const EventIDRange& range,
                               const EventContext& ctx = Gaudi::Hive::currentContext());
    
    const std::string& dbKey() const { return m_hkey.dbKey(); }

    const EventIDRange& getRange() const { return m_range; }
    
  private:

    const EventContext& m_ctx;
    CondCont<T>* m_cc {nullptr};

    const SG::WriteCondHandleKey<T>& m_hkey;

    EventIDRange m_range{};
    bool m_rangeSet {false};
    
  };
  
  //---------------------------------------------------------------------------

  template <typename T>
  WriteCondHandle<T>::WriteCondHandle( const SG::WriteCondHandleKey<T>& key ) :
    WriteCondHandle( key, Gaudi::Hive::currentContext() ) {}

  //---------------------------------------------------------------------------

  template <typename T>
  WriteCondHandle<T>::WriteCondHandle( const SG::WriteCondHandleKey<T>& key,
                                       const EventContext& ctx) :
    m_ctx(ctx),
    m_cc( key.getCC() ),
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
    
    if (m_rangeSet) {
      MsgStream msg(Athena::getMessageSvc(), "WriteCondHandle");
      msg << MSG::ERROR
          << "WriteCondHandle::record(EventIDRange, T*): for key "
          << this->fullKey()
          << " cannot use this method if range has already been set via dependencies"
          << endmsg;
      return StatusCode::FAILURE;
    }

    m_range = r;
    m_rangeSet = true;

    return record( std::move(t) );
  }
  
  template <typename T>
  StatusCode
  WriteCondHandle<T>::record(const EventIDRange& r, T* t)
  {
    return record (r, std::unique_ptr<T> (t));
  }

  template <typename T>
  StatusCode
  WriteCondHandle<T>::record(T* t) {
    return record (std::unique_ptr<T> (t));    
  }

  template <typename T>
  StatusCode
  WriteCondHandle<T>::record(std::unique_ptr<T> t) {
    if (! m_rangeSet) {
      MsgStream msg(Athena::getMessageSvc(), "WriteCondHandle");
      msg << MSG::ERROR 
          << "WriteCondHandle::record() : no range defined for key "
          << this->fullKey()
          << endmsg;
      return StatusCode::FAILURE;
    }

    #ifndef NDEBUG
    MsgStream msg(Athena::getMessageSvc(), "WriteCondHandle");
    if (msg.level() <= MSG::DEBUG) {
      msg << MSG::DEBUG
          << "WriteCondHandle::record() : obj at: " << t.get() << "  range: "
          << m_range << endmsg;
    }
    #endif

    StatusCode sc = m_cc->insert(m_range, std::move(t));
    // Preserve sc for return, since it may be DUPLICATE.
    if (sc.isFailure()) {
      MsgStream msg(Athena::getMessageSvc(), "WriteCondHandle");
      msg << MSG::ERROR 
          << "WriteCondHandle::record() : unable to insert obj in CondCont<T>"
          << endmsg;
      return StatusCode::FAILURE;
    } else if (CondContBase::Category::isOverlap (sc)) {
#if 0
      // Temporarily disable this check until caching issues with IOVDbSvc
      // are sorted out.
      MsgStream msg(Athena::getMessageSvc(), "WriteCondHandle");
      msg << MSG::ERROR 
          << "WriteCondHandle::record() : IOV ranges overlap."
          << endmsg;
      return StatusCode::FAILURE;
#endif
      sc = StatusCode::SUCCESS;
    }

    return sc;
  }
             
  //---------------------------------------------------------------------------


  /**
   * @brief Extend the range of the last IOV.
   * @param range New validity range.
   * @param ctx Event context.
   *
   * Returns failure if the start time of @c range does not match the start time
   * of the last IOV in the container.  Otherwise, the end time for the last
   * IOV is changed to the end time for @c range.  (If the end time for @c range
   * is before the end of the last IOV, then nothing is changed.)
   */
  template <typename T>
  StatusCode
  WriteCondHandle<T>::extendLastRange (const EventIDRange& r, const EventContext& ctx)
  {
    return m_cc->extendLastRange (r, ctx);
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
  WriteCondHandle<T>::isValid() const {

    return (m_cc->valid(m_ctx.eventID()));
  }

  //---------------------------------------------------------------------------

  template <typename T>
  bool 
  WriteCondHandle<T>::isValid(const EventIDBase& t, EventIDRange& range) const {

    return (m_cc->range(t, range));
  }


  //---------------------------------------------------------------------------

  template <typename T>
  bool 
  WriteCondHandle<T>::isValid (EventIDRange& range) const {

    return (m_cc->range(m_ctx.eventID(), range));
  }

  //---------------------------------------------------------------------------

  template <typename T>
  void WriteCondHandle<T>::addDependency(const EventIDRange& range) {
    if ( !m_rangeSet ) {
      m_range = range;
    } else {
      m_range = EventIDRange::intersect(m_range, range);
    }
    m_rangeSet = true;
  }

  // Can't take a const RCH, as RCH.range() can load the ptr.
  template <typename T>
  template< typename R>
  void
  WriteCondHandle<T>::addDependency(SG::ReadCondHandle<R>& rch) {
    return addDependency(rch.getRange());
  }

  template< typename T>
  template <typename R, typename... Args>
  void
  WriteCondHandle<T>::addDependency(ReadCondHandle<R>& rch, Args... args) {
     addDependency( rch );
     return addDependency( args... );
  }
             
             
}

#endif
  
