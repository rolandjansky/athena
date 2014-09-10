/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: V0CandidateVxCandidateAssociationTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/V0CandidateVxCandidateAssociationTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a V0Candidate to a VxCandidate.
 */

#include "V0CandidateVxCandidateAssociationTool.h"
#include "AthenaKernel/errorcheck.h"

#include <VxVertex/VxCandidate.h>
#include <TrkV0Vertex/V0Candidate.h>

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
V0CandidateVxCandidateAssociationTool::V0CandidateVxCandidateAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}


StatusCode V0CandidateVxCandidateAssociationTool::initialize(){

  CHECK ( Base::initialize() );

  //m_v0Tools = ToolHandle<Trk::V0Tools> ("Trk::V0Tools");

  return StatusCode::SUCCESS;
}

/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::VxCandidate *V0CandidateVxCandidateAssociationTool::get (const Trk::V0Candidate& /*v0*/)
{

  return 0;//(const Trk::VxCandidate *)  m_v0Tools->v0Unconstr( &v0 );
}

} // namespace D3PD
