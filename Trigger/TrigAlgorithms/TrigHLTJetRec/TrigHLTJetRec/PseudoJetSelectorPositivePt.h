/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_PSEUDOJETSELECTORPOSITIVEPT_H
#define TRIGHLTJETREC_PSEUDOJETSELECTORPOSITIVEPT_H

#include "TrigHLTJetRec/IPseudoJetSelector.h"
#include "AthenaBaseComps/AthAlgTool.h"

class StatusCode;

using jet::PseudoJetVector;

class PseudoJetSelectorPositivePt: virtual public IPseudoJetSelector, 
  public AthAlgTool {

public:

  PseudoJetSelectorPositivePt(const std::string&,
                              const std::string&,
                              const IInterface*);

  ~PseudoJetSelectorPositivePt(){};
  
  StatusCode select(const PseudoJetVector&,
                    PseudoJetVector&) const override;
};

#endif
