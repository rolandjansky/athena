/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KeepChildrenTruthStrategy.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "KeepChildrenTruthStrategy.h"

// ISF includes
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ISFParticle.h"

// For BSM helper
#include "TruthUtils/PIDHelpers.h"


/** Constructor **/
ISF::KeepChildrenTruthStrategy::KeepChildrenTruthStrategy(const std::string& t, const std::string& n, const IInterface* p)
  : base_class(t,n,p)
  , m_vertexTypesVector(0)
  , m_vertexTypes()
  , m_doVertexRangeCheck(false)
  , m_vertexTypeRangeLow(0)
  , m_vertexTypeRangeHigh(0)
  , m_vertexTypeRangeLength(0)
  , m_passProcessCategory(0)
  , m_bsmParent(false)
  , m_parentPdgCodesVector(0)
  , m_parentPdgCodes()
{
    // if set to true, kinetic cuts are passed even if only child particles pass them
    // (used for special cases such as de-excitation)
    declareProperty("VertexTypes"                , m_vertexTypesVector    );
    declareProperty("VertexTypeRangeLow"         , m_vertexTypeRangeLow   );
    declareProperty("VertexTypeRangeHigh"        , m_vertexTypeRangeHigh  );
    declareProperty("PassProcessCategory",      m_passProcessCategory=9);
    declareProperty("ParentPDGCodes"             , m_parentPdgCodesVector );
    declareProperty("BSMParent"                  , m_bsmParent            );
}

/** Destructor **/
ISF::KeepChildrenTruthStrategy::~KeepChildrenTruthStrategy()
{
}

// Athena algtool's Hooks
StatusCode  ISF::KeepChildrenTruthStrategy::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");

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

bool ISF::KeepChildrenTruthStrategy::pass( ITruthIncident& ti) const {

  // check whether parent PDG code matches with any of the given ones
  // Could require BSM parents and hit BSM, or give a list and be on the list
  // Or require neither and apply to everything
  if ( !( (m_bsmParent && MC::PID::isBSM(abs(ti.parentPdgCode()))) || // BSM parent and parent is BSM
          (m_parentPdgCodes.size() && m_parentPdgCodes.find(ti.parentPdgCode())==m_parentPdgCodes.end()) || // Explicit list and parent in the list
          (!m_bsmParent && !m_parentPdgCodes.size()) ) ){ // Neither BSM parent nor explicit list -- allow all
    return false;
  }

  // vertex type check
  // ----
  int vxtype = ti.physicsProcessCode();
  int processCategory = ti.physicsProcessCategory(); // == G4ProcessType
  if ((processCategory!=m_passProcessCategory && m_passProcessCategory!=0) && // Not in the category we were after
      (m_doVertexRangeCheck || m_vertexTypes.size()) ){ // Should do the category check
    // (part 1) vxtype in given range?: this is a small performance trick (only one comparison operator to check if within range)
    //  -> exactly equivalent to:  m_doVertexRangeCheck  && (m_vertexTypeLow<=vxtype) && (vxtype<=m_vertexTypeRangeHigh)
    bool vtxTypeRangePassed = m_doVertexRangeCheck && ( unsigned(vxtype-m_vertexTypeRangeLow) < m_vertexTypeRangeLength );
    // (part 2) if not in range or range check disabled, check whether vxtype
    //       std::set contains the given vertex type
    if ( (!vtxTypeRangePassed) && (m_vertexTypes.find(vxtype)==m_vertexTypes.end()) ) {
        // vxtype not registered -> not passed
        return false;
    }
  }

  // all cuts passed. Save all children.
  for (unsigned short i=0;i<ti.numberOfChildren();++i){
    ti.setChildPassedFilters(i);
  }

  // This strategy does not cause the *saving* of a vertex -- it only changes the strategy for the children
  return false;
}
