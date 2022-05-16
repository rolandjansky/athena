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

void SelectionAccessorExprBase::setBool(const SG::AuxElement& /*element*/,
                                        bool /*value*/,
                                        const CP::SystematicSet * /*sys*/) const {
  throw std::runtime_error(
      "setting not supported for CP::SelectionAccessorExprBase");
}

void SelectionAccessorExprBase::setBits(const SG::AuxElement& /*element*/,
                                        SelectionType /*selection*/,
                                        const CP::SystematicSet * /*sys*/) const {
  throw std::runtime_error(
      "setting not supported for CP::SelectionAccessorExprBase");
}

}  // namespace CP


