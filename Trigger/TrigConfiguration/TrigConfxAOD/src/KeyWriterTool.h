/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "xAODTrigger/TrigConfKeys.h"


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

    private: 

    SG::ReadCondHandleKey<TrigConf::L1PrescalesSet> m_l1PrescaleSetInputKey{ this, "L1Prescales", "L1Prescales", "L1 prescales set key"};
    SG::ReadCondHandleKey<TrigConf::HLTPrescalesSet> m_hltPrescaleSetInputKey{ this, "HLTPrescales", "HLTPrescales", "HLT prescales set key"};
    SG::ReadHandleKey<TrigConf::HLTMenu> m_hltMenuInputKey {this, "HLTMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu key"};

    Gaudi::Property<bool> m_includeL1PrescaleKey{this, "IncludeL1PrescaleKey", true, "Set this to false when running this tool online."};

    SG::WriteHandleKey<xAOD::TrigConfKeys> m_writeKeys {this, "ConfKeys", "TrigConfKeys", "SG Key of key triplet object."};
  };

}


#endif 
