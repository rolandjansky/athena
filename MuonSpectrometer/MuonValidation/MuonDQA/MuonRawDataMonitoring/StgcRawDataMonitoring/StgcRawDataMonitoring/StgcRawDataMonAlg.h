/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : sTgcRawDataMonAlg
// Author: Sebastian Fuenzalida Garrido
// Local supervisor: Edson Carquin Lopez
// Technical supervisor: Gerardo Vasquez
//
// DESCRIPTION:
// Subject: sTgc --> sTgc raw data monitoring
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef sTgcRawDataMonAlg_H
#define sTgcRawDataMonAlg_H

//Core Include
#include "AthenaMonitoring/AthMonitorAlgorithm.h"

//Helper Includes
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonPrepRawData/sTgcPrepData.h"

//stl includes                                                                                 
#include <string>

namespace Muon 
{
  class sTgcPrepData;
}

namespace GeometricSectors
{
  static const std::array<std::string, 2> sTgc_Side   = {"CSide", "ASide"};
}

class sTgcRawDataMonAlg: public AthMonitorAlgorithm 
{
 public:
  
  sTgcRawDataMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~sTgcRawDataMonAlg()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override;
  
 private:  

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
 
  void fillsTgcOverviewHistograms(const Muon::sTgcPrepData*, const Muon::MuonPrepDataCollection<Muon::sTgcPrepData> &prd) const;
  void fillsTgcSummaryHistograms(const Muon::sTgcPrepData*) const; 
  int get_sectorPhi_from_stationPhi_stName(const int stationPhi, const std::string& stName) const;

  SG::ReadHandleKey<Muon::sTgcPrepDataContainer> m_sTgcContainerKey{this,"sTGCPrepDataContainerName", "STGC_Measurements"};

  Gaudi::Property<bool> m_dosTgcESD{this,"dosTgcESD",true};
  Gaudi::Property<bool> m_dosTgcOverview{this,"dosTgcOverview",true};
};    
#endif
