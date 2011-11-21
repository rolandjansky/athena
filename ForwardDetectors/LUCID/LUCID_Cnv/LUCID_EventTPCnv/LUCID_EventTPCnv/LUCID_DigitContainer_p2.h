/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGITCONTAINER_P2_H
#define LUCID_DIGITCONTAINER_P2_H

// Persistent represenation of a LUCID_DigitContainer
// Author: Jacob Groth-Jensen

#include "AthenaPoolUtilities/TPObjRef.h"
#include "LUCID_Digit_p2.h"
#include <vector>
#include <string>

class LUCID_DigitContainer_p2: public std::vector<LUCID_Digit_p2> {
  
 public:
  
  LUCID_DigitContainer_p2() {};
};

#endif
