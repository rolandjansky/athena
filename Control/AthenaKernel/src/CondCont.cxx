// $Id$
/**
 * @file AthenaKernel/src/CondCont.cpp
 * @author Vakho, Charles, Scott
 * @date Apr, 2018
 * @brief Hold mappings of ranges to condition objects.
 */


#include "AthenaKernel/CondCont.h"
#include <iostream>


/// Default name of the global conditions cleaner service.
std::string CondContBase::s_cleanerSvcName = "Athena::ConditionsCleanerSvc";


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
  return StatusCode::Category::message (code);
}


/**
 * @brief Is code considered success?
 */
bool CondContBase::Category::isSuccess (code_t code) const
{
  return code == static_cast<code_t>( CondContStatusCode::DUPLICATE ) ||
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
 * @brief Dump the container contents for debugging.
 * @param ost Stream to which to write the dump.
 */
void CondContBase::list (std::ostream& ost) const
{
  ost << "id: " << m_id << "  proxy: " << m_proxy << std::endl;

  ost << "clock: [" << m_condSet_clock.size() << "]" << std::endl;
  for (const typename CondContSet::value_type& ent : m_condSet_clock.range()) {
    ost << ent.first.m_range << " " << ent.second << std::endl;
  }
  ost << "RE: [" << m_condSet_RE.size() << "]" << std::endl;
  for (const typename CondContSet::value_type& ent : m_condSet_RE.range()) {
    ost << ent.first.m_range << " " << ent.second << std::endl;
  }
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
  return m_condSet_RE.size() + m_condSet_clock.size();
}


/**
 * @brief Return the number of run+LBN conditions objects
 *        in the container.
 */
size_t CondContBase::entriesRunLBN() const
{
  return m_condSet_RE.size();
}


/**
 * @brief Return the number of timestamp-based conditions objects
 *        in the container.
 */
size_t CondContBase::entriesTimestamp() const
{
  return m_condSet_clock.size();
}


/**
 * @brief Return all IOV validity ranges defined in this container.
 */
std::vector<EventIDRange> 
CondContBase::ranges() const
{
  std::vector<EventIDRange> r;
  r.reserve (m_condSet_RE.size() + m_condSet_clock.size());
  for (const typename CondContSet::value_type& ent : m_condSet_RE.range()) {
    r.push_back( ent.first.m_range );
  }
  for (const typename CondContSet::value_type& ent : m_condSet_clock.range()) {
    r.push_back( ent.first.m_range );
  }

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
 * DUPLICATE if the object wasn't inserted because the range
 * duplicates an existing one, and FAILURE otherwise
 * (ownership of the object will be taken in any case).
 */
StatusCode
CondContBase::typelessInsert (const EventIDRange& r,
                              void* obj,
                              const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  return insertBase (r,
                     CondContSet::payload_unique_ptr (obj, m_condSet_RE.delfcn()),
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
CondContBase::range(const EventIDBase& t, EventIDRange& r) const
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
void CondContBase::erase (const EventIDBase& t,
                          const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  if (t.isRunLumi()) {
    m_condSet_RE.erase (CondContBase::keyFromRunLBN (t), ctx);
  }
  else if (t.isTimeStamp()) {
    m_condSet_clock.erase (CondContBase::keyFromTimestamp (t), ctx);
  }
}


/**
 * @brief Remove unused run+LBN entries from the front of the list.
 * @param keys List of keys that may still be in use.
 *             (Must be sorted.)
 *
 * We examine the objects in the container, starting with the earliest one.
 * If none of the keys in @c keys match the range for this object, then
 * it is removed from the container.  We stop when we either find
 * an object with a range matching a key in @c keys or when there
 * is only one object left.
 *
 * The list @c keys should contain keys as computed by keyFromRunLBN.
 * The list must be sorted.
 *
 * Removed objects are queued for deletion once all slots have been
 * marked as quiescent.
 *
 * Returns the number of objects that were removed.
 */
size_t CondContBase::trimRunLBN (const std::vector<key_type>& keys)
{
  return m_condSet_RE.trim (keys);
}


/**
 * @brief Remove unused timestamp entries from the front of the list.
 * @param keys List of keys that may still be in use.
 *             (Must be sorted.)
 *
 * We examine the objects in the container, starting with the earliest one.
 * If none of the keys in @c keys match the range for this object, then
 * it is removed from the container.  We stop when we either find
 * an object with a range matching a key in @c keys or when there
 * is only one object left.
 *
 * The list @c keys should contain keys as computed by keyFromRunLBN.
 * The list must be sorted.
 *
 * Removed objects are queued for deletion once all slots have been
 * marked as quiescent.
 *
 * Returns the number of objects that were removed.
 */
size_t CondContBase::trimTimestamp (const std::vector<key_type>& keys)
{
  return m_condSet_clock.trim (keys);
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
  m_condSet_RE.quiescent (ctx);
  m_condSet_clock.quiescent (ctx);
}


/**
 * @brief Return the number times an item was inserted into the map.
 */
size_t CondContBase::nInserts() const
{
  return m_condSet_clock.nInserts() + m_condSet_RE.nInserts();
}


/**
 * @brief Return the maximum size of the map.
 */
size_t CondContBase::maxSize() const
{
  return m_condSet_clock.maxSize() + m_condSet_RE.maxSize();
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
CondContBase::extendLastRange (const EventIDRange& newRange,
                               const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  typename CondContSet::const_iterator it = nullptr;
  if (newRange.start().isRunLumi()) {
    it = m_condSet_RE.extendLastRange
      (RangeKey (newRange,
                 CondContBase::keyFromRunLBN (newRange.start()),
                 CondContBase::keyFromRunLBN (newRange.stop())), ctx);
  }
  else if (newRange.start().isTimeStamp()) {
    it = m_condSet_clock.extendLastRange
      (RangeKey (newRange,
                 CondContBase::keyFromTimestamp (newRange.start()),
                 CondContBase::keyFromTimestamp (newRange.stop())), ctx);
  }
  if (it != nullptr) {
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
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
CondContBase::CondContBase (Athena::IRCUSvc& rcusvc,
                            CLID clid,
                            const DataObjID& id,
                            SG::DataProxy* proxy,
                            CondContSet::delete_function* delfcn,
                            size_t capacity)
  : m_clid (clid),
    m_id (id),
    m_proxy (proxy),
    m_condSet_clock (Updater_t (rcusvc, delfcn), delfcn, capacity),
    m_condSet_RE (Updater_t (rcusvc, delfcn), delfcn, capacity),
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

  // LBN part of ranges may be undefined for an open-ended range.
  if (start.run_number() != EventIDBase::UNDEFNUM &&
      stop.run_number() != EventIDBase::UNDEFNUM)
  {
    if (!m_condSet_RE.emplace( RangeKey(r,
                                        keyFromRunLBN (start),
                                        keyFromRunLBN (stop)),
                               std::move(t), ctx ))
    {
      return CondContStatusCode::DUPLICATE;
    }
    
  }
  else if (start.isTimeStamp() && stop.isTimeStamp()) {
    if (!m_condSet_clock.emplace( RangeKey(r,
                                           keyFromTimestamp (start),
                                           keyFromTimestamp (stop)),
                                  std::move(t), ctx ))
    {
      return CondContStatusCode::DUPLICATE;
    }
  }
  else {
    std::cerr << "CondCont<T>::insert error: EventIDRange " << r 
              << " is neither fully RunEvent nor TimeStamp" 
              << std::endl;
    return StatusCode::FAILURE;
  }

  return this->inserted (ctx);
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
  if (t.isRunLumi()) {
    key_type key = keyFromRunLBN (t);
    CondContSet::const_iterator it = m_condSet_RE.find (key);
    if (it && key < it->first.m_stop) {
      if (r) {
        *r = &it->first.m_range;
      }
      ptr = it->second;
    }
  } 

  if (!ptr && t.isTimeStamp()) {
    key_type key = keyFromTimestamp (t);
    CondContSet::const_iterator it = m_condSet_clock.find (key);
    if (it && key < it->first.m_stop) {
      if (r) {
        *r = &it->first.m_range;
      }
      ptr = it->second;
    }
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

