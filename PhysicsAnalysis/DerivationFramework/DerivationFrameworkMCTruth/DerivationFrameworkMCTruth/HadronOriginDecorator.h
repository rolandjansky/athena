/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file DerivationFrameworkTop/TopHeavyFlavorFilterAugmentation.h
 * @date Apr. 2015
 * @brief tool to add a variable to the TruthParticles corresponding to the HF hadrons origin flag
 */
	
	
#ifndef DerivationFrameworkMCTruth_HadronOriginDecorator_H
#define DerivationFrameworkMCTruth_HadronOriginDecorator_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {
  
  class HadronOriginClassifier;
  
  class HadronOriginDecorator : public AthAlgTool, public IAugmentationTool {
    
    
  public:
    HadronOriginDecorator(const std::string& t, const std::string& n, const IInterface* p);
    ~HadronOriginDecorator();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;
    
    
    
  private:
    
    
    std::string m_TruthEventName;
    ToolHandle<DerivationFramework::HadronOriginClassifier> m_Tool;
    
    
  }; /// class
  
} /// namespace


#endif
