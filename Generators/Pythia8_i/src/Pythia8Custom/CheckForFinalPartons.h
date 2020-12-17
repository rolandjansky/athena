/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYTHIA8_CHECK_FOR_FINAL_PARTONS_H
#define PYTHIA8_CHECK_FOR_FINAL_PARTONS_H

#include "Pythia8_i/IPythia8Custom.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "Pythia8/Pythia.h"

class CheckForFinalPartons: public AthAlgTool, virtual public IPythia8Custom{
  
  public:
  
  CheckForFinalPartons(const std::string &type, const std::string &name, const IInterface *parent);
  
  StatusCode ModifyPythiaEvent(Pythia8::Pythia &pythia) const;
  
  private:
  
  bool acceptEvent(Pythia8::Event &event) const;
  
  int m_maxFailures;
  mutable int m_nFailures;
  
};

#endif
