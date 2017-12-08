// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPYTHON_ATHENAPYTHONDICT_H
#define ATHENAPYTHON_ATHENAPYTHONDICT_H

#include <string>
#include <typeinfo>
#include "GaudiKernel/IEvtSelector.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IThinningHdlr.h"
#include "AthenaKernel/ISlimmingHdlr.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaKernel/IValgrindSvc.h"
#include "AthenaKernel/IUserDataSvc.h"
#include "AthenaKernel/IAthenaSealSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaKernel/IEvtIdModifierSvc.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/IIoComponentMgr.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthService.h"

#include "AthenaPython/PyAthenaAlg.h"
#include "AthenaPython/PyAthenaSvc.h"
#include "AthenaPython/PyAthenaTool.h"
#include "AthenaPython/PyAthenaAud.h"

namespace AthenaInternal {

  class ROOT6_AthenaPython_WorkAround_Dummy {};
  
  CLID getClid( IClassIDSvc* self, const std::string& typeName ) {
    CLID clid = CLID_NULL;
    self->getIDOfTypeName(typeName, clid).ignore();
    return clid;
  }

  struct AthenaInternalInstan {
    ServiceHandle<StoreGateSvc> hsg;
    ServiceHandle<IUserDataSvc> huds;
  };

}

#endif // ATHENAPYTHON_ATHENAPYTHONDICT_H
