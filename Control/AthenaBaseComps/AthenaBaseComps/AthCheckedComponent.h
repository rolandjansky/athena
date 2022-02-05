/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENABASECOMPS_ATHCHECKEDCOMPONENT_H
#define ATHENABASECOMPS_ATHCHECKEDCOMPONENT_H

#include "GaudiKernel/StatusCode.h"

/**
 * Mixin class to perform additional checks on a component.
 *
 * Use this only in case additional checks are supposed to be performed
 * during the component initialization.
 *
 * Can be used on Algorithms, Tools and Services like so:
 * @code
 *   class MyTool : public AthCheckedComponent<AthAlgTool> {
 *   };
 * @endcode
 *
 * @author Frank Winklmeier
 */
template <class PBASE>
class AthCheckedComponent : public PBASE {
public:
  using PBASE::PBASE;

  /**
   * Perform system initialization for the component.
   *
   * We overwrite this to call the additional checkers below.
   */
  virtual StatusCode sysInitialize() override;

private:
  /**
   * Check if component is configured in JobOptionsSvc.
   *
   * Checks if at least one property of the component is configured in
   * the JobOptionsSvc. Can be used to detect if a component is missing its
   * python configuration.
   */
  StatusCode isConfigured();
};

#include "AthCheckedComponent.icc"

#endif
