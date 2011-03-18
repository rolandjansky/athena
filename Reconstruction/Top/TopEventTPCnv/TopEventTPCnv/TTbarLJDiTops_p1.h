/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Jean-Raphael Lessard, 2008

#ifndef TTBARLJDITOPS_P1_H
#define TTBARLJDITOPS_P1_H

#include "TopEventTPCnv/TTbarLJ_p1.h"

#include <vector>

struct TTbarLJDiTops_p1 : public TTbarLJ_p1 {
  std::vector<unsigned> m_htop_indexes;
  std::vector<double> m_neutrino;
  unsigned int m_indexHB, m_indexLB;
};

#endif //TTBARLJDITOPS_P1_H
