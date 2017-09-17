// emacs: this is -*- c++ -*-
//
//   @file    utils.cxx        
//            utility functions
//                    
//   @author M.Sutton
// 
//   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   Mon 31 July 2017 08:42:16 CEST sutt




#include <sys/stat.h>
#include <regex>


#include "utils.h"


/// get the date *without* the return
std::string date() { 
  time_t _t;
  time(&_t);
  std::string a = ctime(&_t);
  std::string b = "";
  for ( unsigned i=0 ; i<a.size()-1 ; i++ ) b+=a[i];
  return b;
}


bool file_exists( const std::string& file ) { 
  struct stat buff;
  return stat(file.c_str(),&buff)==0;   
}



/// match a pattern from a vector of possible patterns
bool find( const std::string& s, const std::vector<std::string>& regexv ) { 
  for ( unsigned i=regexv.size() ; i-- ; ) if ( std::regex_match( s, std::regex(regexv[i]) ) ) return true;
  return false;
} 


/// count how many occurances of a regx are in a string 
int count( std::string s, const std::string& regx ) {
  size_t p = s.find( regx );
  int i=0;
  while ( p!=std::string::npos ) {
    i++;
    s.erase( 0, p+1 );
    p = s.find( regx );
  } 
  return i;
}




// chop tokens off the front of a string
std::string chop(std::string& s1, const std::string& s2)
{
  std::string::size_type pos = s1.find(s2);
  std::string s3;
  if ( pos == std::string::npos ) {
    s3 = s1;
    s1.erase(0, s1.size());
  }
  else {
    s3 = s1.substr(0, pos); 
    s1.erase(0, pos+s2.size());
  }
  return s3;
} 

/// split a string on a token
std::vector<std::string> split( const std::string& s, const std::string& t  ) {
    
    std::string _s = s;
    size_t pos = _s.find(t);
    
    std::vector<std::string> tags;
    
    while ( pos!=std::string::npos ) { 
      tags.push_back( chop(_s,t) );
      pos = _s.find(t);
    }
    
    tags.push_back(_s);
    
    return tags;
}


// chop tokens off the front of a string but not including 
// chop character
std::string chopex(std::string& s1, const std::string& s2)
{
  std::string::size_type pos = s1.find(s2);
  std::string s3;
  if ( pos == std::string::npos ) {
    s3 = s1;
    s1.erase(0, s1.size());
  }
  else {
    s3 = s1.substr(0, pos); 
    s1.erase(0, pos+s2.size());
  }
  return s3;
} 


// chomp them off the end
std::string chomp(std::string& s1, const std::string& s2)
{
  std::string::size_type pos = s1.find(s2);
  std::string s3;
  if ( pos == std::string::npos ) {
    s3 = s1;
    s1.erase(0,s1.size());    
  }
  else {
    s3 = s1.substr(pos+s2.size(),s1.size());
    s1.erase(pos,s1.size()); 
  } 
  return s3;
} 



// chop tokens off the end of a string, leave string unchanged
// return choped string
std::string choptoken(std::string& s1, const std::string& s2)
{
  std::string s3 = "";
  std::string::size_type pos = s1.find(s2);
  if ( pos != std::string::npos ) {
    s3 = s1.substr(0, pos+s2.size()); 
    s1.erase(0, pos+s2.size());
  }
  return s3;
} 


// chop tokens off the end of a string, leave string unchanged
// return choped string
std::string chomptoken(std::string& s1, const std::string& s2)
{
  std::string s3 = "";
  std::string::size_type pos = s1.find(s2);
  if ( pos != std::string::npos ) {
    s3 = s1.substr(pos, s1.size());
    s1.erase(pos, s1.size()); 
  }
  return s3;
} 


// chop tokens off the front of a string
std::string chopfirst(std::string& s1, const std::string& s2)
{
  std::string s3;
  std::string::size_type pos = s1.find_first_not_of(s2);
  if ( pos != std::string::npos ) {
    s3 = s1.substr(0, pos); 
    s1.erase(0, pos);
  }
  else {
    s3 = s1;
    s1 = "";
  } 
  return s3;
} 


std::string choplast(std::string& s1, const std::string& s2)
{
  std::string s3 = "";
  std::string::size_type pos = s1.find_last_not_of(s2);
  if ( pos != std::string::npos ) {
    s3 = s1.substr(pos+1, s1.size());
    s1.erase(pos+1, s1.size());
  }
  return s3;
} 



// chop tokens off the front and end of a string
std::string chopends(std::string& s1, const std::string& s2)
{
  chopfirst(s1, s2);
  choplast(s1, s2);
  return s1;
} 



// remove strings from a string
void removespace(std::string& s, const std::string& s2) 
{
  std::string::size_type pos;
  while ( (pos = s.find(s2))!=std::string::npos ) {
    s.erase(pos, s2.size());
  }
} 


// replace from a string
std::string replace( std::string s, const std::string& s2, const std::string& s3 ) {
  std::string::size_type pos;
  //  while ( (pos = s.find(" "))!=std::string::npos ) {
  //    s.replace(pos, 1, "-");
  while ( (pos = s.find(s2))!=std::string::npos ) {
    s.replace(pos, s2.size(), s3);
    if ( contains(s3,s2) ) break;
  }
  return s;
} 


// remove regx from a string
void depunctuate(std::string& s, const std::string& regx ) 
{
  std::string::size_type pos;
  while ( (pos = s.find(regx))!=std::string::npos ) {
    s.erase(pos, regx.size());
  }
} 
  



std::string chopto( std::string& s, const std::string& pattern ) { 
  std::string tag;
  std::string::size_type pos = s.find_first_of( pattern );
  if ( pos==std::string::npos ) { 
    tag = s;
    s = "";
  }
  else { 
    tag = s.substr(0,pos);
    s.erase( 0, pos ); 
  }
  return tag;
}


// remove strings from a string
bool remove( std::string& s, const std::string& s2 ) 
{
  std::string::size_type ssize = s.size();
  std::string::size_type pos;
  while ( (pos=s.find(s2))==0 ) s.erase(pos, s2.size());
  if ( ssize!=s.size() ) return true;
  else                   return false;
} 














