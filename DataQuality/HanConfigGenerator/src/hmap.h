// emacs: this is -*- c++ -*-
//
//   @file    hmap.h        
//            class to store map of regular expressions and corresponding
//            values 
//                    
//   @author M.Sutton
// 
//   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   Mon 31 July 2017 08:41:16 CEST sutt$



#ifndef  HMAP_H
#define  HMAP_H

#include <iostream>
#include <regex>



/// helper class for storing a histogram name, parsed into 
/// directories 

class histogram_name : public std::string { 
  
public:

  histogram_name( std::string s="" ) : std::string(s) { construct( s ); }
  histogram_name( const char* s    ) : std::string(s) { construct( std::string(s) ); }
  
  std::vector<std::string>&       dirs()       { return mdirs; }
  const std::vector<std::string>& dirs() const { return mdirs; }

private:
  
  void construct( std::string s ) { 
    std::string::size_type pos = s.find("/");
    while ( pos!=std::string::npos ) { 
      std::string s0 = chop( s, "/" );
      mdirs.push_back(s0);
      pos = s.find("/");
    } 
    mdirs.push_back(s);
  } 

protected:

  std::vector<std::string> mdirs;

};




/// map class to allow setting of the algorithms 
/// and descriptions

class hmap_t : public std::map<histogram_name,std::string> { 

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


#endif  // HMAP_H 










