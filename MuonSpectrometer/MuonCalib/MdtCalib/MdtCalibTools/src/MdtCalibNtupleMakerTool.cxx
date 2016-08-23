/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibTools/MdtCalibNtupleMakerTool.h"

// gaudi
#include "GaudiKernel/MsgStream.h"

// calibration
#include "MuonCalibNtuple/MdtCalibNtupleMaker.h"
#include "MuonCalibStl/DeleteObject.h"

namespace MuonCalib {

  MdtCalibNtupleMakerTool::MdtCalibNtupleMakerTool(const std::string& t, 
						   const std::string& n,
						   const IInterface* p) 
    :  AthAlgTool(t,n,p)
  {
    declareInterface< IMdtCalibTool >(this) ;
  }

  StatusCode MdtCalibNtupleMakerTool::initialize()
  {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "Initialisation started     " << endmsg;
  
    return StatusCode::SUCCESS;
  }

  StatusCode MdtCalibNtupleMakerTool::finalize()
  {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "finalize started     " << endmsg;
    // delete instances
    std::for_each( m_instances.begin(), m_instances.end(), DeleteObject() );

    return StatusCode::SUCCESS;
  }

  IMdtCalibration* MdtCalibNtupleMakerTool::getImp(std::string name){

    // Create instance of MdtCalibNtupleMaker
    IMdtCalibration* implementation = new MdtCalibNtupleMaker( name );
    m_instances.push_back( implementation );
    return implementation;
  }

}
