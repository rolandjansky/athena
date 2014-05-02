/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------------// 
//                                                                          // 
// Query class.  This class can be used to return the result of a query     // 
// that can fail.  Objects of this class can convert themselves to the      // 
// result type T.  Client code can check for a valid result; if it doesn't, // 
// and the result is valid, no problem.   But if the result is invalid you  // 
// will print an error message and terminate.                               // 
//                                                                          // 
// Joe Boudreau                                                             // 
// based on Barton & Nackman's "Scientific and Engineering C++"             // 
//                                                                          // 
// -------------------------------------------------------------------------// 
#ifndef _QUERY_H_
#define _QUERY_H_
#include <stdexcept>
template < class T > class Query
{

public:

  // Constructor: 
  inline Query (const T &);

  // Default constructor: 
  inline Query ();

  // Convert to "T"  
  inline operator         T () const;

  // Test Validity 
  inline bool isValid () const;

private:

  bool _failed;
  T _instance;

};


template < class T > inline Query < T >::Query (const T & t):
_failed (false),
_instance (t)
{
}

template < class T > inline Query < T >::Query ():
_failed (true),
_instance (T())
{
}

template < class T > inline Query < T >::operator         T () const
{
  if (_failed)
    throw std::range_error ("Failed query");
  return _instance;
}

template < class T > inline bool Query < T >::isValid () const
{
  return !_failed;
}


#endif
