/* emacs: this is -*- c++ -*- */
/**
 **     @file    chainparser.cxx
 **
 **     @author  mark sutton
 **     @date    Mon 23 Jan 2017 12:30:25 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/

#include <iostream> 
#include <fstream> 
#include <string> 
#include <vector> 
#include <cstdlib> 
#include <cstdio> 
#include <map> 
#include <regex>



#include <cmath>

#include <algorithm>

#include "utils.h"

template<typename T>
std::ostream& operator<<( std::ostream& s, std::vector<T>& v ) { 
  for ( size_t i=0 ; i<v.size() ; i++ ) s << v[i] << std::endl;
  return s; 
}


bool contains( const std::string& s, const std::string& regx ) { 
  return s.find( regx )!=std::string::npos;
}

bool contains_end( const std::string& s, const std::string& regx ) { 
  return std::regex_match( s, std::regex(regx+"$") ); 
}


std::vector<std::string> split( std::string& line ) { 
  std::vector<std::string> strings;
 
  while ( contains( line, "/" ) )  line.replace( line.find("/"), 1, "   " );
  while ( contains( line, "\t" ) ) line.replace( line.find("\t"), 1, " " );

  while ( contains( line, " " ) ) { 
    std::string _s = chop( line, " " );
    std::string  s = chop( _s, "\t" );
    if ( contains( s, "\t" ) ) { 
      line.replace( s.find("\t"), 1, " " );
      continue;
    }
    while ( contains( s, " " ) ) line.replace( s.find(" "), 1, "" );
    if ( s != "" ) strings.push_back( s );
  }

  strings.push_back( line );
  
  //  for ( size_t i=0 ; i<strings.size() ; i++ ) std::cout << " : " << strings[i];
  //  std::cout << std::endl;

  return strings;
}


struct threshold { 

  threshold( double t_, int i_, const std::string& c, const std::string& k, int co ) : 
    i(i_), thresh(t_), chain(c), key(k), counts(co)  { } 

  int         i;
  double      thresh;

  std::string chain;
  std::string key;

  int         counts;

};

bool operator< ( const threshold& t0, const threshold& t1 ) { return t0.thresh <  t1.thresh; } 
bool operator> ( const threshold& t0, const threshold& t1 ) { return t0.thresh >  t1.thresh; } 
bool operator==( const threshold& t0, const threshold& t1 ) { return t0.thresh == t1.thresh; } 
bool operator<=( const threshold& t0, const threshold& t1 ) { return t0.thresh <= t1.thresh; } 

std::ostream& operator<<( std::ostream& s, const threshold& t ) { 
  return s << t.chain << "\t" << t.key << "\t(" << t.thresh << ")\t" << t.counts;
}


int usage( int , char** argv, int i=0 ) { 
  if ( i!=0 ) std::cout << "error " << i << "\n\n"; 

  std::cout << "Usage: " << argv[0] << " FILE SLICE [OPTIONS] file\n\n";

  std::cout << "\t decodes which chains are available from a HIST file log\n";
  std::cout << "\t by default, sorts chains by threshold\n";

  std::cout << "Options:\n";
  std::cout << "   -all                     \t list all chains\n";
  std::cout << "   -cr, --return            \t separate chains with a return\n";
  std::cout << "   -c,  --counts            \t sort by counts rather than threshold\n";
  std::cout << "   -t,  --thresh value      \t require at least value entries - default is 100\n";
  std::cout << "   -n            value      \t report only n chains - default is 4\n\n";

  std::cout << "   -k            expression \t match any chain containing expression\n";
  std::cout << "   -K            expression \t match only chains containing expression\n";
  std::cout << "        --veto   expression \t veto all chains containing \"expression\"\n";
  std::cout << "   -x, --excluded           \t also include usually excluded entries\n\n";
  std::cout << "   -s, --sig     sig pat    \t add a new signature, sig, to the expected list with the pattern definied by pat\n";

  std::cout << "   -v,  --verbose           \t verbose output\n\n";
  std::cout << "   -h,  --help              \t this help\n" << std::endl;

  return i;
}


int main( int argc, char** argv ) {
  
  if ( argc<3 ) return usage( argc, argv, 1 );

  bool allflag = false;

  std::vector<std::string> keys;

  //  keys.push_back( "IDTrig" );
  bool nokeys = true;

  std::vector<std::string> Keys;
  bool noKeys = true;

  std::vector<std::string> veto;

  std::string creturn = "";

  bool verbose = false;
  bool usecounts = false;
  bool excluded  = false;

  unsigned n = 4;

  std::string  afile = "";
  std::string aslice = "";
  
  double userthreshold = 100;

  std::string sig="";
  std::string pat="";


  for ( int i=1 ; i<argc ; i++ ) { 
    std::string arg = argv[i];
    if      ( arg=="-all" ) allflag = true; 
    else if ( arg=="-v" || arg=="--verbose" ) verbose = true;
    else if ( arg=="-t" || arg=="--threshold" ) { 
      if ( ++i<argc ) 	userthreshold = std::atof(argv[i]);
      else            return usage( argc, argv, 1 );
    }
    else if ( arg=="-cr" )       creturn = "\n";
    else if ( arg=="-c" || arg=="--counts" )  usecounts = true;
    else if ( arg=="-k" ) {
      if ( ++i<argc ) {
	if ( nokeys ) { 
	  keys.clear();
	  nokeys = false;
	}
	keys.push_back(argv[i]);
      }
      else return usage( argc, argv, 2 );
    } 
    else if ( arg=="-K" ) {
      if ( ++i<argc ) {
	if ( noKeys ) { 
	  Keys.clear();
	  noKeys = false;
	}
	Keys.push_back(argv[i]);
      }
      else return usage( argc, argv, 3 );
    } 
    else if ( arg=="--veto" ) {
      if ( ++i<argc ) veto.push_back(argv[i]);
      else return usage( argc, argv, 4 );
    } 
    else if ( arg=="-n" ) {
      if ( ++i<argc ) n = std::atoi(argv[i]);
      else return usage( argc, argv, 5 );
    } 
    else if (  arg=="-h" || arg=="--help" )     return usage( argc, argv, 6 );
    else if (  arg=="-x" || arg=="--excluded" ) excluded = true;
    else if (  arg=="-s" || arg=="--sig" ) { 
      if ( ++i<argc ) sig = argv[i];
      else            return usage( argc, argv, 1 );
      if ( ++i<argc ) pat = argv[i];
      else            return usage( argc, argv, 1 );
    }
    else { 
      if      (  afile=="" )  afile=arg;
      else if ( aslice=="" ) aslice=arg;
      else  return usage( argc, argv, 7 );
    }
  }

  if ( afile=="" ) return usage( argc, argv, 8 );

  std::ifstream file( afile.c_str() );

  std::vector<std::vector<std::string> > block(1,std::vector<std::string>() );  

  std::string line;

  std::vector<int> position(1, 0);

  if ( aslice=="" ) return usage( argc, argv, 9 ); 

  std::string slice = "/" + aslice + "/";

  std::string rawslice = aslice;

  std::map<std::string, std::string> chains;

  typedef std::map<std::string, std::string> chain_map;
  chains.insert( chain_map::value_type( "Electron", "_e" ) );
  chains.insert( chain_map::value_type( "Egamma",   "_e" ) );
  chains.insert( chain_map::value_type( "Muon",     "_mu" ) );
  chains.insert( chain_map::value_type( "Tau",      "_tau" ) );
  chains.insert( chain_map::value_type( "Bjet",     "_j" ) );
  chains.insert( chain_map::value_type( "FTK",      "_FTK" ) );
  chains.insert( chain_map::value_type( "BjetVtx",  "" ) );

  chains.insert( chain_map::value_type( "EgammaPurity",   "_e" ) );
  chains.insert( chain_map::value_type( "MuonPurity",     "_mu" ) );
  chains.insert( chain_map::value_type( "TauPurity",      "_tau" ) );
  chains.insert( chain_map::value_type( "BjetPurity",     "_j" ) );
  chains.insert( chain_map::value_type( "FTKPurity",      "" ) );

  if ( sig != "" ) chains.insert( chain_map::value_type( sig, pat ) );

  chain_map::const_iterator itr = chains.find( rawslice );

  if ( itr==chains.end() ) { 
    std::cerr << "can not process type " << rawslice << std::endl;
    return -1;
  }
	
  std::string tag = "";
  if ( itr->second != "" ) { 
    tag = "HLT";
    tag += itr->second;
  }

  std::vector<threshold> thresholds;

  std::string fullpath = "";

  /// read through and parse chain list file

  bool purity = contains( slice, "Purity" );

  while( getline( file, line ) && !file.fail() ) {

    if ( !contains( line, slice ) ) continue;

    std::vector<std::string> expl = split( line );
    
    int it = 0;
    
    unsigned expected_size = 0;
    

    int index = 2;

    int diff = 0;

    if ( expl[0].find("run_")==std::string::npos ) index=diff=1;

    //    bool shifter = false;

    if      ( expl[index] == "IDMon" ) expected_size = 6-diff;
    else if ( expl[index] == "TRIDT" ) { 
      if      ( expl[index+2] == "Expert"  && !purity ) expected_size = 7-diff;
      else if ( expl[index+2] == "Shifter" && contains(expl[3],"etVtx") ) expected_size = 7-diff;
      else if ( expl[index+3] == "Fullscan" &&  purity ) expected_size = 7-diff;
      else if ( expl[index+2] == "Shifter" ||  purity ) expected_size = 6-diff;
      else { 
	std::cerr << "unknown HIST type " << expl[index+2] << std::endl;
	return 1;
      }
    }
    else { 
      std::cerr << "unknown HIST type " << expl[index] << " (2)" << std::endl;
      return 1;
    }
      
    if ( expl.size() > expected_size ) { 

      int counts = std::atof(expl[expected_size].c_str());

      //      for ( size_t ie=0 ; ie<expl.size() ; ie++ ) std::cout << ie << " " << expl[ie] << " / ";
      //      std::cout << "counts: " << counts << " (" << expected_size << ")" << std::endl;

      /// ignore chains with no entries ( could print out as problem chains if required)
      if ( counts >= userthreshold ) { 

	std::string tmp = expl[expected_size-2];

        if ( tag != "" ) { 
          if ( contains( tmp, tag ) ) tmp.replace( tmp.find(tag), tag.size(), "" ); 

          size_t pos = tmp.find("_");
          if ( pos!=std::string::npos ) tmp.replace( pos, tmp.size()-pos, "" );
        }

        bool condition = false;
          
	if ( keys.size()>0 ) for ( size_t ik=keys.size() ; ik-- ; ) condition |= contains( expl[expected_size-2], keys[ik] ) ||  contains( expl[expected_size-1], keys[ik] );
	else  condition = true;

	if ( Keys.size()>0 ) for ( size_t ik=Keys.size() ; ik-- ; ) condition &= contains( expl[expected_size-2], Keys[ik] ) ||  contains( expl[expected_size-1], Keys[ik] );
	
	if ( veto.size()>0 ) for ( size_t iv=veto.size() ;  iv-- ; )  condition &= !( contains( expl[expected_size-2], veto[iv] ) ||  contains( expl[expected_size-1], veto[iv] ) );

	if ( condition && ( excluded || ( !contains( expl[5], "EFID" )   && !contains( expl[4], "gsc" )   &&
                                          !contains( expl[4], "medium" ) && !contains( expl[4], "loose" ) && 
                                          !contains( expl[4], "2015" )   &&  !contains( expl[4], "_r1_" ) && 
                                          !( contains( expl[4], "_track" ) && !contains( expl[4], "tracktwo") )&&  
                                          !contains( expl[4], "L2Star" ) && !contains( expl[4], "Shifter" ) ) ) ) {  
	  
	  double thresh_var = std::atof( tmp.c_str() );
	  int    counts_var = counts;
	  
	  if ( usecounts ) { 
	    counts_var = int(thresh_var);
	    thresh_var = counts;
	  }

	  if ( expected_size >= 2 ) { 

	    threshold t( thresh_var, it++, expl[expected_size-2], expl[expected_size-1], counts_var );

	    if ( verbose && fullpath=="" ) for ( unsigned ip=0 ; ip<expected_size-2 ; ip++ )  fullpath += expl[ip]+"/";
	    
	    //    std::cout << "\tfullpath " << fullpath << std::endl;
	    //	  std::cout << "\t" << t << "\t" << contains( expl[4], "_track_" ) << std::endl;
	    //    std::cout << "\tadding: " << t << std::endl;
	    
	    thresholds.push_back( t );
	  }
	}
      }
    }
  }
  
  
  /// sort all the selected thresholds into order

  if ( usecounts ) sort( thresholds.rbegin(), thresholds.rend() );
  else             sort( thresholds.begin(), thresholds.end() );


  /// vector of indices into the threshold vector
 
  std::vector<int> id;
    

  
  if ( allflag ) { 
    
    // write out all thresholds

    for ( size_t i=0 ; i<thresholds.size() ; i++ ) id.push_back(i);

  }
  else if ( usecounts ) { 

    /// write out the top n in terms of numbers of events 

    for ( size_t i=0 ; i<thresholds.size() && i<n ; i++ ) id.push_back(i);

  }
  else { 
    
    // select (up to) 3 representative thresholds

    std::vector<unsigned> ind(n,0);

    if ( thresholds.size() == 0 ) { 
      std::cerr << "no valid thresholds found" << std::endl;
      return -2;
    }
    
    
    if ( rawslice=="Bjet" ) { 
      for ( size_t i=0 ; i<thresholds.size() ; i++ ) { 
	ind[0] = i;
	/// not sure what this is really doing now ....
	/// was originally for consistency with the 30 GeV vtx tracking 
	if ( thresholds[i].thresh > 30 ) break;   
      }
    }
    
    for ( size_t i=thresholds.size() ; i-- ;  ) { 
      //    if ( thresholds[i].counts > 0.05*thresholds[ind[0]].counts ) { 
      if ( thresholds[i].counts > 0.05*thresholds[ind[0]].counts ) { 
	ind[n-1] = i;
	break;
      } 
    } 
    
    /// calculate the intermediate chains with n options

    for ( unsigned it=1 ; n>0 && it<n-1 ; it++ ) {

      double tn = std::log(thresholds[ind[n-1]].thresh);
      double t0 = std::log(thresholds[ind[0]].thresh);
      
      double f = it*1.0/(n-1);

      double cthresh = std::exp( f*tn + (1-f)*t0 );

      //      double cthresh = std::exp( (it/(n-1))*std::log(thresholds[ind[n-1]].thresh) - (it/(n-1) - 1)*std::log(thresholds[ind[0]].thresh ) ;
    
      ind[it] = int( f*ind[n-1]+(1-f)*ind[0] ); 

      //      std::cout << "it " << it << "\tf " << f << "\tcthresh " << cthresh << "\tint " << ind[it] << "\tthresholds size " << thresholds.size() << std::endl;
    
      double delta = cthresh - thresholds[ind[it]].thresh;
      
      double R2 = delta*delta;
      
      for ( size_t i=ind[0]+1 ;  i<ind[n-1]-1 ; i++ ) { 
	double delta = cthresh - thresholds[i].thresh;
	
	double R2_ = delta*delta;
	
	if ( R2_ < R2 ) {
	  ind[it] = i;
	  R2 = R2_;
	}
	
      }
    }      

    id.push_back( ind[0] );
    

    for ( unsigned i=1 ; i<n ; i++ ) { 
      if ( ind[i]!=ind[i-1] ) id.push_back( ind[i] );
    }

    //    std::cout << "indices " << id << std::endl;

  }
    
  for ( size_t i=0 ; i<id.size() ; i++ ) { 
    if ( verbose )  std::cout << "  " << fullpath << thresholds[id[i]].chain << "/" << thresholds[id[i]].key << creturn;
    else            std::cout << "  " << thresholds[id[i]].chain << "_" << thresholds[id[i]].key << creturn;
  }
  
  return 0;
} 

