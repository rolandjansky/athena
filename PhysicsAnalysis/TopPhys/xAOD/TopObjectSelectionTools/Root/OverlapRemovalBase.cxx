/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/OverlapRemovalBase.h"

namespace top {

OverlapRemovalBase::OverlapRemovalBase() {
}

OverlapRemovalBase::~OverlapRemovalBase() {
}

}

std::ostream& operator<<(std::ostream& os, const top::OverlapRemovalBase& overlapremoval) {
    overlapremoval.print(os);
    return os;
}
