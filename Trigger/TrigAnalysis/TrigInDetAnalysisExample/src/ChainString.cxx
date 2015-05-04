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
  std::string(s), mpassed(true) {
  parse(); 
}


ChainString::ChainString(const ChainString& s) :
  std::string(s), 
  mhead(s.mhead), mtail(s.mtail), mextra(s.mextra),
  melement(s.melement), mroi(s.mroi), 
  mpassed(s.mpassed) {  

}


/// parse the full specification string
void ChainString::parse() { 

    std::string _s = *this;
    
    std::vector<std::string> fields;

    //    std::cout << "\nChainString::parse() " << _s << std::endl;
    
    std::string pass = chomp(_s,";");
    if ( pass!="" ) mpassed = ( pass=="DTE" ? false : true );    

    std::string::size_type pos = _s.find(":");

    while ( pos!=std::string::npos ) { 
      fields.push_back( chop( _s, ":" ) ); 
      pos = _s.find(":");
    }
    
    fields.push_back(_s);

    bool usetags   = false;
    //  bool useroitag = false;
    
    mhead = fields[0]; 

    std::string tags[4]   = { "collection=", "index=", "te=", "roi=" };
    std::string alt[4]    = { "key=",        "ind=",   "",    ""     };
    bool        tagged[4] = {  false,         false,    false, false };    
 
    std::string* values[4] = { &mtail, &mextra, &melement, &mroi };

    for ( unsigned i=1 ; i<fields.size() ; i++ ) {
      for ( unsigned itag=0 ; itag<4 ; itag++ ) { 
	if ( tagged[itag] ) continue;
	//	std::cout << itag << " " << i << " " << fields[i] << " " << toupper(fields[i]).find(toupper(tags[itag])) << std::endl; 

	std::string lowfield = tolower(fields[i]);

	if ( lowfield.find(tags[itag])==0 ) { 
	  tagged[itag] = true;
	  fields[i].erase( 0, tags[itag].size() );
	  *values[itag] = fields[i];
	  if ( itag!=3 ) usetags   = true;
	  //  else           useroitag = true; 
	  break;
	}
	else if ( alt[itag]!="" && lowfield.find(alt[itag])==0 ) {  
	  tagged[itag] = true;
	  fields[i].erase( 0, alt[itag].size() );
	  *values[itag] = fields[i];
	  if ( itag!=3 ) usetags   = true;
	  //  else           useroitag = true; 
	  break;
	}


      }
    }
    
    //    std::cout << "use tags " << usetags << std::endl; 

    /// if any tags have been used (except for the Roi tag) - then 
    /// *all* tags should be used   
    if ( !usetags ) { 
      //      std::cout << "fields.size() " << fields.size() << std::endl;
      for ( unsigned i=0 ; i<4 ; i++ ) {
	if ( fields.size()>i+1 ) *values[i] = fields[i+1];
      }
    }      

#if 0
    std::cout << "head :      " << mhead    << std::endl;
    std::cout << "collection: " << mtail    << std::endl;
    std::cout << "index :     " << mextra   << std::endl;
    std::cout << "te :        " << melement << std::endl;
    std::cout << "roi :       " << mroi     << std::endl;
    std::cout << "pass :      " << mpassed  << std::endl;
#endif   
    
    /// replace the string by a raw "basic" string that can be converted
    /// to a root directory etc

    std::string raw = mhead;
    for ( int i=0 ; i<4 ; i++ ) if ( *values[i]!="" ) raw += ":" + *values[i];
    if ( !mpassed ) raw += ";DTE";
    *(std::string*)(this) = raw;

    //    std::cout << "raw: " << *this << std::endl;
  }

