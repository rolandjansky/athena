/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FitQualityFillerTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TrackFitQualityFillerTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Block filler tool for generic fit quality.
 */
#include "TrackFitQualityFillerTool.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackFitQualityFillerTool::TrackFitQualityFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Trk::FitQuality> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode TrackFitQualityFillerTool::book()
{
  CHECK( addVariable ("chi2",   m_chi2)   );
  CHECK( addVariable ("ndof",   m_ndof)   );
 
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param fq The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TrackFitQualityFillerTool::fill (const Trk::FitQuality& fq) {
  this->clearData();

  *m_chi2  = fq.chiSquared();
  *m_ndof  = fq.numberDoF();

  return StatusCode::SUCCESS;
}

/**
 * @brief initialize the variables to default values
 */
void TrackFitQualityFillerTool::clearData(){

  *m_chi2  = 0;
  *m_ndof  = 0;
}

} // namespace D3PD
