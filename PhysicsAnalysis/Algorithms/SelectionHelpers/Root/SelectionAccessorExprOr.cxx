/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SelectionHelpers/SelectionAccessorExprOr.h"

namespace CP {

SelectionAccessorExprOr::SelectionAccessorExprOr(
    std::unique_ptr<ISelectionAccessor> left,
    std::unique_ptr<ISelectionAccessor> right)
    : m_left(std::move(left)), m_right(std::move(right)) {}

bool SelectionAccessorExprOr::getBool(const SG::AuxElement& element) const {
  return m_left->getBool(element) || m_right->getBool(element);
}

std::string SelectionAccessorExprOr::label() const {
  return "( " + m_left->label() + " || " + m_right->label() + " )";
}

}  // namespace CP

