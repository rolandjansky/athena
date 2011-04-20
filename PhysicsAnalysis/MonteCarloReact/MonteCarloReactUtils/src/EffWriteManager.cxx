/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MonteCarloReactUtils/EffWriteManager.h"
#include "MonteCarloReactUtils/EffInfo.h"
#include "MonteCarloReactUtils/BinnedEfficiency.h"
#include "MonteCarloReactUtils/ParameterizedEfficiency.h"

#include <TSystem.h>

#include <fstream>

using namespace std;
using namespace MonteCarloReact;

bool EffWriteManager::save(const Efficiency& eff) {

  // add mcr files from directory path and all subdirectories to the file list
  vector<string> files ;
  m_addFilesToList(m_path.c_str(), files) ;    
  
  EffInfo request = eff.getInfo() ;
  string FileName = request.makeFileName();
  if (FileName == "NOFILENAME") {
    cerr << "MonteCarloReactUtils::EffWriteManager ERROR: request is not valid!" << endl ;
    cerr << request
	 << endl ;
    return false ;
  }

  vector<string>::const_iterator it = files.begin()  ;
  for (; it != files.end(); it++) 
    if(it->find(FileName) != string::npos) break ;
  
  if (it == files.end()) {
    ofstream f((m_path+FileName).c_str()) ;
    f << eff ;
    f.close() ;
    return true ;
  }
 

  // Open the file as a ifstream object, Take care of exceptions
  bool exists = false ;
  ifstream fstr;
  fstr.open(it->c_str(),ios::in);
  string type = request.getEffType();
  if( type == "Binned" ) {
    BinnedEfficiency b ;
    exists = b.isRequestInFile(fstr, &request); 
  } else if( type == "Parameterized") {
    ParameterizedEfficiency p ;
    p.isRequestInFile(fstr, &request );
  }
  fstr.close() ;
  if (exists) {
    cerr << "MonteCarloReactUtils::EffWriteManager ERROR: this efficiency exist in the file \"" 
	 << FileName << "\""<< endl ;
    cerr << request << endl ;
    return false ;
  }
  
  ofstream f((m_path+FileName).c_str(), ios::out | ios::app) ;
  f << endl ;
  f << eff ;
  f.close() ;
  return true ;    
}


void EffWriteManager::m_addFilesToList(const char* path, vector<string>& files) {
  
   void* work_dir = 0 ;
   work_dir = gSystem->OpenDirectory(path)  ;
   if (!work_dir) {
     cerr << "MonteCarloReactUtils::EffTool ERROR: Directory " << path
	  << " is not found!" << endl ;
     return ;
   }
   
   const char* file_name_c ;
   while ( (file_name_c = gSystem->GetDirEntry(work_dir)) != 0 ) {
     Long_t id = 0,  flags = 0, modtime = 0 ;
     Long64_t size = 0 ;
     if (gSystem->GetPathInfo(gSystem->ConcatFileName(path,file_name_c) , 
			      &id, &size, &flags, &modtime ) != 0) continue ;
     string file_name = file_name_c ;
     if (file_name == "." || file_name == "..") continue ;      
     if ( (flags & 2) == 2) {
       m_addFilesToList(gSystem->ConcatFileName(path,file_name_c), files) ;
       continue ;
     }
     if ( file_name.find(".mcr") == string::npos ) continue ;           
     files.push_back((string) gSystem->ConcatFileName(path,file_name_c) ) ;
   }   
   gSystem->FreeDirectory(work_dir) ;
   return ;    
 }
 
