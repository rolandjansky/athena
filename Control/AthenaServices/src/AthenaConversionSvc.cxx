/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	AthenaConversionSvc.cpp
//--------------------------------------------------------------------
//
//====================================================================

#include "AthenaConversionSvc.h"

///--------------------------------------------------------------------
/// Initialize the service.
///--------------------------------------------------------------------
StatusCode AthenaConversionSvc::initialize()     
{
  return ConversionSvc::initialize();
}

///--------------------------------------------------------------------
/// stop the service.
///--------------------------------------------------------------------
StatusCode AthenaConversionSvc::finalize()      
{
  return ConversionSvc::finalize();
}

///--------------------------------------------------------------------
/// Standard Constructor
///--------------------------------------------------------------------
AthenaConversionSvc::AthenaConversionSvc(const std::string& name, 
					 ISvcLocator* svc, 
					 unsigned char type)
 : ConversionSvc(name, svc, type)
{ }

///--------------------------------------------------------------------
/// Standard Destructor
///--------------------------------------------------------------------
AthenaConversionSvc::~AthenaConversionSvc()   
{  }
