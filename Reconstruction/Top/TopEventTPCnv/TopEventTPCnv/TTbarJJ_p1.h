/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARJJ_P1_H
#define TTBARJJ_P1_H

#include "TopEventTPCnv/TTbarJetsBase_p1.h"
#include <vector>

struct TTbarJJ_p1 : public TTbarJetsBase_p1 {
  //  4-momentum is not stored, recomputed on T->P
};

#endif/*TTBARJJ_P1_H*/
