/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SelectionHelpers/SelectionAccessorExprBase.h"

namespace CP {

SelectionType SelectionAccessorExprBase::getBits(
    const SG::AuxElement& element,
    const CP::SystematicSet *sys) const {
  return getBool(element,sys) ? selectionAccept() : selectionReject();
}

}  // namespace CP


