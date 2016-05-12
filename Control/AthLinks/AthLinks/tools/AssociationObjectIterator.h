/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file */
/*! \class AssociationObjectIterator 
 *  \brief object iterator for association maps (internal use only!) 
 */
/*! \class AssociationObjectIterator
 *
 * The \c AssociationObjectIterator protects the \c AssociationMap class from
 * specifics of the internal look-up map.\n
 * This is not a stand-alone class. It can only be compiled within the
 * \c AssociationMap class, or similar classes providing the required type
 * definitions! 
 *
 * \author Peter Loch <loch@physics.arizona.edu>
 * \date June 2, 2004 - first implementation
 *
 * fn AssociationObjectIterator begin()
 * brief begin object iterator
 *
 * fn AssociationObjectIterator end()
 * brief end object iterator 
 *
 * fn AssociationObjectIterator next()
 * brief advances iterator forward
 *
 * fn AssociationObjectIterator prev()
 * brief steps iterator back
 *
 * fn AssociationObjectIterator operator++()
 * brief same as \c next()
 *
 * fn AssociationObjectIterator operator++(int)
 * brief same as \c next()
 *
 * fn AssociationObjectIterator operator--()
 * brief same as \c prev()
 *
 * fn AssociationObjectIterator operator--(int)
 * brief same as \c prev()
 *
 * fn bool operator==(const AssociationObjectIterator&)
 * brief equality comparator
 *
 * fn bool operator!=(const AssociationObjectIterator&)
 * brief inequality comparator
 *
 * fn AssociationObjectIterator operator*()
 * brief iterator value access operator
 *
 * var const object_type* first
 * brief public variable holding object pointer
 *
 * var asso_iterator second
 * brief public variable holding iterator to associations
 * 
 * fn AssociationObjectIterator find(const object_type*)
 * brief returns iterator for a given object
 *
 * fn const object_type* getObject()
 * brief get pointer to object (same as \c first )
 *
 * fn const object_link& getObjectLink()
 * brief get link to object
 *
 * fn asso_iterator getFirstAssociation()
 * brief get iterator to first association (same as \c second.begin() )
 *
 * fn asso_iterator getLastAssocation()
 * brief get end iterator for associations (same as \c second.end() )
 *
 * fn asso_iterator findAssociation(const asso_type*)
 * brief returns iterator for specific associated object
 *
 * fn bool containsAssociation(const asso_type*)
 * brief checks if given object is associated with current object
 *
 * fn size_t size()
 * brief number of associations for current object
 *
 */

/*! \brief code only expands with the \a ASSOCIATIONMAP_CONTEXT */
#ifdef ASSOCIATIONMAP_CONTEXT 
 
class AssociationObjectIterator 
{ 
public: 

/*! \name Constructors and Destructor */ 
/*@{*/
/*! \brief default constructor */
AssociationObjectIterator() : m_store(0) 
{ }

/*! \brief useful alternate constructor 
 *
 * \param theStore  - reference to association map
 */
AssociationObjectIterator(const store_type& theStore)
  : m_store(&theStore) 
  , m_actual(theStore.begin())
{ }

/*! \brief useful alternate constructor 
 *
 * \param theStore  - reference to association map
 * \param pos       - reference to a position in the association map
 */
AssociationObjectIterator(const store_type& theStore, 
			  const typename store_type::const_iterator& pos)
  : m_store(&theStore) 
  , m_actual(pos)
{ }

/*! \brief copy constructor
 */
AssociationObjectIterator(const AssociationObjectIterator& rhs) :
  m_store ( rhs.m_store ),
  m_actual( rhs.m_actual )
{}

/*! \brief assignment operator
 */
AssociationObjectIterator& operator=(const AssociationObjectIterator& rhs)
{
  if ( this != &rhs ) {
    m_store = rhs.m_store;
    m_actual= rhs.m_actual;
  }
  return *this;
}

/*! \brief default destructor */
~AssociationObjectIterator() 
{ }
/*@}*/

/*! \name Iterator Manipulation */
/*@{*/

/*! \brief begin object iterator */
AssociationObjectIterator begin() 
{ return AssociationObjectIterator(*m_store); }

/*! \brief end object iterator */
AssociationObjectIterator end()
{ return AssociationObjectIterator(*m_store, m_store->end()); }

AssociationObjectIterator begin() const
{ return AssociationObjectIterator(*m_store); }
/*! \brief end object iterator */

AssociationObjectIterator end() const
{ return AssociationObjectIterator(*m_store,m_store->end()); }

/*! \brief forward advancement */
AssociationObjectIterator next()
{ ++m_actual; return *this; }

/*! \brief backward stepping */
AssociationObjectIterator prev()
{ --m_actual; return *this; }

/*! \brief post-forward advancement operator */
AssociationObjectIterator operator++()    
{ 
  AssociationObjectIterator tmp(*this); 
  ++m_actual;
  return tmp;
}

/*! \brief pre-forward advancement operator */
AssociationObjectIterator operator++(int) { return this->next(); }

/*! \brief post-backward stepping operator */ 
AssociationObjectIterator operator--()    
{ 
  AssociationObjectIterator tmp(*this);
  --m_actual;
  return tmp;
}

/*! \brief pre-backward stepping operator */
AssociationObjectIterator operator--(int) { return this->prev(); }
/*@}*/

/*! \name Iterator Comparisons */
/*@{*/
/*! \brief equality comparison 
 *
 * \param anOther   - an other \c AssociationObjectIterator
 */
bool operator==(const AssociationObjectIterator& anOther) const
{ return m_actual == anOther.m_actual; } 
// bool operator==(AssociationObjectIterator& anOther)
// { return m_actual == anOther.m_actual; } 
/*! \brief inequality comparison 
 *
 * \param anOther   - an other \c AssociationObjectIterator
 */
bool operator!=(const AssociationObjectIterator& anOther) const
{ return m_actual != anOther.m_actual; }
// bool operator!=(AssociationObjectIterator& anOther)
// { return m_actual != anOther.m_actual; }
/*@}*/

/*! \name Data Access */
/*@{*/
/*! \brief \c * operator returns iterator by value
 *
 * \warning This is not the standard behavior of STL-like iterators! This
 *          implementation hides the actual value type, though, because it
 *          is not the object type, as clients would probably expect. The
 *          stored objects can be accessed using the specialized methods
 *          provided by this class.
 */
AssociationObjectIterator operator*() const { return *this; }

/*! \brief access to underlying object */
const object_type* operator->() 
{ return this->getObject(); }

/*! \brief access to underlying object */
const object_type* operator->() const 
{ return this->getObject(); }

/*! \brief access to object pointer */
const object_type*  first()  const { return *(m_actual->first); }
/*! \brief access to iterator of first association */
asso_iterator       second() const 
{ return asso_iterator(m_actual->second,m_actual->second.begin()); }
/*! \brief find a given object in store 
 *
 * \param objectPointer - pointer reference to the object searched for
 *
 * This method returns an \c AssociationObjectIterator which can immediately
 * provide access to the associations using the \c second public variable,
 * the appropriate \c getFirstAssociation, \c getLastAssociation methods.
 */
AssociationObjectIterator find(const object_type* objectPointer) const
{
  const AssociationObjectIterator lastObj = this->end();
  AssociationObjectIterator firstObj = this->begin();
  AssociationObjectIterator foundObj = lastObj;
  while (firstObj != lastObj )
    {
      if ( firstObj.isValid() && firstObj.getObject() == objectPointer )
	{
	  foundObj = firstObj;
	  firstObj = lastObj;
	}
      else
	{
	  ++firstObj;
	}
    }
  return foundObj;
}
/*! \brief retrieve the pointer to a given object 
 *
 * This method allows direct access to the object pointer stored in each
 * \c AssociationObjectIterator instance.
 * 
 */
const object_type*  getObject()     const { return this->first(); }

/*! \brief retrieve link to the object. */
const object_link&  getObjectLink() const { return m_actual->first; }

bool  isValid()     const { return this->m_actual->first.isValid(); }

/*! \brief retrieve iterator to first associated object */
asso_iterator getFirstAssociation() const 
{ 
  return asso_iterator(m_actual->second, m_actual->second.begin()); 
}

/*! \brief retrieve end iterator to internal associated object store */
asso_iterator getLastAssociation()  const 
{ 
  return asso_iterator(m_actual->second, m_actual->second.end()); 
}

/*! \brief find the iterator for a given object */
asso_iterator findAssociation(const asso_type* assoPointer) const
{ 
  asso_iterator assItr = asso_iterator(m_actual->second,
				       m_actual->second.begin());
  return assItr.find(assoPointer); 
}

/*! \brief check if the given object can be matched among the associated objects */
bool containsAssociation(const asso_type* assoPointer) const
{ return this->findAssociation(assoPointer) != this->getLastAssociation(); }

/*! \brief get number of association for current pointed-to object */
size_t getNumberOfAssociations() const
{ return m_actual->second.size(); }

size_t size() const { return m_store->size(); }

private:

  const store_type* m_store;
  typename store_type::const_iterator m_actual;
};
#endif

