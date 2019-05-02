/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkHiggs/HIGG3TruthDecorator.h"

#include "xAODEventInfo/EventInfo.h"

namespace DerivationFramework {

  // Constructor
  DerivationFramework::HIGG3TruthDecorator::HIGG3TruthDecorator(const std::string& t,
                                            const std::string& n,
                                            const IInterface* p) : 
    AthAlgTool(t, n, p)
  {

    declareInterface<DerivationFramework::IAugmentationTool>(this);

  }

  // Destructor
  DerivationFramework::HIGG3TruthDecorator::~HIGG3TruthDecorator() {
  }

  StatusCode HIGG3TruthDecorator::initialize() {
    return StatusCode::SUCCESS;
  }

  StatusCode HIGG3TruthDecorator::finalize(){
    return StatusCode::SUCCESS;
  }

  StatusCode HIGG3TruthDecorator::addBranches() const {

    const xAOD::EventInfo* eventInfo = nullptr;
    ATH_CHECK( evtStore()->retrieve(eventInfo) );

    // Only run HWWTruthAlg.cxx on the MC
    const bool isSim = eventInfo->eventType(xAOD::EventInfo::EventType::IS_SIMULATION);
    if ( !isSim ) {
      ATH_MSG_DEBUG ("It is a data event... nothing to be done...");
      return StatusCode::SUCCESS;
    }

    return StatusCode::SUCCESS;
  }

}
