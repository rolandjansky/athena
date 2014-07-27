// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         IDScanRoIHeader.h  
 **
 **   Description:    
 **                   
 **                   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Fri May 26 21:29:16 BST 2006
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __IDSCANROIHEADER_H
#define __IDSCANROIHEADER_H


#include <vector>
#include <ostream>

#include <TObject.h>

#include "TrigIDSCAN/IDScanSpacePoint.h"


using namespace std;


class IDScanRoIHeader : public TObject { 

 public:

  IDScanRoIHeader() : mEta(0), mPhi(0), mDeltaEta(0), mDeltaPhi(0)  { } 
  IDScanRoIHeader(double e, double p, double de=0, double dp=0);

  // set / get the eta/phi of the RoI

  double SetEta(double e) { return mEta = e; } 
  double SetPhi(double p) { return mPhi = p; } 

  double SetDeltaEta(double de) { return mDeltaEta = de; } 
  double SetDeltaPhi(double dp) { return mDeltaPhi = dp; } 

  double Eta() const { return mEta; }
  double Phi() const { return mPhi; }

  double DeltaEta() const { return mDeltaEta; }
  double DeltaPhi() const { return mDeltaPhi; }

  // space point handling

  int                             NSpacePoints() const { return mNSpacePoints; } 
  const vector<IDScanSpacePoint>& SpacePoints()  const { return mSpacePoints; } 
  
  void AddSpacePoint(IDScanSpacePoint s) { mSpacePoints.push_back(s); mNSpacePoints++; }


  // user data handling

#ifndef _NOROIUSERDATA

  int                    NUserData()  const {  return mNUserData; }
  const vector<double>&   UserData()  const {  return mUserData; } 
  
  void AddUserData(double d) { mUserData.push_back(d); mNUserData++; }
#endif
  
 private:
  
  double                   mEta;       // eta of the RoI
  double                   mPhi;       // phi of the RoI

  double                   mDeltaEta;  // delta eta of the RoI
  double                   mDeltaPhi;  // delta phi of the RoI
  
  int                      mNSpacePoints;  // number of spacepoints in the RoI
  vector<IDScanSpacePoint> mSpacePoints;   // the spacepoints themselves

#ifndef _NOROIUSERDATA
  int             mNUserData;  // number of user data words in the RoI
  vector<double>  mUserData;   // the userdata themselves
#endif  

  ClassDef(IDScanRoIHeader, 1)
  
};


ostream& operator<<(ostream& , const IDScanRoIHeader& );

#endif  /* __IDSCANROIHEADER_H */










