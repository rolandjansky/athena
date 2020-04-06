/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_EXPR_OR_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_EXPR_OR_H

#include "SelectionHelpers/SelectionAccessorExprBase.h"

#include <memory>

namespace CP {

/// @brief Implements a SelectionAccessor that performs a binary logical OR
class SelectionAccessorExprOr : public SelectionAccessorExprBase {
 public:
   /// @param left Left argument to the OR expression
   /// @param Right Right argument to the OR expression
  SelectionAccessorExprOr(std::unique_ptr<ISelectionAccessor> left,
                          std::unique_ptr<ISelectionAccessor> right);

  /// @param element AuxElement to evaluate the selection on
  /// @return The result of the expression `left OR right`
  virtual bool getBool(const SG::AuxElement &element) const override;

  /// @return Returns a readable and parseable representation
  virtual std::string label() const override;

 private:
  std::unique_ptr<ISelectionAccessor> m_left;
  std::unique_ptr<ISelectionAccessor> m_right;
};

}  // namespace CP

#endif
