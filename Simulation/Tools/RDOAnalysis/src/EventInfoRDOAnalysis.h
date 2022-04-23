/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_INFO_RDO_ANALYSIS_H
#define EVENT_INFO_RDO_ANALYSIS_H

#include <AthenaBaseComps/AthAlgorithm.h>
#include <GaudiKernel/ServiceHandle.h>
#include <GaudiKernel/ITHistSvc.h>
#include <StoreGate/ReadHandleKey.h>
#include <xAODEventInfo/EventInfo.h>

#include <string>
#include <vector>

class TH1;
class TTree;

class EventInfoRDOAnalysis : public AthAlgorithm
{

public:
  EventInfoRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey {this, "EventInfo", "EventInfo", "Input event info key name"};

  Gaudi::Property<std::string> m_histPath {this, "HistPath", "/RDOAnalysis/histos/", ""};
  Gaudi::Property<std::string> m_ntuplePath {this, "NtuplePath", "/RDOAnalysis/ntuples/", ""};
  Gaudi::Property<std::string> m_ntupleName {this, "NtupleName", "EventInfo", ""};

  ServiceHandle<ITHistSvc> m_thistSvc {this, "HistSvc", "THistSvc", ""};

  // Branches
  uint32_t m_runNumber{};
  unsigned long long m_eventNumber{};
  uint32_t m_lumiBlock{};
  uint32_t m_bcid{};

  float m_actualInteractionsPerCrossing{};
  float m_averageInteractionsPerCrossing{};

  float m_beamPosX{};
  float m_beamPosY{};
  float m_beamPosZ{};

  float m_beamPosSigmaX{};
  float m_beamPosSigmaY{};
  float m_beamPosSigmaZ{};

  // Histograms
  TH1 *m_h_actualInteractionsPerCrossing{};
  TH1 *m_h_averageInteractionsPerCrossing{};

  TTree *m_tree{};
};

#endif // EVENT_INFO_RDO_ANALYSIS_H
