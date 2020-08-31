/*                                                                                                                                                                                                       
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                                                                                                                
*/

#include "TopObjectSelectionTools/JetGhostTrackSelectionBase.h"

namespace top {
  JetGhostTrackSelectionBase::JetGhostTrackSelectionBase() {
  }

  JetGhostTrackSelectionBase::~JetGhostTrackSelectionBase() {
  }

  std::ostream& operator << (std::ostream& os, const top::JetGhostTrackSelectionBase& selection) {
    selection.print(os);
    return os;
  }
}
