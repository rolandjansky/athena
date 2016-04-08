/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "UnassociatedHitsFillerTool.h"

#include "AthenaKernel/errorcheck.h"

namespace D3PD {


UnassociatedHitsFillerTool::UnassociatedHitsFillerTool (const std::string& type,
						      const std::string& name,
						      const IInterface* parent)
  : UnassociatedHitsFillerBase (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode UnassociatedHitsFillerTool::initialize(){

  CHECK( UnassociatedHitsFillerBase::initialize() );
  
  return StatusCode::SUCCESS;
}

StatusCode UnassociatedHitsFillerTool::book()
{

  CHECK( addVariable ("nPixel", m_nPixelUA) );
  CHECK( addVariable ("nBlayer", m_nBlayerUA) );
  CHECK( addVariable ("nPixelBarrel", m_nPixelBarrelUA) );
  CHECK( addVariable ("nPixelEndCapA", m_nPixelEndCapAUA) );
  CHECK( addVariable ("nPixelEndCapC", m_nPixelEndCapCUA) );

  CHECK( addVariable ("nSCT", m_nSCTUA) );
  CHECK( addVariable ("nSCTBarrel", m_nSCTBarrelUA) );
  CHECK( addVariable ("nSCTEndCapA", m_nSCTEndCapAUA) );
  CHECK( addVariable ("nSCTEndCapC", m_nSCTEndCapCUA) );

  CHECK( addVariable ("nTRT", m_nTRTUA) );
  CHECK( addVariable ("nTRTBarrel", m_nTRTBarrelUA) );
  CHECK( addVariable ("nTRTEndCapA", m_nTRTEndCapAUA) );
  CHECK( addVariable ("nTRTEndCapC", m_nTRTEndCapCUA) );

  return StatusCode::SUCCESS;
}

StatusCode UnassociatedHitsFillerTool::fill (const UnassociatedHitsFillerObjectType& PRDAssoc)
{

  this->clearData();

  *m_nPixelUA = PRDAssoc.nPixelUA;
  *m_nBlayerUA = PRDAssoc.nBlayerUA;
  *m_nPixelBarrelUA = PRDAssoc.nPixelBarrelUA;
  *m_nPixelEndCapAUA = PRDAssoc.nPixelEndCapAUA;
  *m_nPixelEndCapCUA = PRDAssoc.nPixelEndCapCUA;

  *m_nSCTUA = PRDAssoc.nSCTUA;
  *m_nSCTBarrelUA = PRDAssoc.nSCTBarrelUA;
  *m_nSCTEndCapAUA = PRDAssoc.nSCTEndCapAUA;
  *m_nSCTEndCapCUA = PRDAssoc.nSCTEndCapAUA;

  *m_nTRTUA = PRDAssoc.nTRTUA;
  *m_nTRTBarrelUA = PRDAssoc.nTRTBarrelUA;
  *m_nTRTEndCapAUA = PRDAssoc.nTRTEndCapAUA;
  *m_nTRTEndCapCUA = PRDAssoc.nTRTEndCapAUA;

  return StatusCode::SUCCESS;
}

void UnassociatedHitsFillerTool::clearData(){

  *m_nPixelUA = 0;
  *m_nBlayerUA = 0;
  *m_nPixelBarrelUA = 0;
  *m_nPixelEndCapAUA = 0;
  *m_nPixelEndCapCUA = 0;

  *m_nSCTUA = 0;
  *m_nSCTBarrelUA = 0;
  *m_nSCTEndCapAUA = 0;
  *m_nSCTEndCapCUA = 0;

  *m_nTRTUA = 0;
  *m_nTRTBarrelUA = 0;
  *m_nTRTEndCapAUA = 0;
  *m_nTRTEndCapCUA = 0;
}

} // namespace MinBiasD3PD
