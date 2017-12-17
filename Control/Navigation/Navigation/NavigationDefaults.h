/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVIGATION_NAVIGATIONDEFAULTS_H
#define NAVIGATION_NAVIGATIONDEFAULTS_H
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

#include "AthLinks/ElementLink.h"
#include "AthLinks/tools/GenerateIndexingPolicy.h"

#include <utility>
#include <vector>
#include <algorithm>

namespace NavigationDefaults
{
  ////////////////////////////////
  // Namespace Type Definitions //
  ////////////////////////////////

  //  typedef unsigned int external_index_type;

  //////////////////
  // Default Type //
  //////////////////

  struct DefaultType{};   // completely dummy

  ///////////////////////
  // Default Container //
  ///////////////////////

  struct DefaultContainer // needs to provide a "type"
  {
    public:
      typedef DefaultType type;
  };

  typedef DefaultContainer TerminalNode;

  ////////////////////////////////
  // Default Insertion Handling //
  ////////////////////////////////

  ////////////////////////////////
  // Default Relation Parameter //
  ////////////////////////////////

  struct DefaultWeight {
    public:
    DefaultWeight() {}
    DefaultWeight& operator=(const DefaultWeight&) {return *this;}
    const DefaultWeight& operator+(const DefaultWeight&) const {return *this;}
    const DefaultWeight& operator*(const DefaultWeight&) const {return *this;}
    bool operator==(DefaultWeight&) const {return true;}
    operator double() {return 1.0;}
  };

  typedef DefaultWeight DefaultParameter;

  //////////////////////////////////////////////////////////////////////
  // Default Constituent Collections: Including Relational Parameters //
  //////////////////////////////////////////////////////////////////////

  template <typename CONT, typename RPAR>
    struct DefaultChildColl
    {
      // default container is a vector of (ElementLink,RPAR) pairs
      //typedef typename std::pair< const ElementLink<CONT>*, RPAR > value_type;
      typedef typename std::pair< ElementLink<CONT>, RPAR > value_type;
      typedef typename std::vector< value_type >                  type;
      typedef typename type::iterator                    type_iterator;
      typedef typename type::const_iterator        type_const_iterator;
      typedef typename CONT::base_value_type*                child_ptr;
      typedef const typename CONT::base_value_type*    const_child_ptr;
      typedef typename SG::GenerateIndexingPolicy<CONT>::type::index_type
                                                          external_index_type;
    
      // object pointer (key) access (note: outer *() operates on ElementLink!)
      static const_child_ptr getChildPtr(type_const_iterator thisIter)
      { 
        const_child_ptr thePointer = 0;
	if ((*thisIter).first.isValid() )
	  thePointer = *((*thisIter).first);
	return thePointer; 
      }

      // relational parameter (data) access
      static RPAR getChildPar(type_const_iterator thisIter) 
      { return (*thisIter).second; }

      // ElementLink pointer access 
      static const ElementLink<CONT>* getElementPtr(type_const_iterator 
						    thisIter)
      {
	return &((*thisIter).first);
      }

      // container access through iterator
      static const CONT& getContRef(type_const_iterator thisIter)
      {
	return ((*thisIter).first).getStorableObjectRef();
      }

      // container access through pointer
      static const CONT& getContRef(const type& cont, const_child_ptr aChild)
      {
	return getContRef(find(cont,aChild));
      }

      // pointer to container access by iterator
      static const CONT* getContPtr(type_const_iterator thisIter)
      {
	return ((*thisIter).first).getStorableObjectPointer();
      }

      // pointer to container access by pointer
      static const CONT* getContPtr(const type& cont, const_child_ptr aChild)
      {
	type_const_iterator found = find(cont,aChild);
	return found != cont.end()
	  ? getContPtr(found)
	  : 0;
      }

      // index in container access by iterator
      static bool
      getContIndex(type_const_iterator thisIter,
		   external_index_type& theIndex)
      {
	theIndex = ((*thisIter).first).index();
        const CONT* cont = getContPtr(thisIter);
	return cont ? theIndex < cont->size() : false;
      }

      // index in container access by pointer
      static bool
      getContIndex(const type& cont, const_child_ptr aChild, 
		   external_index_type& theIndex)
      {
        type_const_iterator found = find(cont,aChild);
	if ( found != cont.end() )
	  {
	    return getContIndex(found,theIndex);
	  }
	return false;
      }

      ///////////////////////// Insertion /////////////////////////////////////

      // insertion of ElementLink by object pointer
      static void insert(type& cont, 
			 const CONT* aChildContainer,
			 const_child_ptr aChild, 
			 const RPAR& aPar=RPAR(),
                         size_t sizeHint = 0) 
      {
        if (sizeHint && cont.capacity() < sizeHint)
          cont.reserve(sizeHint);
        cont.push_back(value_type(ElementLink<CONT>(const_cast<child_ptr>(aChild),
                                                    *aChildContainer),
                                  aPar));
      }

      // insertion of ElementLink by object index
      static void insert(type& cont, const CONT* aChildContainer,
			 external_index_type& anIndex,
			 const RPAR& aPar=RPAR(),
                         size_t sizeHint = 0) 
      {
        if (sizeHint && cont.capacity() < sizeHint)
          cont.reserve(sizeHint);
        cont.emplace_back (*aChildContainer,anIndex, aPar);
      }

      // insertion of ElementLink by object index
      static void insert(type& cont, const CONT* aChildContainer,
			 external_index_type& anIndex,
                         IProxyDict* sg,
			 const RPAR& aPar=RPAR(),
                         size_t sizeHint = 0) 
      {
        if (sizeHint && cont.capacity() < sizeHint)
          cont.reserve(sizeHint);
        cont.emplace_back (*aChildContainer,anIndex, sg, aPar);
      }

      // insertion of ElementLink by ElementLink
      static void insert(type& cont, 
			 const ElementLink<CONT>* aLink,
			 const RPAR& aPar=RPAR(),
                         size_t sizeHint = 0) 
      {
        if (sizeHint && cont.capacity() < sizeHint)
          cont.reserve(sizeHint);
	cont.push_back(value_type(*aLink, aPar));
      }

      // change parameter
      static void changeParm(type& cont,
			     const_child_ptr pChild,
			     const RPAR& aPar=RPAR())
      {
	type_iterator iObj = find(cont,pChild);
	if ( iObj != cont.end() ) (*iObj).second = aPar;
      }

      // change parameter
      static void changeParm(type& cont,
			     const CONT* pContainer,
			     external_index_type& refIndex,
			     const RPAR& aPar=RPAR())
      {
	changeParm(cont,(pContainer->operator[])(refIndex),aPar);
      }

      // change parameter
      //      static void changeParm(type_iterator& iter,
      //			     const RPAR& aPar=RPAR())
      //      {
      //	(*iter).second = aPar;
      //      }

      //////////////////////////// allocation/find ////////////////////////////

      // find method implementation for const_iterator on objects
      static type_const_iterator 
      find(const type& cont, const_child_ptr aChild)
      {
	type_const_iterator iEntry = cont.begin();
	type_const_iterator lastEntry  = cont.end();
	while (iEntry != lastEntry && aChild != *((*iEntry).first)) ++iEntry;
	return iEntry;
      }
      
      // find method for iterator on objects
      static type_iterator
      find(type& cont, const_child_ptr aChild)
      {
	type_iterator iEntry = cont.begin();
	type_iterator lastEntry  = cont.end();
	while (iEntry != lastEntry && aChild != *((*iEntry).first)) ++iEntry;
	return iEntry;
      }  

      // find method for ElementLinks
      //      static type_const_iterator
      //      find(const type& cont, const ElementLink<CONT>* anElement)
      //      {
      // const_child_ptr theChild = *(*anElement);
      //	return find(cont,theChild);
      //      }

      // find method for ElementLinks
      //      static type_iterator
      //      find(type& cont, const ElementLink<CONT>* anElement)
      //      {
      //	child_ptr theChild = *(*anElement);
      //	return find(cont,theChild);
      //      }

      /////////////////////////// container management ////////////////////////

      // removal by object pointer
      static bool
      remove(type& cont, const_child_ptr aChild)
      {
	type_iterator found = find(cont,aChild);
	if ( found != cont.end() )
	  {
	    // we assume the container owns the ElementLink/parameter pair, 
            // but not the object linked by ElementLink --- FIXME ??
	    // delete (*found).first;
	    cont.erase(found);
	    return true;
	  }
	else
	  {
	    return false;
	  }
      }

      // removal by ElementLink
      static bool
      remove(type& cont, const ElementLink<CONT>* theElement)
      {
	type_iterator found = find(cont,theElement);
	if ( found != cont.end() )
	  {
	    //	    delete (*found).first;
	    cont.erase(found);
	    return true;
	  }
	else
	  {
	    return false;
	  }
      }

      // replace container in ElementLink
      static bool
      replace(type& cont, const CONT& theNewContainer) {
          type_iterator firstEntry = cont.begin();
          type_iterator lastEntry  = cont.end();
          for ( ; firstEntry != lastEntry ; firstEntry++ ) {
	      // Must force replacement with second arg true
              ((*firstEntry).first).setStorableObject(theNewContainer, true);
          }
          return true;
      }

      // test on containment
      static bool
      contains(const type& cont, const_child_ptr aChild)
      {
	return find(cont,aChild) != cont.end();
      }
      
    };

  ///////////////////////////////////////////////////////////////
  // Default Constituent Collections: No Relational Parameters //
  ///////////////////////////////////////////////////////////////

  template <typename CONT>
    struct DefaultChildColl<CONT, DefaultWeight> 
    {
      // default container is a vector of ElementLinks
      //      typedef const ElementLink<CONT>*               value_type;
      typedef ElementLink<CONT>                      value_type;
      typedef typename std::vector< value_type >           type;
      typedef typename type::iterator             type_iterator;
      typedef typename type::const_iterator type_const_iterator;
      typedef typename CONT::base_value_type*         child_ptr;
      typedef const typename CONT::base_value_type*  const_child_ptr;
      //      typedef typename CONT::const_reference    const_child_ptr;
      //      typedef typename CONT::reference                child_ptr;
      typedef typename SG::GenerateIndexingPolicy<CONT>::type::index_type
                                            external_index_type;

      // object pointer access
      static const_child_ptr  getChildPtr(type_const_iterator iter) 
      { 
	const_child_ptr ret = 0;
	if ((*iter).isValid()) 
	  ret = *(*iter);
	return ret; 
      }

      // default weight is dummy
      static DefaultWeight getChildPar(type_const_iterator /*iter*/) 
      { return DefaultWeight(); }

      // ElementLink pointer access 
      static const ElementLink<CONT>* getElementPtr(type_const_iterator 
						    thisIter)
      {
	return &(*thisIter);
      }

      // container access by iterator
      static const CONT& getContRef(type_const_iterator thisIter)
      {
	return (*thisIter).getStorableObjectRef();
      }

      // container access by pointer
      static const CONT& getContRef(const type& cont, const_child_ptr aChild)
      {
	return getContRef(find(cont,aChild));
      }

      // pointer to container access by iterator
      static const CONT* getContPtr(type_const_iterator thisIter)
      {
	return (*thisIter).getStorableObjectPointer();
      }

      // pointer to container access by pointer
      static const CONT* getContPtr(const type& cont, const_child_ptr aChild)
      {
	type_const_iterator found = find(cont,aChild);
	return found != cont.end()
	  ? getContPtr(found)
	  : 0;
      }

      // index in container access by iterator
      static bool
      getContIndex(type_const_iterator thisIter,
		   external_index_type& theIndex)
      {
	theIndex = (*thisIter).index();
        const CONT* cont = getContPtr(thisIter);
	return cont ? theIndex < cont->size() : false;
      }

      // index in container access by pointer
      static bool
      getContIndex(const type& cont, const_child_ptr aChild, 
		   external_index_type& theIndex)
      {
	type_const_iterator found = find(cont,aChild);
	if ( found != cont.end() )
	  {
	    return getContIndex(found,theIndex);
	  }
	return false;
      }

      // insertion of ElementLink by object pointer 
      static void insert(type& cont, const CONT* aChildContainer,
			 const_child_ptr aChild, 
			 const DefaultWeight&,
                         size_t sizeHint = 0) 
      {
        if (sizeHint && cont.capacity() < sizeHint)
          cont.reserve(sizeHint);
        // cast is an ugly fix!
        cont.push_back(ElementLink<CONT>(const_cast<child_ptr>(aChild),
                                         *aChildContainer));
      }

      // insertion of ElementLink by object index
      static void insert(type& cont, const CONT* aChildContainer,
			 const external_index_type& anIndex,
			 const DefaultWeight&,
                         size_t sizeHint = 0) 
      {
        if (sizeHint && cont.capacity() < sizeHint)
          cont.reserve(sizeHint);
        cont.push_back(ElementLink<CONT>(*aChildContainer,anIndex));
      }

      // direct insertion of ElementLink
      static void insert(type& cont, const ElementLink<CONT>* aLink,
			 const DefaultWeight&,
                         size_t sizeHint = 0)
      {
        if (sizeHint && cont.capacity() < sizeHint)
          cont.reserve(sizeHint);
	cont.push_back(*aLink);
      }
    
      // change parameter
      static void changeParm(type& /* cont */,
			     const_child_ptr /* pChild */,
			     const DefaultWeight& /*refParm*/)
      { }

      // change parameter
      static void changeParm(type& /* cont */,
			     const CONT* /* pContainer */, 
			     external_index_type& /* refIndex */,
			     const DefaultWeight& /* refParm */)
      { }
 
      // change parameter
      static void changeParm(type_iterator& /* iter */,
			     const DefaultWeight& /* refParm */)
      { }

      // allocation of object pointer in list
      static type_const_iterator find(const type& cont, 
				      const_child_ptr aChild) 
      {
	type_const_iterator firstEntry = cont.begin();
	type_const_iterator lastEntry  = cont.end();
	for ( ; firstEntry != lastEntry; firstEntry++ )
	  {
	    if ( *(*firstEntry) == aChild ) break;
	  }
	return firstEntry;
      }

      // same but return iterator 
      static type_iterator find(type& cont, const_child_ptr aChild) 
      {
	type_iterator firstEntry = cont.begin();
	type_iterator lastEntry  = cont.end();
	for ( ; firstEntry != lastEntry; firstEntry++ )
	  {
	    if ( *(*firstEntry) == aChild ) break;
	  }
	return firstEntry;
      }

      // find ElementLink directly
      //      static type_const_iterator find(const type& cont, 
      //				      const ElementLink<CONT>* anElement)
      //      {
      //	return std::find(anElement,cont.begin(),cont.end());
      //      }

      // find ElementLink directly
      //      static type_iterator find(type& cont,
      //				const ElementLink<CONT>* anElement)
      //      {
      //	return std::find(anElement,cont.begin(),cont.end());
      //      }

      // removal 
      static bool remove(type& cont, const_child_ptr aChild)
      {
	type_iterator found = find(cont,aChild);
	if ( found != cont.end() )
	  {
	    // ownership of ElementLink!! FIXME
	    //	    delete *found;
	    cont.erase(found);
	    return true;
	  }
	else
	  {
	    return false;
	  }
      }

      // removal of ElementLink
      static bool remove(type& cont, const ElementLink<CONT>* anElement)
      {
	type_iterator found = find(cont,*anElement);
	if ( found != cont.end() )
	  {
	    //	    delete *found;
	    cont.erase(anElement);
	    return true;
	  }
	else
	  {
	    return false;
	  }
      }

      // replace container in ElementLink
      static bool
      replace(type& cont, const CONT& theNewContainer) {
          type_iterator firstEntry = cont.begin();
          type_iterator lastEntry  = cont.end();
          for ( ; firstEntry != lastEntry ; firstEntry++ ) {
	      // Must force replacement with second arg true
              (*firstEntry).setStorableObject(theNewContainer, true);
          }
          return true;
      }

      // test on containment
      static bool
      contains(const type& cont, const_child_ptr aChild)
      {
	return find(cont,aChild) != cont.end();
      }
    };

  ////////////////////////////////////////////////////////////
  // Default Constituent Collections: Terminal Node (Dummy) //
  ////////////////////////////////////////////////////////////

  template<>
    struct DefaultChildColl< DefaultContainer, DefaultWeight >
    {
      public:
        typedef DefaultType type;
    };

} // end of namespace
#endif


  
