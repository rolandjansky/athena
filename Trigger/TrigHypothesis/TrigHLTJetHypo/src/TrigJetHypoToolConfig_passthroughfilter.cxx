/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigJetHypoToolConfig_passthroughfilter.h"
#include "./PassThroughFilter.h"

#include "GaudiKernel/StatusCode.h"

TrigJetHypoToolConfig_passthroughfilter::TrigJetHypoToolConfig_passthroughfilter(const std::string& type,
										 const std::string& name,
										 const IInterface* parent) :
  base_class(type, name, parent){
}


TrigJetHypoToolConfig_passthroughfilter::~TrigJetHypoToolConfig_passthroughfilter(){
}

StatusCode TrigJetHypoToolConfig_passthroughfilter::initialize() {
  ATH_MSG_INFO("initialising " << name());
  return StatusCode::SUCCESS;
}


FilterPtr
TrigJetHypoToolConfig_passthroughfilter::getHypoJetVectorFilter() const {
  return std::make_unique<PassThroughFilter>();
}

