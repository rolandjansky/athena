/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1DECODER_CTPUNPACKINGTOOL_H
#define L1DECODER_CTPUNPACKINGTOOL_H 1

#include "CTPUnpackingToolBase.h"

#include "DecisionHandling/HLTIdentifier.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
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

  virtual StatusCode updateConfiguration( const std::map<std::string, std::string>& seeding ) override;

private:
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

  SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{this, "HLTMenu", "DetectorStore+HLTMenu",
      "HLT Menu"};

  // TODO  add once L1 menu available as well
  //SG::ReadHandleKey<TrigConf::L1Menu> m_l1MenuKey{this, "L1Menu", "DetectorStore+HLTMenu",
  //      "Menu"};




};


#endif
