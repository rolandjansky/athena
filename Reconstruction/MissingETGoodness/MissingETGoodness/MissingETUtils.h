/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETUTILS_H
#define MISSINGETUTILS_H

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "MissingETEvent/MissingET.h"

//#include "muonEvent/MuonSpShowerContainer.h"
//#include "muonEvent/MuonSpShower.h"
//#include "CxxUtils/BasicTypes.h"


namespace MissingETUtils {

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


}


#endif
