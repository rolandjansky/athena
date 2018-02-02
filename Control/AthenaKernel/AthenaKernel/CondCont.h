/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
//
/**
 * @file AthenaKernel/CondCont.h
 * @author Vakho, Charles, Scott
 * @date 2017
 * @brief Hold mappings of ranges to condition objects.
 */

#ifndef ATHENAKERNEL_CONDCONT_H
#define ATHENAKERNEL_CONDCONT_H

#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/RCUUpdater.h"
#include "CxxUtils/ConcurrentRangeMap.h"

#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventIDRange.h"
#include "GaudiKernel/DataObjID.h"
#include "boost/preprocessor/facilities/overload.hpp"

#include <iostream>
#include <set>
#include <vector>
#include <typeinfo>
#include <mutex>


namespace SG {
  class DataProxy;
}
namespace Athena {
  class IRCUSvc;
}


class CondContBase
{
public:
  /// Payload type held by this class.
  /// Need to define this here for @c cast() to work properly.
  typedef void Payload;

  /// Destructor.
  virtual ~CondContBase() {};


  /**
   * @brief Return the CLID of the most-derived @c CondCont.
   */
  CLID clid() const;


  /**
   * @brief Return CLID/key corresponding to this container.
   */
  virtual const DataObjID& id() const = 0;

  /**
   * @brief Return the associated @c DataProxy, if any.
   */
  virtual SG::DataProxy* proxy() = 0;


  /**
   * @brief Set the associated @c DataProxy.
   * @param proxy The proxy to set.
   */
  virtual void setProxy(SG::DataProxy*) = 0;


  /**
   * @brief Dump the container contents for debugging.
   * @param ost Stream to which to write the dump.
   */
  virtual void list (std::ostream& ost) const = 0;


  /**
   * @brief Return the number of conditions objects in the container.
   */
  virtual int entries() const = 0;


  /**
   * @brief Return all IOV validity ranges defined in this container.
   */
  virtual std::vector<EventIDRange> ranges() const = 0;


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
   * Returns true if the object was successfully inserted; false otherwise
   * (ownership of the object will be taken in either case).
   */
  virtual bool typelessInsert (const EventIDRange& r,
                               void* obj,
                               const EventContext& ctx = Gaudi::Hive::currentContext()) = 0;


  /**
   * @brief Test to see if a given IOV time is mapped in the container.
   * @param t IOV time to check.
   */
  virtual bool valid( const EventIDBase& t) const = 0;


  /**
   * @brief Return the mapped validity range for an IOV time.
   * @param t IOV time to check.
   * @param r[out] The range containing @c t.
   *
   * Returns true if @c t is mapped; false otherwise.
   */
  virtual bool range (const EventIDBase& t, EventIDRange& r) const = 0;


  /**
   * @brief Erase the first element not less than @c t.
   * @param IOV time of element to erase.
   * @param ctx Event context for the current thread.
   */
  virtual void erase (const EventIDBase& t,
                      const EventContext& ctx = Gaudi::Hive::currentContext()) = 0;


  /**
   * @brief Mark that this thread is no longer accessing data from this container.
   * @param ctx Event context for the current thread.
   *
   * This would normally be done through RCU service.
   * Defined here for purposes of testing.
   */
  virtual void quiescent (const EventContext& ctx = Gaudi::Hive::currentContext()) = 0;


  /// Type used to store an IOV time internally.
  /// For efficiency, we pack two 32-bit words into a 64-bit word
  /// Can be either run+lbn or a timestamp.
  typedef uint64_t key_type;


  /**
   * @brief Make a run+lbn key from an EventIDBase.
   * @param Event ID to convert.
   */
  static
  key_type keyFromRunLBN (const EventIDBase& b);


  /**
   * @brief Make a timestamp key from an EventIDBase.
   * @param Event ID to convert.
   */
  static
  key_type keyFromTimestamp (const EventIDBase& b);


  /**
   * @brief Range object to store in @c ConcurrentRangeMap.
   *
   * We need to store the original range as an @c EventIDRange.
   * For efficiency of comparisons, we also store the start and stop
   * times as packed key_types.
   */
  struct RangeKey
  {
    /// Default constructor.
    RangeKey();

    /// Constructor from range+start/stop.
    RangeKey (const EventIDRange& r,
              key_type start,
              key_type stop);


    /// Packed start time.
    key_type m_start;

    /// Packed stop time.
    key_type m_stop;

    /// Original range object.
    EventIDRange m_range;
  };


  /**
   * @brief Comparison object needed by ConcurrentRangeMap.
   */
  struct Compare
  {
    bool operator() (const RangeKey& r1, const RangeKey& r2) const
    { return r1.m_start < r2.m_start; }
    bool operator() (key_type t, const RangeKey& r2) const
    { return t < r2.m_start; }
  };


protected:
  /**
   * @brief Internal constructor.
   * @param rcusvc RCU service instance.
   * @param CLID of the most-derived @c CondCont.
   * @param id CLID+key for this object.
   * @param proxy @c DataProxy for this object.
   */
  CondContBase (Athena::IRCUSvc& rcusvc,
                CLID clid,
                const DataObjID& id,
                SG::DataProxy* proxy);


  /**
   * @brief Do pointer conversion for the payload type.
   * @param clid CLID for the desired pointer type.
   * @param ptr Pointer of type @c T*.
   *
   * Converts @c ptr from @c T* to a pointer to the type
   * given by @c clid.  Returns nullptr if the conversion
   * is not possible.
   */
  const void* cast (CLID clid, const void* ptr) const;


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
  virtual const void* findByCLID (CLID clid,
                                  const EventIDBase& t,
                                  EventIDRange const** r) const = 0;

  
private:
  /// CLID of the most-derived @c CondCont
  CLID m_clid;
};


///////////////////////////////////////////////////////////////////////////


template <class T> class CondCont;


/**
 * @brief Traits class to find the base for @c CondCont.
 *
 * @c CondCont<T> normally derives from @c CondContBase.
 * However, if @c D derives from @c B, then using @c CONDCONT_BASE(D,B)
 * will cause @c ContCont<D> to derive from @c CondCont<B>.
 */
template <typename T>
class CondContBaseInfo
{
public:
  typedef CondContBase Base;
};


namespace SG {
template <typename T>
struct Bases<CondCont<T> >
{
  typedef CondContBase Base1;               
  typedef NoBase Base2;          
  typedef NoBase Base3;      
};
} // namespace SG




/**
 * @brief Declare that conditions object @c D derives from @c B.
 *
 * This allows using @c ReadCondHandle to retrieve a conditions object
 * of type @c D as @c B.
 */
#define CONDCONT_BASE(D, B)       \
template <>                       \
class CondContBaseInfo<D>         \
{                                 \
public:                           \
  typedef CondCont<B> Base;       \
};                                 \
SG_BASE(CondCont<D>, CondCont<B>); \
SG_BASE(D, B)
  



/**
 * @brief Hold mapping of ranges to condition objects.
 *
 * This object holds mappings from a set of IOV ranges (represented
 * as EventIDRange) to conditions objects (which are owned by this object).
 * It has methods to insert a new mapping and to retrieve objects
 * by IOV time.
 *
 * This object is recorded in the conditions store, so it must have a CLID
 * (the @c CondCont object, not @c T).  This CLID should be declared
 * using the  @c CONDCONT_DEF macro:
 *
 *@code
 *  CONDCONT_DEF(TYPE, CLID);
 @endcode
 *
 * It is possible for one conditions object to derive from another.
 * If @c TYPE derives from @c BASE, then declare this with a third
 * argument to @c CONDCONT_DEF:
 *
 *@code
 *  CONDCONT_DEF(TYPE, CLID, BASE);
 @endcode
 *
 * before any references to @c CondCont<TYPE>.
 * This is implemented by having @c CondCont<TYPE> derive from @c CondCont<BASE>.
 * In that case, the mappings will be stored only in the most-derived class.
 */
template <typename T>
class CondCont: public CondContBaseInfo<T>::Base
{
public:
  /// Base class.
  typedef typename CondContBaseInfo<T>::Base Base;

  /// Payload type held by this class.
  typedef T Payload;


  /** 
   * @brief Constructor.
   * @param rcusvc RCU service instance.
   * @param id CLID+key for this object.
   * @param proxy @c DataProxy for this object.
   * @param capacity Initial capacity of the container.
   */
  CondCont (Athena::IRCUSvc& rcusvc,
            const DataObjID& id,
            SG::DataProxy* proxy = nullptr,
            size_t capacity = 16);


  /// Destructor.
  virtual ~CondCont();

  /// No copying.
  CondCont (const CondCont&) = delete;
  CondCont& operator= (const CondCont&) = delete;


  /**
   * @brief Return CLID/key corresponding to this container.
   */
  virtual const DataObjID& id() const override;


  /**
   * @brief Return the associated @c DataProxy, if any.
   */
  virtual SG::DataProxy* proxy() override;


  /**
   * @brief Set the associated @c DataProxy.
   * @param proxy The proxy to set.
   */
  virtual void setProxy (SG::DataProxy* proxy) override;


  /**
   * @brief Dump the container contents for debugging.
   * @param ost Stream to which to write the dump.
   */
  virtual void list (std::ostream& ost) const override;


  /**
   * @brief Return the number of conditions objects in the container.
   */
  virtual int entries() const override;


  /**
   * @brief Return all IOV validity ranges defined in this container.
   */
  virtual std::vector<EventIDRange> ranges() const override;


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
   * Returns true if the object was successfully inserted; false otherwise
   * (ownership of the object will be taken in either case).
   */
  virtual bool typelessInsert (const EventIDRange& r,
                               void* obj,
                               const EventContext& ctx = Gaudi::Hive::currentContext()) override;


  /** 
   * @brief Insert a new conditions object.
   * @param r Range of validity of this object.
   * @param obj Pointer to the object being inserted.
   * @param ctx Event context for the current thread.
   *
   * @c obj must point to an object of type @c T.
   * This will give an error if this is not called
   * on the most-derived @c CondCont.
   *
   * Returns true if the object was successfully inserted; false otherwise.
   */
  bool insert (const EventIDRange& r,
               std::unique_ptr<T> obj,
               const EventContext& ctx = Gaudi::Hive::currentContext());


  /** 
   * @brief Look up a conditions object for a given time.
   * @param t IOV time to find.
   * @param obj[out] Object found.
   * @param r If non-null, copy validity range of the object here.
   *
   * Returns true if the object was found; false otherwide.
   */
  bool find (const EventIDBase& t,
             T const*& obj,
             EventIDRange const** r = nullptr) const;


  /**
   * @brief Test to see if a given IOV time is mapped in the container.
   * @param t IOV time to check.
   */
  virtual bool valid (const EventIDBase& t) const override;


  /**
   * @brief Return the mapped validity range for an IOV time.
   * @param t IOV time to check.
   * @param r[out] The range containing @c t.
   *
   * Returns true if @c t is mapped; false otherwise.
   */
  virtual bool range (const EventIDBase& t, EventIDRange& r) const override;


  /**
   * @brief Erase the first element not less than @c t.
   * @param IOV time of element to erase.
   * @param ctx Event context for the current thread.
   */
  virtual void erase (const EventIDBase& t,
                      const EventContext& ctx = Gaudi::Hive::currentContext()) override;

  
  /**
   * @brief Mark that this thread is no longer accessing data from this container.
   * @param ctx Event context for the current thread.
   *
   * This would normally be done through RCU service.
   * Defined here for purposes of testing.
   */
  virtual void
  quiescent (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/) override;


protected:
  /**
   * @brief Internal constructor.
   * @param rcusvc RCU service instance.
   * @param CLID of the most-derived @c CondCont.
   * @param id CLID+key for this object.
   * @param proxy @c DataProxy for this object.
   */
  CondCont (Athena::IRCUSvc& rcusvc,
            CLID clid,
            const DataObjID& id,
            SG::DataProxy* proxy);


  /**
   * @brief Do pointer conversion for the payload type.
   * @param clid CLID for the desired pointer type.
   * @param ptr Pointer of type @c T*.
   *
   * Converts @c ptr from @c T* to a pointer to the type
   * given by @c clid.  Returns nullptr if the conversion
   * is not possible.
   */
  const void* cast (CLID clid, const void* ptr) const;


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
  virtual const void* findByCLID (CLID clid,
                                  const EventIDBase& t,
                                  EventIDRange const** r) const override;


public:
  /// Helper to ensure that the inheritance information for this class
  /// gets initialized.
  static void registerBaseInit();


private:
  typedef CondContBase::RangeKey RangeKey;
  typedef CondContBase::key_type key_type;


  /// Sets of mapped objects, by timestamp and run+LBN.
  typedef CxxUtils::ConcurrentRangeMap<RangeKey, key_type, T, CondContBase::Compare,
    Athena::RCUUpdater>
    CondContSet;
  CondContSet m_condSet_clock, m_condSet_RE;

  /// CLID+key for this container.
  DataObjID m_id;

  /// Associated @c DataProxy.
  SG::DataProxy* m_proxy;
};


#include "AthenaKernel/CondCont.icc"
#include "AthenaKernel/CondContMaker.h"

#define CONCATUNF_(x,y) x##y
#define CONCATUNF(x,y) CONCATUNF_(x,y)
#define UNIQUEVARNAME CONCATUNF(CONCATUNF(REGCCM_,__COUNTER__),__LINE__)


/// Declare a conditions container along with its CLID.
// For a conditions container not deriving from another, do
//    CONDCONT_DEF(TYPE, CLID);
//
// For a conditions container with a payload deriving from BASE, do
//    CONDCONT_DEF(TYPE, CLID, BASE);
//
#define CONDCONT_DEF_2(T, CLID)    \
  CLASS_DEF( CondCont<T>, CLID, 1) \
  static CondContainer::CondContMaker<T> maker_ ## CLID {}
#define CONDCONT_DEF_3(T, CLID, BASE)            \
  CONDCONT_BASE(T, BASE);                        \
  CONDCONT_DEF_2(T, CLID)
#define CONDCONT_DEF(...)  \
  BOOST_PP_OVERLOAD(CONDCONT_DEF_, __VA_ARGS__)(__VA_ARGS__)

  
#endif // not ATHENAKERNEL_CONDCONT_H

