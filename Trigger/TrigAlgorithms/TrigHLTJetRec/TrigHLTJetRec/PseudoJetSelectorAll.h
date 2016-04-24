/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_PSEUDOJETSELECTORALL_H
#define TRIGHLTJETREC_PSEUDOJETSELECTORALL_H

#include "TrigHLTJetRec/IPseudoJetSelector.h"
#include "AthenaBaseComps/AthAlgTool.h"

class StatusCode;

using jet::PseudoJetVector;

class PseudoJetSelectorAll: virtual public IPseudoJetSelector, 
  public AthAlgTool {

public:

  PseudoJetSelectorAll(const std::string&,
                       const std::string&,
                       const IInterface*);

  ~PseudoJetSelectorAll(){};
  
  StatusCode select(const PseudoJetVector&,
                    PseudoJetVector&) const override;
};

#endif
