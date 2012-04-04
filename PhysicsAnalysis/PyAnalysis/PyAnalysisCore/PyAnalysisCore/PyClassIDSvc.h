/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYANALYSISCORE_PYCLASSIDSVC_H
#define PYANALYSISCORE_PYCLASSIDSVC_H

/**
   A wrapper for ClassIDSvc

    @author Tadashi Maeno
*/
 
#include "AthenaKernel/IClassIDSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

struct PyClassIDSvc
{
  /// get type name associated with clID (if any)
  static std::string getTypeNameOfID (const unsigned int id)
  {
    IClassIDSvc *pSvc = 0;
    Gaudi::svcLocator()->service("ClassIDSvc", pSvc);
    if (!pSvc)
      {
	MsgStream log(Athena::getMessageSvc(), "PyClassIDSvc");
	log << MSG::ERROR << "could not get ClassIDSvc" << endreq;
	return "";
      }
    
    std::string name;
    StatusCode sc = pSvc->getTypeNameOfID (id, name);
    if (sc.isFailure())
      {
	MsgStream log(Athena::getMessageSvc(), "PyClassIDSvc");
	log << MSG::ERROR << "could not get TypeName for " << id << endreq;
      }
    return name;
  }
};

#endif

  
