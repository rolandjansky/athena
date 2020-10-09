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

/**
 * @class decodes CTP information in order to activate HLT chains
 **/
class CTPUnpackingTool : public CTPUnpackingToolBase {
public:

  CTPUnpackingTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  //!< fills the list of chains that should be acvtivated in a given event (note, HLT prescaling happens at a later stage)
  virtual StatusCode decode(const ROIB::RoIBResult& roib, HLT::IDVec& enabledChains) const override;

  virtual StatusCode initialize() override;

  virtual StatusCode start() override;



private:
  // Menu objects (that act here as configuration) need to be available in detector store.
  // They are, at the moment, provided by HLT and LVL1 ConfigSvc during initialize.
  // Threfore these services have to be configured & instantiated in jobs that use this tool (and L1Decoder itself).
  SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu"};
  SG::ReadHandleKey<TrigConf::L1Menu>  m_L1MenuKey{ this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu", "L1 Menu" };

  Gaudi::Property<bool> m_useTBPBit{ this, "UseTBPBits", false, "When true, use Trigger Before Prescale bits instead of Trigger After Veto (for testing only)" };
};


#endif
