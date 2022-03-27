/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauWPDecoratorWrapper.h
// Author: Guillermo Hamity (ghamity@cern.ch)
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TAUWPDECORATORWRAPPER_H
#define DERIVATIONFRAMEWORK_TAUWPDECORATORWRAPPER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "tauRecTools/TauWPDecorator.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTau/TauJetContainer.h"
#include "CxxUtils/CachedValue.h"

/**
 * wrapper tool for tau wp decoration
*/

namespace DerivationFramework {

  class TauWPDecoratorWrapper : public AthAlgTool, public IAugmentationTool {
    public:
      TauWPDecoratorWrapper(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_scoreNameTrans;
      std::vector<std::string> m_decorWPs;
      ToolHandle <TauWPDecorator> m_tTauWPDecorator;
      CxxUtils::CachedValue<bool> m_isDecorAvailable;
      SG::ReadHandleKey<xAOD::TauJetContainer> m_tauContainerKey { this, "TauContainerName", "TauJets", "Input tau container key" };

  };
}

#endif // DERIVATIONFRAMEWORK_TAUWPDECORATORWRAPPER_H
