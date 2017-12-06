/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "EventContainers/IdentifiableCache.h"
#include "EventContainers/IIdentifiableCont.h"
#include <memory>
#include <algorithm>
#include "GaudiKernel/DataObject.h"



//const_iterator and indexFind are provided for backwards compatability. they are not optimal
/*
IT is faster to iterate over the container with this method:
    auto hashes= m_container->GetAllCurrentHashs(); 
    for (auto hash : hashes) {
        T* coll = m_container->indexFindPtr(hash); 
        //Use coll here
    }
*/

template < class T>
class IdentifiableContainerMT : public DataObject, public EventContainers::IIdentifiableCont<T>
{


public:

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
    typedef std::vector<IdentifierHash> Hash_Container;

private:
    ICACHE *m_cacheLink;
    std::vector<bool> m_mask;
    bool m_OnlineMode;

public:

    class const_iterator
    {

    public:

        /// iterator constructor
        const_iterator() : m_sptr(), m_current(nullptr), m_idContainer(nullptr), m_end(false) { }

        /// assignment operator
        const_iterator& operator = ( const const_iterator & it ) {
            m_hashItr     = it.m_hashItr;
            m_sptr        = it.m_sptr;
            m_current     = it.m_current;
            m_idContainer = it.m_idContainer;
            m_end         = it.m_end;
            return *this;
        }

        /// increment operator
        const_iterator& operator ++ () {
            if(m_end) return *this;
            //If called on iterator created by "fast" iterator method const_iterator( const MyType* idC, IdentifierHash hash  )
            if(!m_sptr) { 
                auto ids = m_idContainer->GetAllCurrentHashs();
                m_sptr    = std::shared_ptr<Hash_Container> (new Hash_Container());
                m_sptr->swap(ids);
                m_hashItr = std::find(m_sptr->begin(), m_sptr->end(), m_hash);
                if(m_hashItr==m_sptr->end()) {
                    m_end = true;
                    m_sptr.reset();
                    return *this;
                }
            }
            
#ifdef IdentifiableCacheBaseRemove            
            do {
                ++m_hashItr;
                if(m_hashItr==m_sptr->end()) {
                    m_end = true;
                    m_sptr.reset();
                    return *this;
                }
                m_current = m_idContainer->indexFindPtr(*m_hashItr);
            } while(m_current==nullptr); //If we implement remove, this can happen
            
#else
             ++m_hashItr;
             if(m_hashItr==m_sptr->end()) {
                 m_end = true;
                 m_sptr.reset();
                 return *this;
             }
             m_current = m_idContainer->indexFindPtr(*m_hashItr);
#endif       
             return *this;
        }

        /// increment operator
        const_iterator operator ++ ( int ) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        const T* cptr () const {
            return m_current;
        }

        const T* operator * () const {
            return m_current;
        }

        const T* operator ->() const { return (operator*()); }

        /// comparison operator
        bool operator != ( const const_iterator &it ) const {
            if(m_end || it.m_end) return m_end!= it.m_end;
            return m_current!= it.m_current;
        }

        /// comparison operator
        bool operator == ( const const_iterator &it ) const {
            if(m_end || it.m_end) return m_end == it.m_end;
            return m_current == it.m_current;
        }

        /// hashId of the pointed-to element
        //Calling this on an end iterator is undefined behaviour
        IdentifierHash hashId() const {
            return !m_sptr ? m_hash : *m_hashItr;
        }

    protected:
        friend class IdentifiableContainerMT<T>;

        const_iterator(const MyType* idC, bool end) : m_sptr(), m_current(nullptr), m_idContainer(idC), m_end(end)
        {
            if(!m_end) {
                auto ids = m_idContainer->GetAllCurrentHashs();
                if(ids.empty()) {//For empty containers
                    m_end = true;
                } else {
                    m_sptr   = std::shared_ptr<Hash_Container> (new Hash_Container());
                    m_sptr->swap(ids);
                    m_hashItr = m_sptr->begin();            
                    m_current = m_idContainer->indexFindPtr(*m_hashItr);
#ifdef IdentifiableCacheBaseRemove  
                    if(m_current==nullptr){  ++(*this); } //If we implement remove, this can happen
#endif
                }
            }
        }


        //Alot of existing code creates a lot of iterators but doesn't interator over it
        //This constructor delays creation of hash vector until ++ is called
        const_iterator( const MyType* idC, IdentifierHash hash  ) :m_sptr(),
            m_hash(hash), m_idContainer(idC) {

            m_current = m_idContainer->indexFindPtr(hash);
            m_end = m_current==nullptr;
        }

        Hash_Container::const_iterator m_hashItr;
        std::shared_ptr<Hash_Container> m_sptr;
        const T* m_current;
        IdentifierHash m_hash;
        const MyType *m_idContainer;
        bool m_end;
    };



    // constructor initializes the collection the hashmax, OFFLINE usages pattern
    IdentifiableContainerMT(IdentifierHash hashMax);

    // constructor initializes with a link to a cache, ONLINE usage pattern
    IdentifiableContainerMT(ICACHE *cache);

    ~IdentifiableContainerMT() { if(!m_OnlineMode) delete m_cacheLink; }

    virtual bool hasExternalCache() const override final { return m_OnlineMode; }


    /// return pointer on the found entry or null if out of
    ///  range using hashed index - fast version, does not call cnv
    ///  if object not there
    virtual const T* indexFindPtr( IdentifierHash hashId ) const override final;

    const_iterator indexFind( IdentifierHash hashId ) const {
        const_iterator it(this, hashId);
        return it;
    }

    /// insert collection into container with id hash
    /// if IDC should not take ownership of collection, set ownsColl to false
    StatusCode addCollection(const T* coll, IdentifierHash hashId, bool ownsColl = true) override final;

    /// Tries to add the item to the cache, if the item already exists then it is deleted
    /// This is a convenience method for online multithreaded scenarios
    virtual StatusCode addOrDelete(std::unique_ptr<T>, IdentifierHash hashId) override final;

    ///identical to previous excepts allows counting of deletions
    virtual StatusCode addOrDelete(std::unique_ptr<T>, IdentifierHash hashId, bool &deleted);

    /// Looks in the cache to see if item already exists if not it returns false,
    /// If it does exist it incorporates it into the IDC view but changing the mask.
    virtual bool tryFetch(IdentifierHash hashId) override final;

    /// Tries will look for item in cache, if it doesn't exist will call the cache IMAKER
    /// If cache doesn't have an IMAKER then this fails.
    StatusCode fetchOrCreate(IdentifierHash hashId);

    //This is a method to support bad behaviour in old code. Remove ASAP
    virtual StatusCode naughtyRetrieve(IdentifierHash hashId, T* &collToRetrieve) const override final;


#ifdef IdentifiableCacheBaseRemove
    /// remove collection from container for id hash, returning it
    /// (and ownership) to client
    T*  removeCollection(IdentifierHash hashId);
#endif

    /// set m_hashids to have all the possible hash values given
    /// the hash max
    void setAllCollections(unsigned int hashMax);

    /// reset m_hashids and call IdentifiableCache's cleanup
    virtual void cleanup() override final;

    /// return full size of container
    virtual size_t fullSize() const  override final {
        return m_mask.size();
    }

    //Duplicate of fullSize for backwards compatability
    size_t size() const {
        return m_mask.size();
    }    

    /// return number of collections
    virtual size_t numberOfCollections() const override final{
        if(!m_OnlineMode) return m_cacheLink->numberOfHashes();
        size_t count =0;
        for(auto b : m_mask) count += b;
        return count;
    }

    
    //Returns a collection of all hashes availiable in this IDC.
    //If this is an "offline" mode IDC then this is identical to the cache
    //If this is an "online" mode IDC then this is the items that both exist in the cache 
    //and have a postive mask element
    virtual std::vector<IdentifierHash> GetAllCurrentHashs() const override final {
        if(not m_OnlineMode) return m_cacheLink->ids();
        else{
	    std::vector<IdentifierHash> ids;
            for(size_t i =0 ; i < m_mask.size(); ++i) if(m_mask[i]) ids.emplace_back(i);
            return ids;
        }
    }

    /// return const_iterator for first entry
    const_iterator begin() const {
        const_iterator it(this, false);
        return it;
    }


    /// return const_iterator for end of container
    const_iterator end() const {
        return const_iterator(nullptr, true);
    }
};

#ifdef IdentifiableCacheBaseRemove
template < class T>
T*
IdentifiableContainerMT<T>::removeCollection( IdentifierHash hashId )
{
    using namespace EventContainers;
    const T* ptr = reinterpret_cast<const T*> (m_cacheLink->find (hashId));
    m_cacheLink->remove(hashId);
    m_mask[hashId] = false;
    return const_cast<T*>(ptr);
}
#endif



// Constructor for OFFLINE style IDC
template < class T>
IdentifiableContainerMT<T>::IdentifiableContainerMT(IdentifierHash maxHash) : m_cacheLink(new ICACHE(maxHash, nullptr))
{
    m_mask.assign(maxHash, true);
    m_OnlineMode = false;
}

// Constructor for ONLINE style IDC
template < class T>
IdentifiableContainerMT<T>::IdentifiableContainerMT(ICACHE *cache) : m_cacheLink(cache)
{
    m_mask.assign(cache->fullSize(), false);
    m_OnlineMode = true;
}

// return valuetype on the found entry or null if out of
//  range using hashed index - fast version, does not call cnv
//  if object not there
template < class T>
const T*
IdentifiableContainerMT<T>::indexFindPtr( IdentifierHash hashId ) const
{
    if(m_mask[hashId])  return m_cacheLink->find(hashId);
    else                return nullptr;
}

// insert collection into container with id hash
template < class T>
StatusCode
IdentifiableContainerMT<T>::addCollection(const T* coll, IdentifierHash hashId, bool ownsColl)
{
    // update m_hashids
    if (! m_cacheLink->add(hashId, coll, ownsColl)) return StatusCode::FAILURE;
    m_mask[hashId] = true;
    return StatusCode::SUCCESS;

}



template < class T>
void
IdentifiableContainerMT<T>::cleanup()
{
    if(m_OnlineMode) { m_mask.assign(m_cacheLink->fullSize(), false);  }
    else { m_cacheLink->clearCache(); }
}

template < class T>
StatusCode
IdentifiableContainerMT<T>::fetchOrCreate(IdentifierHash hashId)
{
    auto ptr = m_cacheLink->get(hashId);
    if(ptr==nullptr){
        return StatusCode::FAILURE; 
    }
    m_mask[hashId] = true;
    return StatusCode::SUCCESS; 
}

template < class T>
bool
IdentifiableContainerMT<T>::tryFetch(IdentifierHash hashId)
{
    auto ptr = m_cacheLink->find(hashId);
    if(ptr==nullptr){
        return false; 
    }
    m_mask[hashId] = true;
    return true; 
}

template < class T >
StatusCode 
IdentifiableContainerMT<T>::naughtyRetrieve(IdentifierHash hashId, T* &collToRetrieve) const
{
   if(m_OnlineMode) return StatusCode::FAILURE;//NEVER ALLOW FOR EXTERNAL CACHE
   else {
      collToRetrieve = const_cast< T* > (m_cacheLink->find(hashId));//collToRetrieve can be null on success
      return StatusCode::SUCCESS;
   }
}

template < class T>
StatusCode
IdentifiableContainerMT<T>::addOrDelete(std::unique_ptr<T> ptr, IdentifierHash hashId)
{
    if(hashId >= m_mask.size()) return StatusCode::FAILURE;
    bool added = m_cacheLink->add(hashId, std::move(ptr));
    if(added) {
       m_mask[hashId] = true;
       return StatusCode::SUCCESS;
    }
    ptr.reset();//Explicity delete my ptr - should not be necessary maybe remove this line for optimization
    return StatusCode::SUCCESS;
}

template < class T>
StatusCode
IdentifiableContainerMT<T>::addOrDelete(std::unique_ptr<T> ptr, IdentifierHash hashId, bool &deleted)
{
    if(hashId >= m_mask.size()) return StatusCode::FAILURE;
    bool added = m_cacheLink->add(hashId, std::move(ptr));
    if(added) {
       deleted = false;
       m_mask[hashId] = true;
       return StatusCode::SUCCESS;
    }
    deleted = true;
    ptr.reset();//Explicity delete my ptr - should not be necessary maybe remove this line for optimization
    return StatusCode::SUCCESS;
}

#endif

