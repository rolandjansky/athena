///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyStore.cxx 
// Implementation file for class Py::Store
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// PerfMonEvent includes
#include "PerfMonEvent/PyStore.h"

// STL includes
#include <iostream>

namespace {
  std::size_t defaultBufferSize = 10;
  std::string s_uri = "://";
}

namespace Py {

std::string repr( PyObject* o )
{
  // PyObject_Repr returns a new ref.
  PyObject* py_repr = PyObject_Repr( o );
  if ( !py_repr || !PyString_Check(py_repr) ) {
    Py_XDECREF( py_repr );
    return "";
  }
  
  std::string cpp_repr = PyString_AsString(py_repr);
  Py_DECREF( py_repr );
  return cpp_repr;
}

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

Store::Store() :
  m_bufSize( defaultBufferSize ),
  m_store  ( PyDict_New() )
{}

Store::Store( const std::size_t bufSize ) :
  m_bufSize( bufSize      ),
  m_store  ( PyDict_New() )
{}

// Destructor
///////////////
Store::~Store()
{
  Py_DECREF( m_store );
}

/////////////////////////////////////////////////////////////////// 
// const methods: 
/////////////////////////////////////////////////////////////////// 

std::string Store::repr() const
{
  return Py::repr( m_store );
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void Store::setBufferSize( std::size_t bufferSize )
{
  if ( bufferSize <= m_bufSize ) {
    m_bufSize = bufferSize;
    return;
  }

  m_bufSize = bufferSize;
  // loop over buffers and extend their size

  return;
}

void Store::clear_store()
{
  PyObject *buffers = PyDict_Values( m_store );
  if ( !buffers ) {
    // error handling
    Py_XDECREF( buffers );
    return;
  }
  
  for ( int i = 0, iMax = PyList_GET_SIZE( buffers ); i < iMax; ++i ) {
    PyObject* buf = PyList_GET_ITEM( buffers, i );
    Py_INCREF( buf );
   
    const int len = PyList_GET_SIZE( buf );

    // delete the whole slice
    if ( PyList_SetSlice( buf, 0, len, (PyObject *) NULL ) != 0 ) {
      // error handling

      Py_DECREF( buf );
      continue;
    }

    Py_DECREF( buf );
  }

  Py_DECREF( buffers );
  return;
}

/////////////////////////////////////////////////////////////////// 
// Private methods: 
/////////////////////////////////////////////////////////////////// 

void Store::book_impl( const std::string& key,
		       const std::string& dataName,
		       char /*dataType*/ )
{
  PyObject* buf = PyList_New( m_bufSize );
  if ( !buf ) {
    // error handling
    std::cerr << "Store::book : Could not allocate a buffer for ["
	      << key << s_uri << dataName << "] !!\n";
    Py_XDECREF(buf);
    return;
  }

  // resize to 0: >>> del buf[:]
  if ( PyList_SetSlice( buf, 0, m_bufSize, (PyObject*)NULL ) != 0 ) {
    // error handling
    std::cerr << "Store::book : Could not reset allocated buffer ["
	      << key << s_uri << dataName << "] !!\n"
	      << " buf: " << Py::repr( buf ) << "\n";
    
    Py_DECREF(buf);
    return;
  }

  if ( PyDict_SetItemString( m_store, 
			     const_cast<char*>((key+s_uri+dataName).c_str()),
			     buf ) != 0 ) {
    // error handling
    std::cerr << "Store::book : Could not create buffer at ["
	      << key << s_uri << dataName << "]\n"
	      << " buf: " << Py::repr( buf ) << "\n";
    Py_DECREF(buf);
    return;
  }
}

void Store::fill_impl( const std::string& key,
		       const std::string& dataName,
		       PyObject* data )
{
  PyObject* buf = 
    PyDict_GetItemString( m_store,
			  const_cast<char*>((key+s_uri+dataName).c_str()) );
  Py_XINCREF( buf );
  if ( ! buf ) {
    std::cerr << "Store::fill : Could not retrieve key ["
	      << key << s_uri << dataName << "] from store !!\n";

    // error handling
    PyErr_Clear();
    Py_DECREF ( data );
    Py_XDECREF( buf );
    return;
  }

  if ( !PyList_Check( buf ) ) {
    std::cerr << "Store::fill : buffer at ["
	      << key << s_uri << dataName << "] is NOT a python array !!\n"
	      << " buf: " << Py::repr( buf ) << "\n";

    // error handling
    PyErr_Clear();
    Py_DECREF( data );
    Py_DECREF( buf );
    return;
  }

  if ( 0 != PyList_Append( buf, data ) ) {

    std::cerr << "Store::fill : could not fill buffer at ["
	      << key << s_uri << dataName << "] !!\n"
	      << " buf:  " << Py::repr( buf )  << "\n"
	      << " data: " << Py::repr( data ) << "\n";

    // error handling
    PyErr_Clear();
    Py_DECREF( data );
    Py_DECREF( buf );
    return;
  }

  Py_DECREF( data );
  Py_DECREF( buf );
  return;
}

} //> end namespace Py

// instantiates a few methods...
void instantiates()
{
  Py::Store s( 10 );
  s.book<bool>( "f", "b"       ); 
  s.fill<bool>( "f", "b", true );

  s.book<double>( "f", "d"      ); 
  s.fill<double>( "f", "d", 1.2 );

  s.book<float>( "f", "f"      ); 
  s.fill<float>( "f", "f", 1.2 );

  s.book<int>( "f", "i"    ); 
  s.fill<int>( "f", "i", 1 );
  
  s.book<short int>( "f", "h"    ); 
  s.fill<short int>( "f", "h", 1 );

  s.book<unsigned int>( "f", "I"     ); 
  s.fill<unsigned int>( "f", "I", 10 );

  s.book<long int>( "f", "l"      ); 
  s.fill<long int>( "f", "l", 100 );

  s.book<unsigned long int>( "f", "k"       );
  s.fill<unsigned long int>( "f", "k", 1000 );

  {
    std::vector<double> data(10);
    s.book<std::vector<double> >( "f", "vd"       );
    s.fill<std::vector<double> >( "f", "vd", data );
  }

  {
    std::list<double> data(10);
    s.book<std::list<double> >( "f", "ld"       );
    s.fill<std::list<double> >( "f", "ld", data );
  }

  {
    std::pair<double,float> data; data.first = 10.; data.second = 20.;
    s.book<std::pair<double,float> >( "f", "p"       );
    s.fill<std::pair<double,float> >( "f", "p", data );
  }


}
