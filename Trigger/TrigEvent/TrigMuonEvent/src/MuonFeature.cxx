// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// STL include(s):
#include <sstream>
#include <cmath>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigMuonEvent/MuonFeature.h"

// "Distance" used by the comparison operator(s):
static const double DELTA = 0.001;

MuonFeature::MuonFeature()
{
    m_algoId     = NULLID;
    m_RoIId      = -1;
    m_saddress   = 0;
    m_pt         = 0.;
    m_radius     = 0.;
    m_eta        = 0.;
    m_phi        = 0.;
    m_dir_phi    = 0.;
    m_zeta       = 0.;
    m_dir_zeta   = 0.;
    m_beta       = 0.;
    m_sp1_r      = 0.;
    m_sp1_z      = 0.;
    m_sp1_slope  = 0.;
    m_sp2_r      = 0.;
    m_sp2_z      = 0.;
    m_sp2_slope  = 0.;
    m_sp3_r      = 0.;
    m_sp3_z      = 0.;
    m_sp3_slope  = 0.;
    m_br_radius  = 0.;
    m_br_sagitta = 0.;
    m_ec_alpha   = 0.;
    m_ec_beta    = 0.;
    m_dq_var1    = 0.;
    m_dq_var2    = 0.;;
}

// --- keep for backward compartibility
MuonFeature::MuonFeature(int saddress, float pt, float radius, 
			 float eta, float phi, float dir_phi, float zeta, 
			 float dir_zeta, float beta):
   m_algoId(NULLID), m_RoIId(-1), m_saddress(saddress),
   m_pt(pt), m_radius(radius), m_eta(eta), m_phi(phi), m_dir_phi(dir_phi),
   m_zeta(zeta), m_dir_zeta(dir_zeta), m_beta(beta),
   m_sp1_r(0.), m_sp1_z(0.), m_sp1_slope(0.), 
   m_sp2_r(0.), m_sp2_z(0.), m_sp2_slope(0.), 
   m_sp3_r(0.), m_sp3_z(0.), m_sp3_slope(0.), 
   m_br_radius(0.), m_br_sagitta(0.),
   m_ec_alpha(0.), m_ec_beta(0.), 
   m_dq_var1(0.), m_dq_var2(0.)
{}

// --- new constructor
MuonFeature::MuonFeature(int saddress, float pt, float radius, 
			 float eta, float phi, float dir_phi, float zeta,
			 float dir_zeta, float beta,
			 float sp1_r, float sp1_z, float sp1_slope, 
			 float sp2_r, float sp2_z, float sp2_slope, 
			 float sp3_r, float sp3_z, float sp3_slope, 
			 float br_radius, float br_sagitta, float ec_alpha, float ec_beta,
			 double dq_var1, double dq_var2):
   m_algoId(NULLID), m_RoIId(-1), m_saddress(saddress),
   m_pt(pt), m_radius(radius), m_eta(eta), m_phi(phi), m_dir_phi(dir_phi),
   m_zeta(zeta), m_dir_zeta(dir_zeta), m_beta(beta),
   m_sp1_r(sp1_r), m_sp1_z(sp1_z), m_sp1_slope(sp1_slope), 
   m_sp2_r(sp2_r), m_sp2_z(sp2_z), m_sp2_slope(sp2_slope), 
   m_sp3_r(sp3_r), m_sp3_z(sp3_z), m_sp3_slope(sp3_slope), 
   m_br_radius(br_radius), m_br_sagitta(br_sagitta),
   m_ec_alpha(ec_alpha), m_ec_beta(ec_beta), 
   m_dq_var1(dq_var1), m_dq_var2(dq_var2)
{}

MuonFeature::MuonFeature(const MuonFeature* muon_feature)
{
   m_algoId     = muon_feature->algoId();
   m_RoIId      = muon_feature->roiId();
   m_saddress   = muon_feature->saddress();
   m_pt         = muon_feature->pt();
   m_radius     = muon_feature->radius();
   m_eta        = muon_feature->eta();
   m_phi        = muon_feature->phi();
   m_dir_phi    = muon_feature->dir_phi();
   m_zeta       = muon_feature->zeta();
   m_dir_zeta   = muon_feature->dir_zeta();
   m_beta       = muon_feature->beta();
   m_sp1_r      = muon_feature->sp1_r();
   m_sp1_z      = muon_feature->sp1_z();
   m_sp1_slope  = muon_feature->sp1_slope();
   m_sp2_r      = muon_feature->sp2_r();
   m_sp2_z      = muon_feature->sp2_z();
   m_sp2_slope  = muon_feature->sp2_slope();
   m_sp3_r      = muon_feature->sp3_r();
   m_sp3_z      = muon_feature->sp3_z();
   m_sp3_slope  = muon_feature->sp3_slope();
   m_br_radius  = muon_feature->br_radius();
   m_br_sagitta = muon_feature->br_sagitta();
   m_ec_alpha   = muon_feature->ec_alpha();
   m_ec_beta    = muon_feature->ec_beta();
   m_dq_var1    = muon_feature->dq_var1();
   m_dq_var2    = muon_feature->dq_var2();
}

MuonFeature::MuonFeature(const MuonFeature& muon_feature)
{
   m_algoId     = muon_feature.algoId();
   m_RoIId      = muon_feature.roiId();
   m_saddress   = muon_feature.saddress();
   m_pt         = muon_feature.pt();
   m_radius     = muon_feature.radius();
   m_eta        = muon_feature.eta();
   m_phi        = muon_feature.phi();
   m_dir_phi    = muon_feature.dir_phi();
   m_zeta       = muon_feature.zeta();
   m_dir_zeta   = muon_feature.dir_zeta();
   m_beta       = muon_feature.beta();
   m_sp1_r      = muon_feature.sp1_r();
   m_sp1_z      = muon_feature.sp1_z();
   m_sp1_slope  = muon_feature.sp1_slope();
   m_sp2_r      = muon_feature.sp2_r();
   m_sp2_z      = muon_feature.sp2_z();
   m_sp2_slope  = muon_feature.sp2_slope();
   m_sp3_r      = muon_feature.sp3_r();
   m_sp3_z      = muon_feature.sp3_z();
   m_sp3_slope  = muon_feature.sp3_slope();
   m_br_radius  = muon_feature.br_radius();
   m_br_sagitta = muon_feature.br_sagitta();
   m_ec_alpha   = muon_feature.ec_alpha();
   m_ec_beta    = muon_feature.ec_beta();
   m_dq_var1    = muon_feature.dq_var1();
   m_dq_var2    = muon_feature.dq_var2();
}


MuonFeature&
MuonFeature::operator=(const MuonFeature& muon_feature)
{
   m_algoId     = muon_feature.algoId();
   m_RoIId      = muon_feature.roiId();
   m_saddress   = muon_feature.saddress();
   m_pt         = muon_feature.pt();
   m_radius     = muon_feature.radius();
   m_eta        = muon_feature.eta();
   m_phi        = muon_feature.phi();
   m_dir_phi    = muon_feature.dir_phi();
   m_zeta       = muon_feature.zeta();
   m_dir_zeta   = muon_feature.dir_zeta();
   m_beta       = muon_feature.beta();
   m_sp1_r      = muon_feature.sp1_r();
   m_sp1_z      = muon_feature.sp1_z();
   m_sp1_slope  = muon_feature.sp1_slope();
   m_sp2_r      = muon_feature.sp2_r();
   m_sp2_z      = muon_feature.sp2_z();
   m_sp2_slope  = muon_feature.sp2_slope();
   m_sp3_r      = muon_feature.sp3_r();
   m_sp3_z      = muon_feature.sp3_z();
   m_sp3_slope  = muon_feature.sp3_slope();
   m_br_radius  = muon_feature.br_radius();
   m_br_sagitta = muon_feature.br_sagitta();
   m_ec_alpha   = muon_feature.ec_alpha();
   m_ec_beta    = muon_feature.ec_beta();
   m_dq_var1    = muon_feature.dq_var1();
   m_dq_var2    = muon_feature.dq_var2();
    
   return *this;    
}

std::string str( const MuonFeature& muon ) {

   std::stringstream stream;
   stream << "RoI ID: " << muon.roiId()
          << "; saddress: " << muon.saddress()
          << "; eta: " << muon.eta()
          << "; phi: " << muon.phi()
          << "; pt: " << muon.pt();

   return stream.str();
}

MsgStream& operator<< ( MsgStream& m, const MuonFeature& muon ) {

   return ( m << str( muon ) );

}

bool operator== ( const MuonFeature& left, const MuonFeature& right ) {

   if( ( left.algoId() != right.algoId() ) || 
       ( left.saddress() != right.saddress() ) ||
       ( std::abs( left.pt() - right.pt() ) > DELTA ) ||
       ( std::abs( left.radius() - right.radius() ) > DELTA ) ||
       ( std::abs( left.eta() - right.eta() ) > DELTA ) ||
       ( std::abs( left.phi() - right.phi() ) > DELTA ) ||
       ( std::abs( left.dir_phi() - right.dir_phi() ) > DELTA ) ||
       ( std::abs( left.zeta() - right.zeta() ) > DELTA ) ||
       ( std::abs( left.dir_zeta() - right.dir_zeta() ) > DELTA ) ||
       ( std::abs( left.beta() - right.beta() ) > DELTA ) ||
       ( std::abs( left.sp1_r() - right.sp1_r() ) > DELTA ) ||
       ( std::abs( left.sp1_z() - right.sp1_z() ) > DELTA ) ||
       ( std::abs( left.sp1_slope() - right.sp1_slope() ) > DELTA ) ||
       ( std::abs( left.sp2_r() - right.sp2_r() ) > DELTA ) ||
       ( std::abs( left.sp2_z() - right.sp2_z() ) > DELTA ) ||
       ( std::abs( left.sp2_slope() - right.sp2_slope() ) > DELTA ) ||
       ( std::abs( left.sp3_r() - right.sp3_r() ) > DELTA ) ||
       ( std::abs( left.sp3_z() - right.sp3_z() ) > DELTA ) ||
       ( std::abs( left.sp3_slope() - right.sp3_slope() ) > DELTA ) ||
       ( std::abs( left.br_radius() - right.br_radius() ) > DELTA ) ||
       ( std::abs( left.br_sagitta() - right.br_sagitta() ) > DELTA ) ||
       ( std::abs( left.ec_alpha() - right.ec_alpha() ) > DELTA ) ||
       ( std::abs( left.ec_beta() - right.ec_beta() ) > DELTA ) ||
       ( std::abs( left.dq_var1() - right.dq_var1() ) > DELTA ) ||
       ( std::abs( left.dq_var2() - right.dq_var2() ) > DELTA ) ) {

      return false;

   } else {

      return true;

   }

}

void diff( const MuonFeature& left, const MuonFeature& right,
           std::map< std::string, double >& varChange ) {

   if( std::abs( left.algoId() - right.algoId() ) > DELTA ) {
      varChange[ "algoId" ] = static_cast< double >( left.algoId() - right.algoId() );
   }
   if( left.saddress() != right.saddress() ) {
      varChange[ "saddress" ] = static_cast< double >( left.saddress() - right.saddress() );
   }
   if( std::abs( left.pt() - right.pt() ) > DELTA ) {
      varChange[ "pt" ] = left.pt() - right.pt();
   }
   if( std::abs( left.radius() - right.radius() ) > DELTA ) {
      varChange[ "radius" ] = left.radius() - right.radius();
   }
   if( std::abs( left.eta() - right.eta() ) > DELTA ) {
      varChange[ "eta" ] = left.eta() - right.eta();
   }
   if( std::abs( left.phi() - right.phi() ) > DELTA ) {
      varChange[ "phi" ] = left.phi() - right.phi();
   }
   if( std::abs( left.dir_phi() - right.dir_phi() ) > DELTA ) {
      varChange[ "dir_phi" ] = left.dir_phi() - right.dir_phi();
   }
   if( std::abs( left.zeta() - right.zeta() ) > DELTA ) {
      varChange[ "zeta" ] = left.zeta() - right.zeta();
   }
   if( std::abs( left.dir_zeta() - right.dir_zeta() ) > DELTA ) {
      varChange[ "dir_zeta" ] = left.dir_zeta() - right.dir_zeta();
   }
   if( std::abs( left.beta() - right.beta() ) > DELTA ) {
      varChange[ "beta" ] = left.beta() - right.beta();
   }
   if( std::abs( left.sp1_r() - right.sp1_r() ) > DELTA ) {
      varChange[ "sp1_r" ] = left.sp1_r() - right.sp1_r();
   }
   if( std::abs( left.sp1_z() - right.sp1_z() ) > DELTA ) {
      varChange[ "sp1_z" ] = left.sp1_z() - right.sp1_z();
   }
   if( std::abs( left.sp1_slope() - right.sp1_slope() ) > DELTA ) {
      varChange[ "sp1_slope" ] = left.sp1_slope() - right.sp1_slope();
   }
   if( std::abs( left.sp2_r() - right.sp2_r() ) > DELTA ) {
      varChange[ "sp2_r" ] = left.sp2_r() - right.sp2_r();
   }
   if( std::abs( left.sp2_z() - right.sp2_z() ) > DELTA ) {
      varChange[ "sp2_z" ] = left.sp2_z() - right.sp2_z();
   }
   if( std::abs( left.sp2_slope() - right.sp2_slope() ) > DELTA ) {
      varChange[ "sp2_slope" ] = left.sp2_slope() - right.sp2_slope();
   }
   if( std::abs( left.sp3_r() - right.sp3_r() ) > DELTA ) {
      varChange[ "sp3_r" ] = left.sp3_r() - right.sp3_r();
   }
   if( std::abs( left.sp3_z() - right.sp3_z() ) > DELTA ) {
      varChange[ "sp3_z" ] = left.sp3_z() - right.sp3_z();
   }
   if( std::abs( left.sp3_slope() - right.sp3_slope() ) > DELTA ) {
      varChange[ "sp3_slope" ] = left.sp3_slope() - right.sp3_slope();
   }
   if( std::abs( left.br_radius() - right.br_radius() ) > DELTA ) {
      varChange[ "br_radius" ] = left.br_radius() - right.br_radius();
   }
   if( std::abs( left.br_sagitta() - right.br_sagitta() ) > DELTA ) {
      varChange[ "br_sagitta" ] = left.br_sagitta() - right.br_sagitta();
   }
   if( std::abs( left.ec_alpha() - right.ec_alpha() ) > DELTA ) {
      varChange[ "ec_alpha" ] = left.ec_alpha() - right.ec_alpha();
   }
   if( std::abs( left.ec_beta() - right.ec_beta() ) > DELTA ) {
      varChange[ "ec_beta" ] = left.ec_beta() - right.ec_beta();
   }
   if( std::abs( left.dq_var1() - right.dq_var1() ) > DELTA ) {
      varChange[ "dq_var1" ] = left.dq_var1() - right.dq_var1();
   }
   if( std::abs( left.dq_var2() - right.dq_var2() ) > DELTA ) {
      varChange[ "dq_var2" ] = left.dq_var2() - right.dq_var2();
   }

   return;
}
