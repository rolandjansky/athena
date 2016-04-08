/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TheAnalysis.h
//

#ifndef _Analysis_AnalysisCal_H
#define _Analysis_AnalysisCal_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdContext.h"

class LArDetDescrManager ;
class LArCablingService;

namespace MyAnalysisCal {

  class AnalysisCal : public AthAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    AnalysisCal(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~AnalysisCal();
    
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
    //int m_nevt;

  };
} // end of namespace bracket
#endif
