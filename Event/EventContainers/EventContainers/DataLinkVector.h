/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_DATALINKVECTOR_H
#define EVENTCONTAINERS_DATALINKVECTOR_H
/**
 * @file DataLinkVector.h
 *
 * @brief This class is a a vector of DataLink for objects of type DC
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: DataLinkVector.h,v 1.26 2008-10-27 17:53:55 schaffer Exp $
 */

#include "GaudiKernel/DataObject.h" 
#include <vector>


/** @class DataLinkVector
 *
 * @brief This class is a a vector of DataLink for objects of type DC
 * It uses an internal chain to keep track of modified entries.
 * A call to cleanup is required after processing an event or prior to
 * process a new event.
 * cleanup will call the clear() method of all the touched DataLink instances.
 * The template expects the DataLink to export the DigitCollection class type (DC)
 * and the DigitCollection identifier type
 */

template < class DC >
class DataLinkVector :public DataObject 
{
public:

    typedef DC                      value_type;
    typedef DC*                     pointer;
    typedef const DC*               const_pointer;
    typedef DC&                     reference;
    typedef const DC&               const_reference;

    /** class DataHolder is a simple class which holds a pointer to an
     * object of type DC. This is inserted into the Entry object of the
     * DataLinkVector.
     **/
    class DataHolder 
    {
    public:
        /// contructor
        DataHolder();

        /// is pointer set
        bool            hasData() const;
        
        /// access to pointer
        const_pointer   operator->() const;

        /// access to pointer
        operator        const_pointer () const { return this->getDataPtr(); }

        /// access to pointer
        const_pointer   cptr() const;

        /// reset pointer to 0
        void            reset();
        
        /// set pointer
        void            setDataPtr(const DC* obj);
        
        /// access to pointer
        pointer         getDataPtr() const;
        
    private:

        pointer         m_obj;
    };
        

    typedef DataHolder         DataLinkT;
    typedef DataLinkVector<DC> MyType;

    /** class Entry is a link in a chained list, used for efficient
     * cleanup of vectors. Entry objects are chained together which
     * allows fast iteration over non-empty elements
     **/
    class Entry
    {
    public:
        /// constructor
        Entry();

        /// assignement operator
        Entry& operator = (const Entry& e);

        /// access to next entry
        Entry*             previousEntry();

        /// access to the DataLink
        DataLinkT&         dataLink();

        /// ownObject is true if this object should be deleted by DLV.
        bool               ownObject() const;

        /// set previous entry
        void               setPreviousEntry(Entry* entry);
        
        /// set data link
        void               setDataLink    (const DataLinkT& link);

        /// set object ownership
        void               setOwnObject(bool owns);

    private:
        Entry*    m_previousEntry;
        DataLinkT m_dataLink;
        // ownObject is true if this object should be deleted by DLV.
        // May be false if object is managed by a data pool
        bool      m_ownObject;
    }; 
    typedef std::vector<Entry> EntryVector;
    typedef typename EntryVector::iterator EntryVectorIt; 

    /** class iterator - iterator over elements in DataLinkVector
     * which manages the Entry objects
     **/
    class iterator
    {
    public:
        /// constructor
        iterator();
            
        /// comparison operator
        bool operator != ( const iterator it ) const;
            
        /// comparison operator
        bool operator == ( const iterator it ) const;
            
        /// assignment operator
        iterator& operator = ( const iterator it );
            
        /// increment operator
        iterator& operator ++ ();
            
        /// increment operator
        iterator operator ++ ( int );
            
        /// access to object by ref
        DataLinkT& operator * ();
            
        /// access to object by pointer
        DataLinkT* operator-> ();
            
        /// test if DataHolder has data
        bool hasData();

    protected:
        friend class DataLinkVector<DC>;

        iterator( const MyType* dlv, 
                  EntryVectorIt it );
            

        /// make sure entry is chained
        /// If not, add entry to the chain otherwise do nothing because
        void checkChain();
            
        /// protected method for derived class to access m_vect
        EntryVector& entryVector();

        const MyType* m_dlv;
        EntryVectorIt m_it;
    };

    friend class iterator;


    /// Default constructor
    DataLinkVector();
    
    /// Default destructor
    ~DataLinkVector();
    
    /// initialize vector with nbr DataLinks
    void init( unsigned int nbr );
    
    /// return iterator on first entry
    iterator begin() const;
    
    /// return iterator on after last entry
    iterator end() const;
    
    /// return iterator on the found entry or end() if out of range
    iterator find( IdentifierHash id ) const;
    
    /// set object ownership
    bool setObjectOwnership(IdentifierHash idhash, bool flag);
    
    /// return reference on DataLink
    /// BEWARE: Doesn't check for boundary limits !
    DataLinkT& operator [] ( int id );

    /// cleans up the DataLinkVector
    /// goes trough all chained entries and call clear() method of DataLink
    /// and clear chain of course.
    virtual void cleanup();

    /// access to size of vector
    int size() const;
    
protected:

    /// add pointed entry in chain of modified entries
    void chainEntry( Entry* e ) const;
    	
    mutable EntryVector m_vect;
    mutable Entry* m_last;
};

/// inline definitions

template <class DC>
DataLinkVector<DC>::DataHolder::DataHolder()
        :
        m_obj(0)
{}

template <class DC>
bool
DataLinkVector<DC>::DataHolder::hasData() const
{
    return (m_obj != 0);
}

template <class DC>
typename DataLinkVector<DC>::const_pointer 
DataLinkVector<DC>::DataHolder::operator->() const 
{ 
    return this->getDataPtr(); 
}

template <class DC>
typename DataLinkVector<DC>::const_pointer 
DataLinkVector<DC>::DataHolder::cptr() const 
{ 
    return this->getDataPtr(); 
}

template <class DC>
void
DataLinkVector<DC>::DataHolder::reset()
{
    m_obj = 0;
}
        
template <class DC>
void
DataLinkVector<DC>::DataHolder::setDataPtr(const DC* obj)
{
    m_obj = const_cast<pointer>(obj);
}

template <class DC>
typename DataLinkVector<DC>::pointer 
DataLinkVector<DC>::DataHolder::getDataPtr() const
{
    return (m_obj);
}

template <class DC>
DataLinkVector<DC>::Entry::Entry()
        : 
        m_previousEntry(NULL), 
        m_ownObject(false)
{
    m_dataLink =  DataLinkT(); 
}

template <class DC>
typename DataLinkVector<DC>::Entry& 
DataLinkVector<DC>::Entry::operator = (const Entry& e){
    m_dataLink        = e.m_dataLink ;
    m_previousEntry   = e.m_previousEntry; 
    m_ownObject       = e.m_ownObject;
    return *this; 
}

template <class DC>
typename DataLinkVector<DC>::Entry*             
DataLinkVector<DC>::Entry::previousEntry() 
{
    return m_previousEntry;
}

template <class DC>
typename DataLinkVector<DC>::DataLinkT&   
DataLinkVector<DC>::Entry::dataLink() 
{
    return m_dataLink;
}

template <class DC>
bool               
DataLinkVector<DC>::Entry::ownObject() const
{
    return m_ownObject;
}

template <class DC>
void               
DataLinkVector<DC>::Entry::setPreviousEntry(Entry* entry)
{
    m_previousEntry = entry;
}
        
template <class DC>
void               
DataLinkVector<DC>::Entry::setDataLink    (const DataLinkT& link)
{
    m_dataLink = link;
}

template <class DC>
void               
DataLinkVector<DC>::Entry::setOwnObject(bool owns)
{
    m_ownObject = owns;
}



template <class DC>
DataLinkVector<DC>::iterator::iterator()
        : m_dlv(NULL) {}


template <class DC>
bool
DataLinkVector<DC>::iterator::operator != ( const iterator it ) const
{ return it.m_it != m_it; }

template <class DC>
bool 
DataLinkVector<DC>::iterator::operator == ( const iterator it ) const
{ return it.m_it == m_it; }

template <class DC>
typename DataLinkVector<DC>::iterator& 
DataLinkVector<DC>::iterator::operator = ( const iterator it ) 
{ m_dlv = it.m_dlv; m_it = it.m_it; return *this; }

template <class DC>
typename DataLinkVector<DC>::iterator& 
DataLinkVector<DC>::iterator::operator ++ () 
{ ++m_it; return *this; }

template <class DC>
typename DataLinkVector<DC>::iterator 
DataLinkVector<DC>::iterator::operator ++ ( int ) 
{ return iterator( m_dlv, m_it++ ); }

template <class DC>
typename DataLinkVector<DC>::DataLinkT& 
DataLinkVector<DC>::iterator::operator * () 
{ checkChain(); return m_it->dataLink(); }

template <class DC>
typename DataLinkVector<DC>::DataLinkT* 
DataLinkVector<DC>::iterator::operator-> () 
{ checkChain(); return &m_it->dataLink(); }

template <class DC>
bool 
DataLinkVector<DC>::iterator::hasData() 
{   
    // check if DL is valid, and cash the state
    checkChain(); // this entry has been touched.
    Entry& e      = *m_it; 
    bool DL_valid = e.dataLink().hasData();	
    return DL_valid; 
}

template <class DC>
DataLinkVector<DC>::iterator::iterator( const MyType* dlv, 
                                        EntryVectorIt it )
        : m_dlv(dlv), m_it(it) {}  

// make sur entry is chained
// If not, add entry to the chain otherwise do nothing because

template <class DC>
void
DataLinkVector<DC>::iterator::checkChain()
{ if( m_it->previousEntry() == NULL ) m_dlv->chainEntry( &(*m_it) ) ;  }

// protected method for derived class to access m_vect

template <class DC>
typename DataLinkVector<DC>::EntryVector& 
DataLinkVector<DC>::iterator::entryVector() 
{
    return m_dlv->m_vect ; 
}

// Default constructor
template <class DC>
DataLinkVector<DC>::DataLinkVector() 
        : m_last(NULL) { }

// Default constructor
template <class DC>
DataLinkVector<DC>::~DataLinkVector() 
{
    this->cleanup();
}

// initialize vector with nbr DataLinks
template <class DC>
void
DataLinkVector<DC>::init( unsigned int nbr )
{
    // fill in vector of Entry
    m_vect.resize(nbr); 
}

// return iterator on m_previousEntry entry
template <class DC>
typename DataLinkVector<DC>::iterator
DataLinkVector<DC>::begin() const 
{ return iterator( this, m_vect.begin() ); }

// return iterator on after last entry
template <class DC>
typename DataLinkVector<DC>::iterator
DataLinkVector<DC>::end() const 
{ return iterator( this, m_vect.end()); }

// return iterator on the found entry or end() if out of range
template <class DC>
typename DataLinkVector<DC>::iterator
DataLinkVector<DC>::find( IdentifierHash id ) const 
{ return ( (id >= m_vect.size() )
           ?end():iterator(this,m_vect.begin()+id));}

// set object ownership
template <class DC>
bool
DataLinkVector<DC>::setObjectOwnership(IdentifierHash idhash, bool flag)
{
    if ( idhash >= m_vect.size() )
        return false;
    Entry& e = *(m_vect.begin()+idhash);
    e.setOwnObject(flag);
    return true;
}
    
// return reference on DataLink
// BEWARE: Doesn't check for boundary limits !
template <class DC>
typename DataLinkVector<DC>::DataLinkT&  
DataLinkVector<DC>::operator [] ( int id )
{ 
    Entry* e = &(m_vect[id]);
    if( !e->previousEntry() )
        chainEntry( e );
    return e->dataLink();
}

// cleans up the DataLinkVector
// goes trough all chained entries and call clear() method of DataLink
// and clear chain of course.
template <class DC>
void
DataLinkVector<DC>::cleanup()
{
    Entry *e;
    // assign last to e and check if it is not NULL
    if( (e = m_last) )
        {
            // for chained entries
            do {
                Entry * tmp = e->previousEntry();
                e->setPreviousEntry(NULL);	// clear chain pointer
                if (e->ownObject())     // delete object
                    delete (e->dataLink()).getDataPtr();
                e->dataLink().reset(); // reset keeps it in identified state.
                e->setOwnObject(false); // doesn't own object
                e = tmp;	// go to next entry
            } while( e != m_last ); // until we end where we started
            m_last = NULL;	// clear chain hook
        }
}

template <class DC>
int
DataLinkVector<DC>::size() const {
    return m_vect.size(); 
}

template <class DC>
void
DataLinkVector<DC>::chainEntry( Entry* e ) const 
{
    if( m_last == NULL ) {
        e->setPreviousEntry(e);
        m_last = e;
    }
    else {
        e->setPreviousEntry(m_last->previousEntry());
        m_last->setPreviousEntry(e);
    }
}
	

#endif
