/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TOPJETLEPTONICCONTAINER_P1_H
#define TOPJETLEPTONICCONTAINER_P1_H

#include "TopEventTPCnv/TopJetLeptonic_p1.h"
#include <vector>

// We do want to have separate classes for persistent EJ, MuJ, TauJ to
// preserve the type of the recorded container.  (Note that the
// classes are identical.)

class TopJetElectronContainer_p1 : public std::vector<TopJetLeptonic_p1> {};
class TopJetMuonContainer_p1     : public std::vector<TopJetLeptonic_p1> {};
class TopJetTauContainer_p1      : public std::vector<TopJetLeptonic_p1> {};
class TopJetTruthContainer_p1    : public std::vector<TopJetLeptonic_p1> {};

#endif/*TOPJETLEPTONICCONTAINER_P1_H*/
