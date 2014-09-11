///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyStoreUtils.h 
// Header file for a few C++ meta utils
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONEVENT_PYSTOREUTILS_H 
#define PERFMONEVENT_PYSTOREUTILS_H 

// STL includes
#include <string>
#include <vector>
#include <list>
#include <utility> // for std::pair

namespace Py {

template<typename T>
struct cpp_typeinfo {};

template<typename T, typename U>
struct cpp_typeinfo< std::pair<T,U> >
{
  static const char name = '(';
};

template<typename T>
struct cpp_typeinfo< std::vector<T> >
{
  static const char name = '[';
};

template<typename T>
struct cpp_typeinfo< std::list<T> >
{
  static const char name = '[';
};

template<>
struct cpp_typeinfo<std::string> 
{
  static const char name = 's';
};

template<>
struct cpp_typeinfo<bool> 
{
  static const char name = 'b';
};

template<>
struct cpp_typeinfo<double> 
{
  static const char name = 'd';
};

template<>
struct cpp_typeinfo<float> 
{
  static const char name = 'f';
};

template<>
struct cpp_typeinfo<int> 
{
  static const char name = 'i';
};

template<>
struct cpp_typeinfo<short int> 
{
  static const char name = 'h';
};

template<>
struct cpp_typeinfo<unsigned int>
{
  static const char name = 'I';
};

template<>
struct cpp_typeinfo<long int>
{
  static const char name = 'l';
};

template<>
struct cpp_typeinfo<unsigned long>
{
  static const char name = 'k';
};

} // end namespace Py


#endif // PERFMONEVENT_PYSTOREUTILS_H 
