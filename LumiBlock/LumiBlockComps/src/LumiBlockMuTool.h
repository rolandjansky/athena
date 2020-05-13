/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LumiBlockMuTool
   @brief Tool to provide interactions per crossing (mu) from reading values stored in DB
   @brief Implementing ILumiBlockMuTool interface
   @author E.Torrence
**/

#ifndef LUMIBLOCKCOMPS_LumiBlockMuTool_H
#define LUMIBLOCKCOMPS_LumiBlockMuTool_H

#include "LumiBlockComps/ILumiBlockMuTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadDecorHandleKey.h"

#include <string>

// Usually this will be called to retrieve the mu value directly from EventInfo
// For data which doesn't have this (or to write it there in the first place)
// this can be configured to read the mu value from COOL instead.
//
class LumiBlockMuTool: public extends<AthAlgTool, ILumiBlockMuTool> {

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

 private:
  SG::ReadHandleKey<xAOD::EventInfo>       m_eventInfoKey{this
      ,"EventInfoKey"
      ,"EventInfo"
      ,"RHK for EventInfo"};

  SG::ReadDecorHandleKey<xAOD::EventInfo>  m_rdhkActMu {this
      ,"ActIntPerXKey"
      ,"EventInfo.ActIntPerXDecor"
      ,"Decoration for Actual Interaction Per Crossing"};

  SG::ReadDecorHandleKey<xAOD::EventInfo>  m_rdhkAveMu {this
      ,"AveIntPerXKey"
      ,"EventInfo.AveIntPerXDecor"
      ,"Decoration for Average Interaction Per Crossing"};
};


#endif
