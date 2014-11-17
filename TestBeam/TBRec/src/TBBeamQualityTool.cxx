/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityTool.cxx                    #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#          based on TBEventStreamer                 #
//# Last Modified: Nov 18/2004                        #
//#####################################################


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "StoreGate/StoreGateSvc.h"

#include "TBRec/TBBeamQualityTool.h"

#include <string>
#include <vector>

TBBeamQualityTool::TBBeamQualityTool(const std::string& name,
				     const std::string& type,
				     const IInterface* parent)
  : AthAlgTool(name,type,parent)
{ }

TBBeamQualityTool::~TBBeamQualityTool()
{}

StatusCode TBBeamQualityTool::initialize()
  
{
  this->initializeTool();
  return StatusCode::SUCCESS;
}

StatusCode TBBeamQualityTool::initializeTool()
{
  return StatusCode::SUCCESS;
}

StatusCode TBBeamQualityTool::reject(std::vector<std::string> m_particles)
{
  return this->accept(m_particles).isFailure()
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}


