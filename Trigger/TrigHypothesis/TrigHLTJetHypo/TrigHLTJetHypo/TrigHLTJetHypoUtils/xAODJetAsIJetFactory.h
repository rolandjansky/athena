/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"

class xAODJetAsIJetFactory{
  /* Create a HypoJet::IJet from a xAODJet while remembering index info
     (for example position in a Data vector of the xAODJet) */
  
 public:
  xAODJetAsIJetFactory();
  pHypoJet operator() (const xAOD::Jet* j);
private:
  unsigned int m_ind;
};
