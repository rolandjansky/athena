/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	McCnvSvc.cxx
//--------------------------------------------------------------------
//
//	Package    : McEventSelector
//
//	Author     : M. Shapiro, copied from Paolo Calafiura
//  History    : Updated for Gaudi 0.9 (April 2002) Ian Hinchliffe (thanks to Steve Eckmann)

// Include files
#include <iostream>
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"

#include "McEventSelector/McCnvSvc.h"

//External definitions

/// Standard constructor
McCnvSvc::McCnvSvc(const std::string& name, ISvcLocator* svc)
: ConversionSvc( name, svc, repSvcType())
{ }

/// Standard Destructor
McCnvSvc::~McCnvSvc()   {
}

/// Initialize the service.
StatusCode McCnvSvc::initialize()     {
  StatusCode status = ConversionSvc::initialize();
  return status;
}

/// Update state of the service
StatusCode McCnvSvc::updateServiceState(IOpaqueAddress* pAddress)    {
  MsgStream log(messageService(), name());
  static bool first = true;
  //  static int fid   = 0;
  //  static int recid = 0;
  if ( 0 != pAddress )    {
    GenericAddress* pAddr = dynamic_cast<GenericAddress*>(pAddress);
    if ( 0 != pAddr )    {
      if ( first )    {
        first = false;
      }
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

long
McCnvSvc::repSvcType() const {
  return storageType();
}

long
McCnvSvc::storageType() {
  static long type=0x10;
  return type;
}
