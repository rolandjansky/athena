// emacs: this is -*- c++ -*-
//
//   @file    utils.h        
//            utility functions
//                    
//   @author M.Sutton
// 
//   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   Mon 31 July 2017 08:41:27 CEST sutt


#ifndef  UTILS_H
#define  UTILS_H

#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>

#include "TObject.h"
#include "TKey.h"
#include "TH1D.h"



template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T>& v ) {
  if ( v.empty() ) return s;
  for ( size_t i=0 ; i<v.size() ; i++ ) s << v[i] << "\n";
  return s;
}


template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T*>& v ) {
  if ( v.empty() ) return s;
  for ( int i=0 ; i<v.size() ; i++ ) s << *v[i] << "\n";
  return s;
}



/// get a TObject* from a TKey* 
/// (why can't a TObject be a TKey?)
template<class T>
T* get( TKey* tobj ) { 
  TObject* a = tobj->ReadObj()->Clone();
  ((TH1*)a)->SetDirectory(0); /// shouldn't this crash if it is not a TH1* ?  
  return (T*)a;
}


/// does a string conatian a pattern  
inline bool contains( const std::string& s, const std::string& pattern ) { return s.find(pattern)!=std::string::npos; } 


/// simple error reporting class - should probably throw an exception, 
/// only this is simpler
inline void error( int i, std::ostream& s ) {  s << std::endl; std::exit(i);  }



/// get the date *without* the return
std::string date();

/// does a file exist 
bool file_exists( const std::string& file );


/// match a pattern from a vector of possible patterns
bool find( const std::string& s, const std::vector<std::string>& regexv );



/// count how many occurances of a regx are in a string 
int count( std::string s, const std::string& regx );



// chop tokens off the front of a string
std::string chop(std::string& s1, const std::string& s2);


/// split a string on a token
std::vector<std::string> split( const std::string& s, const std::string& t=":"  );


// chop tokens off the front of a string but not including 
// chop character
std::string chopex(std::string& s1, const std::string& s2);



// chomp them off the end
std::string chomp(std::string& s1, const std::string& s2);


// chop tokens off the end of a string, leave string unchanged
// return choped string
std::string choptoken(std::string& s1, const std::string& s2);


// chop tokens off the end of a string, leave string unchanged
// return choped string
std::string chomptoken(std::string& s1, const std::string& s2);


// chop tokens off the front of a string
std::string chopfirst(std::string& s1, const std::string& s2);


// chop off the end of the string
std::string choplast(std::string& s1, const std::string& s2);


// chop tokens off the front and end of a string
std::string chopends(std::string& s1, const std::string& s2);


// remove strings from a string
void removespace(std::string& s, const std::string& s2); 


// replace from a string
std::string replace( std::string s, const std::string& s2, const std::string& s3);


// remove regx from a string
void depunctuate(std::string& s, const std::string& regx=":"); 


// chop up to the first character matching any of those 
// in the pattern
std::string chopto( std::string& s, const std::string& pattern );


// remove strings from a string
bool remove( std::string& s, const std::string& s2 ); 



#endif  // UTILS_H 










