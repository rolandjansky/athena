/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_EXPR_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_EXPR_H

#include <SelectionHelpers/ISelectionAccessor.h>
#include <SelectionHelpers/SelectionExprParser.h>

#include <memory>
#include <unordered_map>

namespace CP {

/// \brief A \ref ISelectionAccessor which can interpret arbitrary boolean expressions

class SelectionAccessorExpr final : public ISelectionAccessor {
 public:

  SelectionAccessorExpr(
      std::unique_ptr<SelectionExprParser::BooleanExpression> ex,
      std::unordered_map<std::string, std::unique_ptr<ISelectionAccessor>>
          accessors);

  virtual SelectionType getBits(const SG::AuxElement& element) const override;

  virtual void setBits(SG::AuxElement& element,
                       SelectionType selection) const override;

  virtual bool getBool(const SG::AuxElement& element) const override;

  virtual void setBool(SG::AuxElement& element, bool selection) const override;

  virtual std::string label() const override;

 private:
  std::unique_ptr<SelectionExprParser::BooleanExpression> m_expression;
  std::unordered_map<std::string, std::unique_ptr<ISelectionAccessor>>
      m_accessors;
  std::string m_label;
};

}  // namespace CP

#endif
