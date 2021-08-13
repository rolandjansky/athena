/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_JROISUNPACKINGTOOL_H
#define HLTSEEDING_JROISUNPACKINGTOOL_H 1

#include "RoIsUnpackingToolBase.h"
#include "TrigT1Interfaces/RecJetRoI.h"

#include <string>

class JRoIsUnpackingTool : public RoIsUnpackingToolBase {
public:

  JRoIsUnpackingTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  using RoIsUnpackingToolBase::unpack;
  StatusCode unpack(const EventContext& ctx,
                    const ROIB::RoIBResult& roib,
                    const HLT::IDSet& activeChains) const override;

  virtual StatusCode initialize() override;
  virtual StatusCode start() override;

private:
  SG::WriteHandleKey< DataVector<LVL1::RecJetRoI> > m_recRoIsKey{
    this, "OutputRecRoIs", "HLT_RecJETRoIs",
    "Name of the RoIs object produced by the unpacker"};

  Gaudi::Property<float> m_roIWidth{
    this, "RoIWidth", 0.4, "Size of RoI in eta/ phi"};

  Gaudi::Property<std::string> m_fsRoIKey{
    this, "FSRoIKey", "HLT_FSRoI",
    "The key of FS RoI made earlier by the HLTSeeding" };
};

#endif //> !HLTSEEDING_JROISUNPACKINGTOOL_H
