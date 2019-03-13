/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @file TrigLivefractionTool.h
   @class TrigLivefractionTool
   @brief Implementing TrigLivefractionTool interface
   @author E.Torrence
**/

#ifndef LUMIBLOCKCOMPS_TrigLivefractionTool_H
#define LUMIBLOCKCOMPS_TrigLivefractionTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "CoralBase/Blob.h"

#include "LumiBlockComps/ITrigLivefractionTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "LumiBlockComps/ILuminosityTool.h"

#include "xAODEventInfo/EventInfo.h"

#include <string>
#include <vector>

class TrigLivefractionTool: public AthAlgTool, virtual public ITrigLivefractionTool {

 public:

  TrigLivefractionTool(const std::string& type,
		       const std::string& name,
		       const IInterface* parent);


  // ---------------- user interface -------------------

  // L1 livefraction information from /TRIGGER/LUMI/PerBcidDeadtime
  // Use highPriority = false to get low priority triggers

  // Return vector with all BCIDs indexed by bcid number
  const std::vector<float>& l1LivefractionVector(bool highPriority = true) const;

  // Return current BCID luminosity based on EventInfo bcid number
  // Defaults to high priority L1 triggers.  
  float livefractionPerBCID(bool highPriority = true) const;

  // Return specific BCID luminosity for any BCID in the current lumi block
  float livefractionPerBCID(unsigned int bcid, bool highPriority = true) const;

  // Number of turns in this lumi block (can be used to compute errors I guess)
  unsigned int lhcTurnCounter() const;

  // Luminosity-averaged live fraction over all physics BCIDs (high or low priority triggers)
  // Depends on luminosity from LuminosityTool
  float lbAverageLivefraction(bool highPriority=true);

  // Standard tool methods
  StatusCode initialize();
  StatusCode finalize();

  // Callback function
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS);

 private:

  // Callback function
  virtual StatusCode updateLivefraction(IOVSVC_CALLBACK_ARGS);

  // Lumi-weighted livefraction update function
  void recalculateLumiLivefraction();

  // Flag to indicate that cached data has changed
  bool m_recalcLumiLivefraction;

  // Tool handle for luminosity ### NEEDS UPDTING
  PublicToolHandle<ILuminosityTool> m_lumiTool{this, "LuminosityTool", ""};

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo","RHK for EventInfo"};

  // Number of turns in this lumi block
  unsigned int m_turnCounter;

  // Live fraction per BCID
  std::vector<float> m_livefractionHigh;
  std::vector<float> m_livefractionLow;

  // Folder name
  Gaudi::Property<std::string> m_deadtimeFolderName{this,"DeadtimeFolderName", ""}; 

  void fillVector(std::vector<float>& livevec, const coral::Blob& blob);

  // Luminoisty-weighted live fractions
  float m_lumiLiveFractionLo;
  float m_lumiLiveFractionHi;

};

#endif
