/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TauOverlappingElectronLLHDecoratorWrapper.h
 * @author Z. Zinonos - zenon@cern.ch
 * @date November 2015
 */

#ifndef DERIVATIONFRAMEWORK_TAUOVERLAPPINGELECTRONLLHDECORATORWRAPPER_H
#define DERIVATIONFRAMEWORK_TAUOVERLAPPINGELECTRONLLHDECORATORWRAPPER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

//#include "TauAnalysisTools/ITauOverlappingElectronLLHDecorator.h"
//#include "xAODTau/TauJetContainer.h"
//#include "xAODEgamma/ElectronContainer.h"

/**
 * @short Forward declarations
 */
namespace TauAnalysisTools { class ITauOverlappingElectronLLHDecorator; }

/**
 * @class TauOverlappingElectronLLHDecoratorWrapper
 * @brief wrapper tool for decorating reconstructed taus with a likelihood score of matched reconstructed electrons
 */

namespace DerivationFramework {

  class TauOverlappingElectronLLHDecoratorWrapper : public AthAlgTool, public IAugmentationTool {

    public: 
      TauOverlappingElectronLLHDecoratorWrapper(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_tauContainerName;
      std::string m_electronContainerName;

      ToolHandle < TauAnalysisTools::ITauOverlappingElectronLLHDecorator > m_TauOverlappingElectronLLHDecorator;

  }; 
}

#endif

