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
      ToolHandle<VGammaORTool>             m_PhIsoTool;

  }; 

}

#endif
