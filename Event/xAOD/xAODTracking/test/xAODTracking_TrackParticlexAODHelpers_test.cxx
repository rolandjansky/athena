/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <vector>
// #include <iostream>

void testPtErr() {
   xAOD::TrackParticleAuxContainer aux;
   xAOD::TrackParticleContainer tpc;
   tpc.setStore( &aux );

   std::vector<double> pt_err2;
   std::vector<bool> valid;

   std::vector<float> q_over_p;
   q_over_p.push_back(0.);
   q_over_p.push_back(1/10e3);
   q_over_p.push_back(1/1e3);
   q_over_p.push_back(-1/10e3);
   q_over_p.push_back(-1/1e3);

   std::vector<float> theta;
   theta.push_back(0.+2*std::numeric_limits<double>::epsilon());
   theta.push_back(M_PI_4);
   theta.push_back(M_PI_2);
   theta.push_back(3*M_PI_4);
   theta.push_back(M_PI-2*std::numeric_limits<double>::epsilon());

   for (unsigned int i=0; i<q_over_p.size(); ++i) { 
     for (unsigned int j=0; j<theta.size(); ++j) { 
       xAOD::TrackParticle* p = new xAOD::TrackParticle();
       tpc.push_back( p );        
       p->setDefiningParameters(0., 0., 0, theta[j], q_over_p[i]);
       xAOD::ParametersCovMatrix_t cov;
       float theta_uncert = std::abs(theta[j])*1e-3;
       float q_over_p_uncert = std::abs(q_over_p[i])*1e-4;
       float cov_theta_q_over_p =  q_over_p_uncert*theta_uncert*.2;
       cov(3,3)=xAOD::TrackingHelpers::sqr(theta_uncert);
       cov(4,4)=xAOD::TrackingHelpers::sqr(q_over_p_uncert);
       cov(3,4)=cov_theta_q_over_p;
       cov(4,3)=cov_theta_q_over_p;
       p->setDefiningParametersCovMatrix(cov);

       //       double pt = std::abs(1/q_over_p[i]) * sin(theta[j]);
       //       std::cout << "DEBUG calc pt " << pt << " =?= " << p->pt() << " delta " << (pt - p->pt()) / pt << std::endl;
       double pt = p->pt();
       double diff_qp =  - pt / std::abs(q_over_p[i]);
       double diff_theta = pt / tan( theta[j] );
       double a_pt_err2 = xAOD::TrackingHelpers::sqr(diff_qp * q_over_p_uncert);
       a_pt_err2+= xAOD::TrackingHelpers::sqr(diff_theta  * theta_uncert );
       a_pt_err2+= diff_theta  * diff_qp  * cov_theta_q_over_p;

       a_pt_err2 = diff_qp * ( diff_qp * static_cast<float>(xAOD::TrackingHelpers::sqr( q_over_p_uncert)) +  diff_theta  * cov_theta_q_over_p);
       a_pt_err2+= xAOD::TrackingHelpers::sqr(diff_theta)  * static_cast<float>(xAOD::TrackingHelpers::sqr(theta_uncert ));
       
       pt_err2.push_back( a_pt_err2 );
       valid.push_back( std::abs(q_over_p[i])> 0. );
     }
   }
   xAOD::TrackParticle* p = new xAOD::TrackParticle();
   tpc.push_back( p );
   p->setDefiningParameters(0., 0., 0, theta[1], q_over_p[1]);
   pt_err2.push_back( 0. );
   valid.push_back( false );

   tpc.push_back( nullptr );
   pt_err2.push_back( 0. );
   valid.push_back( false );


   std::vector<double>::const_iterator pt_err2_iter = pt_err2.begin();
   std::vector<bool>::const_iterator valid_iter = valid.begin();
   for (const xAOD::TrackParticle *tp : tpc) {
     assert( pt_err2_iter != pt_err2.end() );

     // std::cout << "DEBUG check valid qOverP,cov  "  << *valid_iter << " =?= " <<  xAOD::TrackingHelpers::hasValidCovQoverP(tp)
     //           << std::endl;
     assert( *valid_iter == xAOD::TrackingHelpers::hasValidCovQoverP(tp) );
     
     try {
       // std::cout << "DEBUG check pTErr2 "  << *pt_err2_iter << " vs " << xAOD::TrackingHelpers::pTErr2(tp)
       //           << " -> " << std::abs( (*pt_err2_iter - xAOD::TrackingHelpers::pTErr2(tp)))/ (*pt_err2_iter)
       //           << " <? " << std::numeric_limits<float>::epsilon()
       //           << " | qOverp=" << (tp ? tp->qOverP() : 0 ) << " theta=" << (tp ? tp->theta() : 0)
       //           << std::endl;
       assert( std::abs( (*pt_err2_iter - xAOD::TrackingHelpers::pTErr2(tp)))/ (*pt_err2_iter) < std::numeric_limits<float>::epsilon() );
       assert( std::abs( (sqrt(*pt_err2_iter) - xAOD::TrackingHelpers::pTErr(tp)))/sqrt(*pt_err2_iter) < 2*std::numeric_limits<float>::epsilon() );
     }
     catch (std::exception &err) {
       assert( *valid_iter == false );
     }

     if (*valid_iter ) {
       assert( std::abs( (*pt_err2_iter - xAOD::TrackingHelpers::pTErr2Unsafe(tp)))/ (*pt_err2_iter) < std::numeric_limits<float>::epsilon() );
       assert( std::abs( (sqrt(*pt_err2_iter) - xAOD::TrackingHelpers::pTErrUnsafe(tp)))/sqrt(*pt_err2_iter) < 2*std::numeric_limits<float>::epsilon() );
     }

     ++pt_err2_iter;
     ++valid_iter;
   }   
   
}

namespace {
  class Cov_t
  {
  public:
    Cov_t(double cov_x, double cov_y, double cov_xy) {
      m_cov[0]=cov_x;
      m_cov[1]=cov_y;
      m_cov[2]=cov_xy;
    }

    double x() const { return m_cov[0]; }
    double y() const { return m_cov[1]; }
    double xy() const { return m_cov[2]; }
  private:
    double m_cov[3];
  };

  template <class T>
  bool checkEqual(T ref, T value ) {
    if (std::abs(ref) * std::numeric_limits<T>::epsilon() < 4*std::numeric_limits<T>::min() ) {
      return std::abs( value - ref ) < 4*std::numeric_limits<T>::epsilon();
    }
    else {
      return std::abs( value - ref ) < 4*std::numeric_limits<T>::epsilon() * ref;
    }
  }

  double toRad(double deg) {
    return deg * (M_PI/180);
  }

  double sqr( double a) {
    return a*a;
  }
}

void test_d0significance() {
  xAOD::TrackParticleAuxContainer aux;
  xAOD::TrackParticleContainer tpc;
  tpc.setStore( &aux );

  std::vector<Cov_t>  beamspot_sigma_list;
  std::vector<float> phi_list;
  std::vector<bool>   valid;

  phi_list.push_back(0.) ;
  phi_list.push_back(toRad(60)) ;
  phi_list.push_back(toRad(90)) ;
  phi_list.push_back(toRad(210)) ;
  phi_list.push_back(M_PI+acos(1/sqrt(2))) ;

  beamspot_sigma_list.emplace_back(0.015, 0.01, 0.015*0.01 * 0. );
  valid.push_back(true);

  beamspot_sigma_list.emplace_back(0.015, 0.01, 0.015*0.01 );
  valid.push_back(true);

  beamspot_sigma_list.emplace_back(0.015, 0.01, 0.015*0.01 *.6 );
  valid.push_back(true);

  beamspot_sigma_list.emplace_back(0.01, 0.015, -0.015*0.01 *.6 );
  valid.push_back(true);

  beamspot_sigma_list.emplace_back(0.015, 0.01, -0.015*0.01 );
  valid.push_back(true);

  beamspot_sigma_list.emplace_back(0.015, 0.01, -(0.015*0.015+0.01*0.01)*0.5*(1.+1e-6) );
  valid.push_back(false);

  beamspot_sigma_list.emplace_back(0.015, 0.01, (0.015*0.015+0.01*0.01)*0.5*(1.+1e-6) );
  valid.push_back(false);

  beamspot_sigma_list.emplace_back(0.015, 0.01, -(0.015*0.015+0.01*0.01)*0.5*(1.-1e-5) );
  valid.push_back(true);

  beamspot_sigma_list.emplace_back(0.015, 0.01, (0.015*0.015+0.01*0.01)*0.5*(1.-1e-5) );
  valid.push_back(true);

  float ref_d0=1;
  float ref_z0=10;
  float ref_d0_uncert=.05;
  float ref_z0_uncert=.5;
  float ref_qoverp = 1/30e3;
  float ref_theta=60.0/180.0*M_PI; //coverity 111358
  std::vector<bool>::const_iterator valid_iter = valid.begin();
  for (const Cov_t &beamspot_sigma: beamspot_sigma_list) {
    for( const float &phi: phi_list) {
      assert( valid_iter != valid.end());
      xAOD::TrackParticle* p = new xAOD::TrackParticle();
      tpc.push_back( p );
      p->setDefiningParameters(ref_d0, ref_z0, phi, ref_theta, ref_qoverp);
      xAOD::ParametersCovMatrix_t cov;
      cov(0,0)=xAOD::TrackingHelpers::sqr(ref_d0_uncert);
      cov(1,1)=xAOD::TrackingHelpers::sqr(ref_z0_uncert);
      p->setDefiningParametersCovMatrix(cov);

      double vx_cov=beamspot_sigma.x();
      double vy_cov=beamspot_sigma.y();
      double vxy_cov=beamspot_sigma.xy();
      double expected_d0_uncert_2=sqr(ref_d0_uncert) + sin(phi)*(sin(phi)*sqr(vx_cov)-cos(phi)*vxy_cov)+cos(phi)*(cos(phi)*sqr(vy_cov)-sin(phi)*vxy_cov);
      double phi2 =p->phi();
      assert( checkEqual<float>(phi2, phi));
      assert( checkEqual<float>(ref_d0, p->d0()));
      double d0_uncert2 = p->definingParametersCovMatrixDiagVec()[0];
      assert( checkEqual<float>(sqr(ref_d0_uncert), d0_uncert2));
      expected_d0_uncert_2=p->definingParametersCovMatrixDiagVec()[0]  + sin(phi2)*(sin(phi2)*sqr(vx_cov)-cos(phi2)*vxy_cov)+cos(phi2)*(cos(phi2)*sqr(vy_cov)-sin(phi2)*vxy_cov);

      assert( expected_d0_uncert_2 >= 0.);
      double expected_d0_significance = ref_d0 / sqrt( expected_d0_uncert_2);
      expected_d0_significance = p->d0() / sqrt( expected_d0_uncert_2);

      try {
        if (*valid_iter) {
          double d0_significance = xAOD::TrackingHelpers::d0significanceUnsafe(p, beamspot_sigma.x(), beamspot_sigma.y(), beamspot_sigma.xy());
          assert( checkEqual<float>( d0_significance, expected_d0_significance ) );
        }
        double d0_significance = xAOD::TrackingHelpers::d0significance(p, beamspot_sigma.x(), beamspot_sigma.y(), beamspot_sigma.xy());
        assert( checkEqual<float>( d0_significance, expected_d0_significance ) );
        assert( ( *valid_iter ) );
      }
      catch( std::exception &) {
        assert( !( *valid_iter ) );
      }
    }
    assert( valid_iter != valid.end());
    ++valid_iter;
  }
}

int main() {
   //std::cout << "Run:" << __FILE__ << std::endl;
   // Create the main containers to test:
   xAOD::TrackParticleAuxContainer aux;
   xAOD::TrackParticleContainer tpc;
   tpc.setStore( &aux );


   std::vector<bool> valid;
   std::vector<double> d0_expected;
   std::vector<double> z0_expected;    

   xAOD::TrackParticle* p = new xAOD::TrackParticle();
   tpc.push_back( p ); 
   p->setDefiningParameters(20., 15., 0, M_PI_4, 1.);
   xAOD::ParametersCovMatrix_t cov;
   cov(0,0)=2*2;
   cov(1,1)=3*3;
   p->setDefiningParametersCovMatrix(cov);
   d0_expected.push_back(10.);
   z0_expected.push_back(5.);
   valid.push_back(true);

   xAOD::TrackParticle* p_invalid1 = new xAOD::TrackParticle();
   tpc.push_back( p_invalid1);
   p_invalid1->setDefiningParameters(21., 16., 0, M_PI_4, 1.);
   xAOD::ParametersCovMatrix_t cov_invalid1;
   cov_invalid1(0,0)=0.;
   cov_invalid1(1,1)=0.;
   p_invalid1->setDefiningParametersCovMatrix(cov_invalid1);
   d0_expected.push_back(0.);
   z0_expected.push_back(0.);
   valid.push_back(false);

   xAOD::TrackParticle* p_invalid2 = new xAOD::TrackParticle();
   tpc.push_back( p_invalid2);
   p_invalid2->setDefiningParameters(22., 17., 0, M_PI_4, 1.);
   xAOD::ParametersCovMatrix_t cov_invalid2;
   cov_invalid2(0,0)=-1.;
   cov_invalid2(1,1)=-1.;
   p_invalid2->setDefiningParametersCovMatrix(cov_invalid2);
   d0_expected.push_back(0.);
   z0_expected.push_back(0.);
   valid.push_back(false);


   tpc.push_back( nullptr );
   d0_expected.push_back(0.);
   z0_expected.push_back(0.);
   valid.push_back(false);


   std::vector<bool>::const_iterator tp_valid_iter = valid.begin();
   std::vector<double>::const_iterator d0_exp_iter = d0_expected.begin();
   std::vector<double>::const_iterator z0_exp_iter = z0_expected.begin();
   for (const xAOD::TrackParticle *tp : tpc) {
     assert( tp_valid_iter != valid.end());
     assert( d0_exp_iter != d0_expected.end());
     assert( z0_exp_iter != z0_expected.end());

     // const xAOD::TrackParticle* tp=*tp_iter;
     // allow for some precision loss

     if (!(*tp_valid_iter)) {
       try {
         xAOD::TrackingHelpers::d0significance(tp );
         throw std::logic_error("Invalid inputs were accepted.");
       }
       catch (std::runtime_error &err) {
       }

       try {
         xAOD::TrackingHelpers::z0significance(tp );
         throw std::logic_error("Invalid inputs were accepted.");
       }
       catch (std::runtime_error &err) {
       }
     }
     else {
       assert( std::abs( xAOD::TrackingHelpers::d0significance(tp) - *d0_exp_iter)<4*std::numeric_limits<double>::epsilon());
       assert( std::abs( xAOD::TrackingHelpers::z0significance(tp) - *z0_exp_iter)<4*std::numeric_limits<double>::epsilon());
       assert( std::abs( xAOD::TrackingHelpers::d0significanceUnsafe(tp) - *d0_exp_iter)<4*std::numeric_limits<double>::epsilon());
       assert( std::abs( xAOD::TrackingHelpers::z0significanceUnsafe(tp) - *z0_exp_iter)<4*std::numeric_limits<double>::epsilon());
       //       std::cout << "DEBUG sig_d0 = " << xAOD::TrackingHelpers::d0significance(tp)  << std::endl;
       //       std::cout << "DEBUG sig_z0 = " << xAOD::TrackingHelpers::z0significance(tp)  << std::endl;
     }

     ++tp_valid_iter;
     ++d0_exp_iter;
     ++z0_exp_iter;
   }

   // test primary vertex compatibility check
   xAOD::VertexAuxContainer vx_aux;
   xAOD::VertexContainer vxc;
   vxc.setStore( &vx_aux );

   std::vector<bool> passes_vx_check;

   vxc.push_back(new xAOD::Vertex);
   vxc.back()->setY( 2. * (.5-1e-6) );
   vxc.back()->setX( 0 );
   vxc.back()->setZ( 50 );
   passes_vx_check.push_back(true);

   vxc.push_back(new xAOD::Vertex);
   vxc.back()->setX( 2. * (.5-1e-6) );
   vxc.back()->setY( 0 );
   vxc.back()->setZ( 50 );
   passes_vx_check.push_back(true);


   vxc.push_back(new xAOD::Vertex);
   vxc.back()->setY( sin(30/M_PI) * 2. * (.5-1e-5) );
   vxc.back()->setX( cos(30/M_PI) * 2. * (.5-1e-5) );
   vxc.back()->setZ( 50 );
   passes_vx_check.push_back(true);

   vxc.push_back(new xAOD::Vertex);
   vxc.back()->setY( sin(30/M_PI) * 2. * (.5+1e-5) );
   vxc.back()->setX( cos(30/M_PI) * 2. * (.5+1e-5) );
   vxc.back()->setZ( 50 );
   passes_vx_check.push_back(false);

   vxc.push_back(nullptr);
   passes_vx_check.push_back(false);

   std::vector<bool>::const_iterator passes_vx_check_iter=passes_vx_check.begin();
   for (const xAOD::Vertex *vx : vxc) {
      assert( passes_vx_check_iter != passes_vx_check.end() );
      // if (vx) {
      // std::cout << "DEBUG xAOD::TrackingHelpers::checkPVReference " <<  xAOD::TrackingHelpers::checkPVReference( *(tpc.begin()), vx) <<  " != " << *passes_vx_check_iter << std::endl;
      // std::cout << "DEBUG hasValidCovD0 " <<  xAOD::TrackingHelpers::hasValidCovD0( *(tpc.begin())) << " vx=" << static_cast<const void *>(vx) << std::endl;
      // std::cout << "DEBUG " << std::abs( xAOD::TrackingHelpers::sqr(vx->x())+ xAOD::TrackingHelpers::sqr(vx->y())) 
      //           << " < " << 0.5 * (*(tpc.begin()))->definingParametersCovMatrixVec()[0] << std::endl;
      // }
      
      assert( xAOD::TrackingHelpers::checkPVReference( *(tpc.begin()), vx) == *passes_vx_check_iter );
      ++passes_vx_check_iter;
   }

   // test primary vertex displacement
   tpc.push_back( new xAOD::TrackParticle);
   tpc.back()->setDefiningParameters(20., 15.+50, 0, M_PI_4, 1.);
   {
   xAOD::ParametersCovMatrix_t cov;
   cov(0,0)=2*2;
   cov(1,1)=3*3;
   tpc.back()->setDefiningParametersCovMatrix(cov);
   }      
   // std::cout << "DEBUG " << xAOD::TrackingHelpers::z0significance( *(tpc.begin()) ) << " == " <<   xAOD::TrackingHelpers::z0significance( tpc[tpc.size()-1], *(vxc.begin() )) 
   //           << std::endl;
   assert( !tpc.empty() && xAOD::TrackingHelpers::z0significance( *(tpc.begin()) ) ==  xAOD::TrackingHelpers::z0significance( tpc[tpc.size()-1], *(vxc.begin() )) );


   testPtErr();

   test_d0significance();

   std::cout << "PASSED." << std::endl;
   return 0;
}
