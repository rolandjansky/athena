/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARLLCONTAINER_P1_H
#define TTBARLLCONTAINER_P1_H

#include "TopEventTPCnv/TTbarLL_p1.h"
#include <vector>

// We do want to have separate classes for persistent EJ, MuJ, TauJ to
// preserve the type of the recorded container.  (Note that the
// classes are identical.)

class TTbarEEContainer_p1         : public std::vector<TTbarLL_p1> {};
class TTbarEMuContainer_p1        : public std::vector<TTbarLL_p1> {};
class TTbarETauContainer_p1       : public std::vector<TTbarLL_p1> {};
class TTbarETrackContainer_p1     : public std::vector<TTbarLL_p1> {};
class TTbarMuMuContainer_p1       : public std::vector<TTbarLL_p1> {};
class TTbarMuTauContainer_p1      : public std::vector<TTbarLL_p1> {};
class TTbarMuTrackContainer_p1    : public std::vector<TTbarLL_p1> {};
class TTbarTauTauContainer_p1     : public std::vector<TTbarLL_p1> {};
class TTbarTruthTruthContainer_p1 : public std::vector<TTbarLL_p1> {};

#endif/*TTBARLLCONTAINER_P1_H*/
