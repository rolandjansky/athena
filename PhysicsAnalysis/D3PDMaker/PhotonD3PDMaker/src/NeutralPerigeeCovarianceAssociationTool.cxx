/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: NeutralPerigeeCovarianceAssociationTool.cxx
/**
 * @file PhotonD3PDMaker/src/NeutralPerigeeCovarianceAssociationTool.cxx
 * @author Jean-Francois Marchand <jean-francois.marchadn@cern.ch>
 * @date Nov, 2012
 * @brief Associate from a NeutralPerigee to its covariance matrix.
 */

#include "NeutralPerigeeCovarianceAssociationTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  NeutralPerigeeCovarianceAssociationTool::NeutralPerigeeCovarianceAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
  {
  }
  
  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
   const AmgSymMatrix(5) *NeutralPerigeeCovarianceAssociationTool::get (const Trk::NeutralPerigee& p)
   {
     return p.covariance();
   }
  
} // namespace D3PD
