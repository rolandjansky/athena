/*
Copyright! (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIAS_MbtsHypoTool_H
#define TRIGMINBIAS_MbtsHypoTool_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"

class MbtsHypoTool : virtual public ::AthAlgTool
{
public:
  MbtsHypoTool(const std::string &type,
               const std::string &name,
               const IInterface *parent);

  virtual StatusCode initialize() override;

  struct MbtsHypoInfo
  {
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    const xAOD::TrigT2MbtsBits *mbtsBits;
    TrigCompositeUtils::Decision *decision;
  };

  StatusCode decide(MbtsHypoInfo &decisions) const;

private:
  bool applyCut(const Gaudi::Property<int> &threshold, const xAOD::TrigComposite *composit) const;
  HLT::Identifier m_decisionId;
  Gaudi::Property<bool> m_accptAll{this, "AcceptAll", false, "Accept all events"};

  Gaudi::Property<unsigned int> m_mbtsCounters{this, "MbtsCounters", 2, "The number of MBTS counters required to be above threshold"};

  Gaudi::Property<bool> m_coincidence{this, "Coincidence", false, "A flag to control if this is a coincidence requirement or not"};

  Gaudi::Property<bool> m_or{this, "Or", false, "Flag extending above choice to do Or between the two sides of MBTS"};

  Gaudi::Property<bool> m_veto{this, "Veto", false, "Flag to select lowe than threshold multiplicities"};

  Gaudi::Property<int> m_mbtsmode{this, "MBTSMode", 0, "Flag to count multiplicities only on inner or outer MBTS modules (see T2MBTSUtils) "};

  Gaudi::Property<float> m_threshold{this, "Threshold", 40.0 / 222.0, "Energy threshold in pC"};

  Gaudi::Property<float> m_timeCut{this, "TimeCut", -1.0, "A time cut in ns.  Values <= 0 disable the time cut"};

  Gaudi::Property<float> m_globalTimeOffset{this, "GlobalTimeOffset", 0, "A global time offset in ns about which the time window cuts"};

  Gaudi::Property<std::vector<float>> m_timeOffsets{this, "TimeOffset", std::vector<float>(32), "Offsets with respect to the global offset of all counters. (A0-15 then C0-C15) "};

  ToolHandle<GenericMonitoringTool> m_monTool{this, "MonTool", "", "Monitoring tool"};
  struct Counts
  {
    size_t sideA;
    size_t sideC;
  };
  Counts calculateMultiplicities(const xAOD::TrigT2MbtsBits *t2mbtsBits) const;
};

#endif //> !TRIGMINBIAS_MbtsHypoTool_H
