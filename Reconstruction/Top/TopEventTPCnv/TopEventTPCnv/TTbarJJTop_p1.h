/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2009

#ifndef TTBARJJTOP_P1_H
#define TTBARJJTOP_P1_H

#include "TopEventTPCnv/TTbarJJ_p1.h"
#include <vector>

struct TTbarJJTop_p1 : public TTbarJJ_p1 {
  // persistify one TTbarHadronicTop, the other one is recomputed
  std::vector<unsigned>  m_htop1_indexes;
};

#endif/*TTBARJJTOP_P1_H*/
