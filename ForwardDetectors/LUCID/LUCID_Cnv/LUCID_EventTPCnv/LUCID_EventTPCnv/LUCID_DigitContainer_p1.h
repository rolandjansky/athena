/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGITCONTAINER_P1_H
#define LUCID_DIGITCONTAINER_P1_H

// Persistent represenation of a LUCID_DigitContainer
// Author: Jacob Groth-Jensen

#include "AthenaPoolUtilities/TPObjRef.h"
#include "LUCID_Digit_p1.h"
#include <vector>
#include <string>

class LUCID_DigitContainer_p1: public std::vector<LUCID_Digit_p1> {
  
 public:
  
  LUCID_DigitContainer_p1() {};
};

#endif
