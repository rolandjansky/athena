/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/SaveEventSelector.h"

namespace top {

bool SaveEventSelector::apply(const top::Event&) const {
    return true;
}

std::string SaveEventSelector::name() const {
    return "SAVE";
}

}
