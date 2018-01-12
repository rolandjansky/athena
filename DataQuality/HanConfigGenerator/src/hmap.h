// emacs: this is -*- c++ -*-
//
//   @file    hmap.h        
//            class to store map of regular expressions and corresponding
//            values 
//                    
//   @author M.Sutton
// 
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   Mon  6 Aug 2017 03:13:34 CEST sutt


#ifndef  HANCONFIGGENERATOR_HMAP_H
#define  HANCONFIGGENERATOR_HMAP_H

#include <iostream>
#include <regex>


typedef std::string histogram_name;


/// map class to allow setting of the algorithms 
/// and descriptions

class hmap_t : private std::map<histogram_name,std::string> { 

public:

  typedef std::map<histogram_name,std::string> map_type;

  using map_type::value_type;
  using map_type::insert;

  using map_type::size;
  using map_type::const_iterator;
  using map_type::begin;
  using map_type::end;

public:

  /// sadly, when matching regular expressions, we need to iterate 
  /// through the map and cannot use the standard map::find() methods
  /// which sadly, slows the searching down

  virtual std::map<histogram_name,std::string>::iterator find( const std::string& s ) {
    return std::map<histogram_name,std::string>::find( s );
  }

  virtual std::map<histogram_name,std::string>::const_iterator find( const std::string& s ) const { 
    return std::map<histogram_name,std::string>::find( s );
  }

  virtual std::map<histogram_name,std::string>::iterator match( const std::string& s ) {
    std::map<histogram_name,std::string>::iterator  itr = begin();
    while( itr!=end() ) { 
      if ( std::regex_match( s, std::regex(itr->first) ) ) return itr;
      itr++;
    }
    return itr;
  }

  virtual std::map<histogram_name,std::string>::const_iterator match( const std::string& s ) const { 
    std::map<histogram_name,std::string>::const_iterator  itr = begin();
    while( itr!=end() ) { 
      if ( std::regex_match( s, std::regex(itr->first) ) ) return itr;
      itr++;
    }
    return itr;
  }
  
};


inline std::ostream& operator<<( std::ostream& s, const hmap_t& h ) { 
  for ( hmap_t::const_iterator itr=h.begin() ; itr!=h.end() ; itr++ ) {
    s << "[ " << itr->first << "\t : " << itr->second << " ]\n";
  }
  return s;
}


#endif  // HANCONFIGGENERATOR_HMAP_H 










