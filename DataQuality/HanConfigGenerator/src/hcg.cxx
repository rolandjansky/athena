//
//   @file    hcg.cxx
//            navigates through the directory structure of a monitoring 
//            root files and write out some han config boiler plate
//            
//   @author M.Sutton
// 
//   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: hcg.cxx  Sat 13 May 2017 15:07:09 CEST sutt$



#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <regex>

#include <cstdlib>
#include <cstdio>

#include <fstream>

#include <sys/stat.h>

#include "simpletimer.h"
#include "node.h"
#include "addnode.h"
#include "spacer.h"

#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TKey.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TFile.h"
#include "TClass.h"
#include "TDirectory.h"

#include "TPad.h"
#include "TStyle.h"

#include "utils.h"
#include "hmap.h"


/// file names and file pointers
std::vector<std::string> files;
std::vector<TFile*>      fptr;

std::vector<std::string> savedhistos;
std::vector<std::string> mapped;

bool   verbose = false; 
bool  vverbose = false; 
bool vvverbose = false; 

/// send output to here ...
std::ostream* outp = &std::cout;


bool allhists = true;

std::string base     = "HLT";

std::vector<std::string> subdirs;

std::string outref = "";

std::string algorithm = "HLT_Histogram_Not_Empty&GatherData";

/// glabal timer - how long have I taken so far?
//  struct timeval global_timer;


std::string description = "https://twiki.cern.ch/twiki/bin/view/Atlas/HltTrackingDataQualityMonitoring#Tier0";


/// list of directories to be explicitly remapped
std::map<std::string, std::string> remap;

/// return a remapped string
std::string find( const std::string& s ) { 
  std::map<std::string, std::string>::const_iterator itr = remap.find(s);
  if ( itr!=remap.end() ) return itr->second;
  else                    return s;
} 


/// list of directories to be excluded
std::set<std::string> exclude; 

/// list of directories to be explicitly included, together with 
/// corresponding depths of subdirectories
std::map<std::string, int> dirs;

std::vector<std::string> tags;


/// store any user histogram to algorithm mapping
hmap_t algorithms;

/// store any user histogram to description mapping
hmap_t descriptions;

/// store any user histogram to display mapping
hmap_t displays;


/// store the list of any directories to be wildcarded and 
/// the algorithms to use for them - use "hist=NULL"; if no 
/// algorithm is required
hmap_t  wildcards;



/// look in a histogram name map and return the regex mapped property name
/// goes up the tree of subdirectory name of additional 
/// specialisation for the histogram names all the way to the top if need be 
/// NB: take care, this is a recursive algorithm !!!
 
std::string match( const hmap_t& m, const node* n, const std::string& obj, const std::string& property ) {    
  /// don't do anything is the map is empty
  if ( m.size() && n )  { 
    hmap_t::const_iterator itr = m.match( obj );
    if ( itr!=m.end() ) return itr->second;
    else if ( n->parent() ) return match( m, n->parent(), n->parent()->name()+"/"+obj, property );
  }
  return property;
}



/// look in a histogram name map and return the mapped property name
/// goes up the tree of subdirectory names of additional 
/// specialisation for the histogram names all the way to the top if need be 
/// NB: take care, this is a recursive algorithm !!!

std::string find_internal( const hmap_t& m, const node* n, const std::string& obj, const std::string& property ) {  
  /// don't do anything if the map is empty
  if ( m.size() && n ) { 
    hmap_t::const_iterator itr = m.find( obj );
    if ( itr!=m.end() ) return itr->second;
    else if ( n->parent() )  return find_internal( m, n->parent(), n->parent()->name()+"/", property ); 
  }
  return property;
}



/// look for an exact match first, if you don't find one, search for a regular 
/// expression match  

std::string find( const hmap_t& m, const node* n, const std::string& obj, const std::string& property ) {  
  std::string _property = find_internal( m, n, obj, "FAIL" );
  if ( _property != "FAIL" ) return _property;
  return match( m, n, obj, property );
}



/// parse and individual line - must have the syntax: tag = "value";  

bool parse( const std::string& linein, histogram_name& tag, std::string& val, bool requirequotes=true ) {
  
  std::string line = linein;

  tag = "";
  val = "";

  remove( line, " " ); 
  if ( line.size()==0 ) return false;
  tag = chopto( line, " =" );
  remove( line, " " );  
  if ( !remove( line, "=" ) ) error( 1, std::cerr << "error : tag incorrectly specified\n\t" << linein ); 
  remove( line, " " ); 
  if ( requirequotes ) { 
    if ( !( line.size()>1 && (val += line[0])=="\"" ) ) error( 1, std::cerr << "error : incorrect value syntax - no opening quote\n\t" << linein );
    remove( line, "\"" );
    val += chopto( line, "\"" )+"\"";
    if ( !( line.size()>0 && line[0]=='"' ) )  error( 1, std::cerr << "error : incorrect value syntax - no closing quote\n\t" << linein ); 
    remove( line, "\"" );
  }
  else { 
    val += chopto( line, ";" );
  }
  remove( line, " " );
  if ( line.size()<1 || line[0]!=';' )  error( 1, std::cerr << "error : incorrect value syntax - line not correctly terminated\n\t" << linein );

  return true;
}




std::vector<std::string> read_lines( const std::string& filename ) { 

  std::fstream file( filename );
  
  std::vector<std::string> lines;
  

  /// add some padding at the beginning and end, extraxt file 
  /// contents to a more easily managed string 

  char c;
  std::string buffer = " ";  
  while ( file.get(c) ) buffer += c;
  buffer += " ";

  //  std::cout << "buffer: " << buffer << std::endl;

  /// remove all comments and line breaks

  int quotecount = 0;

  bool terminated = true;

  for ( unsigned i=1 ; i<buffer.size() ; i++ ) {

    /// check we are not still inside a quote 
    if ( buffer[i]=='"' ) quotecount++;

    /// remove comments
    if ( quotecount%2==0 && buffer[i]=='/' && buffer[i+1]=='/' ) {
      for ( unsigned j=i+2; j<buffer.size() ; j++, i++ ) if ( buffer[j]==10 ) { i++; break; }
    }
    //    else if ( quotecount%2==1 || buffer[i]>31 ) { 
    else if ( buffer[i]>31 ) { 
      /// check whether we have an end of line or not
      if ( quotecount%2!=0 || buffer[i]!=';' ) { 
	if ( terminated ) lines.push_back("");
	terminated = false;
	lines.back() += buffer[i];
      }
      else { 
	lines.back() += ";";
	terminated = true;
      }
    }
  }

  return lines;
}


std::vector<std::string> parse_wc( const std::string& filename ) { 
  
  std::vector<std::string> lines = read_lines( filename ); 

  std::vector<std::string> out;
  out.reserve( lines.size() );

  for ( unsigned i=0 ; i<lines.size() ; i++ ) {
    std::string line = lines[i];
    remove( line, " " );  
    std::string val = chopto( line, " ;" );
    if ( val.size() ) out.push_back( val );
  }
    
  return out;
}

hmap_t  parse( const std::string& filename, bool requirequotes=true ) {

  hmap_t       lookup;

  std::vector<std::string> lines = read_lines( filename );  

  /// now parse each line 
    
  for ( unsigned i=0 ; i<lines.size() ; i++ ) { 
    histogram_name     tag = "";
    std::string      value = "";
    if ( parse( lines[i], tag, value, requirequotes ) ) lookup.insert( hmap_t::value_type( tag, value ) );
  }

  return lookup;
}




std::vector<std::string> maphist( const std::vector<std::string>& v ) {   
  mapped.clear();
  for ( unsigned i=0 ; i<v.size() ; i++ ) { 
    if ( contains( v[i], "Expert" ) ){ 
      std::string tmp = v[i];
      std::string path = chop( tmp, "Expert/" );
      path += "Chains/";
      //      std::cerr << " " << v[i] << "\np:" << path << "\t-> " << tmp << std::endl;
      tmp = replace( tmp, "/", "__" );
      path += tmp;
      mapped.push_back( path );
      //     std::cerr << i << "\t" << mapped.back() << std::endl;
    }
    else { 
      mapped.push_back( v[i] );
    }
  }

  return mapped;
}





/// match the individual directories of two strings 
bool match( std::string s1, std::string s2 ) { 
  
  int i1 = count( s1, "/" );
  int i2 = count( s2, "/" );

  int i = ( i1<i2 ? i1 : i2 ); 

  //  std::cerr << "match s1 " << s1 << " " << s2 << std::endl;

  for ( i++ ; i-- ; ) { 
    size_t p1 = s1.find("/");
    size_t p2 = s2.find("/");

    std::string ss1 = s1.substr( 0, p1 );
    std::string ss2 = s2.substr( 0, p2 );
    
    s1.erase( 0, p1+1 );
    s2.erase( 0, p2+1 );

    //    std::cerr << i << "\tmatch :" << ss1 << ":" << ss2 << "::\t " << s1 << ":" << s2 << "\tresult " << (ss1!=ss2 ) << std::endl;
    
    if ( ss1!=ss2 ) return false;

  } 
  
  return true;

}


/// see whether this directory matches any of the directories we are 
/// explicitly being asked to match
 
bool matchdir( const std::string& s ) { 
  bool matched = false;
  // int idepth = count( s, "/" );
  std::map<std::string,int>::const_iterator itr = dirs.begin();
  while ( itr!=dirs.end() ) { 
    if ( match( s, itr->first) ) matched = true;
    //    std::cerr << "\tmatchdir :" << s << "::" << itr->first << ": " << itr->second << std::endl;
    if ( matched ) return true;
    itr++;
  }
  return false;
}

bool matchcwd( const std::string& s ) { 
  if ( dirs.size()==0 ) return true; 
  std::map<std::string,int>::const_iterator itr = dirs.begin();
  while ( itr!=dirs.end() ) { 
    if ( s.find(itr->first)!=std::string::npos ) return true;
    itr++;
  }
  return false;
}



std::string matchcwdstr( const std::string& s ) { 
  if ( dirs.size()==0 ) return ""; 
  std::map<std::string,int>::const_iterator itr = dirs.begin();
  while ( itr!=dirs.end() ) { 
    if ( s.find(itr->first)!=std::string::npos ) return itr->first;
    itr++;
  }
  return "";
}


std::map<std::string,int>::const_iterator matchcwditr( const std::string& s ) { 
  if ( dirs.size()==0 ) return dirs.end(); 
  std::map<std::string,int>::const_iterator itr = dirs.begin();
  while ( itr!=dirs.end() ) { 
    if ( s.find(itr->first)!=std::string::npos ) return itr;
    itr++;
  }
  return dirs.end();
}



class reference { 

public:

  reference( const std::string& n, const std::string& f ) : 
    mname(n), mfile(f) { 
    
    /// oh dear, find the run number from the specified file 

    std::cerr << "opening file " << f << std::endl; 

    TFile* r = TFile::Open(f.c_str());
    if ( r==0 ) { 
      std::cerr << "cannot open root file " << f << std::endl;
      std::exit(-1);
    }
    
    r->cd();

    TList* tl  = gDirectory->GetListOfKeys();
    
    /// go through sub directories
    
    for ( int i=0 ; i<tl->GetSize() ; i++ ) { 
      
      TKey* tobj = (TKey*)tl->At(i);
      
      if ( std::string(tobj->GetClassName()).find("TDirectory")!=std::string::npos ) { 
      //      (*outp) << ns << "Directory " << tobj->GetName() << std::endl;
      
	TDirectory* tnd = (TDirectory*)tobj->ReadObj();
	
	std::string dir = tnd->GetName();

	if ( contains( dir, "run_" ) ) {
	  dir.erase( 0, std::string( "run_").size() ); 
	  mrun = std::atoi( dir.c_str() );
	  break;
	}
       
      }
    }

    r->Close();
  } 


  reference( const reference& r ) : mname(r.mname), mfile(r.mfile), mrun(r.mrun) { } 

  
  std::string name() const { return mname; }
  std::string file() const { return mfile; }

  int run() const { return mrun; }

private:

  std::string mname;
  std::string mfile;

  int         mrun;

};



std::ostream& operator<<( std::ostream& s, const reference& r ) { 
  static bool first = true;
  if ( first ) { 
    s << "##########################\n";
    s << "# References\n";
    s << "##########################\n\n";
    first = false;
  }
  s << "reference "      << r.name() << " {\n";
  s << "\t\tfile = "     << r.file() << "\n";
  s << "\t\tpath = run_" << r.run()  << "\n";
  s << "\t\tname = same_name"        << "\n";
  s << "}\n\n";
  return s;
}



std::vector<reference> references;



class header { 

public:

  header( const std::string& configname="", const std::string& slicename="" ) { 

    std::string user = std::getenv("USER");

    (*outp) << "######################################################################\n";
    if ( configname=="" )  (*outp) << "# file  collisions_run.config " << date() << " " << user << "\n";
    else                   (*outp) << "# file  " << configname << "  " << date() << " " << user << "\n";
    (*outp) << "######################################################################\n";
    
    (*outp) << "\n";
    (*outp) << "#######################\n";
    if ( slicename=="" ) (*outp) << "# HLTidtrk\n";
    else                 (*outp) << "# " << slicename << "\n";
    (*outp) << "#######################\n";

    (*outp) << "\n\n";
  }

};


/// make the sidebar many part of the config

class menu { 

public: 
  
  menu( node& n ) { 

    (*outp) << "\n\n";
    (*outp) << "#######################\n";
    (*outp) << "# Output\n";
    (*outp) << "#######################\n\n\n";

    makemenu( n ); 

  }
 

  void makemenu( node& n, const std::string& space="", std::string path="", std::string rawpath="", bool found=false ) {
    
    bool print = false;
    
    if ( n.name()==base ) found = true;

    if ( n.type()==node::DIRECTORY ) print = found;
    if ( n.name()=="top_level" )     print = true;
    
    if ( n.size() ) { 
      
      /// always try to remap the name

      if ( exclude.find(n.name())!=exclude.end() )  { 
	print = false;
	return;
      }

      std::string newspacer = space;
      
      if ( print ) newspacer += spacer;
      
      std::string output_name = find(n.name());
      
      
      if ( print && n.type()==node::DIRECTORY ) { 
	if ( path=="" ) path += output_name;
	else            path += "/" + output_name;
	if ( rawpath=="" ) rawpath += n.name();
	else               rawpath += "/" + n.name();
      }

      //      std::cerr << "path " << path << "\tmatchdir " << matchdir( path ) << std::endl;

      if ( found && ( dirs.size() && (!matchdir( path ) && !matchdir( rawpath ) ) ) ) return;

      if ( print ) (*outp) << space << "output " << output_name << " {" << "\n"; // " \t\t(" << path << " size " << n.size() << ")\n";

      for ( unsigned i=0 ; i<n.size() ; i++ ) { 
	if ( n[i]->type()!=node::HISTOGRAM ) makemenu( *n[i], newspacer, path, rawpath, found ) ;
      }

      if ( print ) (*outp) << space << "}\n";
    }    
    
  }

};




/// make the histogram assessment part of the config

class ass { 

public: 
  
  ass( node& n, bool ah=true ) : mallhists(ah) { 
    (*outp) << "\n\n";
    (*outp) << "#######################\n";
    (*outp) << "# Histogram Assessments\n";
    (*outp) << "#######################\n\n";
    makeass( n ); 
  }
 

  void makeass( node& n, const std::string& space="", std::string path="", std::string rawpath="", bool found=false ) {
    
    static std::string savedir = "";

    bool print = false;
    
    if ( n.name()==base ) found = true;

    if ( n.type()==node::DIRECTORY ) print = found;
    ///   if ( n.name()=="top_level" )     print = true;
    
    if ( n.size() ) { 
      
      /// always try to remap the name
      
      std::string newspacer = space;
      
      if ( exclude.find(n.name())!=exclude.end() )  { 
	print = false;
	return;
      }

      //     if ( found && dirs.size() && dirs.find(n.name())==dirs.end() ) print = false;

      if ( print ) newspacer += spacer;
      
      std::string output_name = find(n.name());
            
      if ( print && n.type()==node::DIRECTORY ) { 
	if ( path=="" ) path += output_name;
	else            path += "/" + output_name;
	if ( rawpath=="" ) rawpath += n.name();
	else               rawpath += "/" + n.name();
      }

      if ( found && ( dirs.size() &&  !matchdir( path ) && !matchdir( rawpath ) ) ) return;

      if ( print ) (*outp) << space << "dir " << n.name() << " {" << "\n"; // " \t\t(" << path << ")\n";

      bool first_hists = true;

      for ( unsigned i=0 ; i<n.size() ; i++ ) { 
	if       ( n[i]->type()!=node::HISTOGRAM ) makeass( *n[i], newspacer, path, rawpath, found ) ;
	else if  ( n[i]->type()==node::HISTOGRAM ) { 

	  std::string allhst = "";
	  if ( n[i]->parent() ) allhst = find( wildcards, n[i]->parent(), n[i]->parent()->name(), "" ); 

	  //	  std::cerr << "allhst: " << n[i]->parent()->name() << "\tall: " << allhst << ":" << std::endl;

	  std::string algorithm_tmp = algorithm;

	  if ( !mallhists || allhst!="" ) { 
	    if ( first_hists ) {
	      if ( allhst!="" ) algorithm_tmp = allhst; 
	      (*outp) << space << "\t"   << "hist   all_in_dir {\n";
 	      if ( algorithm_tmp!="NULL" && algorithm_tmp!="0" ) (*outp) << space << "\t\t" << "algorithm   \t= " << algorithm_tmp << "\n";
	      (*outp) << space << "\t\t" << "description \t= " << description << "\n";
	      (*outp) << space << "\t\t" << "output      \t= " << path << "\n";
	      (*outp) << space << "\t\t" << "display     \t= StatBox\n";
	      /// extra user specified tags
	      for ( unsigned it=0 ; it<tags.size() ; it++ ) (*outp) << space << "\t\t" << replace(tags[it],"=","\t=") << "\n";
	      (*outp) << space << "\t"   << "}\n";
	    }
	    first_hists = false;
	  }
	  else { 
	    
	    std::string algorithm_tmp   = find( algorithms,   n[i], n[i]->name(), algorithm );
	    std::string description_tmp = find( descriptions, n[i], n[i]->name(), description );
	    std::string display_tmp     = find( displays,     n[i], n[i]->name(), "StatBox" );

	    if ( vverbose ) std::cout << "histogram: " << n[i]->path() << "\n";
	    (*outp) << space << "\t"   << "hist " << n[i]->name() << " {\n";
	    (*outp) << space << "\t\t" << "algorithm   \t= " << algorithm_tmp   << "\n";
	    (*outp) << space << "\t\t" << "description \t= " << description_tmp << "\n";
	    (*outp) << space << "\t\t" << "output      \t= " << path        << "\n";
	    (*outp) << space << "\t\t" << "display     \t= " << display_tmp << "\n";
	    /// extra user specified tags
	    for ( unsigned it=0 ; it<tags.size() ; it++ ) (*outp) << space << "\t\t" << replace(tags[it],"=","\t=") << "\n";
	    (*outp) << space << "\t"   << "}\n";

	    //      TH1* ase = (TH1*)(n[i]->object());
	    //	    if ( ase ) std::cerr << space << "\t\t" << "entries     = " << ase->GetEntries() << "\n";
	    //	    if ( ase ) std::cerr << space << "\t\t" << "entries     = \"" << ase->GetTitle()   << "\"\n";

	  }
	}
      }
      
      if ( print ) (*outp) << space << "}\n"; ///  \t\t##" << n.name() << "\n";
    }    
    
  }
  
private:

  bool mallhists;

};






bool found_dir = false;

std::string currentfile = "";


/// recursive directory search for TH1 and TH2 and TProfiles

void search( TDirectory* td, const std::string& s, std::string cwd, node* n ) { 

  /// not a directory 
  if ( td==0 ) return;

  if ( std::string(td->GetName()).find("_LB")!=std::string::npos ) return;
  if ( std::string(td->GetName()).find("lb_")!=std::string::npos ) return;

  if ( std::regex_match( std::string(td->GetName()), std::regex("run_.*") ) ) std::cerr << "run: " << td->GetName() << "\n";

  //  std::cout << "search() in  " << s << td->GetName() << ":    :" << cwd << ":" << std::endl;

  static int ir = 0;

  ir++;

  /// don't go more than 10 directories deep

  if ( ir>10 ) { 
    std::cerr << "search() WARNING too many levels of directories (max 10) !!!\n";
    return;
  }


  TDirectory* here = gDirectory;

  //  gDirectory->pwd();

  std::string basedir = here->GetName();

  td->cd();
  
  if ( cwd!="" ) cwd += "/";
  cwd += td->GetName();

  node* np = n;

  std::string fulldir = td->GetName();


  bool first_found = false;
  if ( !found_dir && matchcwd( cwd ) ) {

    std::string ase = matchcwdstr( cwd );

    if ( (cwd+"/").find( ase+"/" )!=std::string::npos ) { 
    
      found_dir   = true;
      first_found = true;
      
      std::cerr << "matched dirs " << cwd << " " << matchdir( cwd ) << std::endl;
      
      
      std::map<std::string,int>::const_iterator fitr = matchcwditr( cwd );
      
      if ( fitr!=dirs.end() ) { 
	
	if ( fitr->second>0 ) { 
	  
	  std::vector<std::string> subpath;
	  
	  std::string sp = fitr->first; 
	  
	  while( sp.size() ) subpath.push_back( chop(sp,"/") ); 
	  
	  for ( unsigned ip=0 ; ip<subpath.size()-1 ; ip++ ) { 
	    //	    std::cerr << "subpath " << ip << " " << subpath[ip] << std::endl;
	    
	    node* np_ = addnode( np, subpath[ip] );
	    
	    np = np_;
	    
	  }
	}
      }
      
    }
  }
  

  if ( found_dir ) {
    if ( basedir==base )  np->path( basedir );
    node* np_ = addnode( np, fulldir, td );
    np = np_;
  }

  if ( found_dir && verbose ) std::cerr << s << cwd << std::endl;


  TList* tl  = gDirectory->GetListOfKeys();
  
  //  struct timeval tv = simpletimer_start();
  
  //  (*outp) << "\ttl " << tl << std::endl;
  
  /// go through sub directories

  for ( int i=0 ; i<tl->GetSize() ; i++ ) { 
    
    TKey* tobj = (TKey*)tl->At(i);
    
    if ( tobj==0 ) continue;
    
    //    (*outp) << "tobj " << tobj;
    //    if ( tobj ) (*outp) << " : \t" << tobj->GetName();
    //    (*outp) << std::endl; 
    
    if ( std::string(tobj->GetClassName()).find("TDirectory")!=std::string::npos ) { 
      //      (*outp) << ns << "Directory " << tobj->GetName() << std::endl;
      
      TDirectory* tnd = (TDirectory*)tobj->ReadObj();
      

      /// descend into the subdirectory ... 
      if ( tnd )  search( tnd, s+spacer, cwd, np );
	
    }
    else { 

      /// if this is a directory we want, print (*outp) the histograms 

      if ( found_dir ) { 
	if ( std::string(tobj->GetClassName()).find("TH1")!=std::string::npos ||
	     std::string(tobj->GetClassName()).find("TH2")!=std::string::npos ||
	     std::string(tobj->GetClassName()).find("TProfile")!=std::string::npos ) {
	  
	  //	  TH1* th = (TH1*)tobj->ReadObj();

	  //	  node* h = addnode( np, "", get<TObject>(tobj), node::HISTOGRAM );
	  // node* h = 
	  addnode( np, tobj->GetName(), get<TObject>(tobj), node::HISTOGRAM );
	  

	  /// get the full path to this object path relative to the file	  
	  std::string subdir = cwd;
	  chop( subdir, currentfile+"/" );

	  /// save the histograms in case we need to save the, later ...
	  savedhistos.push_back( subdir+"/"+tobj->GetName() );

	  /// keep the max number of entries updated
	  if ( std::string(tobj->GetName())=="Chain" ) { 
	    double N = ((TH1*)get<TObject>(tobj))->GetBinContent(1);

	    //	    std::cout << "entries " << np->name() << " " << " " << np->parent()->name() << " " << N << std::endl;
	    //    std::cout << "\tentries " << np->parent()->name() << "/" << np->name() << "\t" << N << std::endl;
	    std::cout << "\t" << subdir << "\t" << N << std::endl;

	    node* p  = np->parent();
	    if ( p && p->name()!="Shifter" ) { 

	      p->addrate( p->name(), N );

	      node* p2 = p->parent();
	      if ( p2 ) p2->addrate( p->name(), N ); 
	    }
	  }

	}
	
	//      if ( !status ) std::cerr << "bad status" << std::endl;
      }
    }
    
  }
  

  //  double _t = simpletimer_stop(tv);
  
  //  double global_time = simpletimer_stop(global_timer);
    
  ir--;
    
  here->cd();
  
  if ( first_found ) found_dir = false;

}







std::vector<int> lumiblockrange( TDirectory* idir, int depth=0 ) {
  
  std::string dir = idir->GetName();
  
  if ( depth>1 || ( depth>0 && !contains( dir, "run_" ) ) ) return std::vector<int>(0);
  
  TList* tl  = gDirectory->GetListOfKeys();
  
  /// go through sub directories                                                                                                                                                                         
  
  std::vector<int> limits;
  
  for ( int i=0 ; i<tl->GetSize() ; i++ ) {
    
    TKey* tobj = (TKey*)tl->At(i);
    
    if ( std::string(tobj->GetClassName()).find("TDirectory")!=std::string::npos ) {
      
      TDirectory* tnd = (TDirectory*)tobj->ReadObj();
      
      std::string tdir = tnd->GetName();
      
      if ( contains( tdir, "run_" ) ) { 
        tnd->cd();
        return lumiblockrange( tnd, depth+1 );
      }
      else { 
        
        /// search through for lumi block directories 
        
        if ( contains( tdir, "lb_" ) ) { 
          
          if ( limits.size()<2 ) { 
            limits.resize(2);
            limits[0] = limits[1] = 0;
          }

          int lb = std::atoi( tdir.erase( 0, std::string( "lb_").size() ).c_str()); 
          if ( limits[0]==0 || ( limits[0]!=0 && lb<limits[0] ) ) limits[0] = lb;
          if ( lb>limits[1] ) limits[1] = lb;

        }
      }
      
    }
  }  

  idir->cd();
  return limits;
}






int cost( std::vector<std::string>& files, node& n, const std::string& directory="", bool deleteref=false, bool relocate=false  ) { 

  std::cerr << "processing ..." << std::endl;

  //  std::cerr << "opening root files" << std::endl;

  fptr.resize(files.size());


  for ( unsigned i=0 ; i<files.size() ; i++ ) { 

    currentfile = files[i];

    std::cerr << "opening " << currentfile << std::endl;

    if ( !contains( files[i], "root://eosatlas") && !file_exists( files[i] ) ){ 
      std::cerr << "file " << files[i] << " does not exist" << std::endl;
      return -1;	
    }
    
    /// open the output file
    fptr[i] = TFile::Open( files[i].c_str() );
    
    if ( fptr[i]==0 || fptr[i]->IsZombie() ) { 
      std::cerr << "file " << files[i] << " cannot be opened" << std::endl;
      return -1;
    }
  
    fptr[i]->cd();

    TDirectory* cwd = gDirectory;
    std::vector<int> lbrange = lumiblockrange( gDirectory );

    if ( lbrange.size()>1 ) std::cout << "lumiblockrange: blocks: " << lbrange[0] << "  " << lbrange[1] << std::endl; 
    cwd->cd();


    if ( directory!="" ) fptr[i]->cd(directory.c_str());    

    TDirectory* here = gDirectory;
    
    //    global_timer = simpletimer_start();

    //    int tcount = 0;

    /// navigate the directory structure to 
    /// extract all the info

    search( gDirectory, "", "", &n );

    here->cd();
    

    /// if asked to delete the unused referenece then do so
    /// if however asked to relocate the histgrams, simply 
    /// modify the list of histogram destinations first
    if ( deleteref || relocate ) { 

      std::cerr << "remapping" << std::endl;

      if ( relocate ) mapped = maphist( savedhistos );
      
      if ( relocate && !deleteref ) std::cerr << "saving histograms to file .newhist.root ... " << std::endl;

      if ( outref=="" ) outref = ".newhist.root";
      TFile* fnew = new TFile( outref.c_str(), "recreate" );
      fnew->cd();

      TDirectory*  based = gDirectory;

      if ( mapped.size() != savedhistos.size() ) mapped = savedhistos;

      for ( unsigned ih=0 ; ih<savedhistos.size() ; ih++ ) { 
	
	std::vector<std::string> ldirs = split( mapped[ih], "/" );

	for ( unsigned jh=0 ; jh<ldirs.size()-1 ; jh++ ) { 
	  TDirectory* renedir = gDirectory->GetDirectory( ldirs[jh].c_str() );
	  if ( renedir==0 ) gDirectory->mkdir( ldirs[jh].c_str() );
	  gDirectory->cd( ldirs[jh].c_str() );
	}
	
	TH1* href  = (TH1*)fptr[i]->Get( savedhistos[ih].c_str() );
	if ( href ) {
	  //	  std::cerr << ih << " " << savedhistos[ih] << " 0x" << href << std::endl;
	  href->Write( ldirs.back().c_str() );
	}

	based->cd();
      }


    }

    std::cerr << "closing files" << std::endl; 

    /// why, why, why, why, why-oh-why does root take such a 
    /// long time to close the file without this ?
    gROOT->GetListOfFiles()->Remove(fptr[i]);
    fptr[i]->Close();

    delete fptr[i];

    if ( deleteref ) { 
      if ( outref==".newhist.root" ) { 
	std::cerr << "replacing histogram file" << std::endl;
	std::string cmd = std::string("mv ") + files[i] + " " + files[i] + ".bak";
	std::system( cmd.c_str() );
	cmd = std::string("mv .newhist.root ") + files[i];
	std::system( cmd.c_str() );
      }
    }
    
  }

  return 0;
}





int usage(std::ostream& s, int , char** argv, int status=-1) { 
  s << "Usage: " << argv[0] << " [OPTIONS] input1.root ... inputN.root\n\n";
  s << "Options:\n";
  s << "    -o                FILENAME  \tname of output (filename required)\n";
  s << "    -b,   --base      DIR       \tuse directory DIR as the base for the han config\n";
  s << "    -d,   --dir       DIR       \tonly directories below DIR where DIR is a structure such as HLT/TRIDT etc\n";
  s << "    -x,   --exclude   DIR       \texclude directory DIR\n";
  s << "    -s,   --slice     SLICE     \ttrigger signature name (for comments)\n"; 
  s << "    -r,   --remap     SRC DST   \tremap directory SRC to directory DST\n"; 
  s << "    -a,   --algorithm VALUE     \tuse VALUE as the execution algorithm for each histogram\n";
  s << "    -af,  --algfile   FILENAME  \tread algorithm information from FILENAME\n";
  s << "    -ds,  --desc      DESCRIP   \tuse DESCRIP as the description\n"; 
  s << "    -df,  --descfile  FILENAME  \tread descriptions from FILENAME\n"; 
  s << "    -dp,  --dispfile  FILENAME  \tread display information from FILENAME\n"; 
  s << "    -t,   --tag       VALUE     \tadd the VALUE to the list of command per histogram\n";
  s << "    -wc,  --wildcard            \tprint use hist * rather than a separate entry for each histogram\n";
  s << "    -wf,  --wcfile    FILENAME  \tread list of directories to wildcard from a file\n";
  s << "    -wd,  --wcdir     DIR=ALG   \tadd wildcard for DIRECTORY using ALG for the algorithm\n";
  s << "    -dr,  --deleteref           \tdelete unselected histograms\n";
  s << "    -or,  --outref   FILENAME   \tfile to write reduced output to (overwrites input otherwise) \n";
  s << "    -rh,  --relocate            \trelocate selected histograms\n";
  s << "    -ref, --reference TAG FILE  \tadd FILE as a reference file with tag TAG\n";
  s << "    -rc,  --refconf       FILE  \tadd FILE to the config as a reference block\n";
  s << "    -v,   --verbose             \tprint verbose output\n";
  s << "    -vv,  --vverbose            \tprint very verbose output\n";
  s << "    -vvv, --vvverbose           \tprint very, very verbose output\n";
  s << "    -h,   --help                \tdisplay this help\n";
  s << std::endl;
  return status;
}


std::vector<std::string> refblock;

void referenceblock( const std::string& file ) { 
  std::ifstream strm(file.c_str());
  for ( std::string line ; getline( strm, line ); ) refblock.push_back( line );
} 



int main(int argc, char** argv) { 

  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadTopMargin(0.075);


  TCanvas* tg = new TCanvas("tg", "tg", 700, 600 );
  tg->cd();

  gStyle->SetStatY(0.4);                
  // Set y-position (fraction of pad size)
  gStyle->SetStatX(0.89);                
  // Set x-position (fraction of pad size)
  gStyle->SetStatW(0.25);                
  // Set width of stat-box (fraction of pad size)
  gStyle->SetStatH(0.16);      


  if ( argc<2 ) return usage( std::cerr, argc, argv );

  /// handle the help message option before dealing with
  /// any other arguments 

  for ( int i=1 ; i<argc ; i++ ) { 
    if ( std::string(argv[i])=="-h" || std::string(argv[i])=="--help" )  return usage( *outp, argc, argv, 0 ); 
  }

  /// now properly parse cmdline options and configure
  
  std::string dir = "";

  std::string              wildcardfile;


  bool deleteref = false;
  bool relocate  = false;

  std::string outfile = "";
  std::string   slice = "";

  std::string descriptionfile = "";
  std::string displayfile     = "";
  std::string algfile         = "";


  int offset = 1;

  bool basechanged = false;

  for ( int i=1 ; i<argc ; i++ ) { 

    std::string argvi = std::string(argv[i]);

    if      ( argvi=="-v"   || argvi=="--verbose"  )  verbose = true;
    else if ( argvi=="-vv"  || argvi=="--vverbose" )  verbose = vverbose = true;
    else if ( argvi=="-vvv" || argvi=="--vvverbose" ) verbose = vverbose = vvverbose = true;
    else if ( argvi=="-o" ) {
      if ( ++i<argc-offset ) outfile = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-or" || argvi=="--outref" ) {
      if ( ++i<argc-offset ) outref = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-ref" || argvi=="--reference" ) {
      std::string reftag;
      std::string reffile;
      if ( ++i<argc-offset ) reftag = argv[i];
      else  return usage( std::cerr, argc, argv );
      if ( ++i<argc-offset ) reffile = argv[i];
      else  return usage( std::cerr, argc, argv );
      references.push_back( reference( reftag, reffile ) ); 
      //      std::cerr << references.back() << std::endl;
    } 
    else if ( argvi=="-rc" || argvi=="--refconf" ) {
      if ( ++i<argc-offset ) referenceblock( argv[i] );
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-s" || argvi=="--slice" ) {
      if ( ++i<argc-offset ) slice = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-dr"  || argvi=="--deleteref" ) deleteref = true;
    else if ( argvi=="-rh"  || argvi=="--relocate" )  relocate  = true;
    else if ( argvi=="-wc"  || argvi=="--wildcard"  )  allhists = false;
    else if ( argvi=="-d"   || argvi=="--dir"       ) {
      if ( ++i<argc-offset ) { 
	  dirs.insert( std::map<std::string,int>::value_type( argv[i], count( argv[i], "/" ) ) );
	  std::string tdir = argv[i];
	  do { 
	    subdirs.push_back( chop( tdir, "/" ) );
	  } 
	  while ( tdir.size() );
	  if ( !basechanged && subdirs.size() ) base = subdirs[0];
      }
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-x" || argvi=="--exclude" ) {
      if ( ++i<argc-offset ) exclude.insert( argv[i] );
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-ds" || argvi.find("--desc")==0 ) {
      if ( ++i<argc-offset ) description = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-df" || argvi.find("--descfile")==0 ) {
      if ( ++i<argc-offset ) descriptionfile = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-af" || argvi.find("--algfile")==0 ) {
      if ( ++i<argc-offset ) algfile = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-dp" || argvi.find("--dispfile")==0 ) {
      if ( ++i<argc-offset ) displayfile = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-wd" || argvi.find("--wcdir")==0 ) {
      if ( ++i<argc-offset ) { 
	histogram_name     tag="";
	std::string      value="";
	/// parse the input string - add a terminating ";" just in case 
	if ( parse( std::string(argv[i])+";", tag, value, false ) ) wildcards.insert( hmap_t::value_type( tag, value ) );
	else  return usage( std::cerr << "Could not parse wildcard directory", argc, argv );
      }
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-wf" || argvi.find("--wcfile")==0 ) {
      if ( ++i<argc-offset ) wildcardfile = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-dp" || argvi.find("--dispfile")==0 ) {
      if ( ++i<argc-offset ) displayfile = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-b" || argvi=="--base" ) {
      if ( ++i<argc-offset ) { 
	base = argv[i] ;
	basechanged = true;
      }
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-a" || argvi=="--algorithm" ) {
      if ( ++i<argc-offset ) algorithm = argv[i] ;
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-t" || argvi=="--tag" ) {
      if ( ++i<argc-offset ) tags.push_back( argv[i] );
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( argvi=="-r" || argvi=="--remap" ) { 
      std::string src;
      std::string dest;
      if ( i<argc+2-offset ) { 
	src  = argv[++i];
	dest = argv[++i];
	remap.insert( std::map<std::string,std::string>::value_type( src, dest ) );
      } else return usage( std::cerr, argc, argv );
    }
    else { 
      offset = 0;
      files.push_back(argv[i]);
    }
  }


  //  std::cout << "tags " << tags.size() << " " << tags << std::endl;

  if ( base == "" ) base = dir;

  /// check some input files

  if ( files.size()<1 ) return usage( std::cerr, argc, argv );
  

  /// get algorithm information from a file if required 

  if ( algfile!="" ) { 
    std::cerr << "reading algorithm information from : \t" << algfile << std::endl; 
    if ( !file_exists( algfile ) ) error( 1, std::cerr << "algorithm file " << algfile << " does not esist" ); 
    algorithms = parse( algfile, false );
  }


  /// get descriptions from a file if required

  if ( descriptionfile!="" ) { 
    std::cerr << "reading decriptions from : \t\t" << descriptionfile << std::endl; 
    if ( !file_exists( descriptionfile ) ) error( 1, std::cerr << "decription file " << descriptionfile << " does not esist" ); 
    descriptions = parse( descriptionfile );
  }


  /// get display information from file if required

  if ( displayfile!="" ) { 
    std::cerr << "reading display information from : \t" << displayfile << std::endl; 
    if ( !file_exists( displayfile ) ) error( 1, std::cerr << "display file " << displayfile << " does not esist" ); 
    displays = parse( displayfile, false );
  }


  /// parse the wildcard directories 

  if ( wildcardfile!="" ) { 
    std::cerr << "reading wildcard information from : \t" << wildcardfile << std::endl; 
    if ( !file_exists( wildcardfile ) ) error( 1, std::cerr << "wildcard file " << wildcardfile << " does not esist" ); 
    hmap_t _wildcards = parse( wildcardfile, false );
    wildcards.insert(  _wildcards.begin(), _wildcards.end() );
  }

  //  for ( hmap_t::const_iterator itr = wildcards.begin() ; itr!=wildcards.end() ; itr++ ) {
  //     std::cout << "wildcard: " << itr->first << " " << itr->second << std::endl;
  // }


  //  time the actual running of the cost() routine
  
  if ( verbose ) std::cerr << "timing" << std::endl;

  struct timeval tv = simpletimer_start();

  /// create the structure ...

  node n(0, "");
  n.name( "top_level" );

  //  if ( verbose ) std::cout << "run: " << mrun << " " << dir << "\n"; 

  int status = cost( files, n, "", deleteref, relocate );

  if ( vvverbose )  travel( &n );

  //  std::cerr << "\n\nnodes " << n << std::endl;

  if ( status ) return status;

  if ( outfile!="" ) outp = new std::ofstream(outfile.c_str()); 

  header h( outfile, slice );

  for ( unsigned ir=0 ; ir<references.size() ; ir++ ) (*outp) << references[ir] << std::endl; 

  if ( refblock.size() ) (*outp) << refblock << std::endl;

  /// create the side bar menu part 
  menu m( n );

  /// create the histogram assessment part
  ass( n, allhists );

  /// ensure that buffer is properly flushed
  (*outp) << std::endl;

  double t = simpletimer_stop(tv);
  
  if ( t>1000 ) std::cerr << "total time " << t*0.001 << " s"  << std::endl;
  else          std::cerr << "total time " << t       << " ms" << std::endl;

  return 0;
}
