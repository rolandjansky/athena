/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IDPerfMuonRefitterTER_H
#define IDPerfMuonRefitterTER_H

//==============================================================================
// Include files...
//==============================================================================
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetPerformanceMonitoring/IDPerfMuonRefitter.h"

#include <map>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ToolHandle.h"
#include "InDetPerformanceMonitoring/PerfMonServices.h"


class IegammaTrkRefitterTool;


class IDPerfMuonRefitter : public AthAlgorithm
{
 public:
  // Constructors & destructors

  IDPerfMuonRefitter(const std::string& name, ISvcLocator* pSvcLocator);

  ~IDPerfMuonRefitter();

  // Overriden class functions.
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 protected:

 private:

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter1;

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter2;


  std::string m_outputTracksName;


  mutable int m_N_Muons;
  mutable int m_N_MuonsRefit;
  mutable int m_N_MuonRefitFailures;

  PerfMonServices::CONTAINERS m_container;

};
//==============================================================================

#endif
