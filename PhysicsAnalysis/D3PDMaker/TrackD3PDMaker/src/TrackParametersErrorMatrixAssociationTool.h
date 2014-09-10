// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/TrackParametersErrorMatrixAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2012
 * @brief Associate from TrackParameters to its associated ErrorMatrix, if any.
 */


#ifndef TRACKD3PDMAKER_TRACKPARAMETERSERRORMATRIXASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRACKPARAMETERSERRORMATRIXASSOCIATIONTOOL_H



#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "TrkParameters/TrackParameters.h" // typedef; can't be fwd declared



namespace D3PD {


/**
 * @brief Associate from TrackParameters to its associated ErrorMatrix, if any.
 */
class TrackParametersErrorMatrixAssociationTool
  : public SingleAssociationTool<Trk::TrackParameters, AmgSymMatrix(5)>
{
public:
  typedef SingleAssociationTool<Trk::TrackParameters, AmgSymMatrix(5)> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackParametersErrorMatrixAssociationTool (const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const AmgSymMatrix(5)* get (const Trk::TrackParameters& p);
};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_TRACKPARAMETERSERRORMATRIXASSOCIATIONTOOL_H
