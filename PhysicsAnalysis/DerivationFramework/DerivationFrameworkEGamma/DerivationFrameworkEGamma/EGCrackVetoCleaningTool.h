/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGCrackVetoCleaningTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGCRACKVETOCLEANINGTOOL_H
#define DERIVATIONFRAMEWORK_EGCRACKVETOCLEANINGTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
//
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/WriteHandleKey.h"
//
#include "xAODEgamma/EgammaContainer.h"

namespace DerivationFramework {

class EGCrackVetoCleaningTool
  : public AthAlgTool
  , public IAugmentationTool
{
public:
  EGCrackVetoCleaningTool(const std::string& t,
                          const std::string& n,
                          const IInterface* p);

  StatusCode initialize() override final;
  virtual StatusCode addBranches() const override final;

private:
  SG::ReadHandleKey<xAOD::EgammaContainer> m_containerName{ this,
                                                            "ContainerName",
                                                            "",
                                                            "Input" };

  // Write decoration handle keys
  // these are not really configuarable
  SG::WriteDecorHandleKey<xAOD::EgammaContainer> m_decoratorPass{};
  std::string m_sgName;
};
}

#endif // DERIVATIONFRAMEWORK_EGCRACKVETOCLEANINGTOOL_H
