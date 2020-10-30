/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_IDENTIFIABLECONTAINERMT_H
#define EVENTCONTAINERS_IDENTIFIABLECONTAINERMT_H
/**
 * @file IdentifiableContainer.h
 *
 * @brief This class is a general container which can hold objects of
 * accessed by an IdentifierHash
 *
 * @author A E Barton <abarton@cern.ch>
 *
 *
 */

#include "Identifier/Identifier.h"
#include "EventContainers/IIdentifiableCont.h"
#include <memory>
#include "GaudiKernel/DataObject.h"
#include "EventContainers/IdentifiableContainerBase.h"
#include "EventContainers/IDC_WriteHandleBase.h"
#include "CxxUtils/AthUnlikelyMacros.h"
#include "EventContainers/IdentifiableCache.h"

template < class T>
class IdentifiableContainerMT : public DataObject, public EventContainers::IdentifiableContainerBase, public EventContainers::IIdentifiableCont<T>
{

public:

    class IDC_WriteHandle : EventContainers::IDC_WriteHandleBase {
        friend class IdentifiableContainerMT<T>;
        IdentifiableContainerMT<T> *m_IDC_ptr;
        IdentifierHash m_hashId;
        public:
        IDC_WriteHandle() : IDC_WriteHandleBase(), m_IDC_ptr(0), m_hashId(0) { }
        IDC_WriteHandle& operator=(const IDC_WriteHandle& other) = delete;
        IDC_WriteHandle(const IDC_WriteHandle& other) = delete;
        IDC_WriteHandle& operator=(IDC_WriteHandle&& other) noexcept = delete;
        static void Swap(IDC_WriteHandle& a, IDC_WriteHandle& b) noexcept {
           if(&a == &b) return;
           std::swap(a.m_IDC_ptr, b.m_IDC_ptr);
           std::swap(a.m_hashId, b.m_hashId);
           std::swap(a.m_atomic, b.m_atomic);
           std::swap(a.m_mut, b.m_mut);
        }
        IDC_WriteHandle(IDC_WriteHandle&& other) : IDC_WriteHandle() {
           Swap(*this, other);
        }

        [[nodiscard]] StatusCode addOrDelete(std::unique_ptr<T> ptr){
           if(ATH_UNLIKELY(m_hashId >= m_IDC_ptr->m_link->fullSize())) return StatusCode::FAILURE;
           StatusCode sc = m_IDC_ptr->addLock(std::move(ptr), m_hashId);
           IDC_WriteHandleBase::ReleaseLock();
           return sc;
        }
        bool alreadyPresent() { return m_IDC_ptr->m_link->tryAddFromCache(m_hashId, *this); }
        ///This method is to avoid calling an expensive operation in the offline case
        bool OnlineAndPresentInAnotherView() { return m_IDC_ptr->m_OnlineMode && m_IDC_ptr->m_link->tryAddFromCache(m_hashId, *this); }
    };



    typedef T        IDENTIFIABLE;
    typedef EventContainers::IdentifiableCache<T> ICACHE;
    typedef IdentifiableContainerMT<T> MyType;
    typedef std::vector<std::atomic<const void*> >::size_type size_type;
    typedef T*&        reference;
    typedef T* const & const_reference;
    typedef T*         value_type;
    typedef T**        pointer;
    typedef T* const * const_pointer;
    typedef T          base_value_type;


    class const_iterator
    {

    public:

        /// iterator constructor
        const_iterator() : m_itr() {}
        const_iterator(const_iterator&&) = default;
        const_iterator(const const_iterator&) = default;
        const_iterator& operator = ( const const_iterator & ) = default; //move operators may be useful for shared_ptr
        const_iterator& operator = ( const_iterator && ) = default;
        ~const_iterator() = default;


        /// increment operator
        const_iterator& operator ++ () {
            ++m_itr;
            return *this;
        }

        /// increment operator
        const_iterator operator ++ ( int ) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        const T* cptr () const {
            return reinterpret_cast<const T*>( m_itr->second );
        }

        const T* operator * () const {
            return reinterpret_cast<const T*>( m_itr->second );
        }

        const T* operator ->() const { return (operator*()); }

        /// comparison operator
        bool operator != ( const const_iterator &it ) const {
            return m_itr!= it.m_itr;
        }

        /// comparison operator
        bool operator == ( const const_iterator &it ) const {
            return m_itr == it.m_itr;
        }

        /// hashId of the pointed-to element
        //Calling this on an end iterator is undefined behaviour
        IdentifierHash hashId() const {
            return m_itr->first;
        }

    protected:
        friend class IdentifiableContainerMT<T>;

        const_iterator(EventContainers::I_InternalIDC::InternalConstItr itr) : m_itr(std::move(itr)) {}


        EventContainers::I_InternalIDC::InternalConstItr m_itr;
    };

    friend class IDC_WriteHandle;

    /// constructor initializes the collection the hashmax, OFFLINE usages pattern
    IdentifiableContainerMT(IdentifierHash hashMax);

    IdentifiableContainerMT(IdentifierHash hashMax, EventContainers::Mode);

    /// constructor initializes with a link to a cache, ONLINE usage pattern
    IdentifiableContainerMT(ICACHE *cache);

    ~IdentifiableContainerMT() { m_link->destructor(void_unique_ptr::Deleter<T>::deleter); }

    virtual bool hasExternalCache() const override final { return m_OnlineMode; }


    /// return pointer on the found entry or null if out of
    ///  range using hashed index - fast version, does not call cnv
    ///  if object not there
    virtual const T* indexFindPtr( IdentifierHash hashId ) const override final;

    const_iterator indexFind( IdentifierHash hashId ) const {
        return m_link->indexFind(hashId);
    }

    /// insert collection into container with id hash
    /// if IDC should not take ownership of collection, set ownsColl to false
    virtual StatusCode addCollection(const T* coll, IdentifierHash hashId) override final;

    /// Tries to add the item to the cache, if the item already exists then it is deleted
    /// This is a convenience method for online multithreaded scenarios
    virtual StatusCode addOrDelete(std::unique_ptr<T>, IdentifierHash hashId) override final;
    virtual StatusCode addOrDelete(std::unique_ptr<const T>, IdentifierHash hashId) override final;

    ///identical to previous excepts allows counting of deletions
    StatusCode addOrDelete(std::unique_ptr<T>, IdentifierHash hashId, bool &deleted);

    ///Like the other add methods but optimized for changing from the inprogress state
    StatusCode addLock(std::unique_ptr<T> ptr, IdentifierHash hashId);

    /// Looks in the cache to see if item already exists if not it returns false,
    /// If it does exist it incorporates it into the IDC view but changing the mask.
    virtual bool tryAddFromCache(IdentifierHash hashId) override final;

    /// Tries will look for item in cache, if it doesn't exist will call the cache IMAKER
    /// If cache doesn't have an IMAKER then this fails.
    StatusCode fetchOrCreate(IdentifierHash hashId);

    StatusCode fetchOrCreate(const std::vector<IdentifierHash> &hashId);

    //This is a method to support bad behaviour in old code. Remove ASAP
    virtual StatusCode naughtyRetrieve ATLAS_NOT_THREAD_SAFE (IdentifierHash hashId, T* &collToRetrieve) const override final;


#ifdef IdentifiableCacheBaseRemove
    /// remove collection from container for id hash, returning it
    /// (and ownership) to client
    T*  removeCollection(IdentifierHash hashId);
#endif

    /// reset m_hashids and call IdentifiableCache's cleanup
    virtual void cleanup() override final;

    /// return full size of container
    virtual size_t fullSize() const  override final {
        return m_link->fullSize();
    }

    ///Duplicate of fullSize for backwards compatability
    size_t size() const {
        return m_link->fullSize();
    }    

    void prepareItr() const { m_link->wait(); }

    /// return number of collections
    virtual size_t numberOfCollections() const override final{
        return IdentifiableContainerBase::numberOfCollections();
    }

    const std::vector < EventContainers::hashPair<T> >& GetAllHashPtrPair() const{
        static_assert(sizeof(const T*) == sizeof(const void*) && std::is_pointer<const T*>::value);
        static_assert(sizeof(EventContainers::hashPair<T>) == sizeof(EventContainers::hashPair<void>));
        return reinterpret_cast<const std::vector < EventContainers::hashPair<T> >&>
                (m_link->getAllHashPtrPair());
    }
    
    ///Returns a collection of all hashes availiable in this IDC.
    ///If this is an "offline" mode IDC then this is identical to the cache
    ///If this is an "online" mode IDC then this is the items that both exist in the cache 
    ///and have a postive mask element
    virtual std::vector<IdentifierHash> GetAllCurrentHashes() const override final {
        return IdentifiableContainerBase::GetAllCurrentHashes();
    }

    /// return const_iterator for first entry
    const_iterator begin() const {
        return const_iterator(m_link->cbegin());
    }


    /// return const_iterator for end of container
    const_iterator end() const {
        return const_iterator(m_link->cend());
    }

    [[nodiscard]] IDC_WriteHandle getWriteHandle(IdentifierHash hash)
    {
       IDC_WriteHandle lock;
       lock.m_hashId = hash;
       lock.m_IDC_ptr = this;
       return lock;
    }
};

#ifdef IdentifiableCacheBaseRemove
template < class T>
T*  //Please don't do this we want to get rid of this
IdentifiableContainerMT<T>::removeCollection( IdentifierHash hashId )
{
    return reinterpret_cast<T*>(m_link->removeCollection(hashId));
}
#endif



// Constructor for OFFLINE style IDC
template < class T>
IdentifiableContainerMT<T>::IdentifiableContainerMT(IdentifierHash maxHash) :  IdentifiableContainerBase(maxHash)
{
}

template < class T>
IdentifiableContainerMT<T>::IdentifiableContainerMT(IdentifierHash maxHash, EventContainers::Mode mode) :
     IdentifiableContainerBase(maxHash, mode)
{
}

// Constructor for ONLINE style IDC
template < class T>
IdentifiableContainerMT<T>::IdentifiableContainerMT(ICACHE *cache) : IdentifiableContainerBase(cache)
{
}

/// return valuetype on the found entry or null if out of
///  range using hashed index - fast version, does not call cnv
///  if object not there
template < class T>
const T*
IdentifiableContainerMT<T>::indexFindPtr( IdentifierHash hashId ) const
{
    return reinterpret_cast<const T* > (IdentifiableContainerBase::indexFindPtr(hashId));
}

// insert collection into container with id hash
template < class T>
StatusCode
IdentifiableContainerMT<T>::addCollection(const T* coll, IdentifierHash hashId)
{
    // update m_hashids
    if (ATH_UNLIKELY(! IdentifiableContainerBase::insert(hashId, coll))) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;

}



template < class T>
void
IdentifiableContainerMT<T>::cleanup()
{
    IdentifiableContainerBase::cleanup(void_unique_ptr::Deleter<T>::deleter);
}

template < class T>
StatusCode
IdentifiableContainerMT<T>::fetchOrCreate(IdentifierHash hashId)
{
    return IdentifiableContainerBase::fetchOrCreate(hashId);
}

template < class T>
StatusCode
IdentifiableContainerMT<T>::fetchOrCreate(const std::vector<IdentifierHash> &hashIds)
{
    return IdentifiableContainerBase::fetchOrCreate(hashIds);
}


template < class T>
bool
IdentifiableContainerMT<T>::tryAddFromCache(IdentifierHash hashId)
{
    return IdentifiableContainerBase::tryAddFromCache(hashId);
}

template < class T >
StatusCode 
IdentifiableContainerMT<T>::naughtyRetrieve(IdentifierHash hashId, T* &collToRetrieve) const
{
   if(ATH_UNLIKELY(m_OnlineMode)) return StatusCode::FAILURE;//NEVER ALLOW FOR EXTERNAL CACHE
   else {
      auto p = reinterpret_cast<const T* > (m_link->findIndexPtr(hashId));//collToRetrieve can be null on success
      collToRetrieve = const_cast<T*>(p);
      return StatusCode::SUCCESS;
   }
}

template < class T>
StatusCode
IdentifiableContainerMT<T>::addOrDelete(std::unique_ptr<T> uptr, IdentifierHash hashId)
{
    if(ATH_UNLIKELY(hashId >= m_link->fullSize())) return StatusCode::FAILURE;
    auto ptr = uptr.release();
    bool b = IdentifiableContainerBase::insert(hashId, ptr);
    if(!b) delete ptr;
    return StatusCode::SUCCESS;
}

template < class T>
StatusCode
IdentifiableContainerMT<T>::addOrDelete(std::unique_ptr<const T> uptr, IdentifierHash hashId)
{
    if(ATH_UNLIKELY(hashId >= m_link->fullSize())) return StatusCode::FAILURE;
    auto ptr = uptr.release();
    bool b = IdentifiableContainerBase::insert(hashId, ptr);
    if(!b) delete ptr;
    return StatusCode::SUCCESS;
}

template < class T>
StatusCode
IdentifiableContainerMT<T>::addLock(std::unique_ptr<T> ptr, IdentifierHash hashId)
{
    return m_link->addLock(hashId, ptr.release());
}

template < class T>
StatusCode
IdentifiableContainerMT<T>::addOrDelete(std::unique_ptr<T> uptr, IdentifierHash hashId, bool &deleted)
{
    if(ATH_UNLIKELY(hashId >= m_link->fullSize())) return StatusCode::FAILURE;
    auto ptr = uptr.release();
    bool b = IdentifiableContainerBase::insert(hashId, ptr);
    if(!b) delete ptr;
    deleted = !b;
    return StatusCode::SUCCESS;
}

#endif

