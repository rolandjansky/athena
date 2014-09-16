/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_IDENTIFIABLECONTAINER_H
#define EVENTCONTAINERS_IDENTIFIABLECONTAINER_H
/**
 * @file IdentifiableContainer.h
 *
 * @brief This class is a general container which can hold objects of
 * accessed by an IdentifierHash 
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: IdentifiableContainer.h,v 1.39 2008-10-27 17:53:55 schaffer Exp $
 */

#include "Identifier/IdentifierHash.h"
#include "EventContainers/DataLinkVector.h"

#include <set>
#include <iostream> 

/** @class IdentifiableContainer
 *
 * @brief This class is a general container which can hold objects of
 * accessed by an IdentifierHash 
 *
 */
template < class T>
class IdentifiableContainer : public DataLinkVector<T>
{
public:

    typedef T        IDENTIFIABLE;
    typedef DataLinkVector<T> DLV;
    typedef IdentifiableContainer<T> MyType;
    typedef typename DataLinkVector<T>::DataLinkT DataLinkT;

    /// vector typedefs: it behaves like a vector<T*>
    typedef int        size_type;
    typedef T*&        reference;
    typedef T* const & const_reference;
    typedef T*         value_type;
    typedef T**        pointer;
    typedef T* const * const_pointer;
    typedef T          base_value_type;

    /// type for hash IDs
    typedef std::set<IdentifierHash> HashIDs_t;
    typedef typename HashIDs_t::const_iterator HashIDsIt_t;

    /// this const_iterator is for DataLinkT, a non-const object
    /// but DataLinkT is smart pointer for const T* 
    class const_iterator
    {
    public:

        /// iterator constructor
        const_iterator();

        /// assignment operator
        const_iterator& operator = ( const const_iterator & it );
        
        /// increment operator
        const_iterator& operator ++ ();

        /// increment operator
        const_iterator operator ++ ( int );
        
        /// access to DataLinkT by ref
        DataLinkT& operator * ();
        
        /// access to DataLinkT by pointer
        DataLinkT* operator-> ();

        /// comparison operator
        bool operator != ( const const_iterator &it ) const;

        /// comparison operator
        bool operator == ( const const_iterator &it ) const;

        /// hashId of the pointed-to element
        IdentifierHash hashId() const;
        
    protected:
        friend class IdentifiableContainer<T>;

        const_iterator( const MyType* idC, HashIDsIt_t it );
        const_iterator( const MyType* idC, DataLinkT *dl );
        
        // This method makes sure m_hashIDsIt is points to the first
        // DataHolder with data
        void lookForData();
        const MyType *m_idContainer;
        HashIDsIt_t m_hashIDsIt;
        DataLinkT * m_dl;
    };

    // default constructor initializes the DataLinkVector to hash max 
    IdentifiableContainer(unsigned int hashMax = 0 );

    /// return const_iterator for first entry
    const_iterator begin() const;

    /// return const_iterator for end of container
    const_iterator end() const;

    /// return const_iterator on the found entry or end() if out of
    ///  range using hashed index - fast version, does not call cnv
    ///  if object not there
    const_iterator indexFind( IdentifierHash hashId ) const;
    
    /// insert collection into container with id hash
    /// if IDC should not take ownership of collection, set ownsColl to false
    StatusCode addCollection(const T* coll, IdentifierHash hashId, bool ownsColl = true);

    /// remove collection from container for id hash, returning it
    /// (and ownership) to client
    T*  removeCollection(IdentifierHash hashId);

    /// set m_hashids to have all the possible hash values given
    /// the hash max
    void setAllCollections(unsigned int hashMax);
    
    /// reset m_hashids and call DLV's cleanup
    void cleanup();

    /// return full size of container
    int fullSize() const;
    
    /// return number of collections
    int numberOfCollections() const;
    
protected: 

    // list of hash IDs
    mutable HashIDs_t m_hashids;
};

/// inline methods

template < class T>
IdentifiableContainer<T>::const_iterator::const_iterator() 
        : 
        m_idContainer(0), 
        m_dl(0)
{}

template < class T>
typename IdentifiableContainer<T>::const_iterator& 
IdentifiableContainer<T>::const_iterator::operator = ( const const_iterator & it ) 
{
  if (this != &it) {
    m_idContainer = it.m_idContainer;
    m_hashIDsIt   = it.m_hashIDsIt;
    m_dl          = it.m_dl;
    if (m_dl == 0)
        lookForData();
  }
  return *this;
}

template < class T>
typename IdentifiableContainer<T>::const_iterator&
IdentifiableContainer<T>::const_iterator::operator ++ () 
{
    if (m_dl != 0)
        {
            // just go to end
            m_dl = 0;
            m_hashIDsIt = (m_idContainer->m_hashids).end();
            return *this;
        }

    ++m_hashIDsIt;
    lookForData();
    return *this;
}

template < class T>
typename IdentifiableContainer<T>::const_iterator
IdentifiableContainer<T>::const_iterator::operator ++ ( int ) 
{
    const_iterator tmp = *this;
    if (m_dl != 0)
        {
            // just go to end
            m_dl = 0;
            m_hashIDsIt = (m_idContainer->m_hashids).end();
            return tmp;
        }
    ++m_hashIDsIt;
    lookForData();
    return tmp;
}

template < class T>
typename IdentifiableContainer<T>::DataLinkT& 
IdentifiableContainer<T>::const_iterator::operator * () 
{
    if (m_dl != 0) return *m_dl;
    typename DLV::iterator it = m_idContainer->DLV::find(*m_hashIDsIt);
    return *it;
}

template < class T>
typename IdentifiableContainer<T>::DataLinkT* 
IdentifiableContainer<T>::const_iterator::operator-> () 
{
    if (m_dl != 0) return m_dl;
    typename DLV::iterator it = m_idContainer->DLV::find(*m_hashIDsIt);
    return &(*it);
}

template < class T>
bool 
IdentifiableContainer<T>::const_iterator::operator != ( const const_iterator &it ) const
{
    if ((m_dl != 0) or (it.m_dl != 0)) return it.m_dl != m_dl;
    return it.m_hashIDsIt != m_hashIDsIt;
}

template < class T>
bool
IdentifiableContainer<T>::const_iterator::operator == ( const const_iterator &it ) const
{
    if ((m_dl != 0) or (it.m_dl != 0)) return it.m_dl == m_dl;
    return it.m_hashIDsIt == m_hashIDsIt;
}

template < class T>
IdentifierHash
IdentifiableContainer<T>::const_iterator::hashId() const
{
    return *m_hashIDsIt;
}

template < class T>
IdentifiableContainer<T>::const_iterator::const_iterator( const MyType* idC, HashIDsIt_t it )
        : m_idContainer(idC), m_hashIDsIt(it), m_dl(0)
{}

template < class T>
IdentifiableContainer<T>::const_iterator::const_iterator( const MyType* idC, DataLinkT *dl )
        : m_idContainer(idC), m_dl(dl) 
{}

template < class T>
void
IdentifiableContainer<T>::const_iterator::lookForData() 
{
    if (!m_idContainer) return;
    HashIDsIt_t itE = (m_idContainer->m_hashids).end();
    typename DLV::iterator dlv_itE = m_idContainer->DLV::end();
    while (m_hashIDsIt != itE) {
        // check if DL is in DLV
        typename DLV::iterator it = m_idContainer->DLV::find(*m_hashIDsIt);
        if (dlv_itE != it)
            // check if DL is valid
            if (it.hasData()) break;
        ++m_hashIDsIt;
    }
}


// default constructor initializes the DataLinkVector 
template < class T>
IdentifiableContainer<T>::IdentifiableContainer(unsigned int hashMax)
{  
    // This constructor should be called with hashMax set the the
    // max hash value. If set to 0, one can initialize DLV itself
    // by calling init with the hashmax.

    if(hashMax) DLV::init( hashMax );
}

// return const_iterator on first entry
template < class T>
typename IdentifiableContainer<T>::const_iterator
IdentifiableContainer<T>::begin() const 
{
    const_iterator it(this, m_hashids.begin());
    it.lookForData(); 
    return it; 
}

template < class T>
typename IdentifiableContainer<T>::const_iterator
IdentifiableContainer<T>::end() const 
{ 
    return const_iterator(this, m_hashids.end());
}

// return const_iterator on the found entry or end() if out of
//  range using hashed index - fast version, does not call cnv
//  if object not there
template < class T>
typename IdentifiableContainer<T>::const_iterator
IdentifiableContainer<T>::indexFind( IdentifierHash hashId ) const 
{ 
    // check if DL is valid
    typename DLV::iterator it = DLV::find(hashId);
    if( it != DLV::end() && it->hasData() ) {
        return const_iterator(this, &*it);
    }
    return end();
}

// insert collection into container with id hash
template < class T>
StatusCode      
IdentifiableContainer<T>::addCollection(const T* coll, IdentifierHash hashId, bool ownsColl)
{
            
    // update m_hashids
    if (!m_hashids.insert(hashId).second) return StatusCode::FAILURE;

    // set DataLink to have the transient pointer
    typename DLV::iterator it = DLV::find(hashId);
    if( it != DLV::end()) {
        it->setDataPtr(coll);
        // set IDC to own the collection
        if (ownsColl)DLV::setObjectOwnership(hashId,true);
        return StatusCode::SUCCESS;
    } 
    else {
        std::cout << "addCollection: it is at end " << (unsigned int)hashId << std::endl;
        return StatusCode::FAILURE;
    }
}

template < class T>
T*  
IdentifiableContainer<T>::removeCollection(IdentifierHash hashId)
{
           
    // remove hash from set
    m_hashids.erase(hashId);

    // get collectino pointer if it exists
    typename DLV::iterator it = DLV::find(hashId);
    if( it != DLV::end() ) {
        DLV::setObjectOwnership(hashId,false);
        T* result = it->getDataPtr();
        it->reset();
        return (result);
    }
    else {
        return (0);
    }
}

template < class T>
void
IdentifiableContainer<T>::setAllCollections(unsigned int hashMax)
{
    int iMax = hashMax;
    for (int i=0; i<iMax; ++i)
        m_hashids.insert(i);
}

template < class T>
void
IdentifiableContainer<T>::cleanup()
{
    m_hashids.clear();
    DLV::cleanup();
}

template < class T>
int
IdentifiableContainer<T>::fullSize() const
{
    return DLV::size();
}

template < class T>
int
IdentifiableContainer<T>::numberOfCollections() const
{
    return m_hashids.size();
}

#endif 
