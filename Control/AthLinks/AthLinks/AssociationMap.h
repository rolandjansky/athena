/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHLINKS_ASSOCIATIONMAP_H
#define ATHLINKS_ASSOCIATIONMAP_H

#define ASSOCIATION_CONTEXT
#define ASSOCIATIONMAP_CONTEXT

/*! \brief templated base class for a look-up map for associations
 * 
 *  The templated base class \c AssociationMap provides all functionality to 
 *  support a mapped look-up between object of a given type and their
 *  associations, typically of a different type (many-to-many look-up). 
 *  Concrete objects instantiated from implementation classes derived from
 *  \c AssociationMap are not automatically storable. The concrete class needs
 *  to either be \c DataObject itself, or the corresponding object needs to
 *  be collected into a storable \c DataVector.
 * 
 *  The internal storage model is a matrix, where the rows are keyed by the
 *  object pointer and the columns are pointers to associations. The number of
 *  columns (= associations) typically varies row-by-row.
 * 
 *  \author Peter Loch <loch@physics.arizona.edu>
 *  \date  June 1, 2004 - first implementation
 */

#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "AthLinks/tools/GenerateIndexingPolicy.h"

#include <map>
#include <list>
#include <algorithm>

template<class OBJCONT,class ASSCONT>
class AssociationMap
{
 public:
  
  /*! \name Argument and Internal Store Types */
  /*@{*/
  typedef OBJCONT                                       object_container_type;
  typedef typename object_container_type::base_value_type         object_type;
  typedef 
    typename 
    SG::GenerateIndexingPolicy<object_container_type>::type::index_type
                                                            object_index_type;
  typedef ElementLink<object_container_type>                      object_link;
  typedef typename std::list<const object_type*>                  object_list;

  typedef ASSCONT                                         asso_container_type;
  typedef typename asso_container_type::base_value_type             asso_type;
  typedef 
    typename 
    SG::GenerateIndexingPolicy<asso_container_type>::type::index_type
                                                              asso_index_type;
  typedef typename std::list<const asso_type*>                      asso_list;
  typedef ElementLink<asso_container_type>                          asso_link;
  typedef ElementLinkVector<asso_container_type>                   asso_store;
  typedef typename asso_store::const_iterator             asso_store_iterator;
  typedef typename std::map<object_link,asso_store>                store_type;
  typedef typename store_type::iterator                   store_iterator_type;
  /*@}*/

  /*! \name Constructors and Destructor */
  /*@{*/
  AssociationMap(); 
  virtual ~AssociationMap();
  /*@}*/


  /*! \name Store Iterator Types */
  /*@{*/
#include "AthLinks/tools/AssociationVectorIterator.h"
  /*! \brief association iterator type */
  typedef AssociationVectorIterator                              asso_iterator;
#include "AthLinks/tools/AssociationObjectIterator.h"
  /*! \brief object iterator type */
  typedef AssociationObjectIterator                            object_iterator;
  /*@}*/

  /*! \name Add Associations
   *
   * \param objectContainer   - pointer to the object container
   * \param objectIndex       - index of object in container
   * \param objectPointer     - pointer to object
   * \param assoContainer     - container of potentially associated objects
   * \param assoIndex         - index of associated object in container
   * \param assoPointer       - pointer to associated object
   */
  /*@{*/
  void addAssociation(const object_container_type* objectContainer,
		      const object_index_type&     objectIndex,
		      const asso_container_type*   assoContainer,
		      const asso_index_type&       assoIndex);
  void addAssociation(const object_container_type* objectContainer,
		      const object_type*           objectPointer,
		      const asso_container_type*   assoContainer,
		      const asso_type*             assoPointer);
  void addAssociation(const object_link& objectLink,
                      const asso_link& assoLink);
  /*@}*/

  /*! \name Object and Association Iterators
   *
   * The association iterators are returned for a given object, which can
   * either be referenced by its pointer or its iterator in the internal store.
   * \param objectPointer   - pointer to a given object
   * \param objectIter      - iterator to a given object
   */
  /*@{*/
  /*! \brief begin iterator for objects */
  object_iterator beginObject()  const;
  /*! \brief end iterator for objects */
  object_iterator endObject()   const;

  /*! \brief begin iterator for associations */ 
  asso_iterator beginAssociation(const object_type* objectPointer)  const;
  asso_iterator beginAssociation(const object_iterator& objectIter) const;
  /*! \brief end iterator for associations */
  asso_iterator endAssociation(const object_type* objectPointer)    const;
  asso_iterator endAssociation(const object_iterator& objectIter)   const;
  /*@}*/

  /// \brief get association iterators by object iterator



  /// \brief type-safe retrieval of object pointer from iterator
  const object_type* getObject(const object_iterator& objectIter) const
    { return (*objectIter).getObject(); }

  /// \brief finding an object with allocation
  object_iterator findObject(const object_type* theObject) const
    { return object_iterator(m_associationMap).find(theObject); }

  /// \brief testing if object is in store
  bool containsObject(const object_type* theObject) const
    { return this->findObject(theObject) != this->endObject(); }

  /// \brief retrieve number of objects in store
  size_t getNumberOfObjects() const { return this->size(); }

  /// \brief associations iterator access
  ///
  /// The associations are accessed as function of the row key (the object).


  const asso_type* getAssociation(asso_iterator assoIter) const
    { return *assoIter; }

  /// \brief find association
  asso_iterator findAssociation(const object_iterator& objectIter,
				const asso_type* assoPointer) const
    { return objectIter.findAssociation(assoPointer); }
  asso_iterator findAssociation(const object_type* objectPointer,
				const asso_type* assoPointer) const;

  /// \brief containment check
  bool containsAssociation(const object_iterator& objectIter,
			   const asso_type* assoPointer) const
    { return objectIter.containsAssociation(assoPointer); }

  bool containsAssociation(const object_type* objectPointer,
			   const asso_type* assoPointer) const;
  bool containsAssociation(const asso_type* assoPointer) const;

  /// \brief get all objects for a given association
  bool getObjects(const asso_type* assoPointer, object_list& theObjects) const;

  /// \brief get all objects for a given association
  bool getObjects(const asso_iterator& assoIter, object_list& theObjects) const
    { return this->getObjects(*assoIter,theObjects); }

  /// \brief get all associations for a given object
  bool getAssociations(const object_type* objPointer, asso_list& assocs) const;

  /// \brief get all associations for a given object
  bool getAssociations(const object_iterator& objIter, asso_list& assocs) const
    { return this->getAssociations(objIter.getObject(),assocs); }

  /// \brief get number of associations
  size_t size() { return m_associationMap.size(); }

  /// \brief get number of associations
  size_t size() const { return m_associationMap.size(); }

  size_t size(const object_type* objectPointer) const;

  size_t getNumberOfAssociations(const object_type* objectPointer) const
    { return this->size(objectPointer); }

  size_t size(const object_iterator& objectIter) const
    { return objectIter.getNumberOfAssociations(); }

  size_t getNumberOfAssociations(const object_iterator& objectIter) const
    { return this->size(objectIter); }

 protected:

  /// \brief internal store
  store_type m_associationMap;

  /// \brief internally used find method
  store_iterator_type internalFind(const object_link& objectLink,
				   const asso_link& assoLink)
    {
      // check key
      store_iterator_type mapEnd    = m_associationMap.end();
      store_iterator_type foundIter = mapEnd;
      for ( store_iterator_type iMap = m_associationMap.begin();
	    iMap != mapEnd;
	    ++iMap ) {
	// look for the address of the pointed-at object 
	// must dereference the ElementLink pointer
	if ( iMap->first.cptr() == objectLink.cptr() ) {
	  foundIter = iMap;
	  break;
	}
      }
      
      if ( foundIter == m_associationMap.end() ) { return foundIter; }
      // check data
      if ( std::find((foundIter->second).begin(),
		     (foundIter->second).end(),
		     assoLink) !=
	   (foundIter->second).end() )
	{ return foundIter; }
      // not found at all
      return m_associationMap.end();
    }

  /// \brief internally used function to add links to store
  bool addToStore(const object_link& objectLink, const asso_link& assoLink);

};

#include "AthLinks/AssociationMap.icc"

#undef ASSOCIATION_CONTEXT
#undef ASSOCIATIONMAP_CONTEXT
#endif
