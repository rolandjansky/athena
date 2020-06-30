/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
/* Dear emacs, this is -*-c++-*- */
#ifndef _ObjContainer_H_
#define _ObjContainer_H_

#include <vector>
#include <utility>
#include <limits>
#include <sstream>
#include <iostream>
#include <cassert>

#include "CxxUtils/checker_macros.h"

/** @brief Helper classes to manage shared objects in a scope.
 * Implements smart pointer which allow mixing external i.e. unmanaged resources,
 * resources which are shared or released ( out of the scope.).
 * The pointer to all objects are stored in a central container which
 * tags external and released resources and performs reference counting for shared
 * resources. Non external or released resources are freed if the container goes out
 * scope.
 *
 * The template @ref cloneObj may have to be specialised if the copy constructor
 * of the container object base class does not properly clone the managed objects.
 */

template <class T_Obj, class T_index = unsigned short, class T_signed_count=short>
class ObjContainer;

template <class T_Obj, class T_index = unsigned short, class T_signed_counter=short>
class ObjPtr;

template <class T_index = unsigned short>
class ObjRef;

/** Template to be specialised if object cloning is not handled by the copy operator of the container object class.
 */
template <class T_Obj> T_Obj *cloneObj(const T_Obj *obj)      { return (obj ? new T_Obj(*obj): obj); }

/** @brief Template which can be specialised to eventually instrument the managed objects.
 * Prerequisite for specialisation: the container object base class has a virtual destructor.
 */
template <class T_Obj> T_Obj *replaceManagedPtr(T_Obj *p_ptr) { return p_ptr; }


/** Templates which can be specialised to monitor resources.
 */
namespace Dbg {
   /** Monitor resource access e.g. check the validity of the given pointer.
    */
   template <class T_Obj>
   T_Obj *validateAccess(T_Obj *p_ptr) {
      return p_ptr;
   }

   /** Monitor when objects are shared.
    */
   template <class T_Obj>
   T_Obj *monitorShare(T_Obj *p_ptr) {
      return p_ptr;
   }

   /** Monitor when objects are released
    */
   template <class T_Obj>
   T_Obj *monitorRelease(T_Obj *p_ptr) {
      return p_ptr;
   }
}

/** Helper class to refer to objects in the container @ref ObjContainer.
 * Does not provide object lifetime guarantees. The latter is only guaranteed
 * if an @ref ObjPtr matching this reference is still instantiated.
 */
template <class T_index>
class ObjRef {
   template <class T_Obj_, class T_index_, class T_signed_count_>
   friend class ObjContainer;
   template <class T_Obj_, class T_index_, class T_signed_count_>
   friend class ObjPtr;

public:
   ObjRef()                           = default;

   ObjRef(T_index idx) : m_index(idx) {}

   ObjRef(const ObjRef<T_index> &ref) = default;

   ObjRef(ObjRef<T_index> &&ref) : m_index(ref.m_index) {ref.m_index=invalid(); }

   /** Return true if this class refers to an object which used to exist in the container.
    * If true there is still no guarantee that the object is still alive.
    */
   bool isValid() const { return m_index != invalid(); }

   /** See @ref isValid.
    */
   operator bool() const { return isValid();  }

   ObjRef<T_index> &operator=(const ObjRef<T_index> &ref) = default;

   ObjRef<T_index> &operator=(ObjRef<T_index> &&ref) {
      if (this != &ref) {
         m_index=ref.m_index;
         ref.m_index = invalid();
      }
      return *this;
   }

   bool  operator==(const ObjRef<T_index> &obj) const {
      return obj.idx() == idx();
   }

   bool  operator!=(const ObjRef<T_index> &obj) const {
      return obj.idx() != idx();
   }

private:
   T_index idx() const { return m_index; };

   static constexpr T_index invalid()  { return  std::numeric_limits<T_index>::max(); }
   T_index m_index = invalid();
};

/** Object container base class.
 * contains method which do not depend on the template parameters.
 */
class ObjContainerBase {
   template <class T_Obj_, class T_index_, class T_signed_counter_>
   friend class ObjPtr;

protected:
   static void throwNoContainer() {
      throw std::logic_error("No object container.");
   }

   static void throwMaximumCapacitiyExceeded(size_t max_objs) {
      std::stringstream msg;
      msg << "Maximum object container capacity exceeded (" << max_objs << "). Bail out.";
      throw std::runtime_error(msg.str());
   }

   static void throwMaximumNumberOfSharesExceeded(size_t max_shares) {
      std::stringstream msg;
      msg << "Maximum number of shares exceeded (" << max_shares << "). Bail out.";
      throw std::runtime_error(msg.str());
   }

   static void throwInvalidObject( size_t idx, size_t n_objs) {
      std::stringstream msg;
      msg << "Invalid object reference. Index " << idx << " not in allowed range [0.." << n_objs << "). Bail out.";
      throw std::logic_error(msg.str());
   }

   static void throwObjectAlreadyDeleted(size_t idx) {
      std::stringstream msg;
      msg << "Object " << idx << " already deleted. Bail out.";
      throw std::logic_error(msg.str());
   }

   static void throwObjectExistsAlready(const void *ptr) {
      std::stringstream msg;
      msg << "Object " << ptr << " already registered. Bail out.";
      throw std::logic_error(msg.str());
   }

   static void throwObjectStillAlive(const void *ptr, size_t cnt) {
      std::stringstream msg;
      msg << "Managed object " << ptr << " still has " << cnt << " references, but the container is now being destructed.";
      throw std::logic_error(msg.str());
   }

   static void warnShareDropAfterRelease(size_t idx) {
      (void) idx;
   }
};

/** Container for managed objects.
 * Objects can be either external (unmanaged), released (ownership changed to external), or shared.
 * The methods of this class are mostly not meant to be used directly but rather through @ref ObjPtr.
 * Managed objects are referred to by @ref ObjPtr and references (@ref ObjRef) to the objects can be
 * passed functions. While @ref ObjPtr guarantee that objects stay alive, the references @ref ObjRef
 * do not provide such guarantees.
 *
 * For example:
 * @verbatim
 * using Cont = ObjContainer<...>;
 * using Ptr  = ObjPtr<...>;
 * using Ref  = ObjRef<...>;
 * ... inner(Cont &cont, Ref ref) {
 *    Ptr b_ptr(cont,ref);
 *    ...
 * }
 * ... outer() {
 *    Cont cont;
 *    Ptr a_ptr(cont, ...);
 *    inner(cont, a_ptr.index());
 *    ...
 * }
 * @endverbatim
 * Lifetime guarantee of object referred to by a_ptr in outer, provided because the instance a_ptr
 * will stay alive while inner is called, thus ref will point to a valid object in the container.
 *
 * This will not work:
 * @verbatim
 * ... inner(Cont &cont, Ref ref) {
 *    Ptr b_ptr(cont,ref);
 *    ...
 * }
 * ... outer() {
 *    Cont cont;
 *    Ref dangling_ref;
 *    {
 *      Ptr a_ptr(cont, ...);
 *      dangling_ref=a_ptr.index(); // BAD: dangling_ref has wider scope than a_ptr
 *    }
 *    // object referred to by a_ptr / dangling_ref got deleted
 *    // thus dangling_ref now refers to an invalid object.
 *    inner(cont, dangling_ref);
 *    ...
 * }
 * @endverbatim
 */
template <class T_Obj, class T_index, class T_signed_count>
class ObjContainer : public ObjContainerBase
{
   template <class T_Obj_, class T_index_, class T_signed_counter_>
   friend class ObjPtr;
public:
   ObjContainer(unsigned int min_size=0) { m_objs.reserve(min_size); }

   ~ObjContainer() {
      for (std::pair<T_Obj *, T_signed_count> &elm : m_objs) {
         if (elm.second>0) {
            this->throwObjectStillAlive(elm.first,static_cast<size_t>(elm.second));
         }
         if (elm.first && elm.second>0) {
            this->throwObjectStillAlive(elm.first,static_cast<size_t>(elm.second));
         }
      }
   }

   bool isValid(ObjRef<T_index> ref) const {
      return ref.idx() < m_objs.size() && m_objs[ref.idx()].first; // @TODO the latter test should not be necessary
   }

   // debug :
   std::pair<T_signed_count, bool > search(T_Obj *obj) const {
      ObjRef<T_index> ref(find(obj));
      return std::make_pair( ref ? m_objs[ref.idx()].second : T_signed_count{},  ref.isValid() );
   }

   /** Return the current number of slots on the container.
    * Not each slot may point to an object.
    */
   T_index size() const { return static_cast<T_index>(m_objs.size()); }
protected:
   /** Manage an external object.
    * @param obj to an external object.
    * The ownership is not taken by the container.
    */
   ObjRef<T_index> registerObj(T_Obj &obj) {
      return registerObj(&obj, s_externalObj);
   }

   /** manage a new object.
    * @param obj the object to be managed.
    * This passes ownership to the container.
    */
   ObjRef<T_index> registerObj(T_Obj *obj) {
      return registerObj(replaceManagedPtr(obj), 0);
   }

   /** Increase the share count of a given object.
    * @param ref a reference to the object that is to be shared.
    */
   ObjRef<T_index> share(ObjRef<T_index> ref) {
      ensureValidity(ref);
      switch(m_objs[ref.idx()].second)  {
      case s_externalObj: { return ref; }

      case s_releasedObj: { this->warnShareDropAfterRelease( ref.idx() );
                            Dbg::monitorShare(m_objs[ref.idx()].first);
                            return ref; }

      case 0:             if (!m_objs[ref.idx()].first) {this->throwObjectAlreadyDeleted( ref.idx());}
                          [[fallthrough]]; // throws exception

      default:            { break; }
      }
      Dbg::monitorShare(m_objs[ref.idx()].first);

      if (m_objs[ref.idx()].second >= std::numeric_limits<T_signed_count>::max()) {
         this->throwMaximumNumberOfSharesExceeded(std::numeric_limits<T_signed_count>::max()); // clone object and add new slot ?
      }
      ++(m_objs[ref.idx()].second);

      return ref;
   }

   /** Decrease the share count of the referenced object and eventually destroy it.
    * Will destroy "shared" objects if the share count drops to zero, but external
    * or released objects will not be touched.
    */
   void drop( ObjRef<T_index> &ref) {
      ensureValidity(ref);
      switch(m_objs[ref.idx()].second)  {
      case s_externalObj: { return ; }

      case s_releasedObj: { this->warnShareDropAfterRelease( ref.idx() );
                            return ; }

      case 0:             this->throwObjectAlreadyDeleted( ref.idx());
                          [[fallthrough]]; // throws exception

      default:            { break; }
      }

      if (--(m_objs[ref.idx()].second) == 0) {
         T_Obj *obj=m_objs[ref.idx()].first;
         delete m_objs[ref.idx()].first;
         m_objs[ref.idx()].first=nullptr;
         ObjRef<T_index> ref(find(obj));
         if (ref) {
            this->throwObjectAlreadyDeleted( ref.idx());
         }
         if (m_freeIdx == std::numeric_limits<T_index>::max()) {
            m_freeIdx=ref.idx();
         }
         ref=ObjRef<T_index>();
      }
   }

   /** Get a pointer to a managed object.
    * @param ref reference to the object.
    * @throw if the reference is invalid or the object does not exist.
    */
   const T_Obj *get( ObjRef<T_index> ref) const {
      ensureExists(ref);
      return Dbg::validateAccess(m_objs[ref.idx()].first);
   }

   /** Get a pointer to a managed object.
    * @param ref reference to the object.
    * @throw if the reference is invalid or the object does not exist.
    */
    T_Obj *get( ObjRef<T_index> ref) {
       ensureExists(ref);
       return Dbg::validateAccess(m_objs[ref.idx()].first);
    }

   /** Remove ownership over the given object from the container.
    * @param ref reference to the object.
    * @throw will throw an exception if the reference is invalid.
    * Will remove ownership over the referenced object from the container. The object is
    * still accessible like an external object. Thus, it is up to the user to ensure that
    * released objects remain alive at least as long as the object container.
    * If the object is an external object or was released already a clone is created.
    */
   T_Obj *release( ObjRef<T_index> ref) {
      ensureValidity(ref);
      switch (m_objs[ref.idx() ].second) {
      case 0: this->throwObjectAlreadyDeleted( ref.idx());
              [[fallthrough]]; // will not fall through, because of exception
      case s_releasedObj:
              [[fallthrough]]; // indeed, should also clone the object
      case s_externalObj:   return cloneObj(m_objs[ref.idx()].first);
      default:
         m_objs[ref.idx()].second = s_releasedObj;
         Dbg::monitorRelease(m_objs[ref.idx()].first);
         return m_objs[ref.idx()].first;
      }
   }

   /** Return true if this container owns the object.
    */
   static bool isOwned( ObjRef<T_index> ref) {
      return ref.idx() == 1;
   }

   /** Return true if the object is referred to by more than one @ref ObjPtr.
    */
   static bool isShared( ObjRef<T_index> ref) {
      return ref.idx() > 1;
   }

   /** Return true if the object is external i.e. not owned by this container.
    */
   static bool isExtern( ObjRef<T_index> ref) {
      return ref.idx() == s_externalObj;
   }

protected:
   /** Search for an object in the container.
    * @return a valid reference if the object is found or otherwise an invalid reference.
    */
   ObjRef<T_index> find( T_Obj *obj) const {
      unsigned int idx=m_objs.size();
      for (typename std::vector< std::pair<T_Obj *, T_signed_count> >::const_reverse_iterator
              iter =m_objs.rbegin();
           iter != m_objs.rend();
           ++iter) {
         --idx;
         if (iter->first == obj) {
            assert( m_objs.at(m_objs.rend()-iter-1).first == obj);
            ObjRef<T_index> ref( static_cast<T_index>(m_objs.rend()-iter-1) ); // @TODO std::distance ?
            ensureValidity(ref);
            return ref;
         }
      }
      return ObjRef<T_index>();
   }

   /** Will throw an exception if the reference is not valid
    */
   void ensureValidity(ObjRef<T_index> ref) const {
      if (!isValid(ref)) this->throwInvalidObject( ref.idx(), m_objs.size());
   }

   /** Will throw an exception if the referenced object does not exist
    */
   void ensureExists(ObjRef<T_index> ref) const {
      ensureValidity(ref);
      if (m_objs[ref.idx() ].second == 0) { this->throwObjectAlreadyDeleted( ref.idx()); }
   }

private:
   /** Will throw an exception if the maximum capacity of the container is exceeded.
    */
   void checkCapacity() {
      if (m_objs.size() >= std::numeric_limits<T_index>::max()) {
         this->throwMaximumCapacitiyExceeded(m_objs.size());
      }
   }

   /** Register an object with a given state (external, released, shared)
    */
   ObjRef<T_index> registerObj(T_Obj *obj, T_signed_count initial_count) {
      if (!obj) return ObjRef<T_index>{};
      checkCapacity();

      ObjRef<T_index> ref = find(obj);
      if (isValid(ref))  {
         switch(m_objs.at(ref.idx()).second) {
         case 0: {
            break;
         }
         case s_releasedObj:
            if (initial_count == s_externalObj) {
               break;
            }
            [[fallthrough]];
         default:
            this->throwObjectExistsAlready( obj);
         }
         //         return ref;
      }
      if (m_freeIdx == std::numeric_limits<T_index>::max() && m_objs.size() >= m_objs.capacity()) {
         for (typename std::vector< std::pair<T_Obj *, T_signed_count> >::const_reverse_iterator
                 iter =m_objs.rbegin();
              iter != m_objs.rend();
              ++iter) {
            if (iter->second==0) {
               m_freeIdx=m_objs.rend()-iter-1;
            }
         }
      }
      if (m_freeIdx == std::numeric_limits<T_index>::max()) {
         ref=ObjRef(static_cast<T_index>(m_objs.size()));
         m_objs.push_back(std::make_pair(obj, initial_count));
      }
      else {
         m_objs[m_freeIdx] = std::make_pair( obj,initial_count);
         ref=ObjRef(m_freeIdx);
         m_freeIdx = std::numeric_limits<T_index>::max();
      }
      ensureValidity(ref);
      return ref;
   }

   constexpr static T_signed_count s_externalObj = -2;       ///< "share count" of external objects
   constexpr static T_signed_count s_releasedObj = -1;       ///< "share count" of released objects

   std::vector< std::pair<T_Obj *, T_signed_count> > m_objs; ///< The storage for the object pointers
   T_index                                           m_freeIdx=std::numeric_limits<T_index>::max();
};

/** Pointer to objects managed by @ref ObjContainer
 */
template <class T_Obj, class T_index, class T_signed_counter>
class ObjPtr {
public:
   ObjPtr()                 = default;
   ObjPtr(ObjPtr &&obj_ptr) = default;

   ObjPtr(const ObjPtr &obj_ptr)
      : m_container(obj_ptr.m_container)
   {
      shareAndSet(obj_ptr.index());
   }

   /** Store a managed object in the container.
    * @param container the container to store the given object
    * @param obj the object to be managed.
    */
   ObjPtr(ObjContainer<T_Obj,T_index, T_signed_counter> &container, T_Obj *obj)
      : m_container(&container),
        m_ref(m_container->registerObj(obj))
   {
   }

   /** Store an external object in the container.
    * @param container the container to store the given object
    * @param obj the external object to be stored in the container.
    * The container will not claim ownership.
    */
   ObjPtr(ObjContainer<T_Obj,T_index, T_signed_counter> &container, const T_Obj &obj)
      : m_container(&container),
        m_ref(m_container->registerObj(obj))
   {
   }

   /** Share the referred object managed by the given container.
    * @param container the container which manages the referenced object.
    * @param ref a reference to an object in the container (or an invalid reference) to create on invalid ObjPtr.
    */
   ObjPtr(ObjContainer<T_Obj,T_index, T_signed_counter> &container, ObjRef<T_index> ref=ObjRef<T_index>())
      : m_container(&container),
        m_ref( ref ? container.share(ref) : ref)
   {
   }

   ~ObjPtr() {
      if  (m_ref) {
         m_container->drop(m_ref);
      }
   }

   /** Test whether this pointer points to a valid object.
    */
   operator bool() const { return m_container && m_container->isValid(m_ref); }

   ObjPtr &operator=(const ObjPtr &obj) {
      if (obj.m_ref != m_ref) {
         if (*this) {
            m_container->drop(m_ref);
         }
         if (obj.m_container) {
            assert( !m_container || m_container == obj.m_container);
            m_container = obj.m_container;
         }
         shareAndSet(obj.index());
      }
      return *this;
   }

   ObjPtr &operator=(ObjPtr &&obj) {
      if (&obj != this) {
         if (*this) {
            m_container->drop(m_ref);
         }
         m_ref = std::move(obj.m_ref);
         if (obj.m_container) {
            assert( !m_container || m_container == obj.m_container);
            m_container = obj.m_container;
         }
      }
      return *this;
   }

   ObjPtr &operator=(ObjRef<T_index> ref) {
      if (ref != m_ref) {
         if (*this) {
            m_container->drop(m_ref);
         }
         if (!m_container) {
            ObjContainerBase::throwNoContainer();
         }
         shareAndSet(ref);
      }
      return *this;
   }

   /** Test whether two pointer point to the same object.
    */
   bool  operator==(const ObjPtr &obj) const {
      return obj.m_ref == m_ref;
   }

   /** Test whether two pointer point to different objects.
    */
   bool  operator!=(const ObjPtr &obj) const {
      return obj.m_ref != m_ref;
   }

   /** Helper method which only stores a "new" object if the given object is not the managed object of the given pointer.
    * @param orig a pointer to a managed object (or an invalid pointer).
    * @param obj a new object or the already managed object pointed to by orig (or a nullptr).
    * @return a pointer to a manged object either the newly managed object or the original managed object.
    */
   static ObjPtr recapture(const ObjPtr &orig, T_Obj *obj) {
      if (obj) {
         if (!orig.m_container) {
            ObjContainerBase::throwNoContainer();
         }
         if (orig && obj == orig.get()) {
            return orig;
         }
         else {
            return ObjPtr(*orig.m_container, orig.m_container->registerObj(obj));
         }
      }
      else {
         return ObjPtr();
      }
   }

   T_Obj &operator*() {
      return *m_container->get(m_ref);
   }

   T_Obj *operator->() {
      return get();
   }

   /** Get a pointer to a valid manged object.
    * @throw will throw an exception if this pointer does not point to a valid object.
    */
   T_Obj *get() {
      if (!m_ref) return nullptr;
      else        return m_container->get(m_ref);
   }

   // @TODO useful ? the container does not really allow for const-access.

   /** Get a const pointer to a valid manged object.
    * @throw will throw an exception if this pointer does not point to a valid object.
    */
   const T_Obj *get() const {
      if (!m_ref) return nullptr;
      else        return m_container->get(m_ref);
   }

   /** Release the object this pointer points to from the container.
    * @return a pointer to the object
    * This will remove the ownership over this object from the container. If there
    * are still other ObjPtr instances alive which point to this object, then
    * the object will remain in the container. This it is up to the user to ensure that
    * the object stays alive at least as long as the container.
    */
   T_Obj *release() {
      if (!m_ref) return nullptr;
      else        return m_container->release(m_ref);
   }

   /** Get a light-weight reference to the object pointed to by this pointer
    * The returned light-weight reference does not provide any lifetime guarantees of the object.
    */
   ObjRef<T_index> index() const { return m_ref; }

 protected:
   /** Helper method to increase the share count of the object pointed to by this pointer.
    */
   ObjRef<T_index> share() {
      if (*this) {
         assert(m_container);
         return m_container->share( m_ref);
      }
      else {
         return m_ref;
      }
   }

   /** Helper method to increase the share count of the referenced object and point to that object.
    */
   void shareAndSet(ObjRef<T_index> ref) {
      if (ref) {
         assert(m_container);
         m_ref=m_container->share(ref);
      }
      else {
         m_ref=ref;
      }
   }

 public:
   /** Return true if the object is managed and owned by the container.
    */
   bool isOwned() const {
      return ObjContainer<T_Obj, T_index, T_signed_counter>::isOwned(m_ref);
   }

   /** Return true if the object is managed by the container and if there are more than one instances of ObjPtr which point to this object.
    */
   bool isShared() const {
      return ObjContainer<T_Obj, T_index, T_signed_counter>::isShared(m_ref);
   }

   /** Return true if the object is an external object i.e. if the container does not have ownership over this object.
    */
   bool isExtern() const {
      return ObjContainer<T_Obj, T_index, T_signed_counter>::isExtern(m_ref);
   }


private:
   ObjContainer<T_Obj, T_index, T_signed_counter>  *m_container = nullptr; ///< pointer to the conainer
   ObjRef<T_index>                                  m_ref {};                                            ///< a valid reference to an object stored in the container or an invalid reference.
};
#endif
