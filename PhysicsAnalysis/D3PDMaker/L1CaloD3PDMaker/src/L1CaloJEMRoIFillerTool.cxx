/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloJEMRoIFillerTool.cxx
*/

#include "L1CaloJEMRoIFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

  L1CaloJEMRoIFillerTool::L1CaloJEMRoIFillerTool(  const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent )
  : BlockFillerTool< LVL1::JEMRoI >( type, name, parent )
  {
    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode L1CaloJEMRoIFillerTool::book(){

    CHECK( addVariable ("crate",m_crate) );
    CHECK( addVariable ("jem",m_jem) );
    CHECK( addVariable ("frame",m_frame) );
    CHECK( addVariable ("location",m_location) );
    CHECK( addVariable ("forward",m_forward) );
    CHECK( addVariable ("hits",m_hits) );
    CHECK( addVariable ("error",m_error) );
    CHECK( addVariable ("roiWord",m_roiWord) );

    return StatusCode::SUCCESS;
  }

 StatusCode L1CaloJEMRoIFillerTool::fill( const LVL1::JEMRoI& p ){

    *m_crate     = p.crate();
    *m_jem       = p.jem();
    *m_frame     = p.frame();
    *m_location  = p.location();
    *m_forward   = p.forward();
    *m_hits      = p.hits();
    *m_error     = p.error();
    *m_roiWord   = p.roiWord();

    return StatusCode::SUCCESS;
  }
}
