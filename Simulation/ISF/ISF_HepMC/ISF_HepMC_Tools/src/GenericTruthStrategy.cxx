/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericTruthStrategy.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GenericTruthStrategy.h"

// ISF includes
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ISFParticle.h"

/** Constructor **/
ISF::GenericTruthStrategy::GenericTruthStrategy(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_useParentPt(true),
  m_parentPt2(-1.),
  m_parentEkin(-1.),
  m_useChildPt(true),
  m_childPt2(-1.),
  m_childEkin(-1.),
  m_allowChildrenOrParentPass(false),
  m_vertexTypesVector(0),
  m_vertexTypes(),
  m_doVertexRangeCheck(false),
  m_vertexTypeRangeLow(0),
  m_vertexTypeRangeHigh(0),
  m_vertexTypeRangeLength(0),
  m_parentPdgCodesVector(0),
  m_parentPdgCodes()
{
    // provide either a pT or Ekin cut for the parent and child particles respectively.
    // if none are given for either type, it will not use pT or Ekin cuts
    // (the Pt2 variables get squared in the initialize() method)
    declareProperty("ParentMinPt"               , m_parentPt2            );
    declareProperty("ParentMinEkin"             , m_parentEkin           );
    declareProperty("ChildMinPt"                , m_childPt2             );
    declareProperty("ChildMinEkin"              , m_childEkin        );
    // if set to true, kinetic cuts are passed even if only child particles pass them
    // (used for special cases such as de-excitation)
    declareProperty("AllowChildrenOrParentPassKineticCuts" , m_allowChildrenOrParentPass );
    declareProperty("VertexTypes"                , m_vertexTypesVector   );
    declareProperty("VertexTypeRangeLow"         , m_vertexTypeRangeLow  );
    declareProperty("VertexTypeRangeHigh"        , m_vertexTypeRangeHigh );
    declareProperty("ParentPDGCodes"            , m_parentPdgCodesVector  );
}

/** Destructor **/
ISF::GenericTruthStrategy::~GenericTruthStrategy()
{
}

// Athena algtool's Hooks
StatusCode  ISF::GenericTruthStrategy::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");

    // (*) setup parent particle cuts
    // -----
    //     (compute and store the squared cut parameters (faster comparisons))
    // check whether the user input makes sense (error case)
    if ( (m_parentPt2>=0.) && (m_parentEkin>=0.) ) {
      ATH_MSG_ERROR("Both, pT and Ekin cuts are given for parent particles. Unclear which one to use! ABORT!");
      return StatusCode::FAILURE;
    }
    // neither pT nor Energy cuts were given (never being used so far)
    //   -> enable pT cut and set it to 0.
    else if ( (m_parentPt2<0.) && (m_parentEkin<0.) ) {
      // we don't use a flag to disable energy/momentum checks, because
      // all relevant truth strategies up to now do use such cuts
      m_useParentPt = true;
      m_parentPt2   = 0.;
      m_parentEkin  = 0.;  // would not be needed actually
    }
    // either pT or Ekin cut is given (standard case)
    else {
      // enable pT cut if value given (greater than 0.):
      m_useParentPt  = !(m_parentPt2<0.);
      m_parentPt2   *= m_parentPt2;
    }

    // (*) setup child particle cuts
    // -----
    //     (compute and store the squared cut parameters (faster comparisons))
    // check whether the user input makes sense (error case)
    if ( (m_childPt2>=0.) && (m_childEkin>=0.) ) {
      ATH_MSG_ERROR("Both, pT and Ekin cuts are given for child particles. Unclear which one to use! ABORT!");
      return StatusCode::FAILURE;
    }
    // neither pT nor Energy cuts were given (never being used so far)
    //   -> enable pT cut and set it to 0.
    else if ( (m_childPt2<0.) && (m_childEkin<0.) ) {
      // we don't use a flag to disable energy/momentum checks, because
      // all relevant truth strategies up to now do use such cuts
      m_useChildPt = true;
      m_childPt2   = 0.;
      m_childEkin  = 0.;  // would not be needed actually
    }
    // either pT or Ekin cut is given (standard case)
    else {
      // enable pT cut if value given (greater than 0.):
      m_useChildPt = !(m_childPt2<0.);
      m_childPt2  *= m_childPt2;
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
    m_parentPdgCodes.insert( m_parentPdgCodesVector.begin(), m_parentPdgCodesVector.end());

    // fill vertex type std::set used for optimized search
    m_vertexTypes.insert( m_vertexTypesVector.begin(), m_vertexTypesVector.end());

    return StatusCode::SUCCESS;
}

StatusCode  ISF::GenericTruthStrategy::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}

bool ISF::GenericTruthStrategy::pass( ITruthIncident& ti) const {

  // (1.) momentum/energy check
  // ----
  //
  {
    // check whether parent particle passes cut or not
    bool primFail = (m_useParentPt) ? (ti.parentPt2()<m_parentPt2)
      : (ti.parentEkin()<m_parentEkin) ;


    // if parent particle failed and strategy does not
    // allow for child-only pass -> failed
    if ( ( primFail && (!m_allowChildrenOrParentPass) ) ) {
      return false;
    }

    // check child particles
    bool secPass =  m_useChildPt ? ti.childrenPt2Pass(m_childPt2)
                                 : ti.childrenEkinPass(m_childEkin);

    // if child particles do not pass cuts
    if (!secPass) {
      if (!m_allowChildrenOrParentPass) {
        // child particles were required to pass cuts but did not
        return false;
      } else if (primFail) {
        // neither parent nor child particles passed cuts
        return false;
      }
    }

  }


  // (2.) parent particle PDG code check
  // ----
  // check whether parent PDG code matches with any of the given ones
  if (  m_parentPdgCodes.size() &&
       (m_parentPdgCodes.find(ti.parentPdgCode()) == m_parentPdgCodes.end()) ) {

    // parent particle PDG code not found
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
