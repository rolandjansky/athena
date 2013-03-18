/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloCPMHitsFillerTool.cxx
*/

#include "L1CaloCPMHitsFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

  L1CaloCPMHitsFillerTool::L1CaloCPMHitsFillerTool(  const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
  : BlockFillerTool< LVL1::CPMHits >( type, name, parent )
  {
    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode L1CaloCPMHitsFillerTool::book(){

    CHECK( addVariable ("crate",m_crate) );
    CHECK( addVariable ("module",m_module) );
    CHECK( addVariable ("peak",m_peak) );
    CHECK( addVariable ("hitWord0",m_hitWord0) );
    CHECK( addVariable ("hitWord1",m_hitWord1) );
    CHECK( addVariable ("hitsVec0",m_hitsVec0) );
    CHECK( addVariable ("hitsVec1",m_hitsVec1) );

    return StatusCode::SUCCESS;
  }

 StatusCode L1CaloCPMHitsFillerTool::fill( const LVL1::CPMHits& p ){

    *m_crate     = p.crate();
    *m_module    = p.module();
    *m_peak      = p.peak();
    *m_hitWord0  = p.HitWord0();
    *m_hitWord1  = p.HitWord1();
    *m_hitsVec0  = p.HitsVec0();
    *m_hitsVec1  = p.HitsVec1();

    return StatusCode::SUCCESS;
  }  

}
