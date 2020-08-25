/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DQDummyFilterTool.h"

DQDummyFilterTool::DQDummyFilterTool(const std::string& type,const std::string& name,const IInterface* parent)
: AthAlgTool( type, name, parent )
{
  declareInterface<IDQFilterTool>(this);
}
        
DQDummyFilterTool::~DQDummyFilterTool () {}

bool DQDummyFilterTool::accept() const {
  return true;
}

