/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLLJETSMETCONTAINER_P1_H
#define TTBARLLJETSMETCONTAINER_P1_H

#include "TopEventTPCnv/TTbarLLJetsMet_p1.h"
#include <vector>

class TTbarEEJetsMetContainer_p1         : public std::vector<TTbarLLJetsMet_p1> {};
class TTbarEMuJetsMetContainer_p1        : public std::vector<TTbarLLJetsMet_p1> {};
class TTbarETauJetsMetContainer_p1       : public std::vector<TTbarLLJetsMet_p1> {};
class TTbarETrackJetsMetContainer_p1     : public std::vector<TTbarLLJetsMet_p1> {};
class TTbarMuMuJetsMetContainer_p1       : public std::vector<TTbarLLJetsMet_p1> {};
class TTbarMuTauJetsMetContainer_p1      : public std::vector<TTbarLLJetsMet_p1> {};
class TTbarMuTrackJetsMetContainer_p1    : public std::vector<TTbarLLJetsMet_p1> {};
class TTbarTauTauJetsMetContainer_p1     : public std::vector<TTbarLLJetsMet_p1> {};
class TTbarTruthTruthJetsMetContainer_p1 : public std::vector<TTbarLLJetsMet_p1> {};


#endif/*TTBARLLJETSMETCONTAINER_P1_H*/
