/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/CloneTool.h"

#include <cassert>
#include <vector>

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgTool.h"

namespace CloneTool {
  /** given a pointer to a cloned tool sets a reference to its clone */
  StatusCode typeless_clone(const IAlgTool& cloned, 
			    const std::string& cloneName,
			    const InterfaceID& cloneID, 
			    IAlgTool*& result) 
  {
    //FIXME this only creates an uninitialized AlgTool
    //FIXME it would be better if one could say cloned.clone()
    //FIXME (its default implementation could be this one)
    
    const AlgTool* pCloned(dynamic_cast<const AlgTool*>(&cloned));
    if (0 == pCloned) {
      std::cerr << "CloneTool::FATAL: Could not dcast IAlgTool " 
		<< cloned.name() << " to an AlgTool" << std::endl;
      return StatusCode::FAILURE;
    }

    // can we access the AlgTool locator interface?
    IToolSvc* toolSvc(pCloned->toolSvc());
    assert (0 != toolSvc);
  
    MsgStream mlog( pCloned->msgSvc(), "CloneAlgTool::clone" );
    if (!toolSvc->retrieve(pCloned->type(), 
			   cloneName, 
			   cloneID,
			   result,
			   pCloned->parent(), //cloned's owner
			   /*createIf=*/true).isSuccess()) {
      mlog << MSG::FATAL << "Could not clone AlgTool " 
	   << pCloned->name() << " to " << cloneName << endmsg;
      return StatusCode::FAILURE;
    }
	
    //now copy cloned's properties into result
    std::vector<Gaudi::Details::PropertyBase*>::const_iterator iProp(pCloned->getProperties().begin());
    std::vector<Gaudi::Details::PropertyBase*>::const_iterator eProp(pCloned->getProperties().end());

    AlgTool* aResult=dynamic_cast<AlgTool*>(result);
    if (0 == aResult) {
      mlog << MSG::FATAL << "Could not dcast IAlgTool " 
	   << result->name() << " to an AlgTool" << endmsg;
      return StatusCode::FAILURE;
    }
    while (iProp != eProp &&
	   (aResult->setProperty(**iProp)).isSuccess()) ++iProp;
    if (iProp != eProp) {
      mlog << MSG::ERROR 
	  << "Failed to set result property " << (**iProp).name() 
	  << endmsg;
      return StatusCode::FAILURE;
    } else {
      mlog << MSG::DEBUG 
	  << cloneName << " properties set"
	  << endmsg;
    }

    //finally put the service in the same state as the cloned
    //FIXME should we handle the case in which the cloned is RUNNING?
    if (Gaudi::StateMachine::INITIALIZED == pCloned->FSMState() &&
	Gaudi::StateMachine::CONFIGURED == aResult->FSMState())
      return aResult->sysInitialize(); 
    else {
      mlog << MSG::DEBUG 
	  << "Did not initialize " <<  cloneName
	  << endmsg;
      return StatusCode::SUCCESS;
    }
  }
}
