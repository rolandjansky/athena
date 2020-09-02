/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SelectionHelpers/SelectionAccessorExprBase.h"

namespace CP {

SelectionType SelectionAccessorExprBase::getBits(
    const SG::AuxElement& element) const {
  return getBool(element) ? selectionAccept() : selectionReject();
}

void SelectionAccessorExprBase::setBool(SG::AuxElement& /*element*/,
                                        bool /*value*/) const {
  throw std::runtime_error(
      "setting not supported for CP::SelectionAccessorExprBase");
}

void SelectionAccessorExprBase::setBits(SG::AuxElement& /*element*/,
                                        SelectionType /*selection*/) const {
  throw std::runtime_error(
      "setting not supported for CP::SelectionAccessorExprBase");
}

}  // namespace CP


