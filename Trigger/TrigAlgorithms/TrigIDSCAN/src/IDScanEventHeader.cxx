/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 **
 **   File:         IDScanEventHeader.cxx        
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

#include "TrigIDSCAN/IDScanEventHeader.h"


ClassImp(IDScanEventHeader)

  
IDScanEventHeader::IDScanEventHeader() : mNRoIs(0), mNUserData(0) { } 





ostream& operator<<(ostream& s, const IDScanEventHeader& h) {
  const vector<IDScanRoIHeader>&  r = h.RoIs(); 

  s << "Event "     << h.GetEvent()  
    << "  \tNRoIs " << h.NRoIs() << " \t  " << h.NUserData() << " user data members\n";

#if 0  
  const vector<double>& u = h.UserData();
  for ( int i=0 ; i<h.NUserData() ; i++ ) s << "UserData[" << i << "] = " << u[i] << endl;
  s << endl;
#endif
  for ( int i=0 ; i<h.NRoIs() ; i++ )     s << r[i] << endl;
  
  return s;
}




#endif /** IDSCANDUMP **/


