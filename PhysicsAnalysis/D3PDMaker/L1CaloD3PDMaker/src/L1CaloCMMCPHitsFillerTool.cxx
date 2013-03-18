/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloCMMCPHitsFillerTool.cxx
*/

#include "L1CaloCMMCPHitsFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

  L1CaloCMMCPHitsFillerTool::L1CaloCMMCPHitsFillerTool(  const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent )
  : BlockFillerTool< LVL1::CMMCPHits >( type, name, parent ) 
  {
    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode L1CaloCMMCPHitsFillerTool::book(){
    CHECK( addVariable ("crate",m_crate) );
    CHECK( addVariable ("dataID",m_dataID) );
    CHECK( addVariable ("hitWord0",m_hitWord0) );    
    CHECK( addVariable ("hitWord1",m_hitWord1) );
    CHECK( addVariable ("error0",m_error0) );
    CHECK( addVariable ("error1",m_error1) );
    CHECK( addVariable ("hitsVec0",m_hitsVec0) );
    CHECK( addVariable ("hitsVec1",m_hitsVec1) );
    CHECK( addVariable ("errorVec0",m_errorVec0) );
    CHECK( addVariable ("errorVec1",m_errorVec1) );
    CHECK( addVariable ("peak",m_peak) );

    return StatusCode::SUCCESS;
  }

 StatusCode L1CaloCMMCPHitsFillerTool::fill( const LVL1::CMMCPHits& p ){

   *m_crate      = p.crate();
   *m_dataID     = p.dataID();
   *m_hitWord0   = p.HitWord0();
   *m_hitWord1   = p.HitWord1();
   *m_error0     = p.Error0();
   *m_error1     = p.Error1();
   *m_hitsVec0   = p.HitsVec0();
   *m_hitsVec1   = p.HitsVec1();
   *m_errorVec0  = p.ErrorVec0();
   *m_errorVec1  = p.ErrorVec1();
   *m_peak       = p.peak();

    return StatusCode::SUCCESS;
  }  

}
