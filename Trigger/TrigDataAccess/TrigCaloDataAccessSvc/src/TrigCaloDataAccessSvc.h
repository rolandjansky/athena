/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigCaloDataAccessSvc_TrigCaloDataAccessSvc_h
#define TrigCaloDataAccessSvc_TrigCaloDataAccessSvc_h

#include <mutex>
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/SlotSpecificObj.h"


class TrigCaloDataAccessSvc : public AthService {
 public:
  TrigCaloDataAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigCaloDataAccessSvc();
  
  virtual StatusCode initialize() ;
  virtual StatusCode finalize() ;
  


};


#endif


