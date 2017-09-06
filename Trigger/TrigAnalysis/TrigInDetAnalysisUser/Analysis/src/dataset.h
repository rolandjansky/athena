// emacs: this is -*- c++ -*-
//
//   @file    dataset.h        
//
//                   
//  
//   Copyright (C) 2010 M.Sutton (sutt@cern.ch)    
//
//   $Id: dataset.h 760843 2016-07-09 23:56:54Z sutt $


#ifndef __DATASET_H
#define __DATASET_H

#include <cstdio>
#include <cstdlib>

#include <dirent.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

// #include <XrdPosix/XrdPosixXrootd.hh>

class dataset : public std::vector<std::string> {
  
public:
  
  dataset(const std::string& s) : m_directory(s) {
    std::cout << "dataset::dataset() reading files from " << s << std::endl;

    if ( s.find("root://eos")!=std::string::npos ) { 
      /// need to open on eos
      
      std::string  _cmd = "eos ls "+s+"/";
      std::system( (_cmd+" > .eosfiles.log").c_str() );

      std::string   cmd = "export EOSFILES=$("+_cmd+")";
      std::system(  cmd.c_str() );

      std::ifstream infile(".eosfiles.log");

      while( !infile.fail() ) {
	std::string file;
	infile >>  file;
	if ( !infile.fail() && file.find(".root")!=std::string::npos ) push_back( s+"/"+file );
      }

      //      for ( unsigned i=size() ; i-- ; ) std::cout << at(i) << std::endl;

      /// grrrr, the getenv actually crashes for some reason
      //      std::string files = std::getenv("EOSFILES");
      //      std::cout << "files " << files << std::endl;

    }
    else { 
      DIR *dirp;
      struct dirent *entry;

      if( ( dirp = opendir(s.c_str()) ) )  {
	while( ( entry = readdir(dirp) ) ) {
	  std::string file = entry->d_name;
	  if ( file.find(".root")!=std::string::npos ) { 
	    push_back( s+"/"+file );
	    //   std::cout << " read file: " << entry->d_name << std::endl;
	  }
	}
	closedir(dirp);
      }   
      std::cout << "dataset::dataset() " << size() << " files in dataset" << std::endl;
    } 
  }

  ~dataset() { } 

  std::string directory() const { return m_directory; }

  const std::vector<std::string>& datafiles() { return *this; } 

private: 

  std::string m_directory;

};

inline std::ostream& operator<<( std::ostream& s, const dataset& _d ) { 
  return s << _d.directory() << " " << _d.size();
}


#endif  // __DATASET_H 










