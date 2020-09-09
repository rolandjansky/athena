// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVIGATIONTOKEN_H
#define NAVIGATIONTOKEN_H
///////////////////////////////////////////////////////////////////////////////
//
//  Navigation Token Template Class
//
//  Comments: (P.Loch, January 15, 2003)
//
//  The basic idea of the system is that each composite object can be queried
//  for constituents of one (or many) given object types. If this query can 
//  be honored, pointers to the constituent objects are stored in the token,
//  together with a kinematic weight. Otherwise the query is forwarded, if
//  the constituent objects are of composite nature themselves.
//  
///////////////////////////////////////////////////////////////////////////////

#include "Navigation/INavigationToken.h"
#include "Navigation/INavigable.h"

#include "Navigation/INavigationSelector.h"
#include "Navigation/INavigationCondition.h"

#include "Navigation/NavigationDefaults.h"
#include "Navigation/NavigationTokenHash.h"
#include "AthAllocators/ArenaPoolSTLAllocator.h"
#include <unordered_map>

#include <map>
#include <list>
#include <iterator>
#include <typeinfo>

#include <boost/any.hpp>

struct NavigationTokenIteratorTag
  : public std::bidirectional_iterator_tag
{
};

template <typename CHILD,
          typename CHILDPAR=NavigationDefaults::DefaultWeight,
          typename HASH=NavigationTokenHash<CHILD> >
class NavigationToken : public INavigationToken
{

 public:

 typedef const CHILD*                        const_child_ptr;
 typedef std::unordered_map<const_child_ptr,
                            CHILDPAR,
                            HASH,
                            std::equal_to<const_child_ptr>,
                            SG::ArenaPoolSTLAllocator<
                              std::pair<const const_child_ptr, CHILDPAR> >
                            >
    tokenStore; 
 typedef typename tokenStore::iterator       tokenIterator;
 typedef typename tokenStore::const_iterator tokenConstIterator;
 typedef CHILDPAR                            tokenParameter;

 /////////////////////
 // Iterator access //
 /////////////////////

 class NavigationTokenIterator
   : public std::iterator<NavigationTokenIteratorTag, const_child_ptr>
 {
   public:
   typedef CHILDPAR tokenParameter;
   
   NavigationTokenIterator() : m_store(0) { };
   NavigationTokenIterator(const tokenStore& aStore) 
   : m_store(&aStore)
   , m_actual(aStore.begin()) { };

   NavigationTokenIterator begin()
   {
     m_actual = m_store->begin();
     return *this;
   }

   NavigationTokenIterator end()
   {
     m_actual = m_store->end();
     return *this;
   }

   NavigationTokenIterator operator++()    { ++m_actual; return *this; }
   NavigationTokenIterator operator++(int)
   { NavigationTokenIterator tmp = *this; ++m_actual; return tmp; }
   NavigationTokenIterator operator--()    { --m_actual; return *this; }
   NavigationTokenIterator operator--(int)
   { NavigationTokenIterator tmp = *this; --m_actual; return tmp; }
   bool operator==(NavigationTokenIterator& anOtherToken)
   {
     return m_actual == anOtherToken.m_actual;
   }
   bool operator==(const NavigationTokenIterator& anOtherIterator) const
   {
     return m_actual == anOtherIterator.m_actual; 
   }
     
   bool operator!=(NavigationTokenIterator& anOtherToken)
   {
     return m_actual != anOtherToken.m_actual; 
   }
   bool operator!=(const NavigationTokenIterator& anOtherIterator) const
   {
     return m_actual != anOtherIterator.m_actual; 
   }
   const_child_ptr operator*() const
   {
     return (*m_actual).first;
   }
   const_child_ptr operator->() const
     {
     return (*m_actual).first;
   }
   const_child_ptr operator[](unsigned int theIndex) const
     {
       if ( theIndex >= m_store->size() ) return 0;
       m_actual = m_store->begin();
       unsigned int indexCtr = 0;
       while ( indexCtr != theIndex )
	 {
	   indexCtr++; m_actual++;
	 }
       return (*m_actual).first;
     }
   const tokenParameter& getParameter() const
     {
       return (*m_actual).second;
     }

   private:

   const tokenStore*   m_store;
   tokenConstIterator  m_actual;
 };

 typedef NavigationTokenIterator const_iterator;

 //////////////////////////////////
 // Constructors and destructors //
 //////////////////////////////////

 // default constructor
 NavigationToken(size_t size_hint = 10) : 
   m_data (size_hint),
   m_navCondition(0),
   m_navSelector(0),
   m_lastReject(0)
   { };

 // constructor with navigation selector argument
 NavigationToken( INavigationSelector<CHILD,CHILDPAR>* thisSelector,
                  size_t size_hint = 10,
                  const HASH& hf = HASH()) :
   m_data (size_hint, hf),
   m_navCondition(0),
   m_navSelector(thisSelector),
   m_lastReject(0)
   { };

 // constructor with navigation condition argument
 NavigationToken( INavigationCondition* thisCondition,
                  size_t size_hint = 0,
                  const HASH& hf = HASH()) :
   m_data (size_hint, hf),
   m_navCondition(thisCondition),
   m_navSelector(0),
   m_lastReject(0)
   { };

 // constructor with navigation condition and selector
 NavigationToken( INavigationSelector<CHILD,CHILDPAR>* thisSelector, 
		  INavigationCondition* thisCondition,
                  size_t size_hint = 0,
                  const HASH& hf = HASH()) :
   m_data (size_hint, hf),
   m_navCondition(thisCondition),
   m_navSelector(thisSelector),
   m_lastReject(0)
   { };

 // destructor
 virtual ~NavigationToken() 
 { };

 /////////////////////
 // Fill conditions //
 /////////////////////

 // check if query is to be forwarded
 virtual bool pushQuery( const INavigable& parent, 
			 const INavigable& child );

 //////////////////////////////////////////////////////
 // Access to object containers and objects in Token //
 //////////////////////////////////////////////////////

 // return reference to object store (map)
 const tokenStore& objectMap() { return m_data; };

 // return pointer to object store
 // tokenStore const * operator->() { return &m_data; };

 // return iterators for object access (obsolete - no list anymore) 
 // tokenListIterator  firstObject() { return m_list.begin(); };
 // tokenListIterator  lastObject()  { return m_list.end(); };

 // return iterators for object and weight access
 tokenConstIterator firstMappedObject() { return m_data.begin(); };
 tokenConstIterator lastMappedObject()  { return m_data.end(); };

 // direct type-safe iterators
 const_iterator begin() const { return NavigationTokenIterator(m_data).begin(); }
 const_iterator end()   const { return NavigationTokenIterator(m_data).end(); }
 unsigned int   size()  { return m_data.size(); }

 // return weight for a given object in container
 CHILDPAR getParameter( const_child_ptr data ) const;
 CHILDPAR getParameter( const_iterator& anIter) const;
  
 // clear/reset token
 void clear();
 void reset() { this->clear(); }

 // find out if a certain object is stored in the token
 bool contains( const_child_ptr data ) 
   { 
     return ( m_data.find(data) != m_data.end() );
   }

 ///////////////////////
 // Store object data //
 ///////////////////////

 // objects with default weight
 void setObject( const_child_ptr data )
   {
     this->setObject( data, CHILDPAR() );
   }

 void setObject( const CHILD& /*data*/ ) { };
 void setObject( const CHILD& /*data*/, const CHILDPAR& /*weight*/ ) { };

 // objects with variable weight FIXME!!!!!!!!!!
 void setObject( const_child_ptr data, CHILDPAR weight );

 // check if queried object is of requested type itself
 virtual bool trySetObject( const INavigable* theObject );

 virtual bool trySetObject( const INavigable* theObject,
			    const boost::any& theWeight);


 ////////////////////////////////
 // Operations on Token object //
 ////////////////////////////////

 // copy operator
 NavigationToken& operator=(const NavigationToken & anotherToken)
 {
   if ( this != &anotherToken && anotherToken.m_data != 0 )
     {
       m_data         = anotherToken.m_data;
       m_navCondition = anotherToken.m_navCondition; 
       m_navSelector  = anotherToken.m_navSelector;
     }   
   return *this;
 }

 // move operator
 NavigationToken& operator=(NavigationToken&& anotherToken)
 {
   if ( this != &anotherToken && anotherToken.m_data != 0 )
     {
       m_data         = std::move (anotherToken.m_data);
       m_navCondition = anotherToken.m_navCondition; 
       m_navSelector  = anotherToken.m_navSelector;
     }   
   return *this;
 }

 // dump 
 void dumpStore();

 private:
  
 tokenStore  m_data;

 // internal pointer to associated navigation processor
 INavigationCondition*       m_navCondition;
 INavigationSelector<CHILD,CHILDPAR>* m_navSelector;

  // Used to cache dynamic_cast results.
  const std::type_info*     m_lastReject;
};

#include "Navigation/NavigationToken.icc"

#endif

