// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETMAPBASE_H
#define JETEVENT_JETMAPBASE_H

#include "DataModel/OwnershipPolicy.h"
#include <vector>
#include <map>
#include <memory>

template<class P>
class JetMapBase 
{
public:

  /*! @brief Publish payload data type */ 
  typedef P                      data_t;
  /*! @brief Publish record type */
  typedef std::vector<data_t>    record_t;

  /** \brief pointer wrapper, just to make certain the default pointers in maps are NULL
      
      We need pointers for to reasons :
         - avoid many copying of vectors
         - Some collection maybe in VIEW_ELEMENTS mode, but
          still jets accessed through them should change the same moments
          (so we can't just copy moments)
      This strucuture is maybe overkill  : simple record_t* might work as well... 
   */
  class record_ptr_t {
  public:
    record_ptr_t(record_t* r=NULL) : m_ptr(r) {};
    bool isValid()const{return m_ptr !=NULL;}
    record_t & operator*()const {return *m_ptr;}
    record_t* operator->() const {return m_ptr;}
    record_t* get() const {return m_ptr;}
    void destroy() {if (isValid()) delete m_ptr; m_ptr=NULL;}
    void set(record_t *r){m_ptr = r;}
  protected:
    record_t * m_ptr;
  };
  /*! @brief Publish key type */
  typedef size_t    key_t;  
  /*! @brief publish store type */ 
  typedef std::map<key_t, record_ptr_t>  map_t;


  /*! @brief Constructor  */
  JetMapBase();
  /*! @brief No copy ctor, assignment. */
  JetMapBase(const JetMapBase& base) = delete;
  JetMapBase& operator=(const JetMapBase& base) = delete;
  /*! @brief destructor */
  virtual ~JetMapBase();

  /*! @brief Add a record 
   *
   *  This method adds a record to the store and returns the index of this
   *  record to the client. The returned value is  @c false if the record 
   *  already existed
   *
   *  @param JetIndex reference to modifiable index, will contain the last 
   *         index in the store.
   */
  virtual bool addRecord(size_t jetIndex) const;

  /*! @brief get the full record for a given jet
   *
   *
   */
  virtual const record_t* getRecord(size_t jetIndex) const;

  /*! @brief assign a given record to a given jet
   *
   * The map will 'own' the given record : it will delete it when destructed.
   * If a record already exist at jetIndex, it will be deleted.
   *
   *  @param jetIndex index of the record in the store
   *  @param rec pointer to the record to store.
   */
  virtual void assignRecord(size_t jetIndex, record_t* rec) const;

  /*! @brief Add data 
   *
   *  This method adds data to an existing record.
   *
   *  @param jetIndex index of the record in the store
   *  @param keyIndex index of the data in the record
   *  @param data     reference to non-mdoifiable data to be stored in record
   *
   *  Note that this method will overwrite any previously stored data at the
   *  (jetIndex,keyIndex) position.
   */
  virtual void addData(size_t jetIndex,size_t keyIndex,const data_t& data)
    const;
  /*! @brief Retrieve data
   *
   *  Retrieves data by index pair and stores result in client supplied 
   *  variable. Returns @c false if index pair (jetIndex,keyIndex) refers to 
   *  non-existing  entry, i.e. at least one of (jetIndex,keyIndex) is out of
   *  range.
   *
   *  @param jetIndex index of record in store
   *  @param keyIndex index of data in record
   *  @param data     reference to modifiable variable storing retrieved data
   *
   *  Note that in case of an invalid index pair the original value of @c data
   *  is not changed. 
   */ 
  virtual bool retrieveData(size_t jetIndex,size_t keyIndex,data_t& data) 
    const;

  virtual const data_t& accessData(size_t jetIndex,size_t keyIndex) const;

  virtual size_t numberOfMoments(size_t jetIndex) const;

  virtual void clear();
  virtual void clear(SG::OwnershipPolicy ){clear();}

  SG::OwnershipPolicy ownPolicy(){return m_ownPolicy;};

protected:

  /*! @brief remove a record 
   *
   * Destroy the record associated to an entry.
   *
   *  @param jetIndex identifier of the record to remove
   */
  virtual void removeRecord(size_t jetIndex) const;

  /*! @brief transfer a record from an other map
   *
   *
   *  @param fromMap pointer to the other map
   *  @param oldIndex identifier of the record in the other map
   *  @param newIndex the identifier in this map
   */
  virtual void transferRecord(const JetMapBase<P> *fromMap, size_t oldIndex, size_t newIndex ) const;

  friend class Jet;
  template <class T>
  friend class JetMomentMapConverterBase;

  map_t& map();
  const map_t& map() const;


#if !defined(__REFLEX__) && !defined(__CLING__)
  // /*! @brief Store last valid record index */
  // mutable size_t m_lastIndex; not used so remove it for simplicity
  /*! @brief Data store */
  mutable map_t  m_store;
  /*! @brief Null data reference */
  static data_t m_nullData;

  typename map_t::iterator m_end;
#endif

  
  SG::OwnershipPolicy m_ownPolicy ;
};

class OrphanJetCounter {
public:
  size_t getNewId(){m_counter++;return m_counter;};
  
protected:
  void reset(){m_counter=0;}

  size_t m_counter;
  

};

/*! @class JetMapBase 
 *  
 *  Templated base class for map implementations with indexed lookup. The 
 *  internal data store is organized in records of a payload data type (each
 *  record is an entry in the store).
 */

#if !defined(__REFLEX__) && !defined(__CLING__)
#include "JetEvent/JetMapBase.icc"
#endif

#endif
