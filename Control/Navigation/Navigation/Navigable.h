/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVIGATION_NAVIGABLE_H
#define NAVIGATION_NAVIGABLE_H
///////////////////////////////////////////////////////////////////////////////
///
///              Generalization of Navigable Object Features 
///
/// \author  P. Calafiura
/// \author  P. Loch
/// \author  S. Rajagopalan
///
/// \brief Navigable template generalization to handle navigation
///
///  The \c Navigable<CONT,RPAR,COLL> generalization handles navigation queries
///  and provides persistifiable data stores for composite reconstruction
///  objects. The template parameters are:
///
///  \c CONT - storable container type \n
///  \c RPAR - relational parameter type \n
///  \c COLL - <em>internal collection type for future use<\em>
///
///  \date   Oct  7, 2003 update: first implementation
///  \date   Dec 18, 2003 update: move to ElementLink internal store
///  \date   Apr 27, 2004 update: included in release 8.1.0 and up, bug fixes 
// Authors:  P. Calafiura, P. Loch, S. Rajagopalan                           //
// Comments: Implements navigation query with honoring or forwarding the     //
//           client request for a given object type. The actual              //
//           implementation is in Navigable.icc                              //
//                                                                           //
//           Template Parameters:                                            //
//                                                                           //
//           CONT - container type for given constituent object type. Needs  //
//                  to provide a value_type declaration and the usual        //
//                  container access methods. Defaulted to dummy             //
//                  NavTerminator for terminal nodes in relational tree.     //
//           RPAR - parameter describing the relation between constituent    //
//                  and composite object. Needs to provide +,-,+=,-=,=       //
//                  operators to manage parameter evolution in the           //
//                  relational tree. Defaulted to dummy parameter            //
//                  DefaultWeight for trees without parameters.              //
//           COLL - allows to specify internal storage container. Needs to   //
//                  provide typical container management like find(),        //
//                  insert(), iterators... Defaulted to container type       //
//                  DefaultChildColl<CONT,RPAR>.                             //
//                  There should be no need to provide an explicite internal //
//                  container! Be careful when using this parameter!         //
//                                                                           //
// Created:  07-Oct-03  Calafiura/Loch/Rajagopalan                           //
// Update:   18-Dec-03  Loch                                                 //
//                      - implement ElementLink -> change of template        //
//                        parameters necessary!                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// package includes for the template
#include "Navigation/INavigable.h"
#include "Navigation/NavigableIterator.h"
#include "Navigation/NavigationDefaults.h"

// navigation token management
#include "Navigation/INavigationToken.h"
#include "Navigation/NavigationToken.h"

// services etc.
#include "AthLinks/tools/GenerateIndexingPolicy.h"

// STL includes
#include <vector>
#include <map>
#include <utility>  // pair

// boost include (pointer arbitration)
#include <boost/any.hpp>

// i/o
#include <iostream>

////////////////////////////////////////////////////////////////
// Base class for implementation of Navigation token handling //
////////////////////////////////////////////////////////////////

template < 
  typename CONT=
  NavigationDefaults::TerminalNode,
  typename RPAR=
  NavigationDefaults::DefaultWeight,
  typename COLL=typename 
  NavigationDefaults::DefaultChildColl<CONT, RPAR>::type 
	 >     
class Navigable : virtual public INavigable 
{
  public:

  //////////////////////
  // Type definitions //
  //////////////////////

  // internal collection management helper types
  typedef COLL                                                constituent_coll;
  typedef typename constituent_coll::iterator                 constituent_iter;
  typedef typename constituent_coll::const_iterator     constituent_const_iter;

  // typedefs added to make the template parameter types accessible from outside the template
  // needed for transient/persistent conversion
  // Marcin.Nowak@cern.ch 06.2006
  
  /*! \brief Object container type */
  typedef CONT	 	container_type;
  /*! \ brief Relational parameter type */
  typedef RPAR   	parameter_type;
  /*! \brief Collection manager type */
  typedef typename NavigationDefaults::DefaultChildColl<container_type,parameter_type> navhelper_t;

  
  ////////////////////////////////////////////////////////////////////////////
  //                                                                        //
  // At this point we try to avoid an additional template parameter         //
  // specifying the object type in addition to the object container type.   //
  // We expect DataVector or any other object container to provide this     //
  // object type. Note that this is NOT DataVector<T>::value_type, which is //
  // actually T*. We introduced another typedef                             // 
  // DataVector<T>::base_value_type, which is T.                            //
  //                                                                        //
  // Within the Navigable the following types are available:                //
  //                                                                        //
  // constituent_type          object type                                  //
  // constituent_ptr_ref       object pointer reference                     //
  //                           (constituent_type*&)                         //
  // constituent_const_ptr_ref object const pointer reference               //
  // constituent_ptr           object pointer (constituent_type*)           //
  // external_index_type       object container index type                  //
  // object_iter               internal object container iterator           //
  //                                                                        //
  // Note that the internal object container is used to store pointers to   //
  // the constituents of a composite object, while the (external) object    //
  // container just holds a collection of objects.                          //
  //                                                                        //
  // Template Parameters:   Defaults:                                       //
  //                                                                        //
  // CONT                   TerminalNode     external object collection     //
  // RPAR                   DefaultWeight    relational parameter type      //
  // COLL                   DefaultChildColl internal object collection     //
  //                                                                        //
  ////////////////////////////////////////////////////////////////////////////

  // some typedefs from the object container
  typedef typename CONT::base_value_type constituent_type;
  typedef typename CONT::reference       constituent_ptr_ref;
  typedef typename CONT::const_reference constituent_const_ptr_ref;
  typedef typename CONT::value_type      constituent_ptr;

  // object container index type
  typedef typename SG::GenerateIndexingPolicy<CONT>::type::index_type
                                         external_index_type;

  // external iterator access
  typedef NavigableIterator<CONT,RPAR,COLL> object_iter;

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  Navigable();

  virtual ~Navigable();

  ////////////////////////////////////////////////////////////////////
  // NavigationToken handling (INavigable interface implementation) //
  ////////////////////////////////////////////////////////////////////

  // no relational parameters
  virtual void fillToken(INavigationToken& navToken) const;

  // forwarding of parameters included
  virtual void fillToken(INavigationToken& navToken, 
			 const boost::any& parentParameter) const;

  //////////////////////////////////
  // Add data objects or elements //
  //////////////////////////////////

  // If sizeHint is non-zero, it should give the expected final size
  // of the container.  This may be used to optimize memory allocations.

  // add element to the Navigable: use element pointer (linear search!)
  //  template<typename OBJT>
  void putElement(const CONT*             objectContainer, 
		  const constituent_type* constituentObject,
		  const RPAR&             objectParameter=RPAR(),
                  size_t                  sizeHint = 0);
  // add element to the Navigable: use element index (direct access!)
  void putElement(const CONT*          objectContainer,
		  external_index_type& theIndex,
		  const RPAR&          objectParameter=RPAR(),
                  size_t               sizeHint = 0);
  // insert element without checking if already in store: use element pointer
  void insertElement(const CONT*             objectContainer,
		     const constituent_type* constituentObject,
		     const RPAR&             objectParameter=RPAR(),
                     size_t                  sizeHint = 0);
  // insert element without checking if already in store: use index
  void insertElement(const CONT*          objectContainer,
		     external_index_type& theIndex,
		     const RPAR&          objectParameter=RPAR(),
                     size_t               sizeHint = 0);
  // insert element without checking if already in store: use index
  // store is supplied explicitly.
  void insertElement(const CONT*          objectContainer,
		     external_index_type& theIndex,
                     IProxyDict*          sg,
		     const RPAR&          objectParameter=RPAR(),
                     size_t               sizeHint = 0);
  // insert element without checking if already in store: use ElementLink.
  void insertElement(const ElementLink<CONT>& el,
		     const RPAR&          objectParameter=RPAR(),
                     size_t               sizeHint = 0);

  ///////////////////////
  // Data manipulation //
  ///////////////////////

  // re-weight (overwrite old weight)
  void reweight(const constituent_type* constituentObject,
		const RPAR& objectParameter=RPAR());
  void reweight(const CONT* objectContainer,
		external_index_type& theIndex,
		const RPAR& objectParameter=RPAR());
  void reweight(object_iter& objectIter,
		const RPAR& objectParameter=RPAR());


  // remove object from the Navigable
  bool remove(const constituent_type* aConstituent);
  bool remove(const CONT* objectContainer, external_index_type& theIndex);
  bool removeAll() { m_constituents.clear(); return this->size() == 0 ; }

  /// replace old object container with a new container
  /// assumes that all objects have been or will be 
  /// inserted to the new container 
  bool replace(const CONT* newObjectContainer);

  // check if constituent is already there  
  bool contains(const constituent_type* aConstituent) const;
  bool contains(const CONT* objectContainer,
		external_index_type& theIndex) const;

  ////////////////////////////
  // Access to Constituents //
  ////////////////////////////

  // public object access: iterators, sizes etc.
  virtual object_iter begin() const;    
  virtual object_iter end()   const;
  virtual unsigned int size()  const;

  // public link access
  //  virtual link_iter& beginLinks() const;
  //  virtual link_iter& endLinks()   const;

  // public container access: relational parameter retrieval
  RPAR getParameter(const constituent_type* aConstituent) const;
  RPAR getParameter(const CONT* objectContainer,
		    external_index_type& theIndex) const;
  RPAR getParameter(object_iter& theIter) const;

  //  RPAR getParameter(const ElementLink<CONT>* theLink) const;

  // public container access: retrieve ElementLink for given object pointer
  const CONT* getContainer(const constituent_type* aConstituent) const;
  const CONT* getContainer(constituent_const_iter  anIter)       const;
  const CONT* getContainer(object_iter             objectIter)   const;

  // get index of child in original container
  bool getIndex(const constituent_type* aConstituent, 
		external_index_type& theIndex) const;
  bool getIndex(constituent_const_iter anIter,
		external_index_type& theIndex) const;
  bool getIndex(object_iter objectIter,
		external_index_type& theIndex) const;

  ///////////////////////////
  // Protected data access //
  ///////////////////////////

  protected:

  // get collection of constituents
  const constituent_coll& getConstituents() const { return m_constituents; }
  constituent_coll& getConstituents()             { return m_constituents; }

  // get pointers to collection
  const constituent_coll* getPtrToConstituents() const 
  { return &m_constituents; }
  constituent_coll* getPtrToConstituents() 
  { return &m_constituents; }

  // get pointer to constituent given its collection iterator 
  virtual const constituent_type* getConstituentPtr(constituent_iter 
						    iter) const;
  virtual const constituent_type* getConstituentPtr(constituent_const_iter 
						    iter) const;

  // get the parameter associated with the child, if any
  virtual RPAR getConstituentPar(constituent_iter iter)       const;
  virtual RPAR getConstituentPar(constituent_const_iter iter) const;

  // dump store
  void dumpStore() const;

  /////////////////////////////
  // Internal query handling //
  /////////////////////////////

  private:

  //////
  // to be checked: constituent type is T* -> NavigationToken expects T!
  /////

  // honoring the query: filling token if type matches w/o parameter
  virtual void apply(NavigationToken<constituent_type>& navToken) const;

  // honoring the query: filling token if type matches w/ parameter
  virtual void apply(NavigationToken<constituent_type, RPAR>& navToken,
		     const RPAR& aParameter) const;

  // forwarding the query to constituents: w/o parameter
  virtual void toKidsAfterAccept(INavigationToken& navToken) const;
  virtual void toKidsAfterReject(INavigationToken& navToken) const;

  // forwarding the query to constituents: w/ parameter 
  virtual void toKidsAfterAccept(INavigationToken& navToken, 
				 const boost::any& aParameter) const;
  virtual void toKidsAfterReject(INavigationToken& navToken, 
				 const boost::any& aParameter) const;

  ///////////////////////////
  // Internal data storage //
  ///////////////////////////

  private:

  constituent_coll m_constituents;

};

//////////////////////////
// Termination handling //
//////////////////////////

#include "Navigation/NavigableTerminalNode.h"

//////////////////////////////
// Navigable implementation //
//////////////////////////////

#include "Navigation/Navigable.icc"

/*! \class Navigable
 * 
 *  \brief Generalization of navigable composite object implementation\
 *
 *  This class provides the most common implementation for a navigable
 *  composite object in Athena. In particular, homogeneous composites with only
 *  only one constituent type are easily implemented using this template.
 */ 
#endif 
