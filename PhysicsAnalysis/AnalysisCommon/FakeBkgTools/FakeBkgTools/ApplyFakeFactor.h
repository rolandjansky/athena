/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LeptonFakeFactorTools/ApplyFakeFactor.h
 * @authors Joseph Reichert <Joseph.Reichert@cern.ch>
 * @date August 2016
 * @brief Tool to read fake factor histograms and return 
 * the binned fake factor value
**/

#ifndef ApplyFakeFactor_H
#define ApplyFakeFactor_H

#include <FakeBkgTools/BaseLinearFakeBkgTool.h>

namespace CP
{

class ApplyFakeFactor : public CP::BaseLinearFakeBkgTool
{

  ASG_TOOL_CLASS2(ApplyFakeFactor, ILinearFakeBkgTool, IFakeBkgTool)

public: 
  /// Standard constructor
  ApplyFakeFactor(const std::string& name);
  
  /// Standard destructor
  ~ApplyFakeFactor();

  // Main methods
public:
  // Initialize this class
  
  virtual StatusCode initialize() override;

protected:
  // Methods which must be implemented
  virtual StatusCode addEventCustom() override;
  virtual StatusCode getEventWeightCustom(FakeBkgTools::Weight& weight, const FakeBkgTools::FinalState& fs) override;

  /// This indicates which type of efficiencies/fake factor need to be filled
  virtual FakeBkgTools::Client clientForDB() override final;
  
private:

  bool eventSatisfiesCriteria(const FakeBkgTools::FinalState& fs, unsigned long& expectedTightLeptonsBitset);
};

}

//----------------------------------------------------------------------------------------
#endif
