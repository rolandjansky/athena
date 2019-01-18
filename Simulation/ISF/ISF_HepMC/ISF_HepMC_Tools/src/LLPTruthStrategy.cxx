/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LLPTruthStrategy.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "LLPTruthStrategy.h"

// ISF includes
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ISFParticle.h"
#include "TruthUtils/PIDHelpers.h"

/** Constructor **/
ISF::LLPTruthStrategy::LLPTruthStrategy(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_passProcessCodeRangeLow(0),
  m_passProcessCodeRangeHigh(0),
  m_passProcessCategory(0)
{
    declareProperty("PassProcessCodeRangeLow",  m_passProcessCodeRangeLow=200);
    declareProperty("PassProcessCodeRangeHigh", m_passProcessCodeRangeHigh=299);
    declareProperty("PassProcessCategory",      m_passProcessCategory=9);
}

/** Destructor **/
ISF::LLPTruthStrategy::~LLPTruthStrategy()
{
}

// Athena algtool's Hooks
StatusCode  ISF::LLPTruthStrategy::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");

    ATH_MSG_VERBOSE("Initialization successful.");
    return StatusCode::SUCCESS;
}

StatusCode  ISF::LLPTruthStrategy::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");

    ATH_MSG_VERBOSE("Finalization successful.");
    return StatusCode::SUCCESS;
}

bool ISF::LLPTruthStrategy::pass( ITruthIncident& ti) const {

    int processCode = ti.physicsProcessCode(); // == G4ProcessSubType
    if( (processCode>m_passProcessCodeRangeLow && processCode<m_passProcessCodeRangeHigh) ){ // All kinds of decay processes are included here...
    
      // Check if this is a sparticle - if not, return
      int pdg = ti.parentPdgCode();
      if ( !MC::PID::isRHadron(abs(pdg)) && !MC::PID::isSUSY(abs(pdg)) ){
        // not passed!
        return false;
      }
    
      ATH_MSG_VERBOSE("Saved a truth vertex for pdg "<<pdg);

      // passed!
      return true;
    }

    int processCategory = ti.physicsProcessCategory(); // == G4ProcessType
    if( processCategory==m_passProcessCategory ){//save all interactions for user-defined processes, like rhadron interactions

      int pdg = ti.parentPdgCode();
      ATH_MSG_VERBOSE("Saved a truth interaction fUserDefined for pdg "<<pdg);

      // passed!
      return true;
    }
    
    // not passed!
    return false; 
}
