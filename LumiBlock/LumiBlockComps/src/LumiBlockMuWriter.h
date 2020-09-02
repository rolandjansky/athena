/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUMIBLOCKCOMPS_LUMIBLOCKMUWRITER_H
#define LUMIBLOCKCOMPS_LUMIBLOCKMUWRITER_H

/**
 * @class LumiBlockMuWriter
 *
 * @brief Algorithm to write interactions per crossing (mu) into xAOD::EventInfo as decorations
 *
 **/

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "xAODEventInfo/EventInfo.h"

#include "StoreGate/WriteDecorHandleKey.h"

class LumiBlockMuWriter : public AthReentrantAlgorithm 
{
 public:
  LumiBlockMuWriter(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  
 private:
  SG::ReadCondHandleKey<LuminosityCondData> m_lumiDataKey {this
      ,"LumiDataKey"
      ,"LuminosityCondData"
      ,"RCHK for LuminosityCondData"};

  SG::WriteDecorHandleKey<xAOD::EventInfo> m_actIntPerXKey {this
      ,"ActIntPerXKey"
      ,"EventInfo.ActIntPerXDecor"
      ,"Decoration for Actual Interaction Per Crossing"};

  SG::WriteDecorHandleKey<xAOD::EventInfo> m_aveIntPerXKey {this
      ,"AveIntPerXKey"
      ,"EventInfo.AveIntPerXDecor"
      ,"Decoration for Average Interaction Per Crossing"};

};

#endif
