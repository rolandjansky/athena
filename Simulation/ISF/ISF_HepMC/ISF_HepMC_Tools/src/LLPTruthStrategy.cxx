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
      if ( !isSUSYParticle( abs(pdg) ) ) {
        // not passed!
        return false;
      }
    
      ATH_MSG_VERBOSE("ACHLLP: saved a truth vertex for pdg "<<pdg);

      // passed!
      return true;
    }
    
    int processCategory = ti.physicsProcessCategory(); // == G4ProcessType
    if( processCategory==m_passProcessCategory ){//save all interactions for user-defined processes, like rhadron interactions

      int pdg = ti.parentPdgCode();
      ATH_MSG_VERBOSE("ACHLLP: saved a truth interaction fUserDefined for pdg "<<pdg);

      // passed!
      return true;
    }
    
    // not passed!
    return false; 
}

bool ISF::LLPTruthStrategy::isSUSYParticle(const int id) const
{
    return ( (id>1000000 && id<1000007) || (id>1000010 && id<1000017) || 
             (id>2000000 && id<2000007) || (id>2000010 && id<2000017) || // squarks and sleptons
             (id>1000020 && id<1000026) || (id>1000034 && id<1000040) || // higgses etc 
             id==1000512 || id==1000522 || id==1000991 || id==1000993 || // R-hadrons
             id==1000612 || id==1000622 || id==1000632 || id==1000642 || id==1000652 || id==1005211 ||
             id==1006113 || id==1006211 || id==1006213 || id==1006223 || id==1006311 ||
             id==1006313 || id==1006321 || id==1006323 || id==1006333 ||
             id==1009111 || id==1009113 || id==1009211 || id==1009213 || id==1009311 ||
             id==1009313 || id==1009321 || id==1009323 || id==1009223 || id==1009333 ||
             id==1092112 || id==1091114 || id==1092114 || id==1092212 || id==1092214 || id==1092224 ||
             id==1093114 || id==1093122 || id==1093214 || id==1093224 || id==1093314 || id==1093324 || id==1093334 );
}
