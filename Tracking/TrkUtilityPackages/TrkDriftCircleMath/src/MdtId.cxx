/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/MdtId.h"

// DR (clang32) operator<< in namespace
std::ostream& TrkDriftCircleMath::operator<<(std::ostream& os, const TrkDriftCircleMath::MdtId& id) {
    os << " ml " << id.ml() << " lay " << id.lay() << " tube " << id.tube();
    return os;
}
