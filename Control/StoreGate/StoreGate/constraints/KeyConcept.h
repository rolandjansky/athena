/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONSTRAINTS_KEYCONCEPT_H
#define CONSTRAINTS_KEYCONCEPT_H
#include <string>
#include <boost/concept_check.hpp>

//CONCEPT
template <class T, class ID=std::string > 
struct KeyConcept {
  void constraints() { 
    // check that key can be converted to an identifier
    //needed?    boost::function_requires< boost::ConvertibleConcept<ID, T> >();
    boost::function_requires< boost::ConvertibleConcept<T, ID> >();
    //should also have boost::function_requires< boost::LessThanComparableConcept<T> >();
  }
  T a;
};

template<std::size_t N>
struct KeyConcept<char[N], std::string>  {
  void constraints() { } //no check for char arrays
};

template<>
struct KeyConcept<char*, std::string>  {
  void constraints() { } //no check for char arrays
};

template<>
struct KeyConcept<char, std::string>  {
  void constraints() { } //no check for char arrays
};
template<>
struct KeyConcept<short, std::string>  {
  void constraints() { } //no check for char arrays
};
template<>
struct KeyConcept<int, std::string>  {
  void constraints() { } //no check for char arrays
};
template<>
struct KeyConcept<long, std::string>  {
  void constraints() { } //no check for char arrays
};
template<>
struct KeyConcept<long long, std::string>  {
  void constraints() { } //no check for char arrays
};
template<>
struct KeyConcept<unsigned char, std::string>  {
  void constraints() { } //no check for char arrays
};
template<>
struct KeyConcept<unsigned short, std::string>  {
  void constraints() { } //no check for char arrays
};
template<>
struct KeyConcept<unsigned int, std::string>  {
  void constraints() { } //no check for char arrays
};
template<>
struct KeyConcept<unsigned long, std::string>  {
  void constraints() { } //no check for char arrays
};
template<>
struct KeyConcept<unsigned long long, std::string>  {
  void constraints() { } //no check for char arrays
};


//HELPER FUNCTION AND CLASS TO EXPRESS THE CHECK IN A COMPACT FASHION
template <class T>
inline int classIsKey() {
#ifndef SKIP_GCC_INTERNAL_ERROR_980519 
  //triggers an internal error in 2.91
  boost::function_requires< KeyConcept<T> > ();
#endif
  return 0;
}

template <class T>
struct KeyClass {
  KeyClass() {}
  ~KeyClass() {}
  BOOST_CONCEPT_ASSERT((KeyConcept<T>));
};

#endif







