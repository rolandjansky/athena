/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////
//
// Author : Srivas Prasad (srivas.prasad@cern.ch)
// Date   : February 2010
//
//////////////////////////////////////////////////////
#include "MuonSegmentAuthorFillerTool.h"
#include "TrkSegment/Segment.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


MuonSegmentAuthorFillerTool::MuonSegmentAuthorFillerTool (const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
  : BlockFillerTool<Trk::Segment> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


StatusCode MuonSegmentAuthorFillerTool::book()
{
  CHECK( addVariable ("author",          m_author)  );
  return StatusCode::SUCCESS;
}


StatusCode MuonSegmentAuthorFillerTool::fill (const Trk::Segment& ms) {
  *m_author = ms.author();

  return StatusCode::SUCCESS;
}


} // namespace D3PD
