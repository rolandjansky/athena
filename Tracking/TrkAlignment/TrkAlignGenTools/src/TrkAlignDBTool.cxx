/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrkAlignDBTool.cxx

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgTool.h"

#include "TrkAlignGenTools/TrkAlignDBTool.h"

namespace Trk {
  
    //_______________________________________________________________________
  TrkAlignDBTool::TrkAlignDBTool(const std::string& type, const std::string& name,
			       const IInterface* parent)
    : AthAlgTool(type,name,parent)
  {
    declareInterface<ITrkAlignDBTool>(this);

    m_logStream = nullptr;
  }

   //_______________________________________________________________________
  TrkAlignDBTool::~TrkAlignDBTool()
  {
  }
  //_______________________________________________________________________
  StatusCode TrkAlignDBTool::initialize()
  {
    msg(MSG::DEBUG) << "initialize() of TrkAlignDBTool" << endmsg;  

    return StatusCode::SUCCESS;
  }
  
  //_______________________________________________________________________
  StatusCode TrkAlignDBTool::finalize()
  {
    msg(MSG::DEBUG) << "finalize() of TrkAlignDBTool" << endmsg;  
    
    return StatusCode::SUCCESS;    
  }


}
