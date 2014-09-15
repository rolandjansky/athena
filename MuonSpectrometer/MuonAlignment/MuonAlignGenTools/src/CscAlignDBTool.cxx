/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonAlignGenTools/CscAlignDBTool.h"

namespace Muon {
  
  //________________________________________________________________________
  CscAlignDBTool::CscAlignDBTool(const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
    : MuonAlignDBTool(type,name,parent)
  {
  }
  
  //________________________________________________________________________
  CscAlignDBTool::~CscAlignDBTool() 
  {
    
  }
 
} // end namespace  
  
