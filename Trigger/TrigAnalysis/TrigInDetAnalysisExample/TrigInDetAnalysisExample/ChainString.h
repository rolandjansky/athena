/* emacs: this is -*- c++ -*- */
/**
 **   @file    ChainString.cxx         
 **
 **   @brief   parse an analysis chain specification
 **
 **   @author M.Sutton
 **   @date   Thu 30 Apr 2015 14:03:50 CEST 
 **
 **  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisExample_ChainString_H
#define TrigInDetAnalysisExample_ChainString_H


#include <string>
#include <vector>
#include <iostream>


class ChainString : public std::string {

public:

  ChainString( const std::string& s );

  ChainString( const ChainString& s );

  ChainString& operator=(const ChainString&) = default;
  std::string head()  { return m_head;  }
  std::string tail()  { return m_tail;  }
  std::string extra() { return m_extra; }
  std::string element() { return m_element; }
  std::string roi()     { return m_roi; }
  std::string vtx()     { return m_vtx; }

  bool        passed()  { return m_passed; }

  const std::string& head()  const { return m_head;  }
  const std::string& tail()  const { return m_tail;  }
  const std::string& extra() const { return m_extra; }
  const std::string& element() const { return m_element; }
  const std::string& roi()     const { return m_roi; }
  const std::string& vtx()     const { return m_vtx; }

  const bool&        passed()  const { return m_passed; }

  const std::string& raw() const { return m_raw; }
  
  /// can't make this return a reference in case there 
  /// is no such key - could throw an exception then it 
  /// would work, but that is far too excessive 
  std::string value( const std::string& key ) const { 
    int i=find(key);
    if ( i>=0 ) return m_values[i];
    return "";
  }

  /// same here regarding returning a reference 
  std::string postvalue( const std::string& key ) const { 
    if ( postcount() ) return value( key+"-post" ); 
    return "";
  }

  const std::vector<std::string>& values() const { return m_values; }
  const std::vector<std::string>&   keys() const { return   m_keys; }

  std::string         pre() const { return m_raw.substr( 0, m_raw.find(":post") ); }
  const std::string& post() const { return m_post; }

  size_t postcount() const { return m_postcount; }

public:   

  // chop tokens off the front of a string
  static std::string chop(std::string& s1, const std::string& s2) {
    std::string::size_type pos = s1.find_first_of(s2);
    std::string s3;
    if ( pos == std::string::npos ) {
      s3 = s1;
      s1.erase(0, s1.size());
    }
    else {
      s3 = s1.substr(0, pos); 
      s1.erase(0, pos+1);
    }
    return s3;
  } 

protected:

  // chomp tokens off the end of a string
  static std::string chomp(std::string& s1, const std::string& s2) {
    //    std::string::size_type pos = s1.find(s2);
    std::string::size_type pos = s1.find_first_of(s2);
    std::string s3;
    if ( pos == std::string::npos ) return "";

    s3 = s1.substr(pos+1, s1.size()); 
    s1.erase(pos, s1.size());

    return s3;
  } 

  /// convert to upper case
  static char toupper( char c ) { return ( c>='a' && c<='z' ? c+'A'-'a' : c ); }

  /// convert to upper case
  static std::string toupper( const std::string& s ) { 
    const char* c = s.c_str();
    char tmp[512];
    char* tp = tmp;
    while (( *tp++ = toupper(*c++) ));
    return tmp;
  }

  /// convert to lower case
  static char tolower( char c ) { return ( c>='A' && c<='Z' ? c-'A'+'a' : c ); }

  /// convert to lower case
  static std::string tolower( const std::string& s ) { 
    const char* c = s.c_str();
    char tmp[512];
    char* tp = tmp;
    while (( *tp++ = tolower(*c++) ));
    return tmp;
  }

protected:

  /// parse the full specification string
  void parse();
  void parse( std::string s );

  int find( const std::string& key ) const { 
    for ( int i=m_keys.size() ; i-- ; ) if ( key==m_keys[i] ) return i;
    return -1;
  }

private:

  std::string m_head;
  std::string m_tail;
  std::string m_extra;
  std::string m_element;
  std::string m_roi;
  std::string m_vtx;

  bool        m_passed;

  std::vector<std::string> m_keys;
  std::vector<std::string> m_values;

  std::string m_raw;

  std::string m_post;
  size_t      m_postcount;

};



#endif // TrigInDetAnalysisExample_ChainString_H
