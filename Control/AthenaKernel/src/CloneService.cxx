/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/CloneService.h"

#include <cassert>
#include <vector>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/System.h"

namespace CloneService {
  /** given a pointer to a parent svc sets a reference to a cloned child */
  StatusCode clone(const IService* iParent, const std::string& childName,
		   Service*& child) 
  {
    //FIXME this only creates an uninitialized SERVICE. 
    //FIXME it would be better if one could say parent->clone()
    //FIXME (its default implementation could be this one)

    //check the parent pointer is valid
    const Service* parent(0);
    parent = dynamic_cast<const Service*>(iParent);
    if (0 == parent) { 
      std::cout << " CloneService:clone ERROR: parent " << iParent->name() 
		<< " is not a service!" << std::endl;
      return StatusCode::FAILURE;
    }


    // can we access the Service locator interface?
    ISvcLocator* svcLoc(parent->serviceLocator());
    assert (0 != svcLoc);
  
    MsgStream mlog( parent->msgSvc(), "CloneService::clone" );

    //create the child unless it is already there
    //if the service is not there and can't be created barf
    IService* pIS(0);
    const bool DONOTCREATE(false);
    if ((svcLoc->getService(childName, pIS, DONOTCREATE)).isSuccess() &&
	0 != (child = dynamic_cast<Service*>(pIS))) {
      mlog << MSG::DEBUG 
	  << "Found service " << childName
	  << endmsg;
    } else {
      // can we access the Service Manager interface?
      SmartIF<ISvcManager> svcMgr(svcLoc);
      if ( !svcMgr.isValid() ) {
	mlog << MSG::FATAL 
	    << "ISvcManager interface not found by serviceLocator." 
	    << endmsg;
	return StatusCode::FAILURE;
      }
      const std::string& parentType = System::typeinfoName(typeid(*parent));
      if (svcMgr->createService(parentType, childName,
				pIS).isSuccess() &&
	  0 != (child = dynamic_cast<Service*>(pIS))) {
	mlog << MSG::DEBUG 
	    << "Created service " << childName << " of type " << parentType
	    << endmsg;
      } else {
	mlog << MSG::ERROR
	    << "Failed to create " << childName << " of type " << parentType 
	    << endmsg;
	return StatusCode::FAILURE;
      }

    }
    //now copy parent's properties into child
    std::vector<Gaudi::Details::PropertyBase*>::const_iterator iProp(parent->getProperties().begin());
    std::vector<Gaudi::Details::PropertyBase*>::const_iterator eProp(parent->getProperties().end());
    while (iProp != eProp &&
	   (child->setProperty(**iProp)).isSuccess()) ++iProp;
    if (iProp != eProp) {
      mlog << MSG::ERROR 
	  << "Failed to set child property " << (**iProp).name() 
	  << endmsg;
      return StatusCode::FAILURE;
    } else {
      mlog << MSG::DEBUG 
	  << childName << " properties set"
	  << endmsg;
    }

    //finally put the service in the same state as the parent
    //FIXME should we handle the case in which the parent is RUNNING?
    if (Gaudi::StateMachine::INITIALIZED == parent->FSMState() &&
	Gaudi::StateMachine::CONFIGURED == child->FSMState())
      return child->sysInitialize(); 
    else {
      mlog << MSG::DEBUG 
	  << "Did not initialize " <<  childName
	  << endmsg;
      return StatusCode::SUCCESS;
    }
  }
}
