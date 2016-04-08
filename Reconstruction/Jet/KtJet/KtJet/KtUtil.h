/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef KTJET_KTUTIL_H
#define KTJET_KTUTIL_H

// Define CLHEP namespace token to handle the transition between
// CLHEP versions 1.9 and 2.0. Also include the CLHEP config header
// which was removed from CLHEP v2.0 onwards.
#ifndef CLHEP1
#define CLHEPNAMESPACE CLHEP ::
#else
#define CLHEPNAMESPACE
#include "CLHEP/config/CLHEP.h"
#endif

// Includes
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"


namespace KtJet{
  
#ifdef KTDOUBLEPRECISION
  typedef double KtFloat;
#else
  typedef float KtFloat;
#endif
  
  class KtLorentzVector;
  
  /** Phi angle forced into range -pi to +pi */
  KtFloat phiAngle(KtFloat testphi);
  
} //end of namespace

#endif
