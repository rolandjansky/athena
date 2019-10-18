/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1DECODER_CTPUNPACKINGTOOL_H
#define L1DECODER_CTPUNPACKINGTOOL_H 1

#include "CTPUnpackingToolBase.h"

#include "DecisionHandling/HLTIdentifier.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfInterfaces/IHLTConfigSvc.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"


class CTPUnpackingTool : public CTPUnpackingToolBase {
public:

  CTPUnpackingTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  virtual StatusCode decode(const ROIB::RoIBResult& roib, HLT::IDVec& enabledChains) const override;

  virtual StatusCode initialize() override;

  virtual StatusCode start() override;



private:
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_lvl1ConfigSvc{this, "LVL1ConfigSvc", "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", ""};
  ServiceHandle<TrigConf::IHLTConfigSvc> m_hltConfigSvc{this, "HLTConfigSvc", "TrigConf::HLTConfigSvc/HLTConfigSvc", ""};
  
  SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu",
      "HLT Menu"};
  
  // TODO  add once L1 menu available as well
  //SG::ReadHandleKey<TrigConf::L1Menu> m_l1MenuKey{this, "L1Menu", "DetectorStore+HLTMenu",
  //      "Menu"};




};


#endif
