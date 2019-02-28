/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LumiBlockMuTool
   @brief Tool to provide interactions per crossing (mu) from reading values stored in DB
   @brief Implementing ILumiBlockMuTool interface
   @author E.Torrence
**/

#ifndef LUMIBLOCKCOMPS_LumiBlockMuTool_H
#define LUMIBLOCKCOMPS_LumiBlockMuTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"

#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "LumiBlockComps/ILuminosityTool.h"

#include "xAODEventInfo/EventInfo.h"

#include <string>

// Usually this will be called to retrieve the mu value directly from EventInfo
// For data which doesn't have this (or to write it there in the first place)
// this can be configured to read the mu value from COOL instead.
//
class LumiBlockMuTool: public AthAlgTool, virtual public ILumiBlockMuTool {

 public:

  LumiBlockMuTool(const std::string& type,
		  const std::string& name,
		  const IInterface* parent);

  // ---------------- user interface -------------------

  // Return interactions per crossing (mu) averaged over all BCIDs in physics bunch group
  virtual float averageInteractionsPerCrossing() const override;

  // Return interactions per crossing (mu) for this specific BCID
  virtual float actualInteractionsPerCrossing() const override;

  // Functions
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

 private:
  ToolHandle<ILuminosityTool> m_lumiTool;
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo","RHK for EventInfo"};


  // Take MC mu from lumi block number instead of EventInfo?
  bool m_MCLumiBlockHack;  // Default is FALSE

  // Take data mu from COOL instead of EventInfo?
  bool m_useDB;   // Default is FALSE

};


#endif
