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



ChainString::ChainString(const std::string& s) : 
  std::string(s), m_passed(true), m_raw(s), m_post(""), m_postcount(0) {
  if ( m_raw.find(":post")!=std::string::npos ) m_post = m_raw.substr( m_raw.find(":post")+6, m_raw.size() );
  parse(); 
}


ChainString::ChainString( const ChainString& s ) :
  std::string(s), 
  m_head(s.m_head), m_tail(s.m_tail), m_extra(s.m_extra),
  m_element(s.m_element), m_roi(s.m_roi),
  m_vtx(s.m_vtx),
  m_passed(s.m_passed),
  m_keys(s.m_keys),
  m_values(s.m_values),
  m_raw(s.m_raw), m_post(s.post()), m_postcount(s.postcount()) {

}


void ChainString::parse() { parse( *this ); }

/// parse the full specification string
void ChainString::parse( std::string _s ) { 

    std::vector<std::string> fields;

    while ( _s.find_first_of(":;")!=std::string::npos ) fields.push_back( chop( _s, ":" ) );
    
    fields.push_back(_s);

    bool postkeys = false;

    for ( unsigned i=0 ; i<fields.size() ; i++ ) { 
      std::string dte = chomp( fields[i], ";" );
      if ( !postkeys && dte=="DTE" ) m_passed = false;
      if      ( fields[i]=="DTE" )   m_passed = false;
      else if ( fields[i]=="post" )  postkeys = true; 
      else if ( fields[i].find("=")!=std::string::npos ) { 
	std::string _field = fields[i];
	std::string key = chop( _field, "=" );
	if ( postkeys ) { key += "-post"; m_postcount++; }
	m_keys.push_back( key );
	m_values.push_back( _field );
      }
    }

    bool usetags   = false;
    
    if ( fields.size() ) m_head = fields[0];

    std::string tags[5] = { "collection=", "extra=", "roi=", "vtx=", "te=" };
    std::string  alt[5] = {        "key=",   "ind=",     "",     "",    "" };
    bool      tagged[5] = {         false,    false,  false,  false, false };    
 
    std::string* _values[5] = { &m_tail, &m_extra, &m_roi, &m_vtx, &m_element };

    
    /// get collection, index and roi if not tagged with a label

    unsigned first_tag = 1;
    for ( unsigned i=1 ; i<fields.size() && i<4 ; i++ ) {
      if ( fields[i].find("=")==std::string::npos ) {
	*_values[i-1] = fields[i];
	first_tag = i+1;
	tagged[i-1] = true;
      }
      else { 
	usetags = true;
	break;
      } 
    }

    /// now test whether any tags have been used at all

    for ( unsigned i=1 ; i<fields.size() ; i++ ) {
      if ( fields[i].find("=")!=std::string::npos ) usetags = true;
    }
    
    for ( unsigned i=first_tag ; i<fields.size() ; i++ ) {
      for ( unsigned itag=0 ; itag<5 ; itag++ ) { 

	std::string lowfield = tolower(fields[i]);

	if ( tagged[itag] ) { 
	  if ( lowfield.find(tags[itag])==0 || ( alt[itag]!="" && lowfield.find(alt[itag])==0 ) ) { 
	    std::cerr << "tag already allocated : " << fields[i] << " with value " << *_values[itag] << std::endl;
	  }
	  continue;
	}
	//	std::cout << itag << " " << i << " " << fields[i] << " " << toupper(fields[i]).find(toupper(tags[itag])) << std::endl; 

	if ( lowfield.find(tags[itag])==0 ) { 
	  tagged[itag] = true;
	  fields[i].erase( 0, tags[itag].size() );
	  *_values[itag] = fields[i];
	  //	  if ( itag<2 ) usetags   = true;
	  //  else           useroitag = true; 
	  break;
	}
	else if ( alt[itag]!="" && lowfield.find(alt[itag])==0 ) {  
	  tagged[itag] = true;
	  fields[i].erase( 0, alt[itag].size() );
	  *_values[itag] = fields[i];
	  //  if ( itag<2 ) usetags   = true;
	  //  else           useroitag = true; 
	  break;
	}

      }
    }
    
    /// always enforce tags for the roi, vtx and te, optional for the 
    /// chain, collection and extra (index)
    /// grrrr, 
    if ( !usetags ) { 
      //      std::cout << "fields.size() " << fields.size() << std::endl;
      for ( unsigned i=first_tag ; i<5 ; i++ ) {
	if ( (i+1)<fields.size() ) *_values[i] = fields[i+1];
      }
    }      

#if 0
    std::cout << "head:  " << m_head    << std::endl;
    std::cout << "key:   " << m_tail    << std::endl;
    std::cout << "ind:   " << m_extra   << std::endl;
    std::cout << "te:    " << m_element << std::endl;
    std::cout << "roi:   " << m_roi     << std::endl;
    std::cout << "vtx:   " << m_vtx     << std::endl;
    std::cout << "pass:  " << m_passed  << std::endl;
#endif   
    
    /// replace the string by a parsed basic string that can be converted
    /// to a root directory name etc
    
    std::string raw = m_head;
    for ( int i=0 ; i<5 ; i++ ) if ( *_values[i]!="" ) raw += ":" + *_values[i];
    if ( !m_passed ) raw += ";DTE";
  
    /// overwrite with the parsed string 
    *(std::string*)(this) = raw;

}

