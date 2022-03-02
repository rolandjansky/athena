/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFXAOD_KEYWRITERTOOL_H 
#define TRIGCONFXAOD_KEYWRITERTOOL_H

// Interface includes
#include "TrigConfxAOD/IKeyWriterTool.h"

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Data includes
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/HLTPrescalesSet.h"
#include "TrigConfData/L1PrescalesSet.h"
#include "TrigConfData/L1BunchGroupSet.h"
#include "xAODTrigger/TrigConfKeys.h"
#include "xAODTrigger/BunchConfKey.h"


namespace TrigConf {

  /**
   * @class Writes trigger keys used when event was processed.
   * Optionally returns the written keys to the caller
   **/
  class KeyWriterTool : public extends<AthAlgTool, IKeyWriterTool> {
    public:

    KeyWriterTool() = delete;

    KeyWriterTool( const std::string& type, const std::string& name, const IInterface* parent );

    virtual ~KeyWriterTool();

    virtual StatusCode initialize() final;

    virtual StatusCode writeKeys(const EventContext& ctx, uint32_t&smk, uint32_t& l1psk, uint32_t& hltpsk) const final;

    virtual StatusCode writeKeys(const EventContext& ctx) const final;

    virtual StatusCode writeBunchgroupKey(const EventContext& ctx, uint32_t&bgk) const final;

    virtual StatusCode writeBunchgroupKey(const EventContext& ctx) const final;

    private: 

    SG::ReadCondHandleKey<TrigConf::L1PrescalesSet> m_l1PrescaleSetInputKey{ this, "L1Prescales", "L1Prescales", "L1 prescales set condition handle key"};
    SG::ReadCondHandleKey<TrigConf::HLTPrescalesSet> m_hltPrescaleSetInputKey{ this, "HLTPrescales", "HLTPrescales", "HLT prescales set condition handle key"};
    SG::ReadCondHandleKey<TrigConf::L1BunchGroupSet> m_bgInputKey{this, "L1BunchGroup", "L1BunchGroup", "L1BunchGroupSet condition handle key"};
    SG::ReadHandleKey<TrigConf::HLTMenu> m_hltMenuInputKey {this, "HLTMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu key"};

    Gaudi::Property<bool> m_includeL1PrescaleKey{this, "IncludeL1PrescaleKey", true, "Set this to false when running this tool online."};
    Gaudi::Property<bool> m_includeBunchgroupKey{this, "IncludeBunchgroupKey", true, "Set this to false when running this tool online."};

    SG::WriteHandleKey<xAOD::TrigConfKeys> m_writeKeys {this, "ConfKeys", "TrigConfKeys", "SG Key of key triplet object."};
    SG::WriteHandleKey<xAOD::BunchConfKey> m_writeBgKey {this, "BunchConfKey", "BunchConfKey", "SG Key of bunchgroup key object."};
    
  };

}


#endif 
