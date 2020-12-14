/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVIGATION_NAVWRAPPER_H
#define NAVIGATION_NAVWRAPPER_H

#include "Navigation/INavigationToken.h"
#include "Navigation/NavigableTerminalNode.h"

#include <boost/any.hpp>

template<class NAVIMPL>
class NavWrapper 
{
public:

  /*! \brief Publish navigation implementation type */
  typedef NAVIMPL                                                    navimpl_t;
  // /*!{ \brief Publish important types from \a Navigable, new style */
  //  typedef typename navimpl::container_t                       container_t;
  //  typedef typename navimpl::constituent_t                   constituent_t;
  //  typedef typename navimpl::parameter_t                       parameter_t;
  //  typedef typename navimpl::object_iter_t                   object_iter_t;
  //  typedef typename navimpl::external_index_t             external_index_t;
  /*!}*/
  /*!{ \brief Publish important types from \a Navigable, old style, soon to be
   * <b>obsolete</b>! 
   */
  typedef typename navimpl_t::container_type                   container_type;
  typedef typename navimpl_t::constituent_type               constituent_type;
  typedef typename navimpl_t::parameter_type                   parameter_type;
  typedef typename navimpl_t::object_iter                         object_iter;
  typedef typename navimpl_t::external_index_type         external_index_type;
  /*!}*/

  /*! \brief Default constructor */
  NavWrapper() : m_nav(navimpl_t()) { };
  /*! \brief Optional constructor: construct from Navigable */
  NavWrapper(const navimpl_t& theNav) : m_nav(theNav) { };
  /*! \brief Base class destructor */
  virtual ~NavWrapper() { };

  // common interface with Navigable FIXME improve code organization

  /*! \brief Visitor handler without relational parameter
   *
   *  This is the implementation of the \a INavigable interface.
   */
  virtual void fillToken(INavigationToken& navToken) const
  { m_nav.fillToken(navToken); }

  /*! \brief Visitor handler including a relational parameter 
   *
   *  This is the implementation of the \a INavigable interface.
   */
  virtual void fillToken(INavigationToken& navToken, 
			 const boost::any& parentParameter) const
  { m_nav.fillToken(navToken, parentParameter); }

  //////////////////////////////////
  // Add data objects or elements //
  //////////////////////////////////

  /*! \brief Add element to Navigable store 
   *
   * Element is referenced by pointer (internal linear index search). 
   */
  void putElement(const container_type*   pCont,
		  const constituent_type* pObj,
		  const parameter_type&   oPar = parameter_type(),
                  size_t                  sizeHint = 0)
  { m_nav.putElement(pCont,pObj,oPar,sizeHint); }
  /*! \brief Add element to the Navigable store */
  void putElement(const container_type*   pCont,
		  external_index_type&    iObj,
		  const parameter_type&   oPar = parameter_type(),
                  size_t                  sizeHint = 0)
  { m_nav.putElement(pCont,iObj,oPar,sizeHint); }
  /*! \brief Insert element into Navigable store */
  void insertElement(const container_type*   pCont,
		     const constituent_type* pObj,
		     const parameter_type&   oPar = parameter_type(),
                     size_t                  sizeHint = 0)
  { m_nav.insertElement(pCont,pObj,oPar,sizeHint); }
  /*! \brief Insert element into Navigable store */
  void insertElement(const container_type*   pCont,
		     external_index_type&    iObj,
		     const parameter_type&   oPar = parameter_type(),
                     size_t                  sizeHint = 0)
  { m_nav.insertElement(pCont,iObj,oPar,sizeHint); }

  ///////////////////////
  // Data manipulation //
  ///////////////////////

  // re-weight (overwrite old weight)
  void reweight(const constituent_type* pObj,
		const parameter_type&   oPar = parameter_type())
  { m_nav.reweight(pObj,oPar); }
  void reweight(const container_type* pCont,
		external_index_type&  iObj,
		const parameter_type& oPar = parameter_type())
  { m_nav.reweight(pCont,iObj,oPar); }
  void reweight(object_iter&          oIter,
		const parameter_type& oPar = parameter_type())
  { m_nav.reweight(oIter,oPar); }

  // remove object from the Navigable
  bool remove(const constituent_type* pObj) 
  { return m_nav.remove(pObj); }
  bool remove(const container_type* pCont, external_index_type& iObj)
  { return m_nav.remove(pCont,iObj); }
  bool removeAll() { return m_nav.removeAll(); }

  // check if constituent is already there  
  bool contains(const constituent_type* pObj) const
  { return m_nav.contains(pObj); }
  bool contains(const container_type* pCont,
		external_index_type&  iObj) const
  { return m_nav.contains(pCont,iObj); }

  ////////////////////////////
  // Access to Constituents //
  ////////////////////////////

  // public object access: iterators, sizes etc.
  virtual object_iter begin() const { return m_nav.begin(); }
  virtual object_iter end()   const { return m_nav.end(); }
  virtual size_t size()       const { return size_t(m_nav.size()); }

  // public container access: relational parameter retrieval
  parameter_type getParameter(const constituent_type* pObj) const
  { return m_nav.getParameter(pObj); }
  parameter_type getParameter(const container_type* pCont,
			      external_index_type&  iObj) const
  { return m_nav.getParameter(pCont,iObj); }
  parameter_type getParameter(object_iter& oIter) const
  { return m_nav.getParameter(oIter); }

  // public container access: retrieve ElementLink for given object pointer
  const container_type* getContainer(const constituent_type* pObj)  const
  { return m_nav.getContainer(pObj); }
  //  const container_type* getContainer(constituent_const_iter  oIter) const
  //  { return m_nav.getContainer(oIter); }
  const container_type* getContainer(object_iter             oIter) const
  { return m_nav.getContainer(oIter); }

  // get index of child in original container
  bool getIndex(const constituent_type* pObj, 
		external_index_type&    iObj) const
  { return m_nav.getIndex(pObj,iObj); }
  //  bool getIndex(constituent_const_iter anIter,
  //		external_index_type& iObj) const;
  bool getIndex(object_iter          oIter,
		external_index_type& iObj) const
  { return m_nav.getIndex(oIter,iObj); }

 protected:

  navimpl_t&       getNavImpl()       { return m_nav; }
  const navimpl_t& getNavImpl() const { return m_nav; }

 private:
  /*! \brief Wrapped Navigable implementation */
  navimpl_t m_nav;
};
/*! 
 *  \class NavWrapper
 * 
 *  \brief Wrapper for Navigable implementation
 * 
 *  \date   June 2, 2006
 *  \author Peter Loch <loch@physics.arizona.edu>
 * 
 *  This templated wrapper decouples the navigation implementation 
 *  (implementing \a INavigable) from the client class by forwarding
 *  the \a Navigable behaviour to the actual implementation, rather than
 *  inheriting it. In particular this means this template itself is not
 *  derived from \a INavigable. This wrapper together with the \a P4Wrapper
 *  allows an un-ambiguous implementation of the combined 
 *  \a INavigable4Momentum interface. 
 *
 */

//class NavigableTerminalNode
//{
//public:
//};
/*! \class NavigableTerminalNode
 *
 *  \brief Dummy type needed fro specialized implementation 
 * 
 *  \date   June 2, 2006
 *  \author Peter Loch <loch@physics.arizona.edu>
 */

template<>
class NavWrapper<NavigableTerminalNode>
{ 
public:

  /*! \brief Default constructor */
  NavWrapper() { };
  /*! \brief Base class destructor */
  virtual ~NavWrapper() { };

  /*!{ \brief Do-nothing visitor */
  virtual void fillToken(INavigationToken& /*rToken*/) const { };
  virtual void fillToken(INavigationToken& /*rToken*/,
			 const boost::any& /*rPar*/  ) const { };
  /*!}*/
};
/*! \class NavWrapper<NavigableTerminalNode>
 *
 *  \brief Specialized wrapper for singular navigable data objects
 *
 *  \date   June 2, 2006
 *  \author Peter Loch <loch@physics.arizona.edu>
 *
 *  Specialization of the \a NavWrapper template for singular data objects,
 *  i.e. data objects without constituents. All constituent store manipulations
 *  (adding, removing, reweighting, etc.) are omitted. 
 */
#endif
