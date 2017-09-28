/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DigiTestToolBase.h"

DigiTestToolBase::DigiTestToolBase(const std::string& type, 
                   const std::string& name,
                   const IInterface* parent) : 
  base_class(type, name, parent)
{
  declareProperty("HistPath",  m_path="/truth/");
  declareProperty("McEventKey", m_key="TruthEvent");
}
