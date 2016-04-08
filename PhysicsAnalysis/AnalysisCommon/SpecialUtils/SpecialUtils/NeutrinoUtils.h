/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   Utilities for Neutrino

   @author Tadashi Maeno
 */

#ifndef SPECIALUTILS_NEUTRINOUTILS_H
#define SPECIALUTILS_NEUTRINOUTILS_H

namespace NeutrinoUtils {
  
  /**
     construct neutrino 4-momontum from lepton, missing pt and the W-mass.
     The neutrino mass is zero. This method returns the container of neutrino candidates.
   */
  template <class LEPTON, class COLL>
  bool candidatesFromWMass(LEPTON* lepton, const double pxMiss, const double pyMiss,
			   COLL & neutrinoContainer, bool DealWithNegative = false);

  /**
     tau tau -> a+b+ptmiss utility. This method returns two neutrinos
     given two particles and ptmiss.  Here the constraint is that a and b are
     collinear with the taus (either leptons or a TauJet.  Thus 'a' carries a
     fraction x_a of one tau's momentum and that tau's neutrinos carry (1-x_a)
     of that tau's momentum.
   */
  template <class LEPTON1, class LEPTON2, class COLL>  
  void neutrinosFromColinearApproximation(LEPTON1* particleA, LEPTON2* particleB,
					  const double pxMiss, const double pyMiss, 
					  COLL & neutrinoContainer,
					  double &x_a, double &x_b);
}

#include "SpecialUtils/NeutrinoUtils.icc"

#endif
