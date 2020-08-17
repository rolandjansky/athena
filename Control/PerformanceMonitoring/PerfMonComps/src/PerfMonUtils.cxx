///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonUtils.cxx 
// Implementation file for a few PerfMon utilities
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <vector>
#include <string>

// Framework includes
#include "GaudiKernel/IProperty.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IAuditorSvc.h"
#include "GaudiKernel/IAuditor.h"

// PerfMonComps includes
#include "PerfMonUtils.h"

namespace PerfMon {



StatusCode 
makeAuditor( const std::string& audName, IAuditorSvc* audSvc,
             MsgStream& msg )
{
  if ( 0 == audSvc ) {
    msg << MSG::ERROR << "Null pointer to IAuditorSvc !!" << endmsg;
    return StatusCode::FAILURE;
  }

  if ( 0 != audSvc->getAuditor( audName ) ) {
    msg << MSG::VERBOSE << "AuditorSvc already knows about ["
	<< audName << "]... good."
	<< endmsg;
    return StatusCode::SUCCESS;
  }

  const std::string propName = "Auditors";
  SmartIF<IProperty> audSvcProp(audSvc);
  if ( 0 == audSvcProp ) {
    msg << MSG::ERROR
	<< "Could not dyn-cast IAuditorSvc to an IProperty !!" << endmsg;
    return StatusCode::FAILURE;
  }

  StringArrayProperty audNames;
  audNames.assign( audSvcProp->getProperty(propName) );
  std::vector<std::string> updatedNames( audNames.value() );
  updatedNames.push_back( audName );
  audNames.set( updatedNames );
  audNames.setName( propName );
  if ( !audSvcProp->setProperty( audNames ).isSuccess() ) {
    msg << MSG::ERROR
	<< "Could not add [" << audName << "] to the list of auditors of"
	<< " [AuditorSvc] !!"
	<< endmsg
	<< audSvcProp->getProperty(propName)
	<< endmsg;
    return StatusCode::FAILURE;
  }

  // make sure the auditor has been created...
  IAuditor* aud = audSvc->getAuditor( audName );
  if ( 0 == aud ) {
    msg << MSG::ERROR
        << "Failed to make AuditorSvc instantiating [" 
        << audName << "] !!"
        << endmsg;
    return StatusCode::FAILURE;
  }
  //  aud->release();
  
  return StatusCode::SUCCESS;
}




} //> end namespace PerfMon

