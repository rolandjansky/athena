/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Jean-Raphael Lessard, 2009

#ifndef TTBARLJDITOPSCONTAINER_P1_H
#define TTBARLJDITOPSCONTAINER_P1_H

#include "TopEventTPCnv/TTbarLJDiTops_p1.h"
#include <vector>

class TTbarEJDiTopsContainer_p1     : public std::vector<TTbarLJDiTops_p1> {};
class TTbarMuJDiTopsContainer_p1    : public std::vector<TTbarLJDiTops_p1> {};
class TTbarTauJDiTopsContainer_p1   : public std::vector<TTbarLJDiTops_p1> {};
class TTbarTruthJDiTopsContainer_p1 : public std::vector<TTbarLJDiTops_p1> {};
class TTbarJeJDiTopsContainer_p1    : public std::vector<TTbarLJDiTops_p1> {};

#endif //TTBARLJDITOPSCONTAINER_P1_H
