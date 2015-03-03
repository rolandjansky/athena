/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonFeatureFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "MuonFeatureFillerTool.h"

namespace D3PD {

   MuonFeatureFillerTool::MuonFeatureFillerTool( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
      : BlockFillerTool< MuonFeature >( type, name, parent )
   {
      // Avoid coverity warnings.
      m_writeRecoInfo = true;
      m_writeHitsInfo = true;
      book().ignore();

      declareProperty( "WriteRecoInfo", m_writeRecoInfo = false );
      declareProperty( "WriteHitsInfo", m_writeHitsInfo = false );
   }

   StatusCode MuonFeatureFillerTool::book() {
      if(m_writeRecoInfo){
         CHECK( addVariable( "dir_phi",  m_dir_phi ) );
         CHECK( addVariable( "zeta",     m_zeta ) );
         CHECK( addVariable( "dir_zeta", m_dir_zeta ) );
         CHECK( addVariable( "radius"    , m_radius ) );
         CHECK( addVariable( "beta"      , m_beta ) );
         CHECK( addVariable( "br_radius" , m_br_radius ) );
         CHECK( addVariable( "br_sagitta", m_br_sagitta ) );
         CHECK( addVariable( "ec_alpha"  , m_ec_alpha ) );
         CHECK( addVariable( "ec_beta"   , m_ec_beta ) );
         CHECK( addVariable( "roiId"     , m_RoIId ) );
         CHECK( addVariable( "saddress"  , m_saddress ) );
         CHECK( addVariable( "algoId"    , m_algoId,
                             "Algorithm ID (GEV900ID=0, MUONID=1, HALOID=2, "
                             "NULLID=99999)" ) );
         CHECK( addVariable( "sp1_r"     , m_sp1_r ) );
         CHECK( addVariable( "sp1_z"     , m_sp1_z ) );
         CHECK( addVariable( "sp1_slope" , m_sp1_slope ) );
         CHECK( addVariable( "sp2_r"     , m_sp2_r ) );
         CHECK( addVariable( "sp2_z"     , m_sp2_z ) );
         CHECK( addVariable( "sp2_slope" , m_sp2_slope ) );
         CHECK( addVariable( "sp3_r"     , m_sp3_r ) );
         CHECK( addVariable( "sp3_z"     , m_sp3_z ) );
         CHECK( addVariable( "sp3_slope" , m_sp3_slope ) );
      }
      if(m_writeHitsInfo){
         CHECK( addVariable( "dq_var1"   , m_dq_var1 ) );
         CHECK( addVariable( "dq_var2"   , m_dq_var2 ) );
      }
      return StatusCode::SUCCESS;
   }

   StatusCode MuonFeatureFillerTool::fill( const MuonFeature& muon ) {
      if(m_writeRecoInfo){
         *m_dir_phi  = muon.dir_phi();
         *m_zeta     = muon.zeta();
         *m_dir_zeta = muon.dir_zeta();
         *m_radius     = muon.radius();
         *m_beta       = muon.beta();
         *m_RoIId      = muon.roiId();
         *m_saddress   = muon.saddress();
         *m_br_radius  = muon.br_radius();
         *m_br_sagitta = muon.br_sagitta();
         *m_ec_alpha   = muon.ec_alpha();
         *m_ec_beta    = muon.ec_beta();
         *m_algoId     = muon.algoId();
         *m_sp1_r      = muon.sp1_r();
         *m_sp1_z      = muon.sp1_z();
         *m_sp1_slope  = muon.sp1_slope();
         *m_sp2_r      = muon.sp2_r();
         *m_sp2_z      = muon.sp2_z();
         *m_sp2_slope  = muon.sp2_slope();
         *m_sp3_r      = muon.sp3_r();
         *m_sp3_z      = muon.sp3_z();
         *m_sp3_slope  = muon.sp3_slope();
      }
      if(m_writeHitsInfo){
         *m_dq_var1    = muon.dq_var1();
         *m_dq_var2    = muon.dq_var2();
      }
      return StatusCode::SUCCESS;
   }

} // namespace D3PD
