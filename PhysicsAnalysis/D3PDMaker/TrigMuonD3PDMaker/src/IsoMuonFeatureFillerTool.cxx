/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IsoMuonFeatureFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "IsoMuonFeatureFillerTool.h"

namespace D3PD {

   IsoMuonFeatureFillerTool::IsoMuonFeatureFillerTool( const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent )
      : BlockFillerTool< IsoMuonFeature >( type, name, parent )
   {
      // Avoid coverity warnings.
      m_writeHypoInfo = true;
      m_writeDetailedInfo = true;
      book().ignore();

      declareProperty( "WriteHypoInfo",     m_writeHypoInfo = false );
      declareProperty( "WriteDetailedInfo", m_writeDetailedInfo = false );
   }

   StatusCode IsoMuonFeatureFillerTool::book() {
      if(m_writeHypoInfo){
         CHECK( addVariable( "sumet01", m_sumet01,
                             "Transverse energy sum in 0.1 cone" ) );
         CHECK( addVariable( "sumet02", m_sumet02,
                             "Transverse energy sum in 0.2 cone" ) );
         CHECK( addVariable( "sumet03", m_sumet03,
                             "Transverse energy sum in 0.3 cone" ) );
         CHECK( addVariable( "sumet04", m_sumet04,
                             "Transverse energy sum in 0.4 cone" ) );

         CHECK( addVariable( "sumpt01", m_sumpt01,
                             "Transverse pT sum in 0.1 cone" ) );
         CHECK( addVariable( "sumpt02", m_sumpt02,
                             "Transverse pT sum in 0.2 cone" ) );
         CHECK( addVariable( "sumpt03", m_sumpt03,
                             "Transverse pT sum in 0.3 cone" ) );
         CHECK( addVariable( "sumpt04", m_sumpt04,
                             "Transverse pT sum in 0.4 cone" ) );

         CHECK( addVariable( "ptMuID", m_PtMuID,
                             "Pt of charged track associated to muon" ) );
         CHECK( addVariable( "maxPtID", m_MaxPtID,
                             "Pt of max pt ID track in cone (muon excluded)" ) );
      }
      if(m_writeDetailedInfo){
         CHECK( addVariable( "iso_strategy", m_iso_strategy,
                             "Isolation strategy (1=CombinedMuon seeded, "
                             "2=muFast Muon seeded, 3=L1 muon seeded)" ) );
         CHECK( addVariable( "iso_errorFlag", m_iso_errorFlag,
                             "Error flag" ) );

         CHECK( addVariable( "RoiIdMu ", m_RoiIdMu,
                             "Muon RoI Id" ) );
      }
      return StatusCode::SUCCESS;
   }

   StatusCode IsoMuonFeatureFillerTool::fill( const IsoMuonFeature& muon ) {

      if(m_writeHypoInfo){
         *m_sumet01 = muon.getsumet01();
         *m_sumet02 = muon.getsumet02();
         *m_sumet03 = muon.getsumet03();
         *m_sumet04 = muon.getsumet04();

         *m_sumpt01 = muon.getsumpt01();
         *m_sumpt02 = muon.getsumpt02();
         *m_sumpt03 = muon.getsumpt03();
         *m_sumpt04 = muon.getsumpt04();

         *m_PtMuID  = muon.getPtMuID();
         *m_MaxPtID = muon.getMaxPtID();
      }

      if(m_writeDetailedInfo){
         *m_iso_strategy  = muon.iso_strategy();
         *m_iso_errorFlag = muon.iso_errorFlag();

         *m_RoiIdMu = muon.getRoiIdMu();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
