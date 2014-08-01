/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////
//
// Author : Srivas Prasad (srivas.prasad@cern.ch)
// Date   : February 2010
//
//////////////////////////////////////////////////////
#include "MuonSegmentFitQualityFillerTool.h"
#include "TrkSegment/Segment.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


MuonSegmentFitQualityFillerTool::MuonSegmentFitQualityFillerTool (const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
  : BlockFillerTool<Trk::Segment> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


StatusCode MuonSegmentFitQualityFillerTool::book()
{
  CHECK( addVariable ("chi2",          m_chi2)  );
  CHECK( addVariable ("ndof",          m_ndof)  );
  return StatusCode::SUCCESS;
}


StatusCode MuonSegmentFitQualityFillerTool::fill (const Trk::Segment& ts) {
  const Muon::MuonSegment& mSeg = dynamic_cast<const Muon::MuonSegment&> (ts);

  *m_chi2 = (float) mSeg.fitQuality()->chiSquared();
  *m_ndof = (int) mSeg.fitQuality()->numberDoF();

  return StatusCode::SUCCESS;
}


} // namespace D3PD
