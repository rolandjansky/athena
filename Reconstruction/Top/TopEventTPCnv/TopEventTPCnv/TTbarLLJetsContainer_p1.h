/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARLLJETSCONTAINER_P1_H
#define TTBARLLJETSCONTAINER_P1_H

#include "TopEventTPCnv/TTbarLLJets_p1.h"
#include <vector>

// We do want to have separate classes for persistent EJ, MuJ, TauJ to
// preserve the type of the recorded container.  (Note that the
// classes are identical.)

class TTbarEEJetsContainer_p1         : public std::vector<TTbarLLJets_p1> {};
class TTbarEMuJetsContainer_p1        : public std::vector<TTbarLLJets_p1> {};
class TTbarETauJetsContainer_p1       : public std::vector<TTbarLLJets_p1> {};
class TTbarETrackJetsContainer_p1     : public std::vector<TTbarLLJets_p1> {};
class TTbarMuMuJetsContainer_p1       : public std::vector<TTbarLLJets_p1> {};
class TTbarMuTauJetsContainer_p1      : public std::vector<TTbarLLJets_p1> {};
class TTbarMuTrackJetsContainer_p1    : public std::vector<TTbarLLJets_p1> {};
class TTbarTauTauJetsContainer_p1     : public std::vector<TTbarLLJets_p1> {};
class TTbarTruthTruthJetsContainer_p1 : public std::vector<TTbarLLJets_p1> {};

#endif/*TTBARLLJETSCONTAINER_P1_H*/
