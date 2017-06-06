/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/TauSelectionBase.h"

namespace top {

TauSelectionBase::TauSelectionBase() {
}

TauSelectionBase::~TauSelectionBase() {
}

}

std::ostream& operator<<(std::ostream& os, const top::TauSelectionBase& selection) {
    selection.print(os);
    return os;
}
