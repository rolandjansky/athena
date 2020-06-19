/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** @file NestedContainer.h 
 
 @brief 
 'Nested' template container for storing TRT conditions data.

 Entries are accessed via an identifier (TRTCond::ExpandedIdentifier), which
 specifies barrel, layer, sector, strawlayer and straw. At each level
 it stores a 'default' value which is returned in case the specified
 entry does not exist or is 'invalid' (not-calibrated). The identifier
 can also specify a 'level' which allows to access the constants at a
 particular granularity. For example, to access the t0 of a straw
 'bec=1, layer=2, sector=3, strawlayer=4, straw=5', one uses

   StrawT0Container * strawt0container = ... ;
   ExpandedIdentifier id(3,2,3,4,5,ExpandedIdentifier::STRAW) ;
   const StrawT0& strawt0 = strawt0container->get( id ) ;

 To retrieve the corresponding entry an module level, one would use

   ExpandedIdentifier id(3,2,3,4,5,ExpandedIdentifier::MODULE) ;

 The NestedContainer is implement via two classes 

 - NestedContainerBase is a base class that stores a vector objects of
   template type 'Daughter' as well as a 'default' object of type T. It
   contains all the functionality of the NestedContainer, for example
   setting and retrieving values, printing the contents of the container
   and 'crunching', reducing to a minimal size by using the fact that
   stores daughter objects are the same. One of the template argments is
   the 'NestingLevel' which tells the NestedContainerBase which index in
   an ExpandedIdentifier to use to pick the Daughter.
 
 - NestedContainer is the derived class. It specifies the Daughter and
   NestingLevel of the base class. By default the Daughter is a
   NestedContainer with level 'NestingLevel+1', which creates a template
   loop. The loop is truncated with a template specialization at the
   'STRAWLAYER' level: The NestedContainer at strawlayer level stores
   objects of type T rather than another nested container.
 
 - The NestedContainer is created with a template specialization like
   class StrawT0Container : public NestedContainer<ExpandedIdentifier::DETECTOR, StrawT0,StrawT0Trait>
   In this example 'StrawT0' is the object stored and 'StrawT0Trait' is
   a trait class that provides functionality that the
   NestedContainerBase needs to deal with a class of type StrawT0, like
   how to initialize, delete and copy it.


 @author Wouter Hulsbergen <wouter.hulsbergen@cern.ch>

*/

// $Id: NestedContainer.h,v 1.6 2009-02-11 13:46:55 dwhittin Exp $


#ifndef TRTConditionsData_NestedContainer_h
#define TRTConditionsData_NestedContainer_h

#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"


namespace TRTCond
{
  
  /** @class NestedContainerDaugtherAccessor
      helper class to access daughters of a nested container
  */
  template <class Daughter, class T, class Trait>
  struct NestedContainerDaughterAccessor
  {
    /** get value for given id */
    static const T& get(const Daughter& d, const ExpandedIdentifier& id) { return d.get(id) ; }
    /** resolve the nesting level for a given id */
    static const T& get(const Daughter& d, const ExpandedIdentifier& id , size_t& resolvelevel) { return d.get(id,resolvelevel) ; }
    /** get default for daughter container */
    static const T& get(const Daughter& d) { return d.get() ; }
    /** set value for given id */
    static void set(Daughter& d, const ExpandedIdentifier& id, const T& t) { d.set(id,t) ; }
    /** empty daughter container  */
    static bool empty(Daughter& d) { return d.daughters().empty() ; }
    /** clear container for given id */
    static void clear(Daughter& d, const ExpandedIdentifier& id) { d.clear(id) ; } ;
    /** clear container  */
    static void clear(Daughter& d) { d.clear() ; } ;
    /** print daugther info */
    static void print(const Daughter& d) { d.print() ; }
    /** copy all object pointers into a vector of (id,pointer) pairs  */
    static void getall(const Daughter& d, std::vector< std::pair<ExpandedIdentifier, const T*> >& entries) { 
      d.getall(entries) ; }
    /** make a default daugther */
    static Daughter initialvalue() { return Daughter() ; }
    /** copy daugthers */
    static void copy(Daughter& out, const Daughter& in) { out = in ; }
    /** return allocated memory */
    static size_t footprint(const Daughter& d) { return d.footprint() ; }
    /** compress daugther size */
    static void crunch(Daughter& d) { return d.crunch() ; }
    /** equality operator */
    static bool isequal(const Daughter& lhs, const Daughter& rhs) { return lhs==rhs ; }
    /** total number of atomic objects in daugther  */
    static size_t numObjects(const Daughter& d) { return d.numObjects() ; }
  } ;
  
  /** NestedContainerDaugtherAccessor  
   * specialization if the daughter is not a container,
     but an atomic object ('STRAW' level)
  */
  template <class T, class Trait>
  struct NestedContainerDaughterAccessor<T,T,Trait>
  {
    static const T& get(const T& d, const ExpandedIdentifier&) { return d ; }
    static const T& get(const T& d, const ExpandedIdentifier& , size_t& ) { return d ; }
    static const T& get(const T& d) { return d ; }
    static void set(T& d, const ExpandedIdentifier&, const T& t) { Trait::copy(d,t) ; }
    static bool empty(T&) { return true ; }
    static void clear(T& d, const ExpandedIdentifier&) { Trait::erase(d) ; }
    static void clear(T& d) { Trait::erase(d) ; }
    static void print(const T& d) { Trait::print(d) ; }
    static void getall(const T& d, std::vector< std::pair<ExpandedIdentifier, const T*> >& entries) { 
      if(Trait::isvalid(d) ) {
	ExpandedIdentifier id(0,0,0,0,0,ExpandedIdentifier::STRAW) ;
	entries.push_back(std::pair<ExpandedIdentifier, const T*>(id,&d)) ; 
      }
    }
    static T initialvalue() { return Trait::initialvalue() ; }
    static void copy(T& out, const T& in) { Trait::copy(out,in) ; }
    static size_t footprint(const T& d) { return Trait::footprint(d) ; }
    static void crunch(T&) {}
    static bool isequal(const T& lhs, const T& rhs) { return Trait::isequal(lhs,rhs) ; }
    static size_t numObjects(const T& d) { return Trait::isvalid(d) ? 1 : 0 ; }
  } ;
  


  /** @class NestedContainerBase

     Base class for nested containers
  */
  template <int NestingLevel, class Daughter, class T, class Trait>
  class NestedContainerBase
  {
  private:

    typedef std::vector<Daughter> DaughterContainer ;   //!< typedef of container
    //typedef std::vector<Daughter>::const_iterator ConstIterator ;
    DaughterContainer m_daughters ; //!< Container of Daughters 
    T m_default ; //!< Type of atomic element.
    
  public:
    typedef T value_type ;  //!< Type of atomic element.
    typedef const T const_value_type ;  //!< const Type of atomic element.
    typedef Trait trait_type ;  //!< Type of trait.

    /** constructor */
    NestedContainerBase()  : m_daughters(0), m_default(Trait::initialvalue()) {}
    //    NestedContainerBase(const NestedContainerBase& rhs) 
    //       : m_daughters(0), m_default(Trait::initialvalue()) { *this = rhs ; }

   /** destructor    */
    ~NestedContainerBase() = default;
    //{ 
      //cannot delete the daughter here, because of copy constructors
      //std::cout << "in ~NestedContainerBase(): " << std::flush ;
      //Trait::print(m_default) ;
      //Trait::erase(m_default) ; 
      //std::cout << "done." << std::endl ;
    //}

    //     NestedContainerBase& operator=(const NestedContainerBase& rhs) {
    //       std::cout << "operator=" << m_daughters.size() << " " << rhs.m_daughters.size() << std::endl ;
    //       clear() ;
    //       Trait::copy(m_default,rhs.m_default) ;
    //       m_daughters.resize(rhs.m_daughters.size(),NestedContainerDaughterAccessor<Daughter,T,Trait>::initialvalue()) ;
    //       for(unsigned int i=0; i<m_daughters.size(); ++i) 
    // 	NestedContainerDaughterAccessor<Daughter,T,Trait>::copy( m_daughters[i], rhs.m_daughters[i] ) ;
    //       std::cout << "done with operator=" << std::endl ;
    //       return *this ;
    //     }
    
    /** set a value. using 'level' in id we can specify if we want to set the default value. */
    void set( const ExpandedIdentifier& id, const T& t) {
      if( NestingLevel == id.level() ) {
	// set the default value
	Trait::copy(m_default,t) ;
      } else {
	unsigned int thisindex = id.index(NestingLevel+1) ;
	// expand daughter array if necessary
	if(thisindex>=m_daughters.size()) 
	  m_daughters.resize( thisindex+1, NestedContainerDaughterAccessor<Daughter,T,Trait>::initialvalue() ) ; 
	NestedContainerDaughterAccessor<Daughter,T,Trait>::set(m_daughters[thisindex], id, t ) ;
      }
    }
    
    /** get a value. return the default value if no valid entry for id exists. */
    const T& get( const ExpandedIdentifier& id ) const {
      if( NestingLevel < id.level() ) {
	unsigned int thisindex = id.index(NestingLevel+1) ;
	if(thisindex<m_daughters.size()) {
	  const T& rc = NestedContainerDaughterAccessor<Daughter,T,Trait>::get(m_daughters[thisindex],id) ;
	  // only return this value if it is valid. otherwise, return the default.
	  if( Trait::isvalid(rc) ) return rc ;
	}
      }
      return m_default ;
    }

   const T& get( const ExpandedIdentifier& id, size_t& resolvelevel) const 
   {
     if( NestingLevel < id.level() ) {
       unsigned int thisindex = id.index(NestingLevel+1) ;
       if(thisindex<m_daughters.size()) {
          const T& rc = NestedContainerDaughterAccessor<Daughter,T,Trait>::get(m_daughters[thisindex],id,resolvelevel) ;
           if( Trait::isvalid(rc) ) return rc ;   
         }
      }
      resolvelevel = NestingLevel ; 
      return m_default ;
    }


    /** get the default value */
    const T& get() const { return m_default ; }

    /// set the default value
    void set(const T& t) { Trait::copy(m_default,t) ; }

    /** clear entire container */
    void clear() {
      for( typename DaughterContainer::iterator it = m_daughters.begin() ; it != m_daughters.end(); ++it ) 
	NestedContainerDaughterAccessor<Daughter,T,Trait>::clear(*it) ;
      m_daughters.clear() ;
      Trait::erase(m_default) ;
    }

    /** clear only entries specified */
    void clear( const ExpandedIdentifier& id )  {
      if( NestingLevel == id.level() ) clear() ;
      else {
	unsigned int thisindex = id.index(NestingLevel+1) ;
	if(thisindex<m_daughters.size()) 
	  NestedContainerDaughterAccessor<Daughter,T,Trait>::clear(m_daughters[thisindex],id ) ;
      }
    }

    /** return the vector of daughters */
    const DaughterContainer& daughters() const { return m_daughters ; }


    /** return the total number of valid calibration objects */
    size_t numObjects() const { 
      size_t rc(0) ;
      assert( std::distance(m_daughters.begin(),m_daughters.end())==(int)m_daughters.size()) ;
      assert( m_daughters.size() < 1000 ) ;
      for( typename DaughterContainer::const_iterator it = m_daughters.begin() ; it != m_daughters.end(); ++it ) 
	rc += NestedContainerDaughterAccessor<Daughter,T,Trait>::numObjects(*it) ;
      if( Trait::isvalid(m_default) ) ++rc ;
      return rc ;
    }

    
    /** equality operator, used for compressing */
    bool operator==(const NestedContainerBase& rhs) const {
      return m_daughters.size()==0 && rhs.m_daughters.size()==0 && Trait::isequal(m_default,rhs.m_default) ;
    }
    
    /** another equality operator, used for compressing */
    bool operator==(const T& rhs) const {
      return m_daughters.size()==0 && Trait::isequal(m_default,rhs) ;
    }
    
    /** return the memory allocated by the container and its daughters */
    size_t footprint() const {
      size_t total = Trait::footprint(m_default) + (m_daughters.capacity()-m_daughters.size())*sizeof(Daughter) ;
      for( typename DaughterContainer::const_iterator it = m_daughters.begin() ; it != m_daughters.end(); ++it ) 
	total += NestedContainerDaughterAccessor<Daughter,T,Trait>::footprint(*it) ;
      return total ;
    }
    
    /** reduce the footprint as much as possible
        by removing the extra space allocated by the vectors
    */
    void crunch() {
      // first the daughters 
      for( typename DaughterContainer::iterator it = m_daughters.begin() ; it != m_daughters.end(); ++it ) 
	NestedContainerDaughterAccessor<Daughter,T,Trait>::crunch(*it) ;
      // now use, if anything is there
      if( m_daughters.size()>0 ) {
	// find the first daughter that is valid
	size_t firstvaliddaughter(0);
	bool allequal(true) ;
	for(size_t idau=0; idau<m_daughters.size() && allequal; ++idau) {
	  allequal = NestedContainerDaughterAccessor<Daughter,T,Trait>::empty(m_daughters[idau]) ;
	  if( allequal ) {
	    bool isvalid = Trait::isvalid( NestedContainerDaughterAccessor<Daughter,T,Trait>::get(m_daughters[idau])) ;
	    if( isvalid ) {
	      if( firstvaliddaughter==0 ) firstvaliddaughter = idau ;
	    } else {
	      allequal = NestedContainerDaughterAccessor<Daughter,T,Trait>::isequal(m_daughters[idau],m_daughters[firstvaliddaughter]) ;
	    }
	  }
	}
	if(allequal) {
	  // set the default value to the value form the first daughter
	  T defaultvalue(Trait::initialvalue()) ;
	  if( firstvaliddaughter < m_daughters.size() ) 
	    Trait::copy(defaultvalue,NestedContainerDaughterAccessor<Daughter,T,Trait>::get(m_daughters[firstvaliddaughter])) ;
	  clear() ; // this clears pointers in the daughters. would rather replace that with destructors.
	  m_daughters = DaughterContainer(0) ;
	  m_default = defaultvalue ;
	} else if(m_daughters.size()<m_daughters.capacity())  {
	  DaughterContainer newdaughters(0) ;
	  newdaughters.reserve( m_daughters.size() ) ;
	  newdaughters.insert( newdaughters.end(), m_daughters.begin(), m_daughters.end() ) ;
	  newdaughters.swap(m_daughters) ;
	}
      }
    }
    
    /** some IO for debugging */
    void printindent() const {
      for(int i=0; i<NestingLevel; ++i) 
	std::cout << '\t' ;
    }
    void print() const {
      printindent() ;
      ExpandedIdentifier id;
      std::cout << "level = " << id.name(NestingLevel) << " (" << NestingLevel << ")"  << std::endl ;
      for(unsigned int i=0; i< m_daughters.size(); ++i) {
	printindent() ;
	std::cout << "daughter " << i << std::endl ;
	NestedContainerDaughterAccessor<Daughter,T,Trait>::print(m_daughters[i]) ;
      }
    }

    /// fill vector with all entries in the container. needed to dump to Peter's flat file format
    typedef std::vector< std::pair<ExpandedIdentifier, const T*> > FlatContainer ;
    void getall( FlatContainer& entries ) const {
      // add the daughters
      for(unsigned int idau = 0; idau<m_daughters.size(); ++idau) {
	FlatContainer dauentries ;
	// first get them
	NestedContainerDaughterAccessor<Daughter,T,Trait>::getall(m_daughters[idau], dauentries) ;
	// now update the identifiers to have the dauhgter index  set correctly
	for(unsigned int j=0; j< dauentries.size(); ++j ) 
	  dauentries[j].first.index(NestingLevel+1) = idau ;
	// and append
	entries.insert(entries.end(),dauentries.begin(),dauentries.end()) ;
      }
      // add the default value
      if( Trait::isvalid(m_default) ) {
	ExpandedIdentifier id(0,0,0,0,0,NestingLevel) ;
	entries.push_back( std::pair<ExpandedIdentifier, const T*>(id,&m_default)) ; 
      }
    }
  } ;


  /** @class NestedContainer
    Nested container class, ie a base class at level l with daugthers at level l+1 */
  template <int NestingLevel, class T, class Trait>
  class NestedContainer 
    : public NestedContainerBase<NestingLevel, NestedContainer<NestingLevel+1,T,Trait>, T, Trait>
  {
  } ;
  

  /** Template specialization for the lowest level ('STRAWLAYER')  to truncate the nesting loop */
  template <class T, class Trait>
  class NestedContainer<ExpandedIdentifier::STRAWLAYER,T,Trait>
    : public NestedContainerBase<ExpandedIdentifier::STRAWLAYER,T,T,Trait>
  {
  } ;


  /** @class NestedContainerPointerTrait
    Default trait class if T is an owned pointer */
  template <class T>
  class NestedContainerPointerTrait
  {
  public:
    static void erase( T& x) { delete x ; x = 0 ; }
    static void copy( T& out, const T& in) { if(out) erase(out) ; if(in) out = in->clone() ; }
    static bool isvalid( const T& x ) { return x!=0 ; }
    static void initialize( T& x) { x = 0 ; }
    static size_t footprint( const T& x) { return sizeof(T) + (x ? x->footprint() : 0) ; }
    static void print(const T& x) { std::cout << x << std::endl ; }
    static T initialvalue() { return 0 ; }
    static bool isequal(const T& lhs, const T& rhs) { return lhs==rhs || (lhs && rhs && *lhs==*rhs) ; }
  } ;
}


#endif
