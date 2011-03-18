/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2009

#ifndef TTBARLJTOPCONTAINER_P1_H
#define TTBARLJTOPCONTAINER_P1_H

#include "TopEventTPCnv/TTbarLJTop_p1.h"
#include <vector>

// We do want to have separate classes for persistent EJ, MuJ, TauJ to
// preserve the type of the recorded container.  (Note that the
// classes are identical.)

class TTbarEJTopContainer_p1     : public std::vector<TTbarLJTop_p1> {};
class TTbarMuJTopContainer_p1    : public std::vector<TTbarLJTop_p1> {};
class TTbarTauJTopContainer_p1   : public std::vector<TTbarLJTop_p1> {};
class TTbarTruthJTopContainer_p1 : public std::vector<TTbarLJTop_p1> {};


#endif/*TTBARLJTOPCONTAINER_P1_H*/
