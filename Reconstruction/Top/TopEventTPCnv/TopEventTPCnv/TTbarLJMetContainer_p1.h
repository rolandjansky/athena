/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJMETCONTAINER_P1_H
#define TTBARLJMETCONTAINER_P1_H

#include "TopEventTPCnv/TTbarLJMet_p1.h"
#include <vector>

// We do want to have separate classes for persistent EJ, MuJ, TauJ to
// preserve the type of the recorded container.  (Note that the
// classes are identical.)

class TTbarEJMetContainer_p1     : public std::vector<TTbarLJMet_p1> {};
class TTbarMuJMetContainer_p1    : public std::vector<TTbarLJMet_p1> {};
class TTbarTauJMetContainer_p1   : public std::vector<TTbarLJMet_p1> {};
class TTbarTruthJMetContainer_p1 : public std::vector<TTbarLJMet_p1> {};
class TTbarJeJMetContainer_p1    : public std::vector<TTbarLJMet_p1> {};

#endif/*TTBARLJMETCONTAINER_P1_H*/
