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
#include <iostream>
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
class IdentifiableContainerMT : public EventContainers::IdentifiableCache<T>, public DataObject
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
                auto ids = m_idContainer->ids();
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
                auto ids = m_idContainer->ids();
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



    // default constructor initializes the collection to hash max
    IdentifiableContainerMT(IdentifierHash hashMax, const typename EventContainers::IdentifiableCache<T>::Maker* maker = 0);

    /// return pointer on the found entry or null if out of
    ///  range using hashed index - fast version, does not call cnv
    ///  if object not there
    const T* indexFindPtr( IdentifierHash hashId ) const;

    const_iterator indexFind( IdentifierHash hashId ) const {
        const_iterator it(this, hashId);
        return it;
    }

    /// insert collection into container with id hash
    /// if IDC should not take ownership of collection, set ownsColl to false
    StatusCode addCollection(const T* coll, IdentifierHash hashId, bool ownsColl = true);


#ifdef IdentifiableCacheBaseRemove
    /// remove collection from container for id hash, returning it
    /// (and ownership) to client
    T*  removeCollection(IdentifierHash hashId);
#endif

    /// set m_hashids to have all the possible hash values given
    /// the hash max
    void setAllCollections(unsigned int hashMax);

    /// reset m_hashids and call DLV's cleanup
    void cleanup();

    /// return full size of container
    size_t fullSize() const {
        using namespace EventContainers;
        return IdentifiableCacheBase::fullSize();
    }

    //Duplicate of fullSize for backwards compatability
    size_t size() const {
        using namespace EventContainers;
        return IdentifiableCacheBase::fullSize();
    }    

    /// return number of collections
    size_t numberOfCollections() const {
        using namespace EventContainers;
        return IdentifiableCacheBase::numberOfHashes();
    }

    std::vector<IdentifierHash> GetAllCurrentHashs() const {
        using namespace EventContainers;
        return IdentifiableCacheBase::ids();
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
    const T* ptr = reinterpret_cast<const T*> (IdentifiableCacheBase::find (hashId));
    IdentifiableCacheBase::remove(hashId);
    return const_cast<T*>(ptr);
}
#endif


// default constructor initializes the IndentifiableCache
template < class T>
IdentifiableContainerMT<T>::IdentifiableContainerMT(IdentifierHash maxHash, const typename EventContainers::IdentifiableCache<T>::Maker* maker)
    :  EventContainers::IdentifiableCache<T>(maxHash, maker)
{

}

// return valuetype on the found entry or null if out of
//  range using hashed index - fast version, does not call cnv
//  if object not there
template < class T>
const T*
IdentifiableContainerMT<T>::indexFindPtr( IdentifierHash hashId ) const
{
    return ICACHE::find(hashId);
}

// insert collection into container with id hash
template < class T>
StatusCode
IdentifiableContainerMT<T>::addCollection(const T* coll, IdentifierHash hashId, bool ownsColl)
{
    // update m_hashids
    if (! ICACHE::add(hashId, coll, ownsColl)) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;

}



template < class T>
void
IdentifiableContainerMT<T>::cleanup()
{
    using namespace EventContainers;
    IdentifiableCacheBase::clear (EventContainers::IdentifiableCacheBase::void_unique_ptr::Deleter<T>::deleter);
}

#endif

