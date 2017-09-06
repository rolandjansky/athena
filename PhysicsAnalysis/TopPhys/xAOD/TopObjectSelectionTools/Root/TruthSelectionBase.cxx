/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/TruthSelectionBase.h"

#include "TopEvent/EventTools.h"

#include <cmath>
#include <iostream>

namespace top {

  TruthSelectionBase::TruthSelectionBase() {
  }

  bool TruthSelectionBase::isBhadron(int pid) const {
    pid = (int) std::fabs(pid);
    if ( (pid >= 511 && pid <= 545) || (pid >= 10511 && pid <= 10545) || (pid >= 20511 && pid <= 20545) || (pid >= 5112 && pid <= 5554)) {  // check if it's a B-hadron
      return true;
    }
    return false;
  }

  bool TruthSelectionBase::isChadron(int pid) const {
    pid = (int) std::fabs(pid);
    if ( (pid >= 400 && pid < 500 && pid != 443) || (pid >= 4000 && pid < 5000) || (pid >= 10411 && pid <= 10455) || (pid >= 20411 && pid <= 20445)) {  // check if it's a C-hadron
      return true;
    }
    return false;
  }

}

std::ostream& operator<<(std::ostream& os, const top::TruthSelectionBase& selection) {
    selection.print(os);
    return os;
}
