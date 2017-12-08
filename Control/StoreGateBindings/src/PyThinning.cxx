///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Python.h"

#include "StoreGateBindingsDict.h"

// Framework includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/DataObject.h"
#include "AthenaKernel/IThinningHdlr.h"
#include "AthenaKernel/ISlimmingHdlr.h"
#include "AthenaKernel/IThinningSvc.h"

// SGTools includes
#include "SGTools/DataBucketBase.h"
#include "SGTools/BaseInfo.h"

// ROOT includes
#include "TClass.h"
#include "TClassRef.h"
#include "TROOT.h"

// PyROOT includes
#include "AthenaPyRoot.h"

#include <string>
#include <vector>
#include <algorithm> // for stable_partition

typedef std::vector<char*> bytes_t;

static PyObject* dobj_klass = NULL;

namespace SG {

  /**
   * @brief Concrete @c IThinningHdlr to handle the thinning of a 
   *        @c DataVectorobject via its python binding and use the Root 
   *        dictionary to do conversions.
   */
  struct PyDvThinningHdlr : ::Athena::IThinningHdlr
  {
  
    /** @brief Constructor to handle thinning of a python object
     */
    PyDvThinningHdlr( bytes_t& container );

    /** @brief Destructor
     */
    virtual ~PyDvThinningHdlr();

    /** @brief remove an element from the proxied @c DataVector
     */
    virtual void remove( const std::size_t idx );
    /** @brief pack the proxied @c DataVector
     *         This is needed in order to keep element link indices consistent
     *         and T/P converters unchanged.
     */
    virtual void commit();
  
    /** @brief unpack the proxied @c DataVector ie: restore it as it was 
     *         *before* any thinning took place
     */
    virtual void rollback();
  private:

    /** Vector holding the pointers to the elements of @c DataVector, before
     *  any thinning took place
     */
    const bytes_t m_backup;

    /** Reference to the actual underlying std::vector backing the 
     *  @c DataVector being thinned
     */
    bytes_t& m_container;
  };


  /**
   * @brief Concrete @c IThinningHdlr to handle the thinning of an IDC via 
   *        its python binding and use the Root dictionary to do conversions.
   */
  struct PyIdcThinningHdlr : ::Athena::IThinningHdlr
  {
    typedef PyObject*                         PyIdc_t;
    typedef std::pair<std::size_t, PyObject*> Backup_t;
    typedef std::list<Backup_t>               Backups_t;

    /** @brief Constructor to handle thinning of a python object
     */
    PyIdcThinningHdlr( PyIdc_t& idc );

    /** @brief Destructor
     */
    virtual ~PyIdcThinningHdlr();

    /** @brief remove an element from the proxied @c DataVector
     */
    virtual void remove( const std::size_t idx );

    /** @brief pack the proxied @c IdentifiableContainer
     *         This is needed in order to keep element link indices consistent
     *         and T/P converters unchanged.
     */
    virtual void commit();
  
    /** @brief unpack the proxied @c IdentifiableContainer
     *         ie: restore it as it was *before* any thinning took place
     */
    virtual void rollback();
  private:

    /** Vector holding the pointers to the elements of @c DataVector, before
     *  any thinning took place
     */
    Backups_t m_backup;

    /** Reference to the actual @c IdentifiableContainer
     */
    PyIdc_t& m_container;
  };

} //> end namespace SG

PyObject* 
AthenaInternal::thinContainer( IThinningSvc* self,
			       PyObject* container,
			       PyObject* filter_,
			       int op )
{
  // unlikely to happen, but checking is cheap
   if ( ! self ) {
      PyErr_SetString( PyExc_RuntimeError, 
		       "no thinning svc available, is Athena initialized?" );
      return 0;
   }

   if ( ! TPython::ObjectProxy_Check( container ) ) {
     PyErr_SetString( PyExc_TypeError,
		      "filter() argument 2 must be a PyROOT object" );
     return 0;
   }
   const int nElements = PySequence_Size( container );
   if ( -1 == nElements ) {
     PyErr_SetString
       ( PyExc_TypeError,
	 "filter() argument 2 must be a non zero-length sequence" );
     return 0;
   }

   typedef IThinningSvc::Filter_t Filter_t;
   Filter_t filter;
   if ( PySequence_Check( filter_ ) ) {
     for ( std::size_t i = 0, iMax = nElements; i != iMax; ++i ) {
       PyObject* iFilter = PySequence_GetItem( filter_, i );
       if ( PyBool_Check (iFilter) ) {
	 if ( iFilter == Py_True ) { filter[i] = true;
	 } else                    { filter[i] = false;
	 }
       } else if ( PyInt_CheckExact (iFilter) ) {
	 if ( PyInt_AS_LONG (iFilter) ) { filter[i] = true;
	 } else                         { filter[i] = false;
	 }
       } else if ( PyLong_CheckExact (iFilter) ) {
	 if ( PyLong_AsLong (iFilter) ) { filter[i] = true;
	 } else                         { filter[i] = false;
	 }
       } else {
	 PyErr_Format
	   ( PyExc_TypeError,
	     "filter() item nbr %i in filter-mask is not a bool nor an " \
	     "integer nor a long",
	     static_cast<int>(i) );
	 Py_DECREF (iFilter);
	 return 0;
       }
       Py_DECREF(iFilter);
     }
   } else if ( PyDict_Check (filter_) ) {
     PyObject *dict_key, *dict_value;
     Py_ssize_t itr_pos = 0;

     while (PyDict_Next(filter_, &itr_pos, &dict_key, &dict_value)) {
       // XXX FIXME: handle case where key is a C++ IdentifierHash ?
       std::size_t i = PyInt_AS_LONG(dict_key);
       if ( PyBool_Check (dict_value) ) {
	 if ( dict_value == Py_True ) { filter[i] = true;
	 } else                       { filter[i] = false;
	 }
       } else if ( PyInt_CheckExact (dict_value) ) {
	 if ( PyInt_AS_LONG (dict_value) ) { filter[i] = true;
	 } else                            { filter[i] = false;
	 }
       } else if ( PyLong_CheckExact (dict_value) ) {
	 if ( PyLong_AsLong (dict_value) ) { filter[i] = true;
	 } else                            { filter[i] = false;
	 }
       } else {
	 PyErr_Format
	   ( PyExc_TypeError,
	     "filter() item nbr %i in filter-mask is not a bool nor an " \
	     "integer nor a long",
	     static_cast<int>(i) );
	 return 0;
       }
     }
   } else {
     PyErr_SetString( PyExc_TypeError,
		      "filter() argument 3 must a sequence or a dict" );
     return 0;
   }

   // FIXME: won't work b/c of damn specialization of std::vector<bool> !!
   // -> change IThinningSvc::filter interface or,
   // -> copy the python array of booleans into a temporary vector<bool>
//    Filter_t& filter = 
//      *(Filter_t*)ObjectProxy_ASVOIDPTR(filter);

   SG::DataProxy* proxy = self->proxy( ObjectProxy_ASVOIDPTR(container) );
   if ( !proxy ) {
       PyErr_SetString( PyExc_RuntimeError,
			"could not manage to find a proxy" );
     return 0;
   }

   Athena::IThinningHdlr* hdlr = self->handler(proxy);
   if ( 0 == hdlr ) {
     // decide which handler-type to use
     //  -> rely on IDC inheriting from DataObject to do so to match the way
     //     the C++ side does it.
     // FIXME: should we try something else ?
     if (!dobj_klass) {
       PyObject *dobj = TPython::ObjectProxy_FromVoidPtr ((void*)NULL,
							  "DataObject");
       if (!dobj) {
	 PyROOT::throw_py_exception();
       }
       dobj_klass = PyObject_Type (dobj);
       Py_DECREF (dobj);
       if (!dobj_klass) {
	 PyROOT::throw_py_exception();
       }
     }
     // is 'container' a DataObject ?
     if (PyObject_IsInstance (container, dobj_klass)) {
       hdlr = new SG::PyIdcThinningHdlr (container);
     } else {
       PyObject* pystdcont = PyObject_CallMethod
	 ( container, 
	   const_cast<char*>("stdcont"), const_cast<char*>("") );
       if ( ! TPython::ObjectProxy_Check( pystdcont ) ) {
	 PyErr_SetString( PyExc_RuntimeError,
			  "could not access stdcont() method from DataVector" );
	 return 0;
       } else {
	 bytes_t& stdcont = *reinterpret_cast<bytes_t*> 
	   (ObjectProxy_ASVOIDPTR(pystdcont));
	 hdlr = new SG::PyDvThinningHdlr (stdcont);
       }
     }
   }

   int sc = self->filter_impl( hdlr, proxy, filter, 
			       (IThinningSvc::Operator::Type)op ).isSuccess()
	      ? 1
	      : 0; 
   return Py_BuildValue( const_cast<char*>("i"), sc );
}

std::size_t 
AthenaInternal::thinIdxContainer( IThinningSvc* self,
				  PyObject* container, 
				  std::size_t idx )
{
  // unlikely to happen, but checking is cheap
   if ( ! self ) {
      PyErr_SetString( PyExc_RuntimeError, 
		       "no thinning svc available, is Athena initialized?" );
      return 0;
   }

   if ( ! TPython::ObjectProxy_Check( container ) ) {
     PyErr_SetString( PyExc_TypeError,
		      "index() argument 2 must be a PyROOT object" );
     return 0;
   }

   SG::DataProxy* proxy = self->proxy( ObjectProxy_ASVOIDPTR(container) );
   if ( 0 == proxy ) {
     PyErr_SetString( PyExc_RuntimeError,
		      "could not manage to find a proxy" );
     return 0;
   }
   return self->index( proxy, idx );
}

std::size_t AthenaInternal::thinRemovedIdx() 
{ return IThinningSvc::RemovedIdx; }

//
namespace SG {

  PyDvThinningHdlr::PyDvThinningHdlr( bytes_t& container ) :
    ::Athena::IThinningHdlr( ),
    m_backup     ( container.begin(), container.end() ),
    m_container  ( container )
  {}

  PyDvThinningHdlr::~PyDvThinningHdlr()
  {}

  /** @brief remove an element from the proxied @c DataVector
   */
  void PyDvThinningHdlr::remove( const std::size_t idx )
  {
    m_container[idx] = 0;
  }
  
  /** @brief pack the proxied @c DataVector
   *         This is needed in order to keep element link indices consistent
   *         and T/P converters unchanged.
   */
  void PyDvThinningHdlr::commit()
  {
    typedef bytes_t::iterator  Iter;
    // move non NULL pointers at the begin of the vector, 
    // preserving relative order...
    Iter itr = std::stable_partition( m_container.begin(), 
				      m_container.end(), 
				      Athena::detail::IsNonNullPtr() );
    // nicely truncate our container: removes the NULL elements
    m_container.resize( std::distance( m_container.begin(), itr ) );
  }
  
  /** @brief unpack the proxied @c DataVector ie: restore it as it was 
   *         *before* any thinning took place
   */
  void PyDvThinningHdlr::rollback()
  {
    const std::size_t size = m_backup.size();
    m_container.resize( size );
    std::copy (m_backup.begin(), m_backup.end(),
	       m_container.begin());
  }

  // python thinning handler for IDCs

  typedef PyIdcThinningHdlr::PyIdc_t PyIdc_t;
  PyIdcThinningHdlr::PyIdcThinningHdlr( PyIdc_t& container ) :
    ::Athena::IThinningHdlr( ),
    m_backup    ( ),
    m_container ( container )
  {}

  PyIdcThinningHdlr::~PyIdcThinningHdlr()
  {
    for ( Backups_t::iterator 
	    itr = m_backup.begin(),
	    iEnd= m_backup.end();
	  itr != iEnd;
	  ++itr ) {
      PyObject *obj = (PyObject*)itr->second;
      Py_DECREF (obj);
    }
  }

  /** @brief remove an element from the proxied @c IdentifiableContainer
   */
  void PyIdcThinningHdlr::remove( const std::size_t idx )
  {
    PyObject *res = PyObject_CallMethod ((PyObject*)(m_container),
					 (char*)"removeCollection",
					 (char*)"I", idx); // unsigned int
    if (!res) {
      PyROOT::throw_py_exception();
    }
    if (!TPython::ObjectProxy_Check (res) ) {
      PyErr_Format( PyExc_TypeError,
		    "PyIdcThinningHdlr::remove(%lu) did not return a "	\
		    "ROOT.ObjectProxy",
		    (long unsigned int)idx);
      Py_DECREF (res);
      PyROOT::throw_py_exception();
    }

    // implicitly take ownership of 'res'
    m_backup.push_back (std::make_pair(idx, (PyIdc_t)res));
  }
  
  /** @brief pack the proxied @c IdentifiableContainer
   *         This is needed in order to keep element link indices consistent
   *         and T/P converters unchanged.
   */
  void PyIdcThinningHdlr::commit()
  { /*no-op*/ }
  
  /** @brief unpack the proxied @c IdentifiableContainer
   *         ie: restore it as it was *before* any thinning took place
   */
  void PyIdcThinningHdlr::rollback()
  {
    for ( Backups_t::iterator 
	    itr = m_backup.begin(),
	    iEnd= m_backup.end();
	  itr != iEnd;
	  ++itr ) {
      PyObject *obj = (PyObject*)itr->second; //borrowed
      PyObject *owns_coll = Py_True;          //borrowed
      PyObject *pysc = PyObject_CallMethod ((PyObject*)m_container,
					    (char*)"addCollection",
					    (char*)"OIO",
					    obj, itr->first, owns_coll);
      if (!pysc) {
	PyROOT::throw_py_exception();
      }
      if ( !PyInt_Check (pysc) ) {
	Py_DECREF (pysc);
	PyErr_SetString (PyExc_TypeError,
			 (char*)"unexpected returned type");
	PyROOT::throw_py_exception();
      }

      StatusCode sc = PyInt_AS_LONG( pysc );
      Py_DECREF (pysc);
      if ( !sc.isSuccess() ) {
	PyErr_Format
	  (PyExc_RuntimeError,
	   (char*)"could not add back collection with hash [%lu]",
	   (long unsigned int)itr->first);
	PyROOT::throw_py_exception();
      }
    }
  }

} //< end namespace SG
