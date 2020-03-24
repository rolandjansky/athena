/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/SoftMuonSelectionBase.h"

namespace top {
  SoftMuonSelectionBase::SoftMuonSelectionBase() {
  }

  SoftMuonSelectionBase::~SoftMuonSelectionBase() {
  }

  std::ostream& operator << (std::ostream& os, const top::SoftMuonSelectionBase& selection) {
    selection.print(os);
    return os;
  }
}
