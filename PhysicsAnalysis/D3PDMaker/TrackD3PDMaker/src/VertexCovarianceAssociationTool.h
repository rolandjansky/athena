// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VertexCovarianceAssociationTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/VertexFitQualityAssociationTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a VxCandidate to its Position ErrorMatrix.
 */
#ifndef TRACKD3PDMAKER_VERTEXCOVARIANCEASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_VERTEXCOVARIANCEASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "xAODTracking/Vertex.h"

namespace Trk {
class VxCandidate;
}

namespace D3PD {

/**
 * @brief Associate from a VxCandidate to its Position ErrorMatrix.
 */
class VertexCovarianceAssociationTool
  : public SingleAssociationTool<Types<Trk::VxCandidate, xAOD::Vertex>,
                                 AmgSymMatrix(3)>
{
public:
  typedef SingleAssociationTool<Types<Trk::VxCandidate, xAOD::Vertex>,
                                AmgSymMatrix(3)> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  VertexCovarianceAssociationTool (const std::string& type,
				   const std::string& name,
				   const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const AmgSymMatrix(3)* get (const Trk::VxCandidate& p);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const AmgSymMatrix(3)* get (const xAOD::Vertex& p);
};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_VERTEXCOVARIANCEASSOCIATIONTOOL_H
