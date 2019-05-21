/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_BCDISTANCEAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_BCDISTANCEAUGMENTATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTruth/TruthEventContainer.h"
#include "GammaORTools/VGammaORTool.h"

#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

namespace DerivationFramework {

  class BCDistanceAugmentationTool : public AthAlgTool, public IAugmentationTool {

    public: 
      BCDistanceAugmentationTool( const std::string& t, const std::string& n, const IInterface* p );

      ~BCDistanceAugmentationTool();

      StatusCode  initialize();
      StatusCode  finalize();

      virtual StatusCode addBranches() const;

    private:

      ToolHandle<Trig::IBunchCrossingTool> m_bcTool;
      //int32_t                        m_runNumber;
      ToolHandle<VGammaORTool>            m_PhIsoTool;

      // for the flavour filtering
      const std::vector<float> computeTruthInfo(const xAOD::TruthEventContainer* xTruthEventContainer) const;
      bool isBwithWeakDK(const int pID) const;
      bool isDwithWeakDK(const int pID) const;

  }; 

}

#endif
