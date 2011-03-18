/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Jean-Raphael Lessard, 2009

#ifndef TTBARLJDITOPSCONTAINERFITTED_P1_H
#define TTBARLJDITOPSCONTAINERFITTED_P1_H

#include "TopEventTPCnv/TTbarLJDiTopsFitted_p1.h"
#include <vector>

class TTbarEJDiTopsFittedContainer_p1     : public std::vector<TTbarLJDiTopsFitted_p1> {};
class TTbarMuJDiTopsFittedContainer_p1    : public std::vector<TTbarLJDiTopsFitted_p1> {};
class TTbarTauJDiTopsFittedContainer_p1   : public std::vector<TTbarLJDiTopsFitted_p1> {};
class TTbarTruthJDiTopsFittedContainer_p1 : public std::vector<TTbarLJDiTopsFitted_p1> {};
class TTbarJeJDiTopsFittedContainer_p1    : public std::vector<TTbarLJDiTopsFitted_p1> {};

#endif //TTBARLJDITOPSCONTAINERFITTED_P1_H
