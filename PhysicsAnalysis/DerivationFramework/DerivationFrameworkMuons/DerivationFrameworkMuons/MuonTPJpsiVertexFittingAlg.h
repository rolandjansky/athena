// MuonTPAlg.h 

#ifndef MuonTPJpsiVertexFittingAlg__H
#define MuonTPJpsiVertexFittingAlg__H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework{
    class IAugmentationTool;
}

// algorithm that calls the jpsi finder and applies BPhys decorations to the vertices
// essentially takes the role of the derivation kernel when running in AOD analysis mode 
// in AthAnalysisBase, this alg has no function 
class ITHistSvc;
class IMuonTPTool;

class MuonTPJpsiVertexFittingAlg : public AthAlgorithm { 

public:
  MuonTPJpsiVertexFittingAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  ~MuonTPJpsiVertexFittingAlg(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

private: 

  /// Default constructor: 
  MuonTPJpsiVertexFittingAlg();

protected:

# if !defined(XAOD_ANALYSIS)    
    // the augmentation tools which we want to run
    ToolHandleArray <DerivationFramework::IAugmentationTool> m_augmentation_tools;
#endif

};


#endif //MuonTPJpsiVertexFittingAlg__H
