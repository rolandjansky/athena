/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _Analysis_CreateHits_H
#define _Analysis_CreateHits_H

#include <string>

// Gaudi includes

#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/LArEM_ID.h"

namespace CreateHits {

  class CreateHits : public Algorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CreateHits(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~CreateHits();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();
  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  /** The StoreGate Service */
  StoreGateSvc* m_sgSvc;
  StoreGateSvc* detStore;
  const LArEM_ID*        m_larem_id;

  int m_nevt;

  };
} // end of namespace bracket
#endif
