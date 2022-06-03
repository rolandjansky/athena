/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SelectionHelpers/SelectionAccessorExprOr.h"

#include "PATInterfaces/SystematicSet.h"

namespace CP {

SelectionAccessorExprOr::SelectionAccessorExprOr(
    std::unique_ptr<ISelectionReadAccessor> left,
    std::unique_ptr<ISelectionReadAccessor> right)
    : m_left(std::move(left)), m_right(std::move(right)) {}

bool SelectionAccessorExprOr::getBool(const SG::AuxElement& element,
                                      const CP::SystematicSet *sys) const {
  return m_left->getBool(element,sys) || m_right->getBool(element,sys);
}

std::string SelectionAccessorExprOr::label() const {
  return "( " + m_left->label() + " || " + m_right->label() + " )";
}

CP::SystematicSet SelectionAccessorExprOr ::
getInputAffecting (const ISystematicsSvc& svc,
                   const std::string& objectName) const
{
  CP::SystematicSet result = m_left->getInputAffecting (svc, objectName);
  result.insert (m_right->getInputAffecting (svc, objectName));
  return result;
}

StatusCode SelectionAccessorExprOr ::
fillSystematics (const ISystematicsSvc& svc,
                 const std::vector<CP::SystematicSet>& sysList,
                 const std::string& objectName)
{
  using namespace msgSelectionHelpers;

  ANA_CHECK (m_left->fillSystematics (svc, sysList, objectName));
  ANA_CHECK (m_right->fillSystematics (svc, sysList, objectName));
  return StatusCode::SUCCESS;
}

}  // namespace CP

