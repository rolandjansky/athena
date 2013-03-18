/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloCMMEtSumsFillerTool.cxx
*/

#include "L1CaloCMMEtSumsFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

  L1CaloCMMEtSumsFillerTool::L1CaloCMMEtSumsFillerTool(  const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent )
  : BlockFillerTool< LVL1::CMMEtSums >( type, name, parent )
  {
    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode L1CaloCMMEtSumsFillerTool::book(){

    CHECK( addVariable ("crate",m_crate) );
    CHECK( addVariable ("dataID",m_dataID) );
    CHECK( addVariable ("peak",m_peak) );
    CHECK( addVariable ("et",m_et) );
    CHECK( addVariable ("ex",m_ex) );
    CHECK( addVariable ("ey",m_ey) );
    CHECK( addVariable ("etError",m_etError) );
    CHECK( addVariable ("exError",m_exError) );
    CHECK( addVariable ("eyError",m_eyError) );
    CHECK( addVariable ("etVec",m_etVec) );
    CHECK( addVariable ("exVec",m_exVec) );
    CHECK( addVariable ("eyVec",m_eyVec) );
    CHECK( addVariable ("etErrorVec",m_etErrorVec) );
    CHECK( addVariable ("exErrorVec",m_exErrorVec) );
    CHECK( addVariable ("eyErrorVec",m_eyErrorVec) );

    return StatusCode::SUCCESS;
  }

 StatusCode L1CaloCMMEtSumsFillerTool::fill( const LVL1::CMMEtSums& p ){

   *m_crate       = p.crate();
   *m_dataID      = p.dataID();
   *m_peak        = p.peak();
   *m_et          = p.Et();
   *m_ex          = p.Ex();
   *m_ey          = p.Ey();
   *m_etError     =  p.EtError();
   *m_exError     = p.ExError();
   *m_eyError     = p.EyError();
   *m_etVec       = p.EtVec();
   *m_exVec       = p.ExVec();
   *m_eyVec       = p.EyVec();
   *m_etErrorVec  = p.EtErrorVec();
   *m_exErrorVec  = p.ExErrorVec();
   *m_eyErrorVec  = p.EyErrorVec();

    return StatusCode::SUCCESS;
  }  

}
