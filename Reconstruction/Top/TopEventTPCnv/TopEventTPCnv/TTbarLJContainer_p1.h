/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARLJCONTAINER_P1_H
#define TTBARLJCONTAINER_P1_H

#include "TopEventTPCnv/TTbarLJ_p1.h"
#include <vector>

// We do want to have separate classes for persistent EJ, MuJ, TauJ to
// preserve the type of the recorded container.  (Note that the
// classes are identical.)

class TTbarEJContainer_p1     : public std::vector<TTbarLJ_p1> {};
class TTbarMuJContainer_p1    : public std::vector<TTbarLJ_p1> {};
class TTbarTauJContainer_p1   : public std::vector<TTbarLJ_p1> {};
class TTbarTruthJContainer_p1 : public std::vector<TTbarLJ_p1> {};
class TTbarJeJContainer_p1    : public std::vector<TTbarLJ_p1> {};


#endif/*TTBARLJCONTAINER_P1_H*/
