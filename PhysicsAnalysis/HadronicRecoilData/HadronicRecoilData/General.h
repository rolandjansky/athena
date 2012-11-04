/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HADRONICRECOILDATA_GENERAL_H
#define HADRONICRECOILDATA_GENERAL_H

namespace HadronicRecoil {

  /** Returns the the phi coordinate properly calculated
      (i.e. taking into account the periodicity). */
  double CorrectPhi( double phi );

  /** Returns the difference phi2-phi1 properly calculated
      (i.e. taking into account the periodicity). */
  double DeltaPhi(double phi1, double phi2);
  
  /** Returns the distance in the eta-phi plane. */
  double DeltaR(double eta1, double phi1, double eta2, double phi2);
  
}
#endif
