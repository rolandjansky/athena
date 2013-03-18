/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloJEMHitsFillerTool.cxx
*/

#include "L1CaloJEMHitsFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

  L1CaloJEMHitsFillerTool::L1CaloJEMHitsFillerTool(  const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
  : BlockFillerTool< LVL1::JEMHits >( type, name, parent )
  {
    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode L1CaloJEMHitsFillerTool::book(){

    CHECK( addVariable ("crate",m_crate) );
    CHECK( addVariable ("module",m_module) );
    CHECK( addVariable ("peak",m_peak) );
    CHECK( addVariable ("forward",m_forward) );
    CHECK( addVariable ("jetHits",m_jetHits) );
    CHECK( addVariable ("jetHitsVec",m_jetHitsVec) );

    return StatusCode::SUCCESS;
  }

 StatusCode L1CaloJEMHitsFillerTool::fill( const LVL1::JEMHits& p ){

    *m_crate       = p.crate();
    *m_module      = p.module();
    *m_peak        = p.peak();
    *m_jetHits     = p.JetHits();
    *m_jetHitsVec  = p.JetHitsVec();

   // Do not save bool, save an unsigned int instead
   unsigned int f(0);
   if( p.forward() == true ){f = 1;}
   *m_forward      = f;    

    return StatusCode::SUCCESS;
  }  

}
