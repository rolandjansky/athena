/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloCMMJetHitsFillerTool.cxx
*/

#include "L1CaloCMMJetHitsFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

  L1CaloCMMJetHitsFillerTool::L1CaloCMMJetHitsFillerTool(  const std::string& type,
                                                           const std::string& name,
                                                           const IInterface* parent )
  : BlockFillerTool< LVL1::CMMJetHits >( type, name, parent )
  {
    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode L1CaloCMMJetHitsFillerTool::book(){
    CHECK( addVariable ("crate",m_crate) );
    CHECK( addVariable ("dataID",m_dataID) );
    CHECK( addVariable ("peak",m_peak) );
    CHECK( addVariable ("hits",m_hits) );
    CHECK( addVariable ("error",m_error) );
    CHECK( addVariable ("hitsVec",m_hitsVec) );
    CHECK( addVariable ("errorVec",m_errorVec) );

    return StatusCode::SUCCESS;
  }

 StatusCode L1CaloCMMJetHitsFillerTool::fill( const LVL1::CMMJetHits& p ){

   *m_crate      = p.crate();
   *m_dataID     = p.dataID();
   *m_peak       = p.peak();
   *m_hits       = p.Hits();
   *m_error      = p.Error();
   *m_hitsVec    = p.HitsVec();
   *m_errorVec   = p.ErrorVec();
   
    return StatusCode::SUCCESS;
  }  

}
