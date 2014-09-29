/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MonteCarloReactUtils/ResTool.h"
#include "MonteCarloReactUtils/ResInfo.h"
#include "MonteCarloReactUtils/Resolution.h"
#include "MonteCarloReactUtils/ShiftFunction.h"
#include "MonteCarloReactUtils/RandomFunction.h"
#include "MonteCarloReactUtils/EffTool.h"

#include <TSystem.h>

#include <fstream>
#include <cstdlib>

using namespace std;
using namespace MonteCarloReact;

ResTool::~ResTool() { delete m_res; }

bool ResTool::getInfo(const ResInfo& spec) {
  m_isValid = m_makeResolutionObj(spec);
  return m_isValid;
}

const ResInfo& ResTool::getInfo() const {
  if( m_res ) 
    return m_res->getInfo(); 
  else 
    throw EffConfigException("Info: No Resolution Object Present");
}
  
const Resolution& ResTool::getResObj() const { 
  if( m_res )
    return *m_res; 
  else
    throw EffConfigException("ResObj: No Resolution Object Present");
}


bool ResTool::m_makeResolutionObj(const ResInfo& request, const std::string& path){
  // add mcr files from directory path and all subdirectories to the file list
  vector<string> files ;
  m_addFilesToList(path.c_str(), files) ;    
  
  if (files.size() == 0) {
   cerr << "MonteCarloReactUtils::ResObj ERROR: Directory " 
	 << path  <<  " does not contains any mcr file!" 
	 << endl ;
    exit (1) ;
  }
  // Create a file_name from params
  string FileName = request.makeFileName();
  vector<string>::const_iterator it = files.begin()  ;
  for (; it != files.end(); it++) {
    if(it->size() >= FileName.size() && it->substr(it->size()-FileName.size()) == FileName) break ;
  }
  if (it == files.end()) 
    throw EffRequestFileNotFound(FileName) ;

  // Open the file as a ifstream object, Take care of exceptions
  if (FileName != "NOFILENAME") {
    ifstream fstr;
    
    fstr.open(it->c_str(),ios::in);
    //std::streampos savePoint = fstr.tellg();
    /*
    while(fstr && fstr.good()) {
      std::string str;
      getline(fstr,str);
      cout << str << endl;
    }
    fstr.seekg(0,ios::beg);
    */
    string type;
    try {
      type = request.getResType();
      if( type == "Shift" || type == "Fixed") {
        m_res = new ShiftFunction( fstr, &request );
      }
      else if( type == "Random") {
        m_res = new RandomFunction( fstr, &request );
      }
      else
        m_res = 0;
    } catch( EffRequestNotFoundInFile& error) {
      cerr << "MonteCarloReactUtils::ResTool exception catch: " << error.printErrorMessage()
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
      if( type == "Shift" || type == "Fixed")
	ShiftFunction tmpRes ( fstr, &request );
      else if( type == "Random" )
	RandomFunction tmpRes( fstr, &request );
      //cout << it->c_str() << endl;
      // if there is no further matching efficiency in the file then an exception should have been thrown and we should never reach the next line
      throw EffDuplicateMatchesToRequest();
    }
    catch( const EffRequestNotFoundInFile & error) {
      // this is actually what we were hoping for
      // check that the eff found is valid and return the appropriate bool
      if( m_res) 
	return m_res->isValid();
      else
	return false;
    }
    
    // if we get here there is a problem....
    return false;
   // file closes when it goes out of scope
  }
  else return false;
}

bool ResTool::m_makeResolutionObj(const ResInfo& request, const std::vector<std::string>& path){

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
  for (; it != files.end(); it++) {
    if(it->size() >= FileName.size() && it->substr(it->size()-FileName.size()) == FileName) break ;
  }
  if (it == files.end()) 
    throw EffRequestFileNotFound(FileName) ;

  // Open the file as a ifstream object, Take care of exceptions
  if (FileName != "NOFILENAME") {
    ifstream fstr;
    
    fstr.open(it->c_str(),ios::in);
    //std::streampos savePoint = fstr.tellg();
    /*
    while(fstr && fstr.good()) {
      std::string str;
      getline(fstr,str);
      cout << str << endl;
    }
    fstr.seekg(0,ios::beg);
    */
    string type;
    try {
      type = request.getResType();
      if( type == "Shift" || type == "Fixed") {
        m_res = new ShiftFunction( fstr, &request );
      }
      else if( type == "Random") {
        m_res = new RandomFunction( fstr, &request );
      }
      else
        m_res = 0;
    } catch( EffRequestNotFoundInFile& error) {
      cerr << "MonteCarloReactUtils::ResTool exception catch: " << error.printErrorMessage()
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
      if( type == "Shift" || type == "Fixed")
	ShiftFunction tmpRes( fstr, &request );
      else if( type == "Random" )
	RandomFunction tmpRes( fstr, &request );
      //cout << it->c_str() << endl;
      // if there is no further matching efficiency in the file then an exception should have been thrown and we should never reach the next line
      throw EffDuplicateMatchesToRequest();
    }
    catch( const EffRequestNotFoundInFile & error) {
      // this is actually what we were hoping for
      // check that the eff found is valid and return the appropriate bool
      if( m_res) 
	return m_res->isValid();
      else
	return false;
    }
    
    // if we get here there is a problem....
    return false;
   // file closes when it goes out of scope
  }
  else return false;
}




void ResTool::m_addFilesToList(const char* path, vector<string>& files) {
  
   void* work_dir = 0 ;
   work_dir = gSystem->OpenDirectory(path)  ;
   if (!work_dir) {
     cerr << "MonteCarloReactUtils::ResTool ERROR: Directory " << path
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

