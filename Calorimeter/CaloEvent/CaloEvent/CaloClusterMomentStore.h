/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCLUSTERMOMENTSTORE_H
#define CALOEVENT_CALOCLUSTERMOMENTSTORE_H

#include "CaloEvent/CaloClusterMoment.h"

#include <vector>
#include <map>
#include <cstddef>

class CaloClusterMomentStore
{
 public:

  /*! \brief Default constructor */
  CaloClusterMomentStore();
  /*! Copy constructor */
  explicit CaloClusterMomentStore(const CaloClusterMomentStore& rMomStore);
  explicit CaloClusterMomentStore(const CaloClusterMomentStore* pMomStore);
  /// Assignment.
  CaloClusterMomentStore& operator=(const CaloClusterMomentStore& rMomStore);
  /*! \brief Default destructor */
  virtual ~CaloClusterMomentStore();

  /*! \brief Moment data */
  typedef CaloClusterMoment                         moment_value;
  /*! \brief Moment type */
  typedef CaloClusterMoment::MomentType             moment_type;
  /*! \brief Moment key type */
  typedef int                                       moment_key;
  /*! \brief Internal moment store type */
  typedef std::map<moment_key,moment_value>         moment_store;
  /*! \brief Internal moment store const iterator type */
  typedef moment_store::const_iterator              moment_store_const_iter;
  /*! \brief Internal moment store iterator type */
  typedef moment_store::iterator                    moment_store_iter;
  /*! \brief List of moment types */
  typedef std::vector<moment_type>                  moment_type_list;

  /*! \brief Internally used iterator */
  class CaloClusterMomentIterator
    {
    public:
      /*! \brief Default constructor */
      CaloClusterMomentIterator() { };
      /*! \brief Useful constructor */
      CaloClusterMomentIterator(moment_store_const_iter iStore)
	: m_actual(iStore) { };
      /*! \brief Destructor */
      ~CaloClusterMomentIterator() { };

      /*! \brief Iterator advance method */
      CaloClusterMomentIterator next()          { m_actual++; return *this; }
      /*! \brief Iterator post advance operator */
      CaloClusterMomentIterator operator++()    { return this->next(); }
      /*! \brief Iterator prior advance operator */
      CaloClusterMomentIterator operator++(int) { return this->next(); }
      
      /*! \brief Iterator reverse method */
      CaloClusterMomentIterator prev()          { m_actual--; return *this; }
      /*! \brief Iterator post reverse operator */
      CaloClusterMomentIterator operator--()    { return this->prev(); }
      /*! \brief Iterator prior reverse operator */
      CaloClusterMomentIterator operator--(int) { return this->prev(); }

      /*! \brief Equality comparator
       *
       *  \param anOtherIter reference to non-modifiable iterator to be 
       *                     compared to 
       */
      bool operator==(const CaloClusterMomentIterator& anOtherIter) const
	{ return m_actual == anOtherIter.m_actual; }
      /*! \brief Equality comparator
       *
       *  \overload
       *
       *  \param anOtherIter reference to modifiable iterator to be 
       *                     compared to 
       */
      bool operator==(CaloClusterMomentIterator& anOtherIter)
	{ return m_actual == anOtherIter.m_actual; } 

      /*! \brief Inequality comparator
       *
       *  \param anOtherIter reference to non-modifiable iterator to be 
       *                     compared to 
       */
      bool operator!=(const CaloClusterMomentIterator& anOtherIter) const
	{ return m_actual != anOtherIter.m_actual; }
      /*! \brief Inequality comparator
       *
       *  \overload
       *
       *  \param anOtherIter reference to modifiable iterator to be 
       *                     compared to 
       */
      bool operator!=(CaloClusterMomentIterator& anOtherIter)
	{ return m_actual != anOtherIter.m_actual; }

      /*! \brief Data access operator */
      //      CaloClusterMoment& operator*() { return this->getMoment(); }
      const CaloClusterMoment& operator*() const { return this->getMoment(); }
      /*! \brief Data access method */
      //      CaloClusterMoment& getMoment() { return (*m_actual).second; }
      const CaloClusterMoment& getMoment() const { return (*m_actual).second; }
      /*! \brief Key access method */
      moment_type getMomentType() const   
	{ return (moment_type)(*m_actual).first; }

    private:

      /*! \brief Internal moment iterator */
      moment_store_const_iter m_actual;
    };

  /*! \brief External moment iterator type */
  typedef CaloClusterMomentIterator           moment_iterator;

  /*! \brief Insert key/data pair
   *
   *  \param rMomType     reference to moment type (non-modifiable)
   *  \param rMomData     reference to moment data (non-modifiable)
   */
  virtual void insert(const moment_type&  rMomType,
		      const moment_value& rMomData);
  /*! \brief Retrieve cluster moment for a given key 
   *
   *  \param rMomType     reference to moment type (non-modifiable)
   *  \param rMomData     reference to moment data (modifiable object)
   *
   *  Returns <tt>true</tt> if moment for given key can be retrieved. The 
   *  data word is not modified if the moment type can not be found.
   */
  virtual bool retrieve(const moment_type& rMomType,
			moment_value&      rMomData) const;
  /*! \brief Retrieve cluster moment for a given key 
   *
   *  \overload
   *
   *  \param rMomIter     reference to moment iterator (non-modifiable)
   *  \param rMomData     reference to moment data (modifiable object)
   *
   *  Returns <tt>true</tt> if moment for given key can be retrieved. The 
   *  data word is not modified if the moment type can not be found.
   */
  virtual bool retrieve(const moment_iterator& rMomIter,
			moment_value&          rMomData) const;
  /*! \brief Retrieve list of moment types in store 
   *
   *  \param rMomTypeList  reference to modifiable list of moment types
   *
   *  Returns <tt>true</tt> if any moment types stored.
   */
  virtual bool retrieveMomentTypes(moment_type_list& rMomTypeList) const;
  /*!{ \brief Iterator access to moment store */
  virtual moment_iterator begin() const; /*!< starting iterator */
  virtual moment_iterator end() const;   /*!< iterator loop terminator */
  /*!}*/
  /*! \brief Find moment type
   *  
   *  \param rMomType   reference to non-modifiable moment type 
   *
   *  Implementation is similar to STL method.
   */
  virtual moment_iterator find(const moment_type& rMomType) const;
  /*! \brief Containment check 
   *
   *  \param rMomType   reference to non-modifiable moment type 
   */ 
  virtual bool contains(const moment_type& rMomType) const;
  /*! \brief Number of stored moments */
  size_t size() const;

  /*! \brief Access to internal store
   *
   * \attention Historic, depreciated.
   */
  virtual const moment_store& momentStore() const;
  /*! \brief Set internal store 
   *
   *  \param rMomStore reference to (non-modifiable) moment store
   *
   *  \attention Support for historic interface, depreciated.
   */
  virtual void setMomentStore(const moment_store& rMomStore );
  virtual void setMomentStore(moment_store&& rMomStore );

 private:

  moment_store m_store;
};

inline void CaloClusterMomentStore::insert(const moment_type&  rMomType,
					   const moment_value& rMomData)
{
  (*(m_store.insert(moment_store::value_type((moment_key)rMomType,rMomData))).first).second = rMomData;
}

inline CaloClusterMomentStore::moment_iterator 
CaloClusterMomentStore::begin() const
{
  return moment_iterator(m_store.begin());
}

inline CaloClusterMomentStore::moment_iterator
CaloClusterMomentStore::end() const
{
  return moment_iterator(m_store.end());
}

inline CaloClusterMomentStore::moment_iterator
CaloClusterMomentStore::find(const moment_type& rMomType) const
{
  return moment_iterator(m_store.find((moment_key)rMomType)); 
}

inline bool CaloClusterMomentStore::contains(const moment_type& rMomType) const
{
  return m_store.find((moment_key)rMomType) != m_store.end();
}

inline size_t CaloClusterMomentStore::size() const
{
  return m_store.size();
}

inline const CaloClusterMomentStore::moment_store&
CaloClusterMomentStore::momentStore() const
{
  return m_store;
}

inline void CaloClusterMomentStore::setMomentStore(const moment_store&
						   rMomStore)
{
  m_store = rMomStore;
}
inline void CaloClusterMomentStore::setMomentStore(moment_store&&
						   rMomStore)
{
  m_store = std::move(rMomStore);
}
/*! \class CaloClusterMomentStore
 *
 *  \brief Stores \a CaloClusterMoment in a keyed map.
 */
#endif
