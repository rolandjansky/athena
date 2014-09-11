///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyStoreCnvUtils.h 
// Header file for a few C++->Py meta cnv utils
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONEVENT_PYSTORECNVUTILS_H 
#define PERFMONEVENT_PYSTORECNVUTILS_H 

// Python includes
// Work around a compilation warning.
#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#include "Python.h"

// STL includes
#include <string>
#include <vector>
#include <list>
#include <utility> // for std::pair

// #include <iostream>

namespace Py {

/// helper method to convert a type to a PyObject
template<typename CxxType>
struct cnv 
{
//   static PyObject* toPy( CxxType /*t*/ ) 
//   { 
//     std::cerr << "ERR: no matching converter found for [" 
// 	      << typeid(CxxType).name()
// 	      << "]\n";
//     return NULL; 
//   }
};

template<typename T, typename U>
struct cnv< std::pair<T,U> >
{
  static PyObject* toPy( const std::pair<T,U>& t )
  {
    PyObject* o = PyTuple_New( 2 );
    if ( !o ) {
      Py_XDECREF( o );
      Py_INCREF( Py_None );
      return Py_None;
    }
    PyTuple_SET_ITEM( o, 0, Py::cnv<T>::toPy( t.first  ) );
    PyTuple_SET_ITEM( o, 1, Py::cnv<T>::toPy( t.second ) );
    return o;
  }
    
};

template<typename T>
struct cnv< std::vector<T> >
{
  static PyObject* toPy( const std::vector<T>& t )
  {
    const std::size_t iMax = t.size();
    PyObject* o = PyList_New( iMax );
    if ( !o ) {
      Py_XDECREF( o );
      Py_INCREF( Py_None );
      return Py_None;
    }
    for ( std::size_t i = 0; i < iMax; ++i ){
      PyList_SET_ITEM( o, i, Py::cnv<T>::toPy( t[i] ) );
    }
    return o;
  }
    
};

template<typename T>
struct cnv< std::list<T> >
{
  static PyObject* toPy( const std::list<T>& t )
  {
    const std::size_t iMax = t.size();
    PyObject* o = PyList_New( iMax );
    if ( !o ) {
      Py_XDECREF( o );
      Py_INCREF( Py_None );
      return Py_None;
    }
    typename std::list<T>::const_iterator itr = t.begin();
    for ( std::size_t i = 0; i < iMax; ++i ){
      PyList_SET_ITEM( o, i, Py::cnv<T>::toPy( *(itr++) ) );
    }
    return o;
  }
    
};

template<>
struct cnv< std::string >
{
  static PyObject* toPy( const std::string& t )
  {
    return Py_BuildValue( const_cast<char*>("s#"), 
			  const_cast<char*>(t.c_str()), 
			  t.size() );
  }
};

template<>
struct cnv<bool>
{
  static PyObject* toPy( bool t )
  { return Py_BuildValue( const_cast<char*>("b"), t ); }
};

template<>
struct cnv<double>
{
  static PyObject* toPy( double t )
  { return PyFloat_FromDouble(t); }
};

template<>
struct cnv<float>
{
  static PyObject* toPy( float t )
  { return PyFloat_FromDouble(t); }
};

template<>
struct cnv<int>
{
  static PyObject* toPy( int t )
  { return Py_BuildValue( const_cast<char*>("i"), t ); }
};

template<>
struct cnv<short int>
{
  static PyObject* toPy( short int t )
  { return Py_BuildValue( const_cast<char*>("h"), t ); }
};

template<>
struct cnv<unsigned int>
{
  static PyObject* toPy( unsigned int t )
  { return Py_BuildValue( const_cast<char*>("l"), t ); }
};

template<>
struct cnv<long int>
{
  static PyObject* toPy( long int t )
  { return Py_BuildValue( const_cast<char*>("l"), t ); }
};

template<>
struct cnv<unsigned long>
{
  static PyObject* toPy( unsigned long t )
  { return Py_BuildValue( const_cast<char*>("k"), t ); }
};

} // end namespace Py


#endif // PERFMONEVENT_PYSTORECNVUTILS_H 
