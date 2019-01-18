///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_ITHINNINGHDLR_H 
#define ATHENAKERNEL_ITHINNINGHDLR_H 1

// STL includes
#include <algorithm>
#include <vector>
#include <list>
#include <utility> // for std::pair

#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include "GaudiKernel/DataObject.h"

#include "CxxUtils/checker_macros.h"

// FrameWork includes

// Forward declaration
// class IdentifierHash;
// template <class T, class BASE> class DataVector;
// template <class T> class IdentifiableContainer;

namespace Athena {

/**
 * @class Athena::IThinningHdlr
 * @brief This class defines a protocol to pack and unpack thinned collections
 */
class IThinningHdlr
{
public:
  /** @brief virtual destructor
   */
  virtual ~IThinningHdlr();

  /** @brief remove an element from the proxied @c DataVector
   */
  virtual void remove( const std::size_t idx ) = 0;
  /** @brief pack the proxied collection
   *         This is needed in order to keep element link indices consistent
   *         and T/P converters unchanged.
   */
  virtual void commit() = 0;
  
  /** @brief unpack the proxied @c DataVector ie: restore it as it was *before*
   *         any thinning took place
   */
  virtual void rollback() = 0;

  /** @brief publish the type of underlying indexing (mapping or sequencing)
   */
  virtual bool isMapping() const {return false;}
};

namespace detail {
  /** @brief Predicate to spot non NULL pointers
   */
  struct IsNonNullPtr
  {
    template <typename Element>
    bool operator()( Element* f ) const
    { return f != 0; }
  };
}

/**
 * @class DvThinningHdlr
 * @brief Handle @c DataProxy holding @c DataVector.
 * This class defines a (type-safe) protocol to pack and unpack 
 * thinned @c DataVector.
 *
 * This is not MT-safe!
 */
template <typename Container>
class ATLAS_NOT_THREAD_SAFE DvThinningHdlr : public ::Athena::IThinningHdlr
{
  typedef typename Container::PtrVector PtrVector;
  /** Vector holding the pointers to the elements of @c DataVector, before
   *  any thinning took place
   */
  const PtrVector m_backup;
  PtrVector&      m_container;
  //
public:
  DvThinningHdlr( const Container& c ) : 
    Athena::IThinningHdlr( ),
    m_backup   ( const_cast<Container&>(c).begin(),
		 const_cast<Container&>(c).end() ),
    m_container( const_cast<PtrVector&>(const_cast<Container&>(c).stdcont()) )
  {}

  void remove( const std::size_t idx ) 
  { m_container[idx] = 0; }

  void commit() 
  {
    typedef typename PtrVector::iterator  Iter;
    // move non NULL pointers at the begin of the vector, 
    // preserving relative order...
    Iter itr = std::stable_partition( m_container.begin(), 
				      m_container.end(), 
				      Athena::detail::IsNonNullPtr() );
    // nicely truncate our container: removes the NULL elements
    m_container.resize( std::distance( m_container.begin(), itr ) );
  }
  
  void rollback()
  {
    const std::size_t size = m_backup.size();
    m_container.resize( size );
    std::copy (m_backup.begin(), m_backup.end(),
	       m_container.begin());
  }
}; 

/**
 * @class StdThinningHdlr
 * @brief Handle @c DataProxy holding @c std::vector<T>
 * This class defines a (type-safe) protocol to pack and unpack 
 * thinned @c DataVector.
 *
 * This is not MT-safe!
 */
template <typename Container>
class ATLAS_NOT_THREAD_SAFE StdThinningHdlr : public ::Athena::IThinningHdlr
{
  typedef Container Vector_t;
  /** Vector holding the pointers to the elements of @c std::vector<T>, before
   *  any thinning took place
   */
  const Vector_t m_backup;
  Vector_t&      m_container;
  //
public:
  StdThinningHdlr( const Container& c ) : 
    Athena::IThinningHdlr( ),
    m_backup   ( const_cast<Container&>(c).begin(),
		 const_cast<Container&>(c).end() ),
    m_container( const_cast<Container&>(c) )
  {}

  void remove( const std::size_t idx ) 
  { m_container[idx] = 0; }

  void commit() 
  {
    typedef typename Vector_t::iterator  Iter;
    // move non NULL pointers at the begin of the vector, 
    // preserving relative order...
    Iter itr = std::stable_partition( m_container.begin(), 
				      m_container.end(), 
				      Athena::detail::IsNonNullPtr() );
    // nicely truncate our container: removes the NULL elements
    m_container.resize( std::distance( m_container.begin(), itr ) );
  }
  
  void rollback()
  {
    const std::size_t size = m_backup.size();
    m_container.resize( size );
    std::copy (m_backup.begin(), m_backup.end(),
	       m_container.begin());
  }
}; 

/**
 * @class IdcThinningHdlr
 * @brief Handle @c DataProxy holding @c IdentifiableContainer
 * This class defines a (type-safe) protocol to pack and unpack 
 * thinned @c IdentifiableContainer
 *
 * This is not MT-safe!
 */
template <typename Container>
class ATLAS_NOT_THREAD_SAFE IdcThinningHdlr : public ::Athena::IThinningHdlr
{
  typedef Container                               Idc_t;
  typedef typename Idc_t::IDENTIFIABLE            Identifiable_t;
  typedef std::pair<std::size_t, Identifiable_t*> Backup_t;
  typedef std::list<Backup_t>                     Backups_t;

  /** Vector holding the pointers to the elements of @c IdentifiableContainer,
   *  before any thinning took place
   */
  Backups_t m_backup;
  Idc_t    &m_container;
  //
public:
  IdcThinningHdlr( const Container& c ) : 
    Athena::IThinningHdlr( ),
    m_backup   (),
    m_container( const_cast<Idc_t&>(c) )
  {}

  void remove( const std::size_t idx ) 
  { 
    Identifiable_t *c = m_container.removeCollection (idx);
    m_backup.push_back (std::make_pair(idx, c));
  }

  void commit() {/*no-op*/}
  
  void rollback()
  {
    typedef typename Backups_t::iterator Iter;
    Iter end = m_backup.end();
    for ( Iter itr = m_backup.begin(); itr!=end; ++itr) {
      m_container.addCollection (itr->second, itr->first).ignore();
    }
  }

  /** @brief publish the type of underlying indexing (mapping or sequencing)
   */
  virtual bool isMapping() const {return true;}
}; 

/** @brief metafunction to automagically dispatch on the type of a container
 *         and fetch the right thinning handler
 */
template <class Container>
struct get_thinning_handler
{
private:
  typedef typename Container::value_type value_type;
  typedef typename boost::remove_pointer<value_type>::type base_value_type;

  typedef typename boost::is_base_of<std::vector<value_type>, 
				     Container>
  derives_from_std_vector;

//   typedef typename boost::is_base_of<DataVector<base_value_type>,
// 				     Container>
//   derives_from_datavector;
  
  // ASSUME a Container inheriting from DataObject means IdentifiableContainer
  // XXX that's kind of a bold assumption...
  typedef typename boost::is_base_of<DataObject, Container>
  derives_from_dataobject;
  
public:
#define if_c  ::boost::mpl::if_c
  typedef typename 
  if_c<derives_from_dataobject::value,
       ::Athena::IdcThinningHdlr<Container>,
       typename 
       if_c<derives_from_std_vector::value,
	    ::Athena::StdThinningHdlr<Container>,
	    ::Athena::DvThinningHdlr<Container>
	    >::type
  >::type type;
#undef if_c
};

} //> namespace Athena

#endif //> ATHENAKERNEL_ITHINNINGHDLR_H
