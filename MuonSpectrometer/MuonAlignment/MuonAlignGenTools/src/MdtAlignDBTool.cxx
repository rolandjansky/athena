/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonAlignGenTools/MdtAlignDBTool.h"

namespace Muon {
  
  //________________________________________________________________________
  MdtAlignDBTool::MdtAlignDBTool(const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
    : MuonAlignDBTool(type,name,parent)
  {

  }
  
  //________________________________________________________________________
  MdtAlignDBTool::~MdtAlignDBTool() 
  {
    
  }
  
} // end namespace
