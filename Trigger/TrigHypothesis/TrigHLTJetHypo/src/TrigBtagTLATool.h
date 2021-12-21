/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
   */

/*###########################################################
 ** @class TrigBtagTLATool
 ** @brief Tool to attach available b-tagging informaton in a previous decision to jets in a given jet container. BTagging is duplicated and given link pointing to matching jet.
 ** @author Claire Antel
###########################################################*/


#ifndef TRIGBTAGTLATOOL
#define TRIGBTAGTLATOOL

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

class TrigBtagTLATool: public AthAlgTool{
  public:

    TrigBtagTLATool(const std::string& type,
        const std::string& name,
        const IInterface* parent);

    ~TrigBtagTLATool();
    StatusCode initialize();
    StatusCode finalize();

    //StatusCode record_btag(TrigCompositeUtils::DecisionContainer*, const xAOD::JetContainer*, const EventContext&) const;
    StatusCode record_btag(const xAOD::TrigCompositeContainer*, const xAOD::JetContainer&, const EventContext&) const;  //must pass JetContainer as const reference to preserve pointer addresses.
    SG::WriteHandleKey< xAOD::BTaggingContainer > m_outputBTagInfoKey {
      this, "TLAOutputBTaggingCollection", "HLT_BTagging_TLA", "output b-tagging container key"}; 
    SG::WriteDecorHandleKey<xAOD::BTaggingContainer> m_bTagJetDecorLinkName {this, "JetBTagLinkName", "", "Element Link from BTagging to Jet container"}; // this property not actually configurable, so don't  try

};
#endif
