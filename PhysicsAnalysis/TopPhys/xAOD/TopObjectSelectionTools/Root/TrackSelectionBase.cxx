/*                                                                                                                                                                                                       
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                                                                                                                
*/

#include "TopObjectSelectionTools/TrackSelectionBase.h"

namespace top {
  TrackSelectionBase::TrackSelectionBase() {
  }

  TrackSelectionBase::~TrackSelectionBase() {
  }

  std::ostream& operator << (std::ostream& os, const top::TrackSelectionBase& selection) {
    selection.print(os);
    return os;
  }
}
