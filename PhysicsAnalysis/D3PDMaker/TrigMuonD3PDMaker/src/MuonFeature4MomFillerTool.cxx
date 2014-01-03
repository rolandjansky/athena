/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonFeature4MomFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "MuonFeature4MomFillerTool.h"

namespace D3PD {

   MuonFeature4MomFillerTool::MuonFeature4MomFillerTool( const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent )
      : BlockFillerTool< MuonFeature >( type, name, parent )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode MuonFeature4MomFillerTool::book() {

      CHECK( addVariable( "pt",  m_pt,
                          "MuonFeature transverse momentum" ) );
      CHECK( addVariable( "eta", m_eta,
                          "MuonFeature pseudo-rapidity" ) );
      CHECK( addVariable( "phi", m_phi,
                          "MuonFeature azimuthal angle" ) );

      return StatusCode::SUCCESS;
   }

   StatusCode MuonFeature4MomFillerTool::fill( const MuonFeature& muon ) {

      *m_pt  = muon.pt();
      *m_eta = muon.eta();
      *m_phi = muon.phi();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
