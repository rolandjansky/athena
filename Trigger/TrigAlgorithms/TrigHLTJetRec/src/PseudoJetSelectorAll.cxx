/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolFactory.h"
#include "TrigHLTJetRec/PseudoJetSelectorAll.h"

PseudoJetSelectorAll::PseudoJetSelectorAll(const std::string& s,
                                           const std::string& name,
                                           const IInterface* i):
  AthAlgTool(s, name, i){
  declareInterface<IPseudoJetSelector>(this);
}

StatusCode PseudoJetSelectorAll::select(const PseudoJetVector& in,
                                        PseudoJetVector& out) const {
  out.assign(in.begin(), in.end());
  return StatusCode::SUCCESS;}
  


