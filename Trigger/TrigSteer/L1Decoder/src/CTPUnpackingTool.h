/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1DECODER_CTPUNPACKINGTOOL_H
#define L1DECODER_CTPUNPACKINGTOOL_H 1

#include "CTPUnpackingToolBase.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
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
  // TODO Remove if HLTConfigSvc and LVL1ConfigSvc are no longer needed to put the HLTTriggerMenu and L1Menu in the DetectorStore
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_lvl1ConfigSvc{this, "LVL1ConfigSvc", "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", ""};
  ServiceHandle<TrigConf::IHLTConfigSvc> m_hltConfigSvc{this, "HLTConfigSvc", "TrigConf::HLTConfigSvc/HLTConfigSvc", ""};
  
  SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu"};
  SG::ReadHandleKey<TrigConf::L1Menu>  m_L1MenuKey{ this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu", "L1 Menu" };
  
  Gaudi::Property<bool> m_useNewConfig{ this, "UseNewConfig", false, "When true, read the menu from detector store, when false use the L1ConfigSvc" };
  Gaudi::Property<bool> m_useTBPBit{ this, "UseTBPBits", false, "When true, use Trigger Before Prescale bits instead of Trigger After Veto (for testing only)" };
};


#endif
