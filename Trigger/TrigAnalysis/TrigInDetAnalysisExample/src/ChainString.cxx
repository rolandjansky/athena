//
//   @file    ChainString.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: ChainString.cxx, v0.0   Thu 30 Apr 2015 14:06:56 CEST sutt $


#include "TrigInDetAnalysisExample/ChainString.h"



ChainString::ChainString(const std::string& s) : 
  std::string(s), mpassed(true), mraw(s), mpost(""), mpostcount(0) {
  if ( mraw.find(":post")!=std::string::npos ) mpost = mraw.substr( mraw.find(":post")+6, mraw.size() );
  parse(); 
}


ChainString::ChainString( const ChainString& s ) :
  std::string(s), 
  mhead(s.mhead), mtail(s.mtail), mextra(s.mextra),
  melement(s.melement), mroi(s.mroi), 
  mvtx(s.mvtx),
  mpassed(s.mpassed),
  mkeys(s.mkeys),
  mvalues(s.mvalues),
  mraw(s.mraw), mpost(s.post()), mpostcount(s.postcount()) {  

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
      if ( !postkeys && dte=="DTE" ) mpassed = false;
      if      ( fields[i]=="DTE" )   mpassed = false;
      else if ( fields[i]=="post" )  postkeys = true; 
      else if ( fields[i].find("=")!=std::string::npos ) { 
	std::string _field = fields[i];
	std::string key = chop( _field, "=" );
	if ( postkeys ) { key += "-post"; mpostcount++; }
	mkeys.push_back( key );
	mvalues.push_back( _field );
      }
    }

    bool usetags   = false;
    
    if ( fields.size() ) mhead = fields[0]; 

    std::string tags[5] = { "collection=", "index=", "roi=", "vtx=", "te=" };
    std::string  alt[5] = {        "key=",   "ind=",     "",     "",    "" };
    bool      tagged[5] = {         false,    false,  false,  false, false };    
 
    std::string* _values[5] = { &mtail, &mextra, &mroi, &mvtx, &melement };

    
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
    /// chain, collection and index
    /// grrrr, 
    if ( !usetags ) { 
      //      std::cout << "fields.size() " << fields.size() << std::endl;
      for ( unsigned i=first_tag ; i<5 ; i++ ) {
	if ( (i+1)<fields.size() ) *_values[i] = fields[i+1];
      }
    }      

#if 0
    std::cout << "head:  " << mhead    << std::endl;
    std::cout << "key:   " << mtail    << std::endl;
    std::cout << "ind:   " << mextra   << std::endl;
    std::cout << "te:    " << melement << std::endl;
    std::cout << "roi:   " << mroi     << std::endl;
    std::cout << "vtx:   " << mvtx     << std::endl;
    std::cout << "pass:  " << mpassed  << std::endl;
#endif   
    
    /// replace the string by a parsed basic string that can be converted
    /// to a root directory name etc
    
    std::string raw = mhead;
    for ( int i=0 ; i<5 ; i++ ) if ( *_values[i]!="" ) raw += ":" + *_values[i];
    if ( !mpassed ) raw += ";DTE";
  
    /// overwrite with the parsed string 
    *(std::string*)(this) = raw;

}

