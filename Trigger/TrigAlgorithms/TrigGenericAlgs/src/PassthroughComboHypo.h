/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGENERICALGS_PASSTHROUGHCOMBOHYPO_H
#define TRIGGENERICALGS_PASSTHROUGHCOMBOHYPO_H 1

#include "DecisionHandling/ComboHypo.h"

/**
 * @class PassthroughComboHypo
 * @brief Generic slot-in ComboHypo class to be used for cases where all combined logic has already been applied
 * Copies all input collections to output, unchanged.
 */
class PassthroughComboHypo: public ::ComboHypo {
 public:
  PassthroughComboHypo(const std::string& name, ISvcLocator* pSvcLocator);
  PassthroughComboHypo() = delete;
  virtual ~PassthroughComboHypo() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
};

#endif  // TRIGGENERICALGS_PASSTHROUGHCOMBOHYPO_H