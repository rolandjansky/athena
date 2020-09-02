/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SelectionHelpers/SelectionAccessorExprNot.h"

namespace CP {

SelectionAccessorExprNot::SelectionAccessorExprNot(
    std::unique_ptr<ISelectionAccessor> child)
    : m_child(std::move(child)) {}

bool SelectionAccessorExprNot::getBool(const SG::AuxElement& element) const {
  return !m_child->getBool(element);
}

std::string SelectionAccessorExprNot::label() const {
  return "!" + m_child->label();
}

}  // namespace CP
