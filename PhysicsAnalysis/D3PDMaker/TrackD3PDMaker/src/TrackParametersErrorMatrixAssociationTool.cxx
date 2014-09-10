/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/TrackParametersErrorMatrixAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2012
 * @brief Associate from TrackParameters to its associated ErrorMatrix, if any.
 */


#include "TrackParametersErrorMatrixAssociationTool.h"
#include "TrkParameters/TrackParameters.h" // typedef; can't be fwd declared
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackParametersErrorMatrixAssociationTool::TrackParametersErrorMatrixAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}


/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
  const AmgSymMatrix(5)*
TrackParametersErrorMatrixAssociationTool::get (const Trk::TrackParameters& p)
{
  return p.covariance();
}


} // namespace D3PD
