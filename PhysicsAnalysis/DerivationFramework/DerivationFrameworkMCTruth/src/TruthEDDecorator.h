/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file TruthEDDecorator.h
 * @author Zach Marshall
 * @date Nov 2019
 * @brief tool to decorate EventInfo with truth-level energy density
*/

#ifndef DerivationFrameworkMCTruth_TruthEDDecorator_H
#define DerivationFrameworkMCTruth_TruthEDDecorator_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// Members
#include "AthContainers/AuxElement.h"

// STL includes
#include <string>
#include <vector>

namespace DerivationFramework {

  class TruthEDDecorator : public AthAlgTool, public IAugmentationTool {

  public:
    TruthEDDecorator(const std::string& t, const std::string& n, const IInterface* p);
    ~TruthEDDecorator();
    virtual StatusCode addBranches() const override final;
    StatusCode initialize() override final;

  private:
    std::string m_eventInfoName;
    std::vector<std::string> m_edKeys;
    std::string m_ed_suffix;
    std::vector<SG::AuxElement::Decorator<float> > m_dec_eventShape;
  }; /// class

} /// namespace

#endif