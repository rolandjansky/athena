/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigMuonEFInfoFillerTool.h"

namespace D3PD {

   TrigMuonEFInfoFillerTool::TrigMuonEFInfoFillerTool( const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent )
      : BlockFillerTool< TrigMuonEFInfo >( type, name, parent )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode TrigMuonEFInfoFillerTool::book() {

      CHECK( addVariable( "RoINum",           m_roINum ) );
      CHECK( addVariable( "NSegments",        m_nSegments ) );
      CHECK( addVariable( "NMdtHits",         m_nMdtHits ) );
      CHECK( addVariable( "NRpcHits",         m_nRpcHits ) );
      CHECK( addVariable( "NTgcHits",         m_nTgcHits ) );
      CHECK( addVariable( "NCscHits",         m_nCscHits ) );
      CHECK( addVariable( "EtaPreviousLevel", m_etaPreviousLevel ) );
      CHECK( addVariable( "PhiPreviousLevel", m_phiPreviousLevel ) );

      return StatusCode::SUCCESS;
   }

   StatusCode TrigMuonEFInfoFillerTool::fill( const TrigMuonEFInfo& muon ) {

      *m_roINum           = muon.RoINum();
      *m_nSegments        = muon.NSegments();
      *m_nMdtHits         = muon.NMdtHits();
      *m_nRpcHits         = muon.NRpcHits();
      *m_nTgcHits         = muon.NTgcHits();
      *m_nCscHits         = muon.NCscHits();
      *m_etaPreviousLevel = muon.EtaPreviousLevel();
      *m_phiPreviousLevel = muon.PhiPreviousLevel();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD



