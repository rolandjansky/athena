// emacs: this is -*- c++ -*-  
//
//   @file    ChainString.cxx         
//            parse an analysis chain specification
//
//   @author M.Sutton
// 
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: ChainString.h, v0.0   Thu 30 Apr 2015 14:03:50 CEST sutt $


#ifndef TrigInDetAnalysisExample_ChainString_H
#define TrigInDetAnalysisExample_ChainString_H


#include <string>
#include <vector>
#include <iostream>


class ChainString : public std::string {

public:

  ChainString(const std::string& s);

  ChainString(const ChainString& s);

  std::string head()  { return mhead;  }
  std::string tail()  { return mtail;  }
  std::string extra() { return mextra; }
  std::string element() { return melement; }
  std::string roi()     { return mroi; }
  std::string vtx()     { return mvtx; }

  bool        passed()  { return mpassed; }

  const std::string& head()  const { return mhead;  }
  const std::string& tail()  const { return mtail;  }
  const std::string& extra() const { return mextra; }
  const std::string& element() const { return melement; }
  const std::string& roi()     const { return mroi; }
  const std::string& vtx()     const { return mvtx; }

  const bool&        passed()  const { return mpassed; }

protected:

  // chop tokens off the front of a string
  std::string chop(std::string& s1, const std::string& s2) {
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
  

  // chomp tokens off the end of a string
  std::string chomp(std::string& s1, const std::string& s2) {
    std::string::size_type pos = s1.find(s2);
    std::string s3;
    if ( pos == std::string::npos ) return "";

    s3 = s1.substr(pos+s2.size(), s1.size()); 
    s1.erase(pos, s1.size());

    return s3;
  } 

  /// convert to upper case
  char toupper( char c ) { return ( c>='a' && c<='z' ? c+'A'-'a' : c ); }

  /// convert to upper case
  std::string toupper( const std::string& s ) { 
    const char* c = s.c_str();
    char tmp[512];
    char* tp = tmp;
    while (( *tp++ = toupper(*c++) ));
    return tmp;
  }

  /// convert to lower case
  char tolower( char c ) { return ( c>='A' && c<='Z' ? c-'A'+'a' : c ); }

  /// convert to lower case
  std::string tolower( const std::string& s ) { 
    const char* c = s.c_str();
    char tmp[512];
    char* tp = tmp;
    while (( *tp++ = tolower(*c++) ));
    return tmp;
  }

  /// parse the full specification string
  void parse();

private:

  std::string mhead;
  std::string mtail;
  std::string mextra;
  std::string melement;
  std::string mroi;
  std::string mvtx;

  bool        mpassed;

};



#endif // TrigInDetAnalysisExample_ChainString_H
