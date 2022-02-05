/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_TAUROISUNPACKINGTOOL_H
#define HLTSEEDING_TAUROISUNPACKINGTOOL_H

#include "RoIsUnpackingToolBase.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"

class TAURoIsUnpackingTool : public RoIsUnpackingToolBase { 
public: 
  TAURoIsUnpackingTool(const std::string& type,
                       const std::string& name, 
                       const IInterface* parent);

  using RoIsUnpackingToolBase::unpack;
  StatusCode unpack(const EventContext& ctx,
                    const ROIB::RoIBResult& roib,
                    const HLT::IDSet& activeChains) const override;
  
  virtual StatusCode initialize() override;
  virtual StatusCode start() override;  

private: 
  SG::WriteHandleKey<DataVector<LVL1::RecEmTauRoI>> m_recRoIsKey{
    this, "OutputRecRoIs", "HLT_RecTAURoIs", "Name of the RoIs object produced by the unpacker"};

  Gaudi::Property<float> m_roIWidthEta {
    this, "RoIWidthEta", 0.4, "Size of RoI in eta"};

  Gaudi::Property<float> m_roIWidthPhi {
    this, "RoIWidthPhi", M_PI/8., "Size of RoI in phi"};
}; 

#endif //> !HLTSEEDING_TAUROISUNPACKINGTOOL_H
