/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolFactory.h"
#include "TrigHLTJetRec/PseudoJetSelectorPositivePt.h"

PseudoJetSelectorPositivePt::PseudoJetSelectorPositivePt(const std::string& s,
                                           const std::string& name,
                                           const IInterface* i):
  AthAlgTool(s, name, i){
  declareInterface<IPseudoJetSelector>(this);
}

bool isPositive(const fastjet::PseudoJet& pj) { return pj.pt() > 0; }

StatusCode PseudoJetSelectorPositivePt::select(const PseudoJetVector& in,
                                        PseudoJetVector& out) const {
  std::copy_if(in.begin(),
               in.end(),
               std::back_inserter(out),
               isPositive);
  return StatusCode::SUCCESS;
}
  



