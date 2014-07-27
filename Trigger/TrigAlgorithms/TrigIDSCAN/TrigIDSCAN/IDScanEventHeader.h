// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         IDScanEventHeader.h  
 **
 **   Description:  IDScan event and RoI header and standalone 
 **                 spacepoint class  
 **                   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Thu May 25 16:42:14 BST 2006
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __IDSCANEVENTHEADER_H
#define __IDSCANEVENTHEADER_H

#include <vector>
#include <ostream>

#include <TObject.h>

#include "TrigIDSCAN/IDScanRoIHeader.h"

using namespace std;



class IDScanEventHeader : public TObject { 

 public:
  
  IDScanEventHeader();

  // RoI data handling

  int                            NRoIs() const { return mNRoIs; } 
  const vector<IDScanRoIHeader>&  RoIs() const { return mRoIs; } 
  void AddRoI(IDScanRoIHeader& r)              { mRoIs.push_back(r); mNRoIs++; }
 

  // User event data handling

  int                   NUserData() const { return mNUserData; } 
  const vector<double>&  UserData() const { return mUserData; } 
  void AddUserData(double d)              { mUserData.push_back(d); mNUserData++; }

  
  // set/get the run/event number

  void SetRun(int r)    { mRun=r; } 
  void SetEvent(int e)  { mEvent=e; } 

  int  GetRun()   const { return mRun; } 
  int  GetEvent() const { return mEvent; } 


  // reset this event 
  
  void ResetHeader() { 
    mNRoIs     = 0;   mNUserData = 0;
    mRoIs.clear();    mUserData.clear();
  }
  
protected:
  
  int                     mRun;    // event number
  int                     mEvent;  // event number
  
  int                     mNRoIs;   // count of the number of RoI's in the event
  vector<IDScanRoIHeader> mRoIs;    // the RoI spacepoint data
  
  int                     mNUserData;  // count of user data
  vector<double>          mUserData;   // the user event datat
  
  
  ClassDef(IDScanEventHeader, 1)
};


ostream& operator<<(ostream& , const IDScanEventHeader& );


#endif  /* __IDSCANEVENTHEADER_H */










