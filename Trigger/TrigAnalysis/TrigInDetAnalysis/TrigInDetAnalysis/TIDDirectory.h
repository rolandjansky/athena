// emacs: this is -*- c++ -*-
/**************************************************************************
 **
 **   File:         TIDDirectory.h  
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


#ifndef __TIDDIRECTORY_H
#define __TIDDIRECTORY_H

#include "TDirectory.h"
#include "TH1.h"

// #include "utils.h"

class TIDDirectory {

 public:

  TIDDirectory() : mHAddState(true), mDAddState(true), mPop(NULL), mDir(NULL) { } 
  TIDDirectory(const std::string& n) : mPop(gDirectory), mDir(gDirectory->mkdir(n.c_str())) { 
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

 protected:
  
  bool        mHAddState;
  bool        mDAddState;

  TDirectory* mPop;
  TDirectory* mDir;

};



#endif  /* __TIDDIRECTORY_H */










