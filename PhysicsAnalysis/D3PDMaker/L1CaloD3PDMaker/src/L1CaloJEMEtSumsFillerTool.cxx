/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloJEMEtSumsFillerTool.cxx
*/

#include "L1CaloJEMEtSumsFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

  L1CaloJEMEtSumsFillerTool::L1CaloJEMEtSumsFillerTool(  const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent )
  : BlockFillerTool< LVL1::JEMEtSums >( type, name, parent )
  {
    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode L1CaloJEMEtSumsFillerTool::book(){

    CHECK( addVariable ("crate",m_crate) );
    CHECK( addVariable ("module",m_module) );
    CHECK( addVariable ("peak",m_peak) );
    CHECK( addVariable ("forward",m_forward) );
    CHECK( addVariable ("et",m_et) );
    CHECK( addVariable ("ex",m_ex) );
    CHECK( addVariable ("ey",m_ey) );
    CHECK( addVariable ("etVec",m_etVec) );
    CHECK( addVariable ("exVec",m_exVec) );
    CHECK( addVariable ("eyVec",m_eyVec) );

    return StatusCode::SUCCESS;
  }

 StatusCode L1CaloJEMEtSumsFillerTool::fill( const LVL1::JEMEtSums& p ){

   *m_crate       = p.crate();
   *m_module      = p.module();
   *m_peak        = p.peak();
   *m_et          = p.Et();
   *m_ex          = p.Ex();
   *m_ey          = p.Ey();
   *m_etVec       = p.EtVec();
   *m_exVec       = p.ExVec();
   *m_eyVec       = p.EyVec();

   // Do not save bool, save an unsigned int instead
   unsigned int f(0);
   if( p.forward() == true ){f = 1;}
   *m_forward     = f;

    return StatusCode::SUCCESS;
  }  

}
