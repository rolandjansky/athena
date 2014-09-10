// emacs: this is -*- c++ -*-
//
//   @file    dataset.h        
//
//                   
//  
//   Copyright (C) 2010 M.Sutton (sutt@cern.ch)    
//
//   $Id: dataset.h 513250 2012-08-10 14:35:11Z sutt $


#ifndef __DATASET_H
#define __DATASET_H

#include <stdio.h>
#include <dirent.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <iostream>
#include <vector>

class dataset : public std::vector<std::string> {
  
public:
  
  dataset(const std::string& s) : m_directory(s) {
    std::cout << "dataset::dataset() reading files from " << s << std::endl;
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










