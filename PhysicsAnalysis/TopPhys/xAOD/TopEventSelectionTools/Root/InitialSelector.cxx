/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/InitialSelector.h"

namespace top {

bool InitialSelector::apply(const top::Event&) const {
    return true;
}

std::string InitialSelector::name() const {
    return "INITIAL";
}

}
