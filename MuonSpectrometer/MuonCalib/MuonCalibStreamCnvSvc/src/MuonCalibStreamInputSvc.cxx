/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files.
#include "MuonCalibStreamCnvSvc/MuonCalibStreamInputSvc.h"

// Constructor.
MuonCalibStreamInputSvc::MuonCalibStreamInputSvc(const std::string& name, ISvcLocator* svcloc)
  : AthService(name,svcloc )
{

}

// Destructor.
MuonCalibStreamInputSvc::~MuonCalibStreamInputSvc()
{}

StatusCode MuonCalibStreamInputSvc::initialize()
{
   
 return StatusCode::SUCCESS; 

}

