// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: V0CandidateVxCandidateAssociationTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file EventCommonD3PDMaker/src/V0CandidateVxCandidateAssociationTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a VxCandidate to its FitQuality.
 */
#ifndef TRACKD3PDMAKER_V0CANDIDATEVXCANDIDATEASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_V0CANDIDATEVXCANDIDATEASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"
//#include "TrkVertexAnalysisUtils/V0Tools.h"

namespace Trk {
class V0Candidate;
class VxCandidate;
}

namespace D3PD {

/**
 * @brief Associate from a V0Candidate to a VxCandidate.
 */
class V0CandidateVxCandidateAssociationTool
  : public SingleAssociationTool<Trk::V0Candidate, Trk::VxCandidate>
{
public:
  typedef SingleAssociationTool<Trk::V0Candidate, Trk::VxCandidate> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  V0CandidateVxCandidateAssociationTool (const std::string& type,
					 const std::string& name,
					 const IInterface* parent);


  virtual StatusCode initialize();

  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::VxCandidate* get (const Trk::V0Candidate& v0);

 private:

  //ToolHandle<Trk::V0Tools> m_v0Tools;

};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_V0CANDIDATEVXCANDIDATEASSOCIATIONTOOL_H
