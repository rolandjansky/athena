/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGONLINEMONITOR_TRIGOPMONITOR_H
#define TRIGONLINEMONITOR_TRIGOPMONITOR_H

/**
 * @file   TrigOpMonitor.h
 * @author Frank Winklmeier
 * @date   Aug, 2019
 * @brief  Online operations monitoring
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IOVRange.h"
#include "EventInfo/EventID.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "TH1I.h"
#include "TH2I.h"
#include "TProfile.h"

#include <unordered_map>
#include <string>
#include <vector>

class TrigOpMonitor : public AthAlgorithm {
public:
  TrigOpMonitor(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode start() override;
  virtual StatusCode execute() override;

  /* Ensure this algorithm is a singleton.
     This should not cause any bottle-necks as the algorithm does very
     minimal work. */
  virtual unsigned int cardinality() const override { return 1; }

private:
  void fillMagFieldHist();
  void fillReleaseDataHist();
  void fillIOVDbHist();
  void fillIOVDbChangeHist(const EventContext& ctx);
  void fillLumiHist(const EventContext& ctx);

  StatusCode bookHists();

  /// Histograms for COOL folders
  struct FolderHist {
    TH1F* h_time{nullptr};
    TH1F* h_bytes{nullptr};
    float total_time{0};
    float total_bytes{0};
  };

  ServiceHandle<ITHistSvc> m_histSvc{this, "THistSvc", "THistSvc"};

  MagField::IMagFieldSvc* m_magFieldSvc{nullptr};
  IIOVDbSvc* m_IOVDbSvc{nullptr};

  TH2I* m_magFieldHist{nullptr};
  TH2I* m_iovChangeHist{nullptr};
  TH1I* m_releaseHist{nullptr};
  TProfile* m_lumiHist{nullptr};
  TProfile* m_muHist{nullptr};

  EventID::number_type m_previousLB{0};                     //!< LB of previous event
  std::unordered_map<std::string, IOVRange> m_currentIOVs;  //!< current IOVs managed by IOVDbSvc
  std::unordered_map<std::string, FolderHist> m_folderHist; //!< histograms for COOL folders
  std::string m_histPath;                                   //!< histogram booking path

  /* Properties */
  SG::ReadCondHandleKey<LuminosityCondData> m_luminosityCondDataKey{this, "LuminosityCondDataKey",
                                                                    "LuminosityCondData", ""};

  Gaudi::Property<std::string> m_releaseData{
      this, "ReleaseDataFile", "../ReleaseData",
      "Path to ReleaseData file (relative to LD_LIBRARY_PATH entries"};

  Gaudi::Property<bool> m_detailedHists{this, "DetailedFolderHists", true,
                                        "Detailed histograms for COOL folder updates during run"};

  Gaudi::Property<unsigned short int> m_maxLB{this, "MaxLumiblocks", 3000,
                                              "Number of lumiblocks for histograms"};

  Gaudi::Property<std::vector<std::string>> m_projects{
      this, "ProjectNames", {"AthenaP1", "Athena"}, "Valid projects names for release"};
};

#endif // TRIGSTEERMONITOR_TRIGOPMONI_H
