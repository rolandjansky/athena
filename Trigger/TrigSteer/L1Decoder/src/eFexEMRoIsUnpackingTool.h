/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_EFEXEMROISUNPACKINGTOOL_H
#define L1DECODER_EFEXEMROISUNPACKINGTOOL_H 1

#include "RoIsUnpackingToolBase.h"
#include "TrigConfData/L1Menu.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

class eFexEMRoIsUnpackingTool : public RoIsUnpackingToolBase {
public:
  eFexEMRoIsUnpackingTool(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  using RoIsUnpackingToolBase::unpack;
  virtual StatusCode unpack(const EventContext& ctx,
                            const xAOD::TrigComposite& l1TriggerResult,
                            const HLT::IDSet& activeChains) const override;

  virtual StatusCode initialize() override;
  virtual StatusCode start() override;

private:
  SG::ReadHandleKey<TrigConf::L1Menu> m_l1MenuKey{
    this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu",
    "Name of the L1Menu object to read configuration from"};

  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigRoIsKey{
    this, "OutputTrigRoIs", "HLT_eEMRoIs",
    "Name of the TrigRoiDescriptor collection produced by the unpacker"};

  Gaudi::Property<std::string> m_eFexEMRoILinkName {
    this, "eFexEMRoILinkName", "L1_eEMRoI",
    "Name of the link to read from L1TriggerResult for eFexEMRoI container"};

  Gaudi::Property<float> m_roiWidth{
    this, "RoIWidth", 0.1, "Size of RoI in eta/phi"};
};

#endif //> !L1DECODER_EFEXEMROISUNPACKINGTOOL_H
