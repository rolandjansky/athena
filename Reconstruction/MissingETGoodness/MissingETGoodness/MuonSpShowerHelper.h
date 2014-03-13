/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GOODNESSMUONSPHELPER_H
#define GOODNESSMUONSPHELPER_H

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "MissingETEvent/MissingET.h"

#include "muonEvent/MuonSpShowerContainer.h"
#include "muonEvent/MuonSpShower.h"
// DRQ: This next line is necessary for compilation on MacOSX. It's a no-op for Linux
#include "CxxUtils/BasicTypes.h"


namespace MuonSpShowerHelper {

   // Wrapping difference between two angles
   inline float DiffPhi(double phi1,double phi2){
      double dphi = fabs(phi1-phi2);
      if (dphi > M_PI) dphi = fabs(dphi - 2*M_PI);  //watch for wraparound
      return dphi;
   }

   // Wrapping difference between an angle and a jet
   inline float JetAngleToPhi(const Jet *jet,double phi){
      return DiffPhi(jet->phi(),phi);
   }

   // Wrapping difference between an angle(x,y) and a jet
   inline float JetAngleToXY(const Jet *jet,double x,double y){
      double tmp_phi=atan2(y,x);
      return JetAngleToPhi(jet,tmp_phi);
   }

   // Wrapping difference between Jet and Met
   inline float JetAngleToMet(const Jet *jet,const MissingET *met){
      return JetAngleToXY(jet,met->etx(),met->ety());
   }

   //=============================================
   // Muon Sp Shower related:
   
  uint TotalSpHits(Rec::MuonSpShower const * const MuonSpShower) ;

   // Sorted index list, sorted by total number of hits
   std::vector<int> SortListSpShower(Rec::MuonSpShowerContainer const * const MuonSpShowers) ;

   
  // Find SpShower closest to some Eta,Phi
  bool FindSpShower(uint & FoundIndex,
                     Rec::MuonSpShowerContainer const * const MuonSpShowers,
                     double atEta,double atPhi,bool PhiOnly=false) ;

  // Find SpShower closest to some Phi
  bool FindSpShower(uint & FoundIndex,
                     Rec::MuonSpShowerContainer const * const MuonSpShowers,
                     double atPhi) ;
   
  // Find SpShower with highest number of hits 
  bool FindSpShowerMaxHits(uint & FoundIndex,                            
                     Rec::MuonSpShowerContainer const * const MuonSpShowers) ;
   
  // Sum MuonSpShower vector. Eta,Phi becomes Hit weighted averages
  bool SumSpShowers(Rec::MuonSpShower & ResultSpShower,
                     Rec::MuonSpShowerContainer const * const MuonSpShowers) ;

   // GetSpShower by Index
  bool GetSpShower(Rec::MuonSpShower & ResultSpShower,
                     Rec::MuonSpShowerContainer const * const MuonSpShowers,uint index) ;





}


#endif
