/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/FwdElectronSelectionBase.h"

#include <iostream>

namespace top {
  FwdElectronSelectionBase::FwdElectronSelectionBase() {
  }

  FwdElectronSelectionBase::~FwdElectronSelectionBase() {
  }

  std::ostream& operator << (std::ostream& os, const top::FwdElectronSelectionBase& selection) {
    selection.print(os);
    return os;
  }
}
