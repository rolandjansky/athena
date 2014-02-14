/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibTools/DCSLFitterTool.h"

// gaudi
#include "GaudiKernel/MsgStream.h"

#include "MdtCalibFitters/DCSLFitter.h"

namespace MuonCalib {

  DCSLFitterTool::DCSLFitterTool(const std::string& t, const std::string& n, const IInterface* p) :
    AlgTool(t,n,p)
  {
    declareInterface< IMdtSegmentFitterTool >(this) ;
  }


  StatusCode DCSLFitterTool::initialize()
  {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "initialize     " << endreq;

    // create an instance of DCSLFitter
    m_implementation = new DCSLFitter();
  
    return StatusCode::SUCCESS;
  }

  StatusCode DCSLFitterTool::finalize()
  {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "finalize     " << endreq;

    // delete instance
    delete m_implementation;

    return StatusCode::SUCCESS;
  }
 
}
