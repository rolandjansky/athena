/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <SelectionHelpers/SelectionAccessorExpr.h>

namespace CP {

SelectionAccessorExpr::SelectionAccessorExpr(
    std::unique_ptr<SelectionExprParser::BooleanExpression> ex,
    std::unordered_map<std::string, std::unique_ptr<ISelectionAccessor>>
        accessors)
    : m_expression(std::move(ex)), m_accessors(std::move(accessors)) {
  m_label = m_expression->toString();
}

SelectionType SelectionAccessorExpr::getBits(
    const SG::AuxElement& element) const {
  return getBool(element) ? selectionAccept() : selectionReject();
}

void SelectionAccessorExpr ::setBits(SG::AuxElement& /*element*/,
                                     SelectionType /*selection*/) const {
  throw std::runtime_error(
      "setting not supported for CP::SelectionAccessorExpr");
}

bool SelectionAccessorExpr::getBool(const SG::AuxElement& element) const {
  return m_expression->evaluate([&](const std::string& key) {
    const ISelectionAccessor& accessor = *m_accessors.at(key);
    return accessor.getBool(element);
  });
}

void SelectionAccessorExpr ::setBool(SG::AuxElement& /*element*/,
                                     bool /*selection*/) const {
  throw std::runtime_error(
      "setting not supported for CP::SelectionAccessorExpr");
}

std::string SelectionAccessorExpr ::label() const { return m_label; }
}  // namespace CP
