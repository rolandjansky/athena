/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VxTrackAtVertexAssociationTool.cxx
/**
 * @file PhotonD3PDMaker/src/VxTrackAtVertexAssociationTool.cxx
 * @author Jean-Francois Marchand <jean-francois.marchand@cern.ch>
 * @date November 2012
 * @brief Associate VxTrackAtVertex to conversions vertex
 */

#include "VxTrackAtVertexAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "AthenaKernel/errorcheck.h"

#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"


namespace D3PD {

  
  VxTrackAtVertexAssociationTool::VxTrackAtVertexAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
  {
  }

  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  StatusCode VxTrackAtVertexAssociationTool::reset (const Trk::VxCandidate& vx)
  {    
    const std::vector<Trk::VxTrackAtVertex*>* tracks = vx.vxTrackAtVertex();   
    m_trkItr = tracks->begin();
    m_trkEnd = tracks->end();    
    return StatusCode::SUCCESS;
  }
  
  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  const Trk::VxTrackAtVertex* VxTrackAtVertexAssociationTool::next()
  {
    // reached the end
    if(m_trkItr == m_trkEnd) return 0;    
    const Trk::VxTrackAtVertex *track = *m_trkItr;
    m_trkItr++;
    return track;
  }
    
} // namespace D3PD
