/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DerivationFrameworkTop/BoostedHadTopAndTopPairFilterAugmentation.h
 * @author Georges Aad
 * @date Apr. 2015
 * @brief tool to add a variable to the EventInfo corresponding to the ttbar+HF filter flag
*/


#ifndef DerivationFramework_BoostedHadTopAndTopPairFilterAugmentation_H
#define DerivationFramework_BoostedHadTopAndTopPairFilterAugmentation_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class BoostedHadTopAndTopPairFilterTool;
  
  class BoostedHadTopAndTopPairFilterAugmentation : public AthAlgTool, public IAugmentationTool {


  public:
    BoostedHadTopAndTopPairFilterAugmentation(const std::string& t, const std::string& n, const IInterface* p);
    ~BoostedHadTopAndTopPairFilterAugmentation();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;



  private:

    std::string m_eventInfoName;
    ToolHandle<DerivationFramework::BoostedHadTopAndTopPairFilterTool> m_filterTool_High;
    ToolHandle<DerivationFramework::BoostedHadTopAndTopPairFilterTool> m_filterTool_Low;

    

  }; /// class

} /// namespace


#endif
