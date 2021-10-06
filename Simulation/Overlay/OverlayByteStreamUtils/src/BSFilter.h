/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYBYTESTREAMUTILS_BSFILTER_H
#define OVERLAYBYTESTREAMUTILS_BSFILTER_H

#include <string>
#include <map>

#include "GaudiKernel/EventIDBase.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

class BSFilter : public AthAlgorithm
{
 public:

  BSFilter( const std::string& name, ISvcLocator* pSvcLocator );
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  void writeEventIdFile(const EventIDBase& evt) const;

  Gaudi::Property<std::string> m_l1ZB{
    this, "L1Trigger", "L1_ZB", "Name of the L1 zero-bias trigger"};
  Gaudi::Property<std::string> m_noalg{
    this, "noalgTrigger", "HLT_noalg_zb_L1ZB", "Name of the 'noalg' zero-bias trigger"};
  Gaudi::Property<std::string> m_j40{
    this, "j40Trigger", "HLT_j40_L1ZB", "Name of the 'j40' zero-bias trigger"};
  Gaudi::Property<std::string> m_filterfile{
    this, "filterfile", {}, "Output file with event IDs"};
  Gaudi::Property<std::string> m_EventIdFile{
    this, "EventIdFile", {}, "Output file with event IDs for EvtIdModifierSvc"};

  ServiceHandle< TrigConf::ITrigConfigSvc > m_trigConf{
    this, "TrigConfigSvc", "TrigConf::xAODConfigSvc/xAODConfigSvc"};
  PublicToolHandle<Trig::TrigDecisionTool> m_trigDecTool{
    this, "TrigDecisionTool", "Trig::TrigDecisionTool/TrigDecisionTool"};

  int m_All{0};
  int m_pass{0};

  struct filterinfo{
    int trig{0};
    int nvtx{0};
    int magic{99999};
    double dt{0.0};
  };
  std::map<int, std::map<uint64_t, filterinfo>> m_filtermap;

  FILE* m_efile{nullptr};
  FILE* m_ffile{nullptr};
};
#endif // OVERLAYBYTESTREAMUTILS_BSFILTER_H
