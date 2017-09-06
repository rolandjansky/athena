/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/JetSelectionBase.h"

namespace top {

JetSelectionBase::JetSelectionBase() {
}

JetSelectionBase::~JetSelectionBase() {
}

}

std::ostream& operator<<(std::ostream& os, const top::JetSelectionBase& selection) {
    selection.print(os);
    return os;
}
