/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DerivationFrameworkTop/JetMSVAugmentation.h
 * @author Georges Aad
 * @date Nov. 2016
 * @brief tool to add some MSV variables to jets
*/


#ifndef DerivationFramework_JetMSVAugmentation_H
#define DerivationFramework_JetMSVAugmentation_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {


  class JetMSVAugmentation : public AthAlgTool, public IAugmentationTool {


  public:
    JetMSVAugmentation(const std::string& t, const std::string& n, const IInterface* p);
    ~JetMSVAugmentation();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;



  private:

    std::string m_jetCollectionName;
    std::string m_vtxAlgName;
    

  }; /// class

} /// namespace


#endif
