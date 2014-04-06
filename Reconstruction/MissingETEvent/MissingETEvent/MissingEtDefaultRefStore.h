// Yo emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef METREFINEMENT_MISSINGETDEFAULTREFSTORE_H
#define METREFINEMENT_MISSINGETDEFAULTREFSTORE_H

#include "EventKernel/INavigable4Momentum.h"

#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                   Default Reference Store Implementation                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


//namespace MissingEtMap
//{
  class MissingEtDefaultRefStore
  {
  public:
    /*! \brief Default reference object type                        */
    typedef INavigable4Momentum                        refobject_t;
    /*! \brief Default weight type in reference object              */ 
    typedef double                                     refweight_t;
    /*! \brief Default reference data type                          */
    typedef std::pair<const refobject_t*,refweight_t>  refdata_t;
    /*! \brief Default reference data store type                    */
    typedef std::vector<refdata_t>                     refstore_t;

#include "MissingETEvent/MissingEtDefaultRefStoreIterator.h"
#include "MissingETEvent/MissingEtDefaultRefStoreConstIterator.h"

    typedef MissingEtDefaultRefStoreIterator      iterator;
    typedef MissingEtDefaultRefStoreConstIterator const_iterator;

    // constructors
    MissingEtDefaultRefStore() { };
    virtual ~MissingEtDefaultRefStore() { };

    // store operations need to be defined here!
    iterator begin() { return iterator(m_store.begin()); }
    const_iterator begin() const { return const_iterator(m_store.begin()); }
    iterator end() { return iterator(m_store.end()); }
    const_iterator end() const { return const_iterator(m_store.end()); }

    // SIL size
    int size() const { return m_store.size(); }
    bool empty() const { return m_store.empty(); }
     
    // finding pointers
    iterator find(const refobject_t* pRef)
    {
      // search store
      iterator fRef = this->begin();
      iterator lRef = this->end();
      while ( fRef != lRef && pRef != fRef.getObjectPtr() ) { fRef++; }
      //
      return fRef;
    }
    const_iterator find(const refobject_t* pRef) const
    {
      // search store
      const_iterator fRef = this->begin();
      const_iterator lRef = this->end();
      while ( fRef != lRef && pRef != fRef.getObjectPtr() ) { fRef++; }
      //
      return fRef;
    }
    // inserting data
    void insert(const refobject_t* pRef, const refweight_t& weight)
    {
      refdata_t newRefEntry(pRef,weight);
      // check if already in store
      iterator fRef = this->find(pRef);
      if (fRef != this->end() )
	{
	  newRefEntry.second += fRef.getWeight();
	  this->erase(fRef);
	}
      m_store.push_back(newRefEntry);
    }
    // erasing data
    iterator erase(iterator fRef)
    {
      return iterator(m_store.erase(fRef.getInternalIterator()));
    }
    iterator erase(const refobject_t* pRef)
    {
      return this->erase(this->find(pRef));
    }

  private:
    refstore_t m_store;
  };
//};
#endif
