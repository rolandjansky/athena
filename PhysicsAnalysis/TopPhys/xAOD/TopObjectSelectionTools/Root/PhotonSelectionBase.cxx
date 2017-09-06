/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/PhotonSelectionBase.h"

#include <iostream>

namespace top {

PhotonSelectionBase::PhotonSelectionBase() {
}

PhotonSelectionBase::~PhotonSelectionBase() {
}

}

std::ostream& operator<<(std::ostream& os, const top::PhotonSelectionBase& selection) {
    selection.print(os);
    return os;
}
