/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PerigeeCovarianceAssociationTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/PerigeeCovarianceAssociationTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a Perigee to its covariance matrix.
 */

#include "PerigeeCovarianceAssociationTool.h"

//#include "TrkParameters/Perigee.h"
//#include "TrkParameters/TrackParameters.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PerigeeCovarianceAssociationTool::PerigeeCovarianceAssociationTool
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
  const AmgSymMatrix(5) * PerigeeCovarianceAssociationTool::get(const Trk::TrackParameters& p)
{
  // cast to MeasuredPerigee
  //const Trk::MeasuredPerigee *measPerigee = dynamic_cast<const Trk::MeasuredPerigee *>(&p);
  const AmgSymMatrix(5) * cov  = p.covariance();
  if (cov) {
    // return pointer to Covariance Matrix
    return cov;
  }else{
    REPORT_MESSAGE (MSG::WARNING) << "Could not cast Trk::Perigee to Trk::MeasuredPerigee";
  }

  return 0;
}

} // namespace D3PD
