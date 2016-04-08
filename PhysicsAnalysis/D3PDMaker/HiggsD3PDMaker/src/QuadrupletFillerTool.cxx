/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: QuadrupletFillerTool.cxx 500852 2012-05-15 15:01:24Z ssnyder $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "QuadrupletFillerTool.h"

namespace D3PD {

   QuadrupletFillerTool::QuadrupletFillerTool( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
      : BlockFillerTool< HSG2::Quadruplet >( type, name, parent )
   {
     book().ignore();  // Avoid coverity warnings.
   }

   StatusCode QuadrupletFillerTool::book() {

      CHECK( addVariable( "finalState", m_fs, "Final state type (0: 4 muon; "
                          "1: 4 electron; 2: 2e2mu)" ) );
 
      return StatusCode::SUCCESS;
   }

   StatusCode QuadrupletFillerTool::fill( const HSG2::Quadruplet& q ) {

      *m_fs = static_cast< unsigned int >( q.finalState() );

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
