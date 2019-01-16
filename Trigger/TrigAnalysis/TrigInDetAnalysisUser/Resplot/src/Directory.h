/* emacs: this is -*- c++ -*- */
/**
 **
 **   @file         Directory.h  
 **
 **                 class to keep a directory for each object 
 **                 in a root sort of way, but needed to keep 
 **                 the root objects out of the actual code.   
 ** 
 **   @author       M.Sutton  
 **
 **   @date         Wed May  4 17:54:25 BST 2005
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **                   
 **                   
 **
 **/


#ifndef RESPLOT_DIRECTORY_H
#define RESPLOT_DIRECTORY_H

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



#endif  /* RESPLOT_DIRECTORY_H */










