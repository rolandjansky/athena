///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// helper functions for 4 momentum classes
// to use these classes with a HepLorentzVector class, simply
// convert it on the fly into a P4PxPyPzE like that :
// P4Help::deltaR(theTrack, P4PxPyPzE(theHlv))

#include "FourMomUtils/P4Helpers.h"
