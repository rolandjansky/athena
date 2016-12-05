/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_TRIGHLTJETRECFROMJET_H
#define TRIGHLTJETREC_TRIGHLTJETRECFROMJET_H

#include "TrigHLTJetRec/TrigHLTJetRecBase.h"
#include "xAODJet/JetContainer.h"

class TrigHLTJetRecFromJet: public TrigHLTJetRecBase<xAOD::JetContainer>{

 public:
  TrigHLTJetRecFromJet(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetRecFromJet();

 protected:
  const xAOD::JetContainer* build() const override;

}; 
#endif
