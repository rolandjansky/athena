//
//   @file    hanconfig.cxx         
//            navigates through the directory structure of a monitoring 
//            root files and write out some han config boiler plate
//            
//   @author M.Sutton
// 
//      
//


#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

#include <cstdlib>
#include <cstdio>

#include <fstream>

#include <sys/stat.h>

#include "simpletimer.h"
#include "node.h"
#include "addnode.h"
#include "spacer.h"

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



/// file names and file pointers
std::vector<std::string> files;
std::vector<TFile*>      fptr;

std::vector<std::string> savedhistos;
std::vector<std::string> mapped;


/// sadly, includes a return at the end 
std::string date() { 
  time_t t;
  time(&t);
  std::string a = ctime(&t);
  std::string b = "";
  for ( unsigned i=0 ; i<a.size()-1 ; i++ ) b+=a[i];
  return b;
}


bool file_exists( const std::string& file ) { 
  struct stat buff;
  return stat(file.c_str(),&buff)==0;   
}

bool verbose = false; 

/// send output to here ...
std::ostream* outp = &std::cout;


bool allhists = true;

std::string base     = "HLT";

std::string outref = "";

std::string algorithm = "HLT_Histogram_Not_Empty&GatherData";

/// glabal timer - how long have I taken so far?
//  struct timeval global_timer;


std::string description = "https://twiki.cern.ch/twiki/bin/view/Atlas/HltTrackingDataQualityMonitoring#Tier0";


/// list of directories to be explicitly remapped
std::map<std::string, std::string> remap;

/// list of directories to be excluded
std::set<std::string> exclude; 

/// list of directories to be explicitly included, together with 
/// corresponding depths of subdirectories
std::map<std::string, int> dirs;


std::vector<std::string> tags;

template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T>& v ) {
  if ( v.empty() ) return s;
  for ( size_t i=0 ; i<v.size() ; i++ ) s << v[i] << "\n";
  return s;
}

bool contains( const std::string& s, const std::string& regx ) { return s.find(regx)!=std::string::npos; } 



template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T*>& v ) {
  if ( v.empty() ) return s;
  for ( int i=0 ; i<v.size() ; i++ ) s << *v[i] << "\n";
  return s;
}



/// get a TObject* from a TKey* 
/// (why can't a TObject be a TKey?)
template<class T>
T* get( TKey* tobj ) { 
  TObject* a = tobj->ReadObj()->Clone();
  ((TH1*)a)->SetDirectory(0);
  return (T*)a;
}


/// return a remapped string
std::string find( const std::string& s ) { 
  std::map<std::string, std::string>::const_iterator itr = remap.find(s);
  if ( itr!=remap.end() ) return itr->second;
  else                    return s;
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


std::vector<std::string> split( const std::string& s, const std::string& t=":"  ) {
    
    std::string s2 = s;
    size_t pos = s2.find(t);
    
    std::vector<std::string> tags;
    
    while ( pos!=std::string::npos ) { 
      tags.push_back( chop(s2,t) );
      pos = s2.find(t);
    }
    
    tags.push_back(s2);
    
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
std::string replace( std::string s, const std::string& s2, const std::string& s3) {
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
void depunctuate(std::string& s, const std::string& regex=":") 
{
  std::string::size_type pos;
  while ( (pos = s.find(regex))!=std::string::npos ) {
    s.erase(pos, regex.size());
  }
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
    m_name(n), m_file(f) { 
    
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
	  m_run = std::atoi( dir.c_str() );
	  
	  break;
	}
       
      }
    }

    r->Close();
  } 


  reference( const reference& r ) : m_name(r.m_name), m_file(r.m_file), m_run(r.m_run) { } 

  
  std::string name() const { return m_name; }
  std::string file() const { return m_file; }

  int run() const { return m_run; }

private:

  std::string m_name;
  std::string m_file;

  int         m_run;

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

  header( ) { 

    std::string user = std::getenv("USER");

    (*outp) << "######################################################################\n";
    (*outp) << "# $Id: collisions_run.config " << date() << " " << user << " $\n";
    (*outp) << "######################################################################\n";
    
    (*outp) << "\n";
    (*outp) << "#######################\n";
    (*outp) << "# HLTidtrk\n";
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

      //      bool exclude_dir = false;

      if ( exclude.find(n.name())!=exclude.end() )  { 
	print = false;
	//	exclude_dir = true;
	return;
      }

      //      if ( found && ( dirs.size() && dirs.find(n.name())==dirs.end() ) ) print = false;
   
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
  
  ass( node& n, bool ah=true ) : m_allhists(ah) { 
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
	  if ( !m_allhists ) { 
	    if ( first_hists ) {
	      (*outp) << space << "\t"   << "hist .* {\n";
	      (*outp) << space << "\t\t" << "regex       \t= 1\n";
	      (*outp) << space << "\t\t" << "algorithm   \t= " << algorithm << "\n";
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
	    (*outp) << space << "\t"   << "hist " << n[i]->name() << " {\n";
	    (*outp) << space << "\t\t" << "algorithm   \t= " << algorithm << "\n";
	    (*outp) << space << "\t\t" << "description \t= " << description << "\n";
	    (*outp) << space << "\t\t" << "output      \t= " << path << "\n";
	    (*outp) << space << "\t\t" << "display     \t= StatBox\n";
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

  bool m_allhists;

};






bool found_dir = false;

std::string currentfile = "";


/// recursive directory search for TH1 and TH2 and TProfiles

void search( TDirectory* td, const std::string& s, std::string cwd, node* n ) { 

  /// not a directory 
  if ( td==0 ) return;

  if ( std::string(td->GetName()).find("_LB")!=std::string::npos ) return;
  if ( std::string(td->GetName()).find("lb_")!=std::string::npos ) return;

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
	    double N = ((TH1*)get<TObject>(tobj))->GetEntries();

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

    if ( directory!="" ) fptr[i]->cd(directory.c_str());    

    TDirectory* here = gDirectory;
    
    //    global_timer = simpletimer_start();

    //    int tcount = 0;

    /// navigate the directory structure to 
    /// extracting all the info

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

      TDirectory*  base = gDirectory;

      if ( mapped.size() != savedhistos.size() ) mapped = savedhistos;

      for ( unsigned ih=0 ; ih<savedhistos.size() ; ih++ ) { 
	
	std::vector<std::string> dirs = split( mapped[ih], "/" );

	for ( unsigned jh=0 ; jh<dirs.size()-1 ; jh++ ) { 
	  /// std::cerr << "\t" << dirs[jh] << std::endl;
	  TDirectory* renedir = gDirectory->GetDirectory( dirs[jh].c_str() );
	  if ( renedir==0 ) gDirectory->mkdir( dirs[jh].c_str() );
	  gDirectory->cd( dirs[jh].c_str() );
	}
	
	TH1* href  = (TH1*)fptr[i]->Get( savedhistos[ih].c_str() );
	if ( href ) {
	  //	  std::cerr << ih << " " << savedhistos[ih] << " 0x" << href << std::endl;
	  href->Write( dirs.back().c_str() );
	}

	base->cd();
      }


    }

    std::cerr << "closing files" << std::endl; 
        
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
  s << "    -o             FILENAME  \tname of output (filename required)\n";
  s << "    -b,   --base   DIR       \tuse directory DIR as the base for the han config\n";
  s << "    -d,   --dir    DIR       \tonly directories below DIR where DIR is a structure such as HLT/TRIDT etc\n";
  s << "    -x,            DIR       \texclude directory DIR\n";
  s << "    -r             SRC DST   \tremap directory SRC to directory DST\n"; 
  s << "    -ds,  --desc   DESCRIP   \tuse DESCRIP as the description\n"; 
  s << "    -t,   --tag    VALUE     \tadd the VALUE to the list of command per histogram\n";
  s << "    -a,   --algorithm VALUE  \tuse VALUE as the execution algorithm for each histogram\n";
  s << "    -wc,  --wildcard         \tprint use hist * rather than a separate entry for each histogram\n";
  s << "    -dr,  --deleteref        \tdelete unselected histograms\n";
  s << "    -or,  --outref FILENAME  \tdelete file to write reduced output to (overwrites input otherwise) \n";
  s << "    -rh,  --relocate         \trelocate selected histograms\n";
  s << "    -ref, --reference TAG FILE \tadd FILE as a reference file with tag TAG\n";
  s << "    -rc,  --refconf       FILE \tadd FILE to the config as a reference block\n";
  s << "    -v,   --verbose          \tprint verbose output\n";
  s << "    -h,   --help             \tdisplay this help\n";
  s << std::endl;
  return status;
}


std::vector<std::string> refblock;

void referenceblock( const std::string& file ) { 
  std::ifstream strm(file.c_str());
  for ( std::string line ; getline( strm, line ); ) refblock.push_back( line );
} 


int main(int argc, char** argv) { 

  //  std::string cock = "HLT_j150_bperf_split/InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF_SuperRoi/Chain"; 

  //  replace 

  //  std::cout << replace 

  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadTopMargin(0.075);

  //  TCanvas* tg = new TCanvas("tg", "tg", 650, 900 );
  TCanvas* tg = new TCanvas("tg", "tg", 700, 600 );
  tg->cd();

  gStyle->SetStatY(0.4);                
  // Set y-position (fraction of pad size)
  gStyle->SetStatX(0.89);                
  // Set x-position (fraction of pad size)
  gStyle->SetStatW(0.25);                
  // Set width of stat-box (fraction of pad size)
  gStyle->SetStatH(0.16);      


  //  if ( argc<3 ) usage( std::cerr << "not enough command options", argc, argv );
  if ( argc<2 ) return usage( std::cerr, argc, argv );


  for ( int i=1 ; i<argc ; i++ ) { 
    if ( std::string(argv[i])=="-h" || std::string(argv[i])=="--help" )  return usage( *outp, argc, argv, 0 ); 
  }
  
  std::string dir = "";

  std::vector<std::string> subdirs;


  bool deleteref = false;
  bool relocate  = false;

  std::string outfile = "";

  int offset = 1;


  for ( int i=1 ; i<argc ; i++ ) { 
    if      ( std::string(argv[i])=="-v" || std::string(argv[i])=="--verbose" ) verbose = true;
    else if ( std::string(argv[i])=="-o" ) {
      ++i;
      if ( i<argc-offset ) outfile = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( std::string(argv[i])=="-or" || std::string(argv[i])=="--outrefr" ) {
      ++i;
      if ( i<argc-offset ) outref = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( std::string(argv[i])=="-ref" || std::string(argv[i])=="--reference" ) {
      std::string reftag;
      std::string reffile;
      ++i;
      if ( i<argc-offset ) reftag = argv[i];
      else  return usage( std::cerr, argc, argv );
      ++i;
      if ( i<argc-offset ) reffile = argv[i];
      else  return usage( std::cerr, argc, argv );
      references.push_back( reference( reftag, reffile ) ); 
      //      std::cerr << references.back() << std::endl;
    } 
    else if ( std::string(argv[i])=="-rc" || std::string(argv[i])=="-refconf" ) {
      ++i;
      if ( i<argc-offset ) referenceblock( argv[i] );
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( std::string(argv[i])=="-dr"  || std::string(argv[i])=="--deleteref" ) deleteref = true;
    else if ( std::string(argv[i])=="-rh"  || std::string(argv[i])=="--relocate" )  relocate  = true;
    else if ( std::string(argv[i])=="-wc"  || std::string(argv[i])=="--wildcard"  )  allhists = false;
    else if ( std::string(argv[i])=="-d"   || std::string(argv[i])=="--dir"       ) {
      ++i;
      
      if ( i<argc-offset ) { 
	  dirs.insert( std::map<std::string,int>::value_type( argv[i], count( argv[i], "/" ) ) );
	  
	  std::string tdir = argv[i];
	  
	  //	  std::cerr << "dirs " << argv[i] << std::endl;
	  
	  do { 
	    subdirs.push_back( chop( tdir, "/" ) );
	    //   std::cerr << "chop  " << subdirs.back() << std::endl;
	  }
	  while ( tdir.size() ); 
      }
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( std::string(argv[i])=="-x" ) {
      ++i;
      if ( i<argc-offset ) exclude.insert( argv[i] );
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( std::string(argv[i])=="-ds" || std::string(argv[i]).find("--desc")==0 ) {
      ++i;
      if ( i<argc-offset ) description = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( std::string(argv[i])=="-b" || std::string(argv[i])=="--base" ) {
      ++i;
      if ( i<argc-offset ) base = argv[i] ;
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( std::string(argv[i])=="-a" || std::string(argv[i])=="--algorithm" ) {
      ++i;
      if ( i<argc-offset ) algorithm = argv[i] ;
      else  return usage( std::cerr, argc, argv );
    } 
    //    else if ( std::string(argv[i])=="-o" ) { 
    //    ++i;
    //    if ( i<argc ) output_file = argv[i];
    //   else  return usage( std::cerr, argc, argv );
    //   }
    else if ( std::string(argv[i])=="-t" || std::string(argv[i])=="--tag" ) {
      ++i;
      if ( i<argc-offset ) tags.push_back( argv[i] );
      else  return usage( std::cerr, argc, argv );
    } 
    else if ( std::string(argv[i])=="-r" ) { 
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

  /// if output file is not defined
  //  if ( output_file == "" ) return usage( std::cerr, argc, argv );
  
  //  dataset data("test_EF");
  //  files = data.datafiles();
  
  /// check some input files

  if ( files.size()<1 ) return usage( std::cerr, argc, argv );
  

  //  time the actual running of the cost() routine
  
  if ( verbose ) std::cerr << "timing" << std::endl;

  struct timeval tv = simpletimer_start();

  /// create the structure ...

  node n(0, "" );
  n.name( "top_level" );

  int status = cost( files, n, "", deleteref, relocate );

  //  std::cerr << "\n\nnodes " << n << std::endl;

  if ( status ) return status;

  if ( outfile!="" ) outp = new std::ofstream(outfile.c_str()); 

  header h;

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
