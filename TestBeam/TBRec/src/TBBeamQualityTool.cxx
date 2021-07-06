/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


#include "TBBeamQualityTool.h"

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
  return this->initializeTool();
}

StatusCode TBBeamQualityTool::initializeTool()
{
  return StatusCode::SUCCESS;
}

StatusCode TBBeamQualityTool::reject(const std::vector<std::string>& particles)
{
  return this->accept(particles).isFailure()
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}


