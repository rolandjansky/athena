/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTools/AnalysisTools.h"


// constructor
AnalysisTools::AnalysisTools( const std::string& type, const std::string& name,
			      const IInterface* parent )
  :  IAnalysisTools (type,name,parent)
{
  declareInterface<AnalysisTools>( this );
}

// initialize
StatusCode AnalysisTools::initialize()
{
  MsgStream log(msgSvc(), name());
  return StatusCode::SUCCESS;
}
