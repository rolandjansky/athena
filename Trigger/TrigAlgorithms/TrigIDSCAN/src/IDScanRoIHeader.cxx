/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 **
 **   File:         IDScanRoIHeader.cxx        
 **
 **   Description:    
 **                   
 **                   
 ** 
 **   Author:       M.Sutton    
 **
 **   Created:      Fri May 26 21:26:22 BST 2006
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 

#ifdef IDSCANDUMP


#include "TrigIDSCAN/IDScanRoIHeader.h"

ClassImp(IDScanRoIHeader)



IDScanRoIHeader::IDScanRoIHeader(double e, double p, double de, double dp) : 
  mEta(e), mPhi(p), 
  mDeltaEta(de), mDeltaPhi(dp)
{ 
  mNSpacePoints = 0; 
#ifndef _NOROIUSERDATA
  mNUserData    = 0;
#endif 
} 





ostream& operator<<(ostream& s, const IDScanRoIHeader& r) {

#ifndef _NOROIUSERDATA

  s << "RoI :  eta=" << r.Eta() << "\t phi=" << r.Phi() 
    << "\tNsp=" << r.NSpacePoints() << "\t NUserData=" << r.NUserData() << "\n"; 


  //  const vector<double>&            d = r.UserData();

  //  for ( int j=0 ; j<r.NUserData() ; j++ ) s << " UserData[" << j << "]=" << d[j] << "\n";

#else

  s << "RoI :  "
    << "\teta=" << r.Eta() << " +- " << r.DeltaEta()  
    << "\tphi=" << r.Phi() << " +- " << r.DeltaPhi() 
    << "\tNsp=" << r.NSpacePoints() << "\n"; 

#endif

  const vector<IDScanSpacePoint>& sp = r.SpacePoints();

  for ( int j=0 ; j<r.NSpacePoints() ; j++ ) { 
    s << " " << j << "  " << sp[j] << "\n";
    //    if ( (j+1)%5==0 ) s << "\n";
  }

  return s;
}


#endif /** IDSCANDUMP **/
