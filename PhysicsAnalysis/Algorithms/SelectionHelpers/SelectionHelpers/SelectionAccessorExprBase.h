/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_EXPR_BASE_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_EXPR_BASE_H

#include "SelectionHelpers/ISelectionReadAccessor.h"

namespace CP {

/// @brief Serves as the base for a few logical expression classes.
class SelectionAccessorExprBase : public ISelectionReadAccessor {
  // leave getBool pure virtual
  // leave label pure virtual

 public:
  virtual SelectionType getBits(const SG::AuxElement &element,
                                const CP::SystematicSet *sys) const override;
};

}  // namespace CP

#endif
