/* emacs: this is -*- c++ -*- */
/**
 **     @file    ChainString.cxx
 **
 **     @author  mark sutton
 **     @date    Thu 30 Apr 2015 14:06:56 CEST 
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/


#include "TrigInDetAnalysisExample/ChainString.h"



ChainString::ChainString( const std::string& s ) : 
  std::string(s),  
  m_head(""), m_tail(""), m_roi(""), m_vtx(""), m_element(""), m_extra(""), 
  m_passed(true), m_raw(""), m_post(""), m_postcount(0) {
  if ( s.find(":post")!=std::string::npos ) m_post = s.substr( s.find(":post")+6, s.size() );
  parse(s);
}


ChainString::ChainString( const ChainString& s ) :
  std::string(s), 
  m_head(s.m_head), m_tail(s.m_tail), m_roi(s.m_roi), m_vtx(s.m_vtx),
  m_element(s.m_element), m_extra(s.m_extra),
  m_passed(s.m_passed),
  m_raw(s.m_raw), m_post(s.post()), m_postcount(s.postcount()),
  m_keys(s.m_keys),
  m_values(s.m_values) {
}



/// parse the full specification string
void ChainString::parse( std::string s ) { 

    std::vector<std::string> fields;

    while ( !s.empty() ) fields.push_back( chop( s, ":" ) );
    
    bool postkeys = false;

    size_t keycount = 0;

    if ( fields.size() ) m_head = fields[0];

    for ( size_t i=1 ; i<fields.size() ; i++ ) { 
      std::string dte = chomp( fields[i], ";" );
      if ( !postkeys && dte=="DTE" ) m_passed = false;
      if      ( fields[i]=="DTE" )   m_passed = false;
      else if ( fields[i]=="post" )  postkeys = true; 
      else { // if ( fields[i].find("=")!=std::string::npos ) { 
	std::string f = fields[i];
	std::string key = chop( f, "=" );
	if ( f=="" ) { 
	  f=key;
	  key="";
	}
	if ( postkeys ) { key += "-post"; m_postcount++; }
	else keycount++;
	m_keys.push_back( tolower(key) );
	m_values.push_back( f );
      }
    }

    std::string tags[5] = {  "key",  "roi",  "vtx",  "te",    "extra" };
    bool      tagged[5] = {  false,  false,  false,  false,   false   };
    
    std::string* values[5] = { &m_tail, &m_roi, &m_vtx, &m_element, &m_extra };
    
    for ( size_t i=0 ; i<keycount && i<5 ; i++ ) { 
      if ( m_keys[i] == "" ) {  
	if ( tagged[i] ) std::cerr << "tag already allocated : " << tags[i] << " with value " << *values[i] << std::endl; 
	else *values[i] = m_values[i];
      }
      else { 
	bool unset = true;
	for ( int j=0 ; j<5 ; j++ ) {
	  if ( tags[j]==m_keys[i] ) {
	    if ( tagged[j] ) std::cerr << "tag already allocated : " << tags[j] << " with value " << *values[j] << std::endl; 
	    else { 
	      *values[j] = m_values[i];
	      unset = false;
	      tagged[j] = true;
	    }
	  }
	}
	if ( unset ) std::cerr << "no such tag: " << m_keys[i] << std::endl; 
      }
    }
   
#if 0
    std::cout << "head:  " << m_head    << std::endl;
    std::cout << "key:   " << m_tail    << std::endl;
    std::cout << "roi:   " << m_roi     << std::endl;
    std::cout << "vtx:   " << m_vtx     << std::endl;
    std::cout << "te:    " << m_element << std::endl;
    std::cout << "ind:   " << m_extra   << std::endl;
    std::cout << "pass:  " << m_passed  << std::endl;
#endif   

    /// replace the string by a parsed basic string 

    std::string raw = m_head;
    for ( int i=0 ; i<5 ; i++ ) if ( *values[i]!="" ) raw += ":" + *values[i];
    if ( !m_passed ) raw += ";DTE";
  
    /// overwrite with the parsed string 
    *(std::string*)(this) = raw;

    raw = m_head;
    for ( int i=0 ; i<5 ; i++ ) if ( *values[i]!="" ) raw += ":" + tags[i] + "=" + *values[i];
    if ( !m_passed ) raw += ";DTE";

    if ( postcount() ) raw += ":post:" + m_post; 

    m_raw = raw;

}




std::string ChainString::subs( std::string s ) const { 

  std::string tags[5] = { "key=", "roi=", "vtx=", "te=", "extra=" };
 
  const std::string* values[5] = { &m_tail, &m_roi, &m_vtx, &m_element, &m_extra };

  for ( int i=0 ; i<5 ; i++ ) if ( *values[i]!="" ) s += ":" + tags[i] + *values[i];
  if ( !m_passed ) s += ";DTE";
  
  if ( postcount() ) s += ":post:" + m_post;
  
  return s;
}
