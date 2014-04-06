// -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETEVENT_MISSINGETMAP_H
#define MISSINGETEVENT_MISSINGETMAP_H

#include "GaudiKernel/MsgStream.h"

#include "MissingETEvent/MissingEtDefaultRefStore.h"

#include <string>
#include <cmath>
#include <vector>
#include <utility>

#include <ios>
#include <iomanip>

/////////////////////////////////

template<typename KEYOBJECT,
	 typename REFSTORE=MissingEtDefaultRefStore >
class MissingEtMap
{
 public:
  
  // publishing types: key objects and reference store types
  typedef KEYOBJECT                              key_t; 
  typedef REFSTORE                          refstore_t; 
  
  typedef typename refstore_t::refdata_t     refdata_t;
  typedef typename refstore_t::refobject_t refobject_t;
  typedef typename refstore_t::refweight_t refweight_t;

  typedef std::pair<const key_t*,refstore_t>    data_t;
  typedef std::vector<data_t>                  store_t;
  
  // key iterator
  typedef typename store_t::iterator           iiterator;
  typedef typename store_t::const_iterator     iconst_iterator;

  // data iterators
  typedef typename refstore_t::iterator             ref_iterator;
  typedef typename refstore_t::const_iterator       const_ref_iterator;

  /*! \brief Constructor with pre-storage allocation (optional) */
  MissingEtMap(size_t initAlloc=0,
	       size_t initSize=0) 
  { 
    // check allocation
    if ( initAlloc != 0 ) 
      m_store.reserve((initAlloc > initSize ? initAlloc : initSize)); 
    // check initial size
    if ( initSize != 0 ) this->resize(initSize);
  };


  virtual ~MissingEtMap() { };

#include "MissingETEvent/MissingEtKeyIterator.h"

 typedef MissingEtKeyIterator      iterator;
 typedef MissingEtKeyConstIterator const_iterator;
 
 // map handlers
 iterator       begin()       { return iterator(m_store.begin());       }
 const_iterator begin() const { return const_iterator(m_store.begin()); }

 ref_iterator       begin(iterator fKey);
 const_ref_iterator begin(const_iterator fKey) const;
  
 iterator       end()         { return iterator(m_store.end());         }
 const_iterator end() const   { return const_iterator(m_store.end());   }

 ref_iterator       end(iterator lKey);
 const_ref_iterator end(const_iterator lKey) const; 

 iterator       erase(iterator  anIter);
 iterator       erase(const key_t* pKey);
 iterator       erase(const size_t& iKey); 
 iterator       remove(iterator anIter)  { return this->erase(anIter); }
 iterator       remove(const key_t* pKey) { return this->erase(pKey);  }
 iterator       remove(const size_t& iKey) { return this->erase(iKey);  }
 void           clear()                   { m_store.clear(); }

 void insert(const key_t* pKey, 
	     const refobject_t* pRef, 
	     const double& weight=double(1.0)); 
 void insert(const key_t* pKey, 
	     const size_t& iKey, 
	     const refobject_t* pRef,
	      const double& weight=double(1.0));
 
 // other information
 size_t size()                  const;
 size_t resize(size_t& newSize,data_t nullData=data_t((const key_t*)0,
						      refstore_t()));
 bool empty()                   const { return m_store.empty(); }
 size_t getNumberOfReferences() const;
 size_t getNumberOfKeys()       const;
  
  // key access
  const key_t* getKeyPtr(const_iterator anIter) const;
  const key_t* getKeyPtr(const size_t& iKey)     const;
  const key_t& getKey(const_iterator anIter)    const; 
  const key_t& getKey(const size_t& iKey)        const;

  // index access
  size_t index(const_iterator fKey) const 
  { return this->index(fKey,this->begin()); }
  size_t index(const_iterator lKey,const_iterator fKey) const;
  size_t index(iterator fKey) const 
  { return this->index(fKey,this->begin()); }
  size_t index(iterator lKey,iterator fKey) const;

  // data access
  const refstore_t&  getRefStore(const_iterator anIter) const;
  refstore_t&        getRefStore(iterator anIter);
  const refstore_t&  getRefStore(const key_t* pKey) const;
  refstore_t&        getRefStore(const key_t* pKey);
  const refstore_t&  getRefStore(const size_t& iKey) const;
  refstore_t&        getRefStore(const size_t& iKey);

  // data access through operators
  const refstore_t&  operator[](size_t iKey) const 
  { return this->getRefStore(iKey); }
  refstore_t&        operator[](size_t iKey)
  { return this->getRefStore(iKey); }
  const refstore_t&  operator[](const key_t* pKey) const
  { return this->getRefStore(pKey); }
  refstore_t&        operator[](const key_t* pKey)
  { return this->getRefStore(pKey); }

  // find method
  const_iterator find(const size_t& iKey) const;
  iterator       find(const size_t& iKey);
  const_iterator find(const key_t* pKey) const;
  iterator       find(const key_t* pKey);

  // dump map
  void dump(MsgStream& report, MSG::Level msgLvl, 
	    const std::string& mapName=std::string("DEFAULT")) const;

  // specific filters
  //  template<typename REFOBEJCT>
  //  bool getKeyObjects(typename std::vector<const key_t*>& listOfKey) const;

  // get map type
  const std::string& getMapType() const { return m_mapType; }

  // get the list of all the objects used of any type, i.e. electrons, taus, etc. and the corresponding weights
  template<class object_type>
  bool getObjects(typename std::vector<const object_type*>& objects,std::vector<double>& weights) const;

 protected:

  store_t m_store;

  std::string m_mapType;
  void setMapType(const std::string& mapType) { m_mapType = mapType; }

  iterator getIterator(size_t iKey);
  const_iterator getIterator(size_t iKey) const;

  iiterator getIIterator(size_t iKey);
  iconst_iterator getIIterator(size_t iKey) const;
};

#include "MissingETEvent/MissingEtMap.icc"

/*! \class MissingEtMap
 * 
 *  \brief General purpose event store for MissingEt
 *
 *  \author Peter Loch <loch@physics.arizona.edu>
 *  \date March 24, 2006
 *
 * This is a multi-dimensional store managing relations between objects,
 * including possible weights. It is very similar to framework association 
 * objects and may be replaced by them at some point in time. For the moment
 * this somewhat specialized implementation has been adapted for the concrete
 * use cases from MissingEt reconstruction.
 *
 * Store Organization:
 * -------------------
 *
 * The object relations in this store are referenced by a key object 
 * \a KEYOBJECT and an associated index. Typically, for each key object a
 * list of relations is kept, including the kinematic weight. This list
 * is stored in the \a REFSTORE storage object, which is defaulted to the
 * \a MissingEtDefaultStore implementation. 
 * 
 */

#endif
