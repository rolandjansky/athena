/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYANALYSISCORE_PYCLASSIDSVC_H
#define PYANALYSISCORE_PYCLASSIDSVC_H

/**
   A wrapper for ClassIDSvc

    @author Tadashi Maeno
*/
 
#include "GaudiKernel/IClassIDSvc.h"
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
    Gaudi::svcLocator()->service("ClassIDSvc", pSvc).ignore();
    if (!pSvc)
      {
	MsgStream log(Athena::getMessageSvc(), "PyClassIDSvc");
	log << MSG::ERROR << "could not get ClassIDSvc" << endmsg;
	return "";
      }
    
    std::string name;
    StatusCode sc = pSvc->getTypeNameOfID (id, name);
    if (sc.isFailure())
      {
	MsgStream log(Athena::getMessageSvc(), "PyClassIDSvc");
	log << MSG::ERROR << "could not get TypeName for " << id << endmsg;
      }
    return name;
  }
};

#endif

  
