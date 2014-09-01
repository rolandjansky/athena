/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericTruthStrategy.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ISF_HepMC_Tools/GenericTruthStrategy.h"

// ISF includes
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ISFParticle.h"

/** Constructor **/
ISF::GenericTruthStrategy::GenericTruthStrategy(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_usePrimaryPt(true),
  m_primaryPt2(-1.),
  m_primaryEkin(-1.),
  m_useSecondaryPt(true),
  m_secondaryPt2(-1.),
  m_secondaryEkin(-1.),
  m_allowSecondaryOrPrimaryPass(false),
  m_vertexTypesVector(0),
  m_vertexTypes(),
  m_doVertexRangeCheck(false),
  m_vertexTypeRangeLow(0),
  m_vertexTypeRangeHigh(0),
  m_vertexTypeRangeLength(0),
  m_primPdgCodesVector(0),
  m_primPdgCodes()
{
    declareInterface<ISF::ITruthStrategy>(this);

    // provide either a pT or Ekin cut for the primary and secondary particles respectively.
    // if none are given for either type, it will not use pT or Ekin cuts
    // (the Pt2 variables get squared in the initialize() method)
    declareProperty("PrimaryMinPt"               , m_primaryPt2          );
    declareProperty("PrimaryMinEkin"             , m_primaryEkin         );
    declareProperty("SecondaryMinPt"             , m_secondaryPt2        );
    declareProperty("SecondaryMinEkin"           , m_secondaryEkin       );
    // if set to true, kinetic cuts are passed even if only secondaries pass them
    // (used for special cases such as de-excitation)
    declareProperty("AllowSecondaryOrPrimaryPassKineticCuts" , m_allowSecondaryOrPrimaryPass );
    declareProperty("VertexTypes"                , m_vertexTypesVector   );
    declareProperty("VertexTypeRangeLow"         , m_vertexTypeRangeLow  );
    declareProperty("VertexTypeRangeHigh"        , m_vertexTypeRangeHigh );
    declareProperty("PrimaryPDGCodes"            , m_primPdgCodesVector  );
}

/** Destructor **/
ISF::GenericTruthStrategy::~GenericTruthStrategy()
{
}

// Athena algtool's Hooks
StatusCode  ISF::GenericTruthStrategy::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");

    // (*) setup primary particle cuts
    // -----
    //     (compute and store the squared cut parameters (faster comparisons))
    // check whether the user input makes sense (error case)
    if ( (m_primaryPt2>=0.) && (m_primaryEkin>=0.) ) {
      ATH_MSG_ERROR("Both, pT and Ekin cuts are given for primary particles. Unclear which one to use! ABORT!");
      return StatusCode::FAILURE;
    }
    // neither pT nor Energy cuts were given (never being used so far)
    //   -> enable pT cut and set it to 0.
    else if ( (m_primaryPt2<0.) && (m_primaryEkin<0.) ) {
      // we don't use a flag to disable energy/momentum checks, because
      // all relevant truth strategies up to now do use such cuts
      m_usePrimaryPt = true;
      m_primaryPt2   = 0.;
      m_primaryEkin  = 0.;  // would not be needed actually
    }
    // either pT or Ekin cut is given (standard case)
    else {
      // enable pT cut if value given (greater than 0.):
      m_usePrimaryPt  = !(m_primaryPt2<0.);
      m_primaryPt2   *= m_primaryPt2;
    }

    // (*) setup secondary particle cuts
    // -----
    //     (compute and store the squared cut parameters (faster comparisons))
    // check whether the user input makes sense (error case)
    if ( (m_secondaryPt2>=0.) && (m_secondaryEkin>=0.) ) {
      ATH_MSG_ERROR("Both, pT and Ekin cuts are given for secondary particles. Unclear which one to use! ABORT!");
      return StatusCode::FAILURE;
    }
    // neither pT nor Energy cuts were given (never being used so far)
    //   -> enable pT cut and set it to 0.
    else if ( (m_secondaryPt2<0.) && (m_secondaryEkin<0.) ) {
      // we don't use a flag to disable energy/momentum checks, because
      // all relevant truth strategies up to now do use such cuts
      m_useSecondaryPt = true;
      m_secondaryPt2   = 0.;
      m_secondaryEkin  = 0.;  // would not be needed actually
    }
    // either pT or Ekin cut is given (standard case)
    else {
      // enable pT cut if value given (greater than 0.):
      m_useSecondaryPt = !(m_secondaryPt2<0.);
      m_secondaryPt2  *= m_secondaryPt2;
    }

    // VertexTypeRanges:
    //
    // check whether user input makes sense:
    if ( m_vertexTypeRangeHigh < m_vertexTypeRangeLow) {
      ATH_MSG_ERROR("The given parameter VertexTypeRangeLow is bigger than VertexTypeRangeHigh. ABORT");
      return StatusCode::FAILURE;
    }
    // the length of the given vertex type range
    m_vertexTypeRangeLength = unsigned(m_vertexTypeRangeHigh - m_vertexTypeRangeLow) + 1;
    // if neither lower now upper range limit given, disable range check
    m_doVertexRangeCheck    = m_vertexTypeRangeLow && m_vertexTypeRangeHigh;


    // fill PDG code std::set used for optimized search
    m_primPdgCodes.insert( m_primPdgCodesVector.begin(), m_primPdgCodesVector.end());

    // fill vertex type std::set used for optimized search
    m_vertexTypes.insert( m_vertexTypesVector.begin(), m_vertexTypesVector.end());

    return StatusCode::SUCCESS;
}

StatusCode  ISF::GenericTruthStrategy::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}

bool ISF::GenericTruthStrategy::pass( const ITruthIncident& ti) const {

  // (1.) momentum/energy check
  // ----
  //
  {
    // check whether primary particle passes cut or not
    bool primFail = (m_usePrimaryPt) ? (ti.primaryPt2()<m_primaryPt2)
      : (ti.primaryEkin()<m_primaryEkin) ;
    
    
    // if primary particle failed and strategy does not
    // allow for secondary-only pass -> failed
    if ( ( primFail && (!m_allowSecondaryOrPrimaryPass) ) ) return false;
    //	 ( (!primFail) && m_allowSecondaryOrPrimaryPass) )
    //return false;
    
    
    // check secondaries
    bool secPass =  m_useSecondaryPt ? ti.secondaryPt2Pass(m_secondaryPt2)
      : ti.secondaryEkinPass(m_secondaryEkin) ;
    
    
    // if secondary particles do not pass cuts (either)
    if (!secPass && (!m_allowSecondaryOrPrimaryPass))
      return false;
    
    if (m_allowSecondaryOrPrimaryPass && primFail && !secPass) return false;
    
  }
  
  
  // (2.) primary PDG code check
  // ----
  // check whether primary PDG code matches with any of the given ones
  if (  m_primPdgCodes.size() &&
       (m_primPdgCodes.find(ti.primaryPdgCode()) == m_primPdgCodes.end()) ) {

    // primary particle PDG code not found
    return false;
  }


  // (3.) vertex type check
  // ----
  if ( m_doVertexRangeCheck || m_vertexTypes.size()) {
    int vxtype = ti.physicsProcessCode();

    // (3.1) vxtype in given range?: this is a small performance trick (only one comparison operator to check if within range)
    //  -> exactly equivalent to:  m_doVertexRangeCheck  && (m_vertexTypeLow<=vxtype) && (vxtype<=m_vertexTypeRangeHigh) 
    bool vtxTypeRangePassed = m_doVertexRangeCheck && ( unsigned(vxtype-m_vertexTypeRangeLow) < m_vertexTypeRangeLength );
    // (3.2) if not in range or range check disabled, check whether vxtype
    //       std::set contains the given vertex type
    if ( (!vtxTypeRangePassed) && (m_vertexTypes.find(vxtype)==m_vertexTypes.end()) ) {
        // vxtype not registered -> not passed
        return false;
    }
  }

  // all cuts passed
  return true;
}

