/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TheAnalysis.h
//

#ifndef CALOTESTS_ANALYSISCAL_H
#define CALOTESTS_ANALYSISCAL_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdContext.h"
#include "GeneratorObjects/McEventCollection.h"
#include "LArSimEvent/LArHitContainer.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

class LArDetDescrManager ;

namespace MyAnalysisCal {

  class AnalysisCal : public AthReentrantAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    AnalysisCal(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    virtual ~AnalysisCal();
    
    /** standard Athena-Algorithm method */
    virtual StatusCode          initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          execute(const EventContext& ctx) const override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          finalize() override;
  private:
    SG::ReadHandleKey<McEventCollection> m_mcCollName { this, "MCColl", "GEN_AOD" };
    SG::ReadHandleKeyArray<LArHitContainer> m_hitContainerNames { this, "HitContainers",
                                                                  {"LArHitEMB",
                                                                   "LArHitEMEC",
                                                                   "LArHitHEC",
                                                                   "LArHitFCAL"} };
    SG::ReadHandleKeyArray<CaloCalibrationHitContainer> m_calibHitContainerNames { this, "CalibHitContainers",
                                                                  {"LArCalibrationHitActive",
                                                                   "LArCalibrationHitDeadMaterial",
                                                                   "LArCalibrationHitInactive"} };
  };
} // end of namespace bracket
#endif
