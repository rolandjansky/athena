/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/MuonSelectionBase.h"

namespace top {

MuonSelectionBase::MuonSelectionBase() {
}

MuonSelectionBase::~MuonSelectionBase() {
}

}

std::ostream& operator<<(std::ostream& os, const top::MuonSelectionBase& selection) {
    selection.print(os);
    return os;
}
