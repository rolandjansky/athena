/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MonteCarloReactUtils/EffTool.h"
#include "MonteCarloReactUtils/EffInfo.h"
#include "MonteCarloReactUtils/Efficiency.h"
#include "MonteCarloReactUtils/BinnedEfficiency.h"
#include "MonteCarloReactUtils/ParameterizedEfficiency.h"

#include <TSystem.h>

#include <fstream>
#include <cstdlib>

using namespace std;
using namespace MonteCarloReact;

EffTool::EffTool(const EffInfo& spec, const std::string& path) {
  m_isValid = m_makeEfficiencyObj(spec, path);
  if( !m_isValid ) m_eff = 0;
}

EffTool::EffTool(const EffInfo& spec, const std::vector<std::string>& path) {
  m_isValid = m_makeEfficiencyObj(spec, path);
  if( !m_isValid ) m_eff = 0;
}



EffTool::~EffTool() { delete m_eff; }

bool EffTool::getInfo(const EffInfo& spec) {
  m_isValid = m_makeEfficiencyObj(spec);
  return m_isValid;
}

const EffInfo& EffTool::getInfo() const {
  if( m_eff ) 
    return m_eff->getInfo(); 
  else 
    throw EffConfigException("Info: No Efficiency Object Present");
}



const Efficiency& EffTool::getEffObj() const { 
  if( m_eff )
    return *m_eff; 
  else
    throw EffConfigException("EffObj: No Efficiency Object Present");
}

bool EffTool::m_makeEfficiencyObj(const EffInfo& request, const std::string& path){

  // add mcr files from directory path and all subdirectories to the file list
  vector<string> files ;
  m_addFilesToList(path.c_str(), files) ;    
  
  if (files.size() == 0) {
    cerr << "MonteCarloReactUtils::EffObj ERROR: Directory " 
	 << path  <<  " does not contain any mcr file!" 
	 << endl ;
    exit (1) ;
  }
  // Create a file_name from params
  string FileName = request.makeFileName();
  vector<string>::const_iterator it = files.begin()  ;
  for (; it != files.end(); it++) 
    if(it->size() >= FileName.size() && it->substr(it->size()-FileName.size()) == FileName) break ;
  
  if (it == files.end()) 
    throw EffRequestFileNotFound(FileName) ;


  // Open the file as a ifstream object, Take care of exceptions
  if (FileName != "NOFILENAME") {
    ifstream fstr;
    fstr.open(it->c_str(),ios::in);
    string type;
    try {
    type = request.getEffType();
    if( type == "Binned" )  
      m_eff = new BinnedEfficiency( fstr, &request );
    else if( type == "Parameterized") 
      m_eff = new ParameterizedEfficiency( fstr, &request );
    else
      m_eff = 0;
    } catch( EffRequestNotFoundInFile& error) {
      cerr << "MonteCarloReact::EffTool exception catch: " << error.printErrorMessage()
	   << endl 
	   << "==================================================" << endl << request
	   << "==================================================" << endl << it->c_str()
	   << endl ;
      
      return false ;    
    }
    // now see if we can find another one!
    // this is kind of an evil use of exceptions since we are relying on an exception happening in cases of there being no error!
    // a better way might be to add some state to the efficiency whereby the Efficiency object can tell you if it contains anything
    // if its empty then nothing was found.

    try{ 
      if( type == "Binned" )
	BinnedEfficiency tmpEff ( fstr, &request );
      else if( type == "Parameterized") 
	ParameterizedEfficiency tmpEff ( fstr, &request );

      //cout << it->c_str() << endl; <---- Check this
      // if there is no further matching efficiency in the file then an exception should have been thrown and we should never reach the next line
      throw EffDuplicateMatchesToRequest();
    }
    catch( const EffRequestNotFoundInFile & error) {
      // this is actually what we were hoping for
      // check that the eff found is valid and return the appropriate bool
      if( m_eff) 
	return m_eff->isValid();
      else
	return false;
    }
    // if we get here there is a problem....
    return false;
   // file closes when it goes out of scope
  }
  //  if( _eff && _eff->isValid()) {
  //if (request != _eff->Info()) 
  //  cout << "MonteCarloReact::EffTool Warning: The requested meta data are less specific than the found one!"
  //   << endl << "Will use the first efficiency found in the file!"
  //   << endl << "REQUESTED: " << request
  //   << endl << "FOUND: " << _eff->Info() 
  //   << endl ;    
  //return true;
   else return false;
}


bool EffTool::m_makeEfficiencyObj(const EffInfo& request, const std::vector<std::string>& path){

  // add mcr files from directory path and all subdirectories to the file list
  vector<string> files ;
  typedef vector<string>::size_type size_t;

  for(size_t p = 0; p < path.size(); p++) {
    m_addFilesToList(path[p].c_str(), files) ;    
  }
  
  if (files.size() == 0) {
    cerr << "MonteCarloReactUtils::EffObj ERROR: Directory ";
    for(size_t p = 0; p < path.size(); p++) {
      cerr << path[p] << " ";
    }
    cerr <<  "does not contain any mcr file!" 
	 << endl ;
    exit (1) ;
  }
  // Create a file_name from params
  string FileName = request.makeFileName();
  vector<string>::const_iterator it = files.begin()  ;
  for (; it != files.end(); it++) 
    if(it->size() >= FileName.size() && it->substr(it->size()-FileName.size()) == FileName) break ;
  
  if (it == files.end()) 
    throw EffRequestFileNotFound(FileName) ;


  // Open the file as a ifstream object, Take care of exceptions
  if (FileName != "NOFILENAME") {
    ifstream fstr;
    fstr.open(it->c_str(),ios::in);
    string type;
    try {
    type = request.getEffType();
    if( type == "Binned" )  
      m_eff = new BinnedEfficiency( fstr, &request );
    else if( type == "Parameterized") 
      m_eff = new ParameterizedEfficiency( fstr, &request );
    else
      m_eff = 0;
    } catch( EffRequestNotFoundInFile& error) {
      cerr << "MonteCarloReact::EffTool exception catch: " << error.printErrorMessage()
	   << endl 
	   << "==================================================" << endl << request
	   << "==================================================" << endl << it->c_str()
	   << endl ;
      
      return false ;    
    }
    // now see if we can find another one!
    // this is kind of an evil use of exceptions since we are relying on an exception happening in cases of there being no error!
    // a better way might be to add some state to the efficiency whereby the Efficiency object can tell you if it contains anything
    // if its empty then nothing was found.

    try{ 
      if( type == "Binned" )
	BinnedEfficiency tmpEff( fstr, &request );
      else if( type == "Parameterized") 
	ParameterizedEfficiency tmpEff( fstr, &request );

      //cout << it->c_str() << endl; <---- Check this
      // if there is no further matching efficiency in the file then an exception should have been thrown and we should never reach the next line
      throw EffDuplicateMatchesToRequest();
    }
    catch( const EffRequestNotFoundInFile & error) {
      // this is actually what we were hoping for
      // check that the eff found is valid and return the appropriate bool
      if( m_eff) 
	return m_eff->isValid();
      else
	return false;
    }
    // if we get here there is a problem....
    return false;
   // file closes when it goes out of scope
  }
  //  if( _eff && _eff->isValid()) {
  //if (request != _eff->Info()) 
  //  cout << "MonteCarloReact::EffTool Warning: The requested meta data are less specific than the found one!"
  //   << endl << "Will use the first efficiency found in the file!"
  //   << endl << "REQUESTED: " << request
  //   << endl << "FOUND: " << _eff->Info() 
  //   << endl ;    
  //return true;
   else return false;
}




void EffTool::m_addFilesToList(const char* path, vector<string>& files) {
  
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

