// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**************************************************************************
 **
 **   File:         Directory.h  
 **
 **   Description:  class to keep a directory for each object 
 **                 in a root sort of way, but needed to keep 
 **                 the root objects out of the actual code.   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Wed May  4 17:54:25 BST 2005
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __DIRECTORY_H
#define __DIRECTORY_H

#include "TDirectory.h"
#include "TH1.h"

// #include "utils.h"

class Directory {

 public:

  Directory() : mHAddState(true), mDAddState(true), mPop(NULL), mDir(NULL) { } 
  Directory(const std::string& n) : mPop(gDirectory), mDir(gDirectory->mkdir(n.c_str())) { 
    //   push();
  } 

  virtual ~Directory() {  }

  void push() { mPop = gDirectory; if (mDir) mDir->cd(); } 
  void pop()  { if (mPop) mPop->cd(); }

  void Write() { push(); mDir->Write(); pop(); } 

  void  pwd() const { mDir->pwd(); }  
  TDirectory* cwd() { return mDir; }  

  void disable() {  }
  void restore() {  }

 protected:
  
  bool        mHAddState;
  bool        mDAddState;

  TDirectory* mPop;
  TDirectory* mDir;

};



#endif  /* __DIRECTORY_H */










