/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DerivationFrameworkTop/TopHeavyFlavorFilterAugmentation.h
 * @author Georges Aad
 * @date Apr. 2015
 * @brief tool to add a variable to the EventInfo corresponding to the ttbar+HF filter flag
*/


#ifndef DerivationFramework_TopHeavyFlavorFilterAugmentation_H
#define DerivationFramework_TopHeavyFlavorFilterAugmentation_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class TTbarPlusHeavyFlavorFilterTool;
  
  class TopHeavyFlavorFilterAugmentation : public AthAlgTool, public IAugmentationTool {


  public:
    TopHeavyFlavorFilterAugmentation(const std::string& t, const std::string& n, const IInterface* p);
    ~TopHeavyFlavorFilterAugmentation();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;



  private:

    std::string m_eventInfoName;
    ToolHandle<DerivationFramework::TTbarPlusHeavyFlavorFilterTool> m_filterTool;
    

  }; /// class

} /// namespace


#endif
