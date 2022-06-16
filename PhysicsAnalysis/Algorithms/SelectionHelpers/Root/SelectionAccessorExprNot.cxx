/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SelectionHelpers/SelectionAccessorExprNot.h"

#include "PATInterfaces/SystematicSet.h"

namespace CP {

SelectionAccessorExprNot::SelectionAccessorExprNot(
    std::unique_ptr<ISelectionReadAccessor> child)
    : m_child(std::move(child)) {}

bool SelectionAccessorExprNot::getBool(const SG::AuxElement& element,
                                       const CP::SystematicSet *sys) const {
  return !m_child->getBool(element,sys);
}

std::string SelectionAccessorExprNot::label() const {
  return "!" + m_child->label();
}

CP::SystematicSet SelectionAccessorExprNot ::
getInputAffecting (const ISystematicsSvc& svc,
                   const std::string& objectName) const
{
  return m_child->getInputAffecting (svc, objectName);
}

StatusCode SelectionAccessorExprNot ::
fillSystematics (const ISystematicsSvc& svc,
                 const std::vector<CP::SystematicSet>& sysList,
                 const std::string& objectName)
{
  return m_child->fillSystematics (svc, sysList, objectName);
}

}  // namespace CP
