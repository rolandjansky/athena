/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"

xAODJetAsIJetFactory::xAODJetAsIJetFactory(): m_ind(-1){}

pHypoJet xAODJetAsIJetFactory::operator() (const xAOD::Jet* j){
  return new HypoJet::xAODJetAsIJet(j, ++m_ind);
}

