/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASYMPT_MATRIX_TOOL_H
#define ASYMPT_MATRIX_TOOL_H

#include "FakeBkgTools/BaseLinearFakeBkgTool.h"

#include <array>
#include <vector>
#include <string>
#include <bitset>
#include <unordered_map>
#include <functional>

namespace CP
{

class AsymptMatrixTool : public CP::BaseLinearFakeBkgTool
{
    ASG_TOOL_CLASS2(AsymptMatrixTool, ILinearFakeBkgTool, IFakeBkgTool)

  public:
    AsymptMatrixTool(const std::string& name);
    virtual ~AsymptMatrixTool();
    virtual StatusCode initialize() override;
  protected:
    virtual StatusCode addEventCustom() override;
    virtual StatusCode getEventWeightCustom(FakeBkgTools::Weight& weight, const FakeBkgTools::FinalState& fs) override;

    std::vector<std::array<double,2>> m_components; //!
    std::vector<std::array<double,2>> m_derivatives; //!
  protected:
    /// This indicates which type of efficiencies/fake factor need to be filled
    virtual FakeBkgTools::Client clientForDB() override;
};

}

#endif
