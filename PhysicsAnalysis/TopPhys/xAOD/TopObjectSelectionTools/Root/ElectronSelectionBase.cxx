/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/ElectronSelectionBase.h"

#include <iostream>

namespace top {

ElectronSelectionBase::ElectronSelectionBase() {
}

ElectronSelectionBase::~ElectronSelectionBase() {
}

}

std::ostream& operator<<(std::ostream& os, const top::ElectronSelectionBase& selection) {
    selection.print(os);
    return os;
}
