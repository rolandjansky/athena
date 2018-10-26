/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Tool to decorate the Electrons object with additional information for merged electron ID 
// Authors: A.Morley
	
#ifndef DerivationFrameworkHiggs_MergedElectronDetailsDecorator_H
#define DerivationFrameworkHiggs_MergedElectronDetailsDecorator_H

#include <string>
#include <vector>
#include <TEnv.h>
#include <TString.h>
#include <TSystem.h>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class IEMExtrapolationTools;

namespace DerivationFramework {
  
  class MergedElectronDetailsDecorator : public AthAlgTool, public IAugmentationTool {
        
  public:
    MergedElectronDetailsDecorator(const std::string& t, const std::string& n, const IInterface* p);
    ~MergedElectronDetailsDecorator();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;

  private:

    void fillMatchDetails( std::vector<float>& trkMatchTrk, const  xAOD::TrackParticle* tp, const xAOD::CaloCluster* cluster) const;

    ToolHandle<IEMExtrapolationTools> m_emExtrapolationTool; 

    float m_minET;
    
  }; /// class
  
} /// namespace

#endif
