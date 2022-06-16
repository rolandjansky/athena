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
  SelectionAccessorExprOr(std::unique_ptr<ISelectionReadAccessor> left,
                          std::unique_ptr<ISelectionReadAccessor> right);

  /// @param element AuxElement to evaluate the selection on
  /// @return The result of the expression `left OR right`
  virtual bool getBool(const SG::AuxElement &element,
                       const CP::SystematicSet *sys) const override;

  /// @return Returns a readable and parseable representation
  virtual std::string label() const override;

  virtual CP::SystematicSet
  getInputAffecting (const ISystematicsSvc& svc,
                     const std::string& objectName) const override;

  virtual StatusCode
  fillSystematics (const ISystematicsSvc& svc,
                   const std::vector<CP::SystematicSet>& sysList,
                   const std::string& objectName) override;

 private:
  std::unique_ptr<ISelectionReadAccessor> m_left;
  std::unique_ptr<ISelectionReadAccessor> m_right;
};

}  // namespace CP

#endif
