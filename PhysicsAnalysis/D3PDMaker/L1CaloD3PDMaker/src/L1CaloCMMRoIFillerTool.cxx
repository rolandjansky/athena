/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloCMMRoIFillerTool.cxx
*/

#include "L1CaloCMMRoIFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

  L1CaloCMMRoIFillerTool::L1CaloCMMRoIFillerTool(  const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : BlockFillerTool< LVL1::CMMRoI >( type, name, parent )
  {
    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode L1CaloCMMRoIFillerTool::book(){

    CHECK( addVariable("jetEtHits",m_jetEtHits) );
    CHECK( addVariable("sumEtHits",m_sumEtHits) );
    CHECK( addVariable("missingEtHits",m_missingEtHits) );
    CHECK( addVariable("ex",m_ex) );
    CHECK( addVariable("ey",m_ey) );
    CHECK( addVariable("et",m_et) );
    CHECK( addVariable("jetEtError",m_jetEtError) );
    CHECK( addVariable("sumEtError",m_sumEtError) );
    CHECK( addVariable("missingEtError",m_missingEtError) );
    CHECK( addVariable("missingEtSigError",m_missingEtSigError) );
    CHECK( addVariable("exError",m_exError) );
    CHECK( addVariable("eyError",m_eyError) );
    CHECK( addVariable("etError",m_etError) );
    CHECK( addVariable("jetEtRoiWord",m_jetEtRoiWord) );
    CHECK( addVariable("energyRoiWord0",m_energyRoiWord0) );
    CHECK( addVariable("energyRoiWord1",m_energyRoiWord1) );
    CHECK( addVariable("energyRoiWord2",m_energyRoiWord2) );

    return StatusCode::SUCCESS;
  }

 StatusCode L1CaloCMMRoIFillerTool::fill( const LVL1::CMMRoI& p ){
    *m_jetEtHits         = p.jetEtHits();
    *m_sumEtHits         = p.sumEtHits();
    *m_missingEtHits     = p.missingEtHits();
    *m_ex                = p.ex();
    *m_ey                = p.ey();
    *m_et                = p.et();
    *m_jetEtError        = p.jetEtError();
    *m_sumEtError        = p.sumEtError();
    *m_missingEtError    = p.missingEtError();
    *m_missingEtSigError = p.missingEtSigError();
    *m_exError           = p.exError();
    *m_eyError           = p.eyError();
    *m_etError           = p.etError();
    *m_jetEtRoiWord      = p.jetEtRoiWord();
    *m_energyRoiWord0    = p.energyRoiWord0();
    *m_energyRoiWord1    = p.energyRoiWord1();
    *m_energyRoiWord2    = p.energyRoiWord2();

    return StatusCode::SUCCESS;
  }  

}
