/* emacs: this is -*- c++ -*- */
/**
 **   @file         TIDDirectory.h  
 **
 **   @brief        class to keep a directory for each object 
 **                 in a root sort of way, but needed to keep 
 **                 the root objects out of the actual code.   
 ** 
 **   @author       M.Sutton  
 **
 **   @date         Wed May  4 17:54:25 BST 2005
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDA_TIDDIRECTORY_H
#define TIDA_TIDDIRECTORY_H

#include "TDirectory.h"
#include "TH1.h"

// #include "utils.h"

class TIDDirectory {

public:
  
  //  TIDDirectory() : mHAddState(true), mDAddState(true), mPop(0), mDir(0) { } 

  TIDDirectory(const std::string& n="") : 
    mname(n), mPop(gDirectory), mDir(0) { 
    
    if ( n=="" ) { 
      mDir = gDirectory;
      return;
    }

    
    
    if ( n.find("/")==std::string::npos ) { 
      /// only create directory if it doesn't already exist      
      mDir = gDirectory->GetDirectory(n.c_str());
      if ( mDir==0 ) mDir = gDirectory->mkdir(n.c_str());
      return;
    }
    
    std::string _dir = n;

    if ( _dir.find("/")!=std::string::npos ) { 
    
      std::string dir = chop( _dir, "/" );

      if ( _dir == "" ) { 
	/// only create directory if it doesn't already exist      
	mDir = gDirectory->GetDirectory( dir.c_str());
	if ( mDir==0 ) mDir = gDirectory->mkdir( dir.c_str() );
	return;
      }

      /// only create directory if it doesn't already exist      
      mDir = gDirectory->GetDirectory( dir.c_str());
      if ( mDir==0 ) mDir = gDirectory->mkdir( dir.c_str() );
      push();

      /// recursively create any level of actual directories
      TIDDirectory d(_dir);
      mDir = d.cwd();
      pop();
    }

    //   push();
  } 


  virtual ~TIDDirectory() {  }

  void push() { mPop = gDirectory; if (mDir) mDir->cd(); } 
  void pop()  { if (mPop) mPop->cd(); }

  void Write() { push(); mDir->Write(); pop(); } 

  void  pwd() const { mDir->pwd(); }  
  TDirectory* cwd() { return mDir; }  

  void disable() {  }
  void restore() {  }

  std::string name() const { return mname; }

 protected:
  
  std::string chop( std::string& s1, const std::string s2="/" ) { 
    std::string s3 = "";
    std::string::size_type pos = s1.find(s2);
    if ( pos != std::string::npos ) {
      s3 = s1.substr(0, pos+s2.size()); 
      s1.erase(0, pos+s2.size());
    }
    return s3;
  } 

 protected:

  //  bool        mHAddState;
  //  bool        mDAddState;

  std::string mname;

  TDirectory* mPop;
  TDirectory* mDir;

};



#endif  /* TIDA_TIDDIRECTORY_H */


