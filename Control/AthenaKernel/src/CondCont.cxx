/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/src/CondCont.cpp
 * @author Vakho, Charles, Scott
 * @date Apr, 2018
 * @brief Hold mappings of ranges to condition objects.
 */


#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "CxxUtils/AthUnlikelyMacros.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>


/// Default name of the global conditions cleaner service.
std::string CondContBase::s_cleanerSvcName = "Athena::ConditionsCleanerSvc";


/**
 * @brief Test if two ranges overlap, and adjust if needed.
 * @param ctx Event context passed to emplace().
 * @param oldRange An existing range in the container.
 * @param newRange New range being added.
 *
 * Returns one of:
 *   0 -- no overlap between the ranges; NEWRANGE is unmodified.
 *   1 -- ranges overlap.  NEWRANGE has been adjusted to avoid the overlap.
 *        If the start of NEWRANGE is changed, it must
 *        only be moved forward (increased), never backwards.
 *  -1 -- duplicate: NEWRANGE is entirely inside OLDRANGE.
 *        Delete the new range.         
 */
int CondContBase::Compare::overlap (const EventContext& ctx,
                                    const RangeKey& oldRange,
                                    RangeKey& newRange) const
{
  //                              |--- oldRange ---|
  //  |--- newRange ---|
  if (newRange.m_stop <= oldRange.m_start) {
    return 0;
  }
        
  //  |--- oldRange ---|
  //                        |--- newRange ---|
  if (oldRange.m_stop <= newRange.m_start) {
    return 0;
  }

  //   |---     oldRange    ---|
  //      |--- newRange ---|
  if (newRange.m_start >= oldRange.m_start &&
      newRange.m_stop <= oldRange.m_stop)
  {
    return -1;
  }

  // Current IOV key.
  key_type cur;
  if (newRange.m_range.start().isTimeStamp()) {
    cur = keyFromTimestamp (ctx.eventID());
  }
  else {
    EventIDBase eid = ctx.eventID();
    if (ctx.hasExtension()) {
      EventIDBase::number_type conditionsRun =
        Atlas::getExtendedEventContext (ctx).conditionsRun();
      if (conditionsRun != EventIDBase::UNDEFNUM) {
        eid.set_run_number (conditionsRun);
      }
    }
    cur = keyFromRunLBN (eid);
  }

  // Either this:
  //      |---     oldRange    ---|
  //                 |--- newRange ---|
  //
  // Or this
  //      |---     oldRange    ---|
  //  |---               newRange ---|
  //  with the current IOV key after the end of oldRange.
  //
  // In either case, we start the start of newRange to the end of oldRange.
  if (newRange.m_stop > oldRange.m_stop &&
      (newRange.m_start >= oldRange.m_start ||
       cur > oldRange.m_stop))
  {
    newRange.m_start = oldRange.m_stop;
    if (oldRange.m_range.stop().isTimeStamp()) {
      // If this is a mixed key, we need to copy only the timestamp part.
      EventIDBase oldId = oldRange.m_range.stop();
      EventIDBase newId = newRange.m_range.start();
      newRange.m_range = EventIDRange (EventIDBase (newId.run_number(),
                                                    newId.event_number(),
                                                    oldId.time_stamp(),
                                                    oldId.time_stamp_ns_offset(),
                                                    newId.lumi_block(),
                                                    newId.bunch_crossing_id()),
                                       newRange.m_range.stop());
    }
    else {
      newRange.m_range = EventIDRange (oldRange.m_range.stop(),
                                       newRange.m_range.stop());
    }
  }


  //              |---     oldRange    ---|
  //      |--- newRange  ---|
  else if (newRange.m_start < oldRange.m_start &&
           newRange.m_stop > oldRange.m_start)
  {
    // Change the end of newRange to the start of oldRange.
    newRange.m_stop = oldRange.m_start;
    if (oldRange.m_range.start().isTimeStamp()) {
      // If this is a mixed key, we need to copy only the timestamp part.
      EventIDBase oldId = oldRange.m_range.start();
      EventIDBase newId = newRange.m_range.stop();
      newRange.m_range = EventIDRange (newRange.m_range.start(),
                                       EventIDBase (newId.run_number(),
                                                    newId.event_number(),
                                                    oldId.time_stamp(),
                                                    oldId.time_stamp_ns_offset(),
                                                    newId.lumi_block(),
                                                    newId.bunch_crossing_id()));
    }
    else {
      newRange.m_range = EventIDRange (newRange.m_range.start(),
                                       oldRange.m_range.start());
    }
  }

  // Should have handled all cases by now!
  else {
    std::abort();
  }

  return 1;
}


/**
 * @brief Possibly extend an existing range at the end.
 * @param range THe existing range.
 * @param newRange Range being added.
 *
 * Returns one of:
 *   0 -- no change was made to RANGE.
 *   1 -- RANGE was extended. 
 *  -1 -- newRange is a duplicate.
 */
int CondContBase::Compare::extendRange (RangeKey& range,
                                        const RangeKey& newRange) const
{
  //      |--- range ---|
  //  |--- newRange ....
  if (newRange.m_start < range.m_start) {
    return 0;
  }

  //      |--- range ---|
  //                         |--- newRange ....
  if (newRange.m_start > range.m_stop) {
    return 0;
  }

  //      |--- range ----------------|
  //         |--- newRange ---|
  if (newRange.m_stop <= range.m_stop) {
    return -1;
  }

  //      |--- range ----|
  //         |--- newRange ---|
  range.m_stop = newRange.m_stop;
  range.m_range = EventIDRange (range.m_range.start(),
                                newRange.m_range.stop());
  return 1;
}


/**
 * @brief Name of the category.
 */
const char* CondContBase::Category::name() const
{
  return "CondCont";
}


/**
 * @brief Description for code within this category.
 */
std::string CondContBase::Category::message (code_t code) const
{
  if (code == static_cast<code_t> (CondContStatusCode::DUPLICATE)) {
    return "DUPLICATE";
  }
  else if (code == static_cast<code_t> (CondContStatusCode::OVERLAP)) {
    return "OVERLAP";
  }
  else if (code == static_cast<code_t> (CondContStatusCode::EXTENDED)) {
    return "EXTENDED";
  }
  return StatusCode::Category::message (code);
}


/**
 * @brief Is code considered success?
 */
bool CondContBase::Category::isSuccess (code_t code) const
{
  return code == static_cast<code_t>( CondContStatusCode::DUPLICATE ) ||
    code == static_cast<code_t>( CondContStatusCode::OVERLAP ) ||
    code == static_cast<code_t>( CondContStatusCode::EXTENDED ) ||
    code == static_cast<code_t>( CondContStatusCode::SUCCESS );
}


/// Helper to test whether a code is DUPLICATE.
bool CondContBase::Category::isDuplicate (code_t code)
{
  return code == static_cast<code_t> (CondContStatusCode::DUPLICATE);
}


/// Helper to test whether a code is DUPLICATE.
bool CondContBase::Category::isDuplicate (StatusCode code)
{
  return isDuplicate (code.getCode());
}


/// Helper to test whether a code is OVERLAP.
bool CondContBase::Category::isOverlap (code_t code)
{
  return code == static_cast<code_t> (CondContStatusCode::OVERLAP);
}


/// Helper to test whether a code is OVERLAP.
bool CondContBase::Category::isOverlap (StatusCode code)
{
  return isOverlap (code.getCode());
}


/// Helper to test whether a code is EXTENDED.
bool CondContBase::Category::isExtended (code_t code)
{
  return code == static_cast<code_t> (CondContStatusCode::EXTENDED);
}


/// Helper to test whether a code is EXTENDED.
bool CondContBase::Category::isExtended (StatusCode code)
{
  return isExtended (code.getCode());
}


STATUSCODE_ENUM_IMPL (CondContStatusCode, CondContBase::Category)


/**
 * @brief Set the associated @c DataProxy.
 * @param proxy The proxy to set.
 */
void CondContBase::setProxy (SG::DataProxy* proxy)
{
  m_proxy = proxy;
}


/**
 * @brief Dump the container to cout.  For calling from the debugger.
 */
void CondContBase::print() const
{
  list (std::cout);
}


/**
 * @brief Return the number of conditions objects in the container.
 */
size_t CondContBase::entries() const
{
  return m_condSet.size();
}


/**
 * @brief Remove unused entries from the front of the list.
 * @param keys List of keys that may still be in use.
 *             (Must be sorted.)
 *
 * We examine the objects in the container, starting with the earliest one.
 * If none of the keys in @c keys match the range for this object, then
 * it is removed from the container.  We stop when we either find
 * an object with a range matching a key in @c keys or when there
 * is only one object left.
 *
 * The list @c keys should contain keys as computed by keyFromRunLBN
 * or keyFromTimestamp, as appropriate for the container's key type
 * (as returned from keyType()).  The list must be sorted.
 *
 * Removed objects are queued for deletion once all slots have been
 * marked as quiescent.
 *
 * Returns the number of objects that were removed.
 */
size_t CondContBase::trim (const std::vector<key_type>& keys)
{
  return m_condSet.trim (keys);
}


/**
 * @brief Mark that this thread is no longer accessing data from this container.
 * @param ctx Event context for the current thread.
 *
 * This would normally be done through RCU service.
 * Defined here for purposes of testing.
 */
void
CondContBase::quiescent (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  m_condSet.quiescent (ctx);
}


/**
 * @brief Return the number times an item was inserted into the map.
 */
size_t CondContBase::nInserts() const
{
  return m_condSet.nInserts();
}


/**
 * @brief Return the maximum size of the map.
 */
size_t CondContBase::maxSize() const
{
  return m_condSet.maxSize();
}


/**
 * @brief Internal constructor.
 * @param rcusvc RCU service instance.
 * @param keyType Key type for this container.
 * @param CLID of the most-derived @c CondCont.
 * @param id CLID+key for this object.
 * @param proxy @c DataProxy for this object.
 * @param delfcn Deletion function for the actual payload type.
 * @param capacity Initial capacity of the container.
 */
CondContBase::CondContBase (Athena::IRCUSvc& rcusvc,
                            KeyType keyType,
                            CLID clid,
                            const DataObjID& id,
                            SG::DataProxy* proxy,
                            CondContSet::delete_function* delfcn,
                            size_t capacity)
  : m_keyType (keyType),
    m_clid (clid),
    m_id (id),
    m_proxy (proxy),
    m_condSet (Updater_t (rcusvc, delfcn), delfcn, capacity),
    m_cleanerSvc (s_cleanerSvcName, "CondContBase")
{
  if (!m_cleanerSvc.retrieve().isSuccess()) {
    std::abort();
  }
}


/** 
 * @brief Insert a new conditions object.
 * @param r Range of validity of this object.
 * @param t Pointer to the object being inserted.
 * @param ctx Event context for the current thread.
 *
 * Returns SUCCESS if the object was successfully inserted;
 * EXTENDED if the last existing range in the container was extended
 * to match the new range;
 * OVERLAP if the object was inserted but the range partially overlaps
 * with an existing one;
 * DUPLICATE if the object wasn't inserted because the range
 * duplicates an existing one, and FAILURE otherwise
 * (ownership of the object will be taken in any case).
 */
StatusCode
CondContBase::insertBase (const EventIDRange& r,
                          CondContSet::payload_unique_ptr t,
                          const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  EventIDBase start = r.start();
  EventIDBase stop = r.stop();

  key_type start_key, stop_key;

  if (m_keyType == KeyType::MIXED) {
    if (start.run_number() == EventIDBase::UNDEFNUM ||
        stop.run_number() == EventIDBase::UNDEFNUM)
    {
      MsgStream msg (Athena::getMessageSvc(), title());
      msg << MSG::ERROR << "CondContBase::insertBase: "
          << "Run part of range invalid in mixed container."
          << endmsg;
      return StatusCode::FAILURE;
    }

    start_key = keyFromRunLBN (start);
    stop_key  = keyFromRunLBN (stop);
  }
  else if (start.isTimeStamp() && stop.isTimeStamp()) {
    if (m_keyType == KeyType::SINGLE) {
      m_keyType = KeyType::TIMESTAMP;
    }
    else if (m_keyType != KeyType::TIMESTAMP) {
      MsgStream msg (Athena::getMessageSvc(), title());
      msg << MSG::ERROR << "CondContBase::insertBase: "
          << "Timestamp key used in non-timestamp container."
          << endmsg;
      return StatusCode::FAILURE;
    }

    start_key = keyFromTimestamp (start);
    stop_key  = keyFromTimestamp (stop);
  }

  else if (start.run_number() != EventIDBase::UNDEFNUM &&
           stop.run_number() != EventIDBase::UNDEFNUM)
  {
    if (m_keyType == KeyType::SINGLE) {
      m_keyType = KeyType::RUNLBN;
    }
    else if (m_keyType != KeyType::RUNLBN) {
      MsgStream msg (Athena::getMessageSvc(), title());
      msg << MSG::ERROR << "CondContBase::insertBase: "
          << "Run/LBN key used in non-Run/LBN container."
          << endmsg;
      return StatusCode::FAILURE;
    }

    start_key = keyFromRunLBN (start);
    stop_key  = keyFromRunLBN (stop);
  }

  else {
    MsgStream msg (Athena::getMessageSvc(), title());
    msg << MSG::ERROR << "CondContBase::insertBase: EventIDRange " << r 
        << " is neither fully RunEvent nor TimeStamp" 
        << endmsg;
    return StatusCode::FAILURE;
  }

  CondContSet::EmplaceResult reslt =
    m_condSet.emplace( RangeKey(r, start_key, stop_key),
                       std::move(t),
                       m_keyType != KeyType::MIXED,
                       ctx );

  if (reslt == CondContSet::EmplaceResult::DUPLICATE)
  {
    return CondContStatusCode::DUPLICATE;
  }
  else if (reslt == CondContSet::EmplaceResult::EXTENDED)
  {
    return CondContStatusCode::EXTENDED;
  }
  else if (reslt == CondContSet::EmplaceResult::OVERLAP) {
    return CondContStatusCode::OVERLAP;
  }

  return this->inserted (ctx);
}


/**
 * @brief Erase the first element not less than @c t.
 * @param IOV time of element to erase.
 * @param ctx Event context for the current thread.
 */
StatusCode
CondContBase::eraseBase (const EventIDBase& t,
                         const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  switch (m_keyType) {
  case KeyType::RUNLBN:
    if (!t.isRunLumi()) {
      MsgStream msg (Athena::getMessageSvc(), title());
      msg << MSG::ERROR << "CondContBase::erase: "
          << "Non-Run/LBN key used in Run/LBN container."
          << endmsg;
      return StatusCode::FAILURE;
    }
    m_condSet.erase (CondContBase::keyFromRunLBN (t), ctx);
    break;
  case KeyType::TIMESTAMP:
    if (!t.isTimeStamp()) {
      MsgStream msg (Athena::getMessageSvc(), title());
      msg << MSG::ERROR << "CondContBase::erase: "
          << "Non-Timestamp key used in timestamp container."
          << endmsg;
      return StatusCode::FAILURE;
    }
    m_condSet.erase (CondContBase::keyFromTimestamp (t), ctx);
    break;
  case KeyType::SINGLE:
    break;
  default:
    std::abort();
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Extend the range of the last IOV.
 * @param newRange New validity range.
 * @param ctx Event context.
 *
 * Returns failure if the start time of @c newRange does not match the start time
 * of the last IOV in the container.  Otherwise, the end time for the last
 * IOV is changed to the end time for @c newRange.  (If the end time for @c newRange
 * is before the end of the last IOV, then nothing is changed.)
 */
StatusCode
CondContBase::extendLastRangeBase (const EventIDRange& newRange,
                                   const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  key_type start;
  key_type stop;
  switch (m_keyType) {
  case KeyType::RUNLBN:
  case KeyType::MIXED:  // To handle the extension case of insertMixed().
    if (!newRange.start().isRunLumi()) {
      MsgStream msg (Athena::getMessageSvc(), title());
      msg << MSG::ERROR << "CondContBase::extendLastRange: "
          << "Non-Run/LBN range used in Run/LBN container."
          << endmsg;
      return StatusCode::FAILURE;
    }
    start = keyFromRunLBN (newRange.start());
    stop  = keyFromRunLBN (newRange.stop());
    break;
  case KeyType::TIMESTAMP:
    if (!newRange.start().isTimeStamp()) {
      MsgStream msg (Athena::getMessageSvc(), title());
      msg << MSG::ERROR << "CondContBase::extendLastRange: "
          << "Non-timestamp range used in timestamp container."
          << endmsg;
      return StatusCode::FAILURE;
    }
    start = keyFromTimestamp (newRange.start());
    stop  = keyFromTimestamp (newRange.stop());
    break;
  case KeyType::SINGLE:
    // Empty container.
    return StatusCode::FAILURE;
  default:
    std::abort();
  }
  
  if (m_condSet.extendLastRange (RangeKey (newRange, start, stop), ctx) >= 0)
  {
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}


/** 
 * @brief Internal lookup function.
 * @param clid CLID for the desired pointer type.
 * @param t IOV time to find.
 * @param r If non-null, copy validity range of the object here.
 *
 * Looks up the conditions object corresponding to the IOV time @c t.
 * If found, convert the pointer to a pointer to the type identified
 * by CLID and return it.  Otherwise, return nullptr.
 */
const void* CondContBase::findBase (const EventIDBase& t,
                                    EventIDRange const** r) const
{
  const void* ptr = nullptr;
  key_type key;
  switch (m_keyType) {
  case KeyType::RUNLBN:
  case KeyType::MIXED:
    if (ATH_UNLIKELY (!t.isRunLumi())) {
      MsgStream msg (Athena::getMessageSvc(), title());
      msg << MSG::ERROR << "CondContBase::findBase: "
          << "Non-Run/LBN key used in Run/LBN container."
          << endmsg;
      return nullptr;
    }
    key = keyFromRunLBN (t);
    break;
  case KeyType::TIMESTAMP:
    if (ATH_UNLIKELY (!t.isTimeStamp())) {
      MsgStream msg (Athena::getMessageSvc(), title());
      msg << MSG::ERROR << "CondContBase::findBase: "
          << "Non-timestamp key used in timestamp container."
          << endmsg;
      return nullptr;
    }
    key = keyFromTimestamp (t);
    break;
  case KeyType::SINGLE:
    // Empty container.
    return nullptr;
  default:
    std::abort();
  }

  CondContSet::const_iterator it = m_condSet.find (key);
  if (it && key < it->first.m_stop) {
    if (r) {
      *r = &it->first.m_range;
    }
    ptr = it->second;
  } 

  return ptr;
}


/**
 * @brief Tell the cleaner that a new object was added to the container.
 */
StatusCode CondContBase::inserted (const EventContext& ctx)
{
  return m_cleanerSvc->condObjAdded (ctx, *this);
}


/**
 * @brief Allow overriding the name of the global conditions cleaner
 *        service (for testing purposes).
 * @param name The name of the global conditions cleaner service.
 */
void CondContBase::setCleanerSvcName (const std::string& name)
{
  s_cleanerSvcName = name;
}


/**
 * @brief Helper to report an error due to using a base class for insertion.
 * @param usedCLID CLID of the class used for insertion.
 */
void CondContBase::insertError (CLID usedCLID) const
{
  MsgStream msg (Athena::getMessageSvc(), title());
  msg << MSG::ERROR << "CondCont<T>::insert: Not most-derived class; "
      << "CLID used: " << usedCLID
      << "; container CLID: " << m_clid
      << endmsg;
}


/**
 * @brief Return the deletion function for this container.
 */
CondContBase::delete_function* CondContBase::delfcn() const
{
  return m_condSet.delfcn();
}


/**
 * @brief Description of this container to use for Msgstream.
 */
std::string CondContBase::title() const
{
  return m_id.fullKey();
}


//****************************************************************************


/**
 * @brief Dump the container contents for debugging.
 * @param ost Stream to which to write the dump.
 */
void CondContSingleBase::list (std::ostream& ost) const
{
  ost << "id: " << id() << "  proxy: " << proxy() << " ["
      << entries() << "] entries" << std::endl;
  forEach ([&] (const CondContSet::value_type& ent)
           { ost << ent.first.m_range << " " << ent.second << std::endl; });
}


/**
 * @brief Return all IOV validity ranges defined in this container.
 */
std::vector<EventIDRange> 
CondContSingleBase::ranges() const
{
  std::vector<EventIDRange> r;
  r.reserve (entries());

  forEach ([&] (const CondContSet::value_type& ent)
           { r.push_back (ent.first.m_range); });

  return r;
}


/** 
 * @brief Insert a new conditions object.
 * @param r Range of validity of this object.
 * @param obj Pointer to the object being inserted.
 * @param ctx Event context for the current thread.
 *
 * @c obj must point to an object of type @c T,
 * except in the case of inheritance, where the type of @c obj must
 * correspond to the most-derived @c CondCont type.
 * The container will take ownership of this object.
 *
 * Returns SUCCESS if the object was successfully inserted;
 * EXTENDED if the last existing range in the container was extended
 * to match the new range;
 * OVERLAP if the object was inserted but the range partially overlaps
 * with an existing one;
 * DUPLICATE if the object wasn't inserted because the range
 * duplicates an existing one, and FAILURE otherwise
 * (ownership of the object will be taken in any case).
 */
StatusCode
CondContSingleBase::typelessInsert (const EventIDRange& r,
                                    void* obj,
                                    const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  return insertBase (r,
                     CondContSet::payload_unique_ptr (obj, delfcn()),
                     ctx);
}


/**
 * @brief Return the mapped validity range for an IOV time.
 * @param t IOV time to check.
 * @param r[out] The range containing @c t.
 *
 * Returns true if @c t is mapped; false otherwise.
 */
bool
CondContSingleBase::range (const EventIDBase& t, EventIDRange& r) const
{
  const EventIDRange* rp = nullptr;
  if (findBase (t, &rp) != nullptr) {
    r = *rp;
    return true;
  }
  return false;
}


/**
 * @brief Erase the first element not less than @c t.
 * @param IOV time of element to erase.
 * @param ctx Event context for the current thread.
 */
StatusCode
CondContSingleBase::erase (const EventIDBase& t,
                           const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  return CondContBase::eraseBase (t, ctx);
}


/**
 * @brief Extend the range of the last IOV.
 * @param newRange New validity range.
 * @param ctx Event context.
 *
 * Returns failure if the start time of @c newRange does not match the start time
 * of the last IOV in the container.  Otherwise, the end time for the last
 * IOV is changed to the end time for @c newRange.  (If the end time for @c newRange
 * is before the end of the last IOV, then nothing is changed.)
 */
StatusCode
CondContSingleBase::extendLastRange (const EventIDRange& newRange,
                                     const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  return CondContBase::extendLastRangeBase (newRange, ctx);
}


/**
 * @brief Internal constructor.
 * @param rcusvc RCU service instance.
 * @param CLID of the most-derived @c CondCont.
 * @param id CLID+key for this object.
 * @param proxy @c DataProxy for this object.
 * @param delfcn Deletion function for the actual payload type.
 * @param capacity Initial capacity of the container.
 */
CondContSingleBase::CondContSingleBase (Athena::IRCUSvc& rcusvc,
                                        CLID clid,
                                        const DataObjID& id,
                                        SG::DataProxy* proxy,
                                        CondContSet::delete_function* delfcn,
                                        size_t capacity)
  : CondContBase (rcusvc, KeyType::SINGLE, clid, id, proxy, delfcn, capacity)
{
}


//****************************************************************************


/**
 * @brief Internal constructor.
 * @param rcusvc RCU service instance.
 * @param CLID of the most-derived @c CondCont.
 * @param id CLID+key for this object.
 * @param proxy @c DataProxy for this object.
 * @param payloadDelfcn Deletion function for the actual payload type.
 * @param capacity Initial capacity of the container.
 */
CondContMixedBase::CondContMixedBase (Athena::IRCUSvc& rcusvc,
                                      CLID clid,
                                      const DataObjID& id,
                                      SG::DataProxy* proxy,
                                      CondContSet::delete_function* payloadDelfcn,
                                      size_t capacity)
  : CondContBase (rcusvc, KeyType::MIXED, clid, id, proxy, delfcn, capacity),
    m_rcusvc (rcusvc),
    m_payloadDelfcn (payloadDelfcn)
{
}


/**
 * @brief Dump the container contents for debugging.
 * @param ost Stream to which to write the dump.
 */
void CondContMixedBase::list (std::ostream& ost) const
{
  ost << "id: " << id() << "  proxy: " << proxy() << " ["
      << entries() << "] run+lbn entries" << std::endl;
  forEach ([&] (const CondContSet::value_type& ent)
           {
             const CondContSet* tsmap =
               reinterpret_cast<const CondContSet*> (ent.second);
             for (const CondContSet::value_type& ent2 : tsmap->range()) {
               ost << ent2.first.m_range << " " << ent2.second << std::endl;
             }
           });
}


/**
 * @brief Return all IOV validity ranges defined in this container.
 */
std::vector<EventIDRange> 
CondContMixedBase::ranges() const
{
  std::vector<EventIDRange> r;
  r.reserve (entries()*2);

  forEach ([&] (const CondContSet::value_type& ent)
           {
             const CondContSet* tsmap =
               reinterpret_cast<const CondContSet*> (ent.second);
             for (const CondContSet::value_type& ent2 : tsmap->range()) {
               r.push_back (ent2.first.m_range);
             }
           });

  return r;
}


/** 
 * @brief Insert a new conditions object.
 * @param r Range of validity of this object.
 * @param obj Pointer to the object being inserted.
 * @param ctx Event context for the current thread.
 *
 * @c obj must point to an object of type @c T,
 * except in the case of inheritance, where the type of @c obj must
 * correspond to the most-derived @c CondCont type.
 * The container will take ownership of this object.
 *
 * Returns SUCCESS if the object was successfully inserted;
 * OVERLAP if the object was inserted but the range partially overlaps
 * with an existing one;
 * DUPLICATE if the object wasn't inserted because the range
 * duplicates an existing one, and FAILURE otherwise
 * (ownership of the object will be taken in any case).
 */
StatusCode
CondContMixedBase::typelessInsert (const EventIDRange& r,
                                   void* obj,
                                   const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  return insertMixed (r,
                      CondContSet::payload_unique_ptr (obj, payloadDelfcn()),
                      ctx);
}


/**
 * @brief Return the mapped validity range for an IOV time.
 * @param t IOV time to check.
 * @param r[out] The range containing @c t.
 *
 * Returns true if @c t is mapped; false otherwise.
 */
bool
CondContMixedBase::range (const EventIDBase& t, EventIDRange& r) const
{
  const EventIDRange* rp = nullptr;
  if (findMixed (t, &rp) != nullptr) {
    r = *rp;
    return true;
  }
  return false;
}


/**
 * @brief Erase the first element not less than @c t.
 * @param IOV time of element to erase.
 * @param ctx Event context for the current thread.
 *
 * This is not implemented for mixed containers.
 */
StatusCode
CondContMixedBase::erase (const EventIDBase& /*t*/,
                          const EventContext& /*ctx = Gaudi::Hive::currentContext()*/)
{
  MsgStream msg (Athena::getMessageSvc(), title());
  msg << MSG::ERROR << "CondContMixedBase::erase: "
      << "erase() is not implemented for mixed containers."
      << endmsg;
  return StatusCode::FAILURE;
}


/**
 * @brief Extend the range of the last IOV.
 * @param newRange New validity range.
 * @param ctx Event context.
 *
 * Returns failure if the start time of @c newRange does not match the start time
 * of the last IOV in the container.  Otherwise, the end time for the last
 * IOV is changed to the end time for @c newRange.  (If the end time for @c newRange
 * is before the end of the last IOV, then nothing is changed.)
 *
 * This is not implemented for mixed containers.
 */
StatusCode
CondContMixedBase::extendLastRange (const EventIDRange& /*newRange*/,
                                    const EventContext& /*ctx = Gaudi::Hive::currentContext()*/)
{
  MsgStream msg (Athena::getMessageSvc(), title());
  msg << MSG::ERROR << "CondContMixedBase::extendLastRange: "
      << "extendLastRange() is not implemented for mixed containers."
      << endmsg;
  return StatusCode::FAILURE;
}


/** 
 * @brief Insert a new conditions object.
 * @param r Range of validity of this object.
 * @param t Pointer to the object being inserted.
 * @param ctx Event context for the current thread.
 *
 * Returns SUCCESS if the object was successfully inserted;
 * EXTENDED if the last existing range in the container was extended
 * to match the new range;
 * OVERLAP if the object was inserted but the range partially overlaps
 * with an existing one;
 * DUPLICATE if the object wasn't inserted because the range
 * duplicates an existing one, and FAILURE otherwise
 * (ownership of the object will be taken in any case).
 */
StatusCode
CondContMixedBase::insertMixed (const EventIDRange& r,
                                CondContBase::CondContSet::payload_unique_ptr t,
                                const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  // Serialize insertions.
  std::lock_guard<std::mutex> lock (m_mutex);

  const EventIDRange* range = nullptr;
  const void* tsmap_void = findBase (r.start(), &range);
  CondContSet* tsmap ATLAS_THREAD_SAFE =
    const_cast<CondContSet*>(reinterpret_cast<const CondContSet*> (tsmap_void));

  // Only test start timestamp.  stop timestamp may be missing
  // for open-ended ranges.
  if (!r.start().isTimeStamp() )
  {
    MsgStream msg (Athena::getMessageSvc(), title());
    msg << MSG::ERROR << "CondContMixedBase::insertMixed: "
        << "Range does not have start timestamp defined."
        << endmsg;
    return StatusCode::FAILURE;
  }

  key_type start_key = keyFromTimestamp (r.start());
  key_type stop_key  = keyFromTimestamp (r.stop());

  StatusCode sc = StatusCode::SUCCESS;
  sc.ignore();
  if (tsmap) {
    if (r.start().run_number() != range->start().run_number() ||
        r.stop().run_number() != range->stop().run_number() ||
        r.start().lumi_block() != range->start().lumi_block() ||
        r.stop().lumi_block() != range->stop().lumi_block())
    {
      // Run+lbn part doesn't match.  If this range contains only a single
      // timestamp range which matches the timestamp part of the key,
      // then try to extend it.  Otherwise, give an error.

      bool extended = false;
      if (tsmap->size() == 1) {
        CondContSet::const_iterator elt = tsmap->find (start_key);
        if (elt) {
          if (elt->first.m_start == start_key &&
              elt->first.m_stop == stop_key)
          {
            if (extendLastRangeBase (r, ctx).isSuccess())
            {
              sc = CondContStatusCode::EXTENDED;
              sc.ignore();
              extended = true;

              // We also need to update the ending run+lbn value
              // for each entry in the tsmap.
              // (This doesn't affect sorting within the tsmap.)
              tsmap->updateRanges ([&] (RangeKey& k)
                                   { EventIDBase start = k.m_range.start();
                                     EventIDBase stop = k.m_range.stop();
                                     stop.set_run_number(r.stop().run_number());
                                     stop.set_lumi_block(r.stop().lumi_block());
                                     k.m_range = EventIDRange (start, stop);
                                   },
                                   ctx);
            }
          }
        }
      }

      if (!extended) {
        MsgStream msg (Athena::getMessageSvc(), title());
        msg << MSG::ERROR << "CondContMixedBase::insertMixed: "
            << "Run+lbn part of new range doesn't match existing range, "
            << "or can't extend. "
            << "New: " << r << "; existing: " << *range
            << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }
  else {
    auto newmap = std::make_unique<CondContSet>
      (Updater_t (m_rcusvc, m_payloadDelfcn), m_payloadDelfcn, 16);
    tsmap = newmap.get();
    sc = insertBase (r, std::move (newmap), ctx);
    if (sc.isFailure()) {
      return sc;
    }
    else if (Category::isDuplicate (sc)) {
      // Shouldn't happen...
      std::abort();
    }
  }

  CondContSet::EmplaceResult reslt =
    tsmap->emplace ( RangeKey(r, start_key, stop_key),
                     std::move(t), false, ctx );

  if (reslt == CondContSet::EmplaceResult::OVERLAP) {
    return CondContStatusCode::OVERLAP;
  }
  else if (Category::isExtended (sc)) {
    return sc;
  }
  else if (reslt == CondContSet::EmplaceResult::DUPLICATE)
  {
    return CondContStatusCode::DUPLICATE;
  }
  else if (reslt == CondContSet::EmplaceResult::EXTENDED)
  {
    std::abort();  // Shouldn't happen.
  }
  
  return sc;
}


/** 
 * @brief Internal lookup function.
 * @param t IOV time to find.
 * @param r If non-null, copy validity range of the object here.
 *
 * Looks up the conditions object corresponding to the IOV time @c t.
 * If found, return the pointer (as a pointer to the payload type
 * of the most-derived CondCont).  Otherwise, return nullptr.
 */
const void*
CondContMixedBase::findMixed (const EventIDBase& t,
                              EventIDRange const** r) const
{
  const void* ptr = CondContBase::findBase (t, nullptr);
  if (!ptr) return nullptr;

  if (!t.isTimeStamp()) {
    MsgStream msg (Athena::getMessageSvc(), title());
    msg << MSG::ERROR << "CondContMixedBase::findMixed: "
        << "No valid timestamp in key used with mixed container."
        << endmsg;
    return nullptr;
  }

  const CondContSet* tsmap = reinterpret_cast<const CondContSet*> (ptr);
  key_type key = keyFromTimestamp (t);
  CondContSet::const_iterator it = tsmap->find (key);

  if (it && key < it->first.m_stop) {
    if (r) {
      *r = &it->first.m_range;
    }
    ptr = it->second;
  }
  else {
    ptr = nullptr;
  }

  return ptr;
}


/**
 * @brief Return the payload deletion function for this container.
 */
CondContBase::delete_function* CondContMixedBase::payloadDelfcn() const
{
  return m_payloadDelfcn;
}


/**
 * @brief Do pointer conversion for the payload type.
 * @param clid CLID for the desired pointer type.
 * @param ptr Pointer of type @c T*.
 *
 * This just aborts, since we don't currently implement inheritance
 * for mixed types.
 */
const void* CondContMixedBase::doCast (CLID /*clid*/, const void* /*ptr*/) const
{
  std::abort();
}
