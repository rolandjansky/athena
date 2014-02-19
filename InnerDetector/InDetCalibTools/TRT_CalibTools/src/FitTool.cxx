/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************


********************************************************************/

// INCLUDES:
#include "FitTool.h"
#include "TRT_ConditionsData/FloatArrayStore.h"

FitTool::FitTool(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent)
//  m_log(msgSvc(), name)

{
  declareInterface<IFitTool>(this);
}

StatusCode FitTool::initialize()
{
  msg(MSG::INFO) << "initialize()" << endreq;
  return StatusCode::SUCCESS;
}

bool FitTool::fit(std::vector<IdentifierProfileHistogram*>& histograms, std::vector<FloatArrayStore>& results) {
  msg(MSG::INFO) << "dummy fit" << endreq;
  if (histograms.size()==0) {};
  if (results.size()==0) {};
  return true;
}

