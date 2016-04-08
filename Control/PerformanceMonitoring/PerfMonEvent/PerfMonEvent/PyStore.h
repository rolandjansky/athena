///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Py::Store.h 
// Header file for class Py::Store
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONEVENT_PY_STORE_H 
#define PERFMONEVENT_PY_STORE_H 

// Python includes
// Work around a compilation warning.
#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE
# undef _XOPEN_SOURCE
#endif
#include "Python.h"

// STL includes
#include <string>
#include <vector>

// PerfMonEvent includes
#include "PerfMonEvent/PyStoreUtils.h"
#include "PerfMonEvent/PyStoreCnvUtils.h"

// Forward declaration

namespace Py {

class Store
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  Store();

  /// Constructor with parameters: size of buffers
  Store( const std::size_t bufSize );

  /// Destructor: 
  ~Store(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// retrieve the underlying python dictionary
  const PyObject* pydict() const { Py_INCREF(m_store); return m_store; }

  /// retrieve the current buffer size
  std::size_t bufferSize() const { return m_bufSize; }

  /// python representation
  std::string repr() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /// reset the buffer size.
  /// A no-op if new buffer size is same as old or smaller than old
  void setBufferSize( std::size_t bufferSize );

  /// retrieve the underlying python dictionary
  //PyObject* pydict() { Py_INCREF(m_store); return m_store; }

  /// register a variable with the store
  template< typename T >
  void book( const std::string& key,
	     const std::string& dataName )
  {
    //PyObject* buf = Py::cnv<T>::toPy( T() );
    //PyObject* buf = PyList_New(0);
    //PyObject* buf = Py::cnv<T>::to( T() );
    this->book_impl( key, dataName, 'd' );
    //Py_XDECREF(buf);
  }
  
  /// fill some already registered node with data
  template< typename T >
  void fill( const std::string& key,
	     const std::string& dataName,
	     const T& data )
  {
//     std::cerr << "PyStore::fill( " << key << ", " << dataName
// 	      << ", " << typeid(data).name() << " => "
// 	      << ClassName<T>::name() << "\n";
    PyObject* pyData = Py::cnv<T>::toPy( data );
    this->fill_impl( key, dataName, pyData );
    // fill_impl steals pyData reference...
    return;
  }

  /// clear buckets (but keeps registered/booked keys around)
  void clear_store();

  /// @{ python API
  void py_book( const std::string& key,
		const std::string& dataName,
		char dataType = 'd' )
  { return this->book_impl( key, dataName, dataType ); }
  
  void py_fill( const std::string& key,
		const std::string& dataName,
		PyObject* data )
  { return this->fill_impl( key, dataName, data ); }
  ///

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// register a variable with the store
  void book_impl( const std::string& key,
		  const std::string& dataName,
		  char dataType = 'd' );

  /// fill some already registered node with data
  void fill_impl( const std::string& key,
		  const std::string& dataName,
		  PyObject* data );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// the buffer size of all buckets 
  /// (ie: the pre-reserved length of all PyArrays)
  std::size_t m_bufSize;

  /// data store: a python dictionary
  PyObject* m_store;
  //arrayobject* m_aobj;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} // end namespace Py

#endif //> PERFMONEVENT_PY_DATASTORE_H
