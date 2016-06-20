/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTBH6BeamInfo.h"

#include "HitManagement/AthenaHitsVector.h"
#include "LArG4TBSimEvent/LArG4H6FrontHitCollection.h"

// Gaudi Includes
#include "GaudiKernel/SystemOfUnits.h"
#include "StoreGate/DataHandle.h"
#include "CxxUtils/make_unique.h" // For make unique

using namespace Gaudi::Units;

LArTBH6BeamInfo::LArTBH6BeamInfo(const std::string& name, ISvcLocator* pSvcLocator)
 :AthAlgorithm(name, pSvcLocator),
  m_Primary(true),
  m_pcode(999),
  m_cryoX(0.),
  m_numEv(0),
  m_theEventInfo("TBEventInfo")
{
   declareProperty("HitsContainer",m_HitsCollNames);
//   m_HitsCollNames.push_back("LArTBFrontHitCollection");
   declareProperty("PrimaryTrackOnly",m_Primary);
   declareProperty("PrimaryParticle",m_pcode);

  for (const auto &it : m_HitsCollNames){
    m_hitcoll.push_back( SG::ReadHandle< AthenaHitsVector<LArG4H6FrontHit> >( it ) );
  }

}

LArTBH6BeamInfo::~LArTBH6BeamInfo()
{}

//****************************************************************************
//* Initialization
//****************************************************************************

StatusCode LArTBH6BeamInfo::initialize()
{

   if((!m_Primary) && (m_pcode == 999)) {
      ATH_MSG_ERROR ( "Pcode should be in jO, if not PrimaryTrackOnly !" );
      return StatusCode::FAILURE;
   }

// End of Initialization
   ATH_MSG_INFO ( "LArTBH6BeamInfo initialisation completed" );
   return StatusCode::SUCCESS;
}

//****************************************************************************
//* Execution
//****************************************************************************

StatusCode LArTBH6BeamInfo::execute()
{
   ATH_MSG_INFO ( "LArTBH6BeamInfo in execute" );
   CLHEP::Hep3Vector pos;

   if ( m_numEv == 0 ){
      m_cryoX = m_theEventInfo->getCryoX();
   }

   dVect v_x;
   dVect v_y;
   dVect v_xz;
   dVect v_yz;
   dVect v_ex;
   dVect v_ey;

// loop hit containers
   for (auto &it : m_hitcoll) {

      ATH_MSG_INFO (" hit container: "<< it->Name() <<" size: "<<it->size() );

      for (const auto &hit : *it){
          // loop over hits, find track==0 and make a fit, store a TBTrack to StoreGate
          if(hit->GetSC() > 0) continue; // scintilator hit
          if(m_Primary) {
            if(hit->GetTrackID() != 1) continue; // not a primary particle
          } else {
            if(hit->GetPcode() != m_pcode) continue; // not a beam particle
          }
          pos = hit->GetPos();
          if(hit->GetX() > 0) { // X-coordinate
             v_x.push_back(pos.x());
             v_xz.push_back(pos.z()+21600.*mm);
             v_ex.push_back(0.01);   // take the error from somewhere !!!
          } else {
             v_y.push_back(pos.y());
             v_yz.push_back(pos.z()+21600.*mm);
             v_ey.push_back(0.01);   // take the error from somewhere !!!
          }
      }

   }

   if(v_x.size() < 2 || v_y.size() < 2) { // Could not fit
      ATH_MSG_DEBUG ( "Could not fit, setting zero. "<<v_x.size()<<"/"<<v_y.size() );
      m_track = CxxUtils::make_unique<TBTrack>(0,0);
      m_track->setUintercept(0.);
      m_track->setVintercept(0.);
      m_track->setUslope(0.);
      m_track->setVslope(0.);
//      m_track->setResidualu(0, 0);
//      m_track->setResidualv(0, 0);
      m_track->setChi2_u(1000.);
      m_track->setChi2_v(1000.);
      m_track->setCryoHitu(0.);
      m_track->setCryoHitv(0.);
      m_track->setCryoHitw(30000.);
      return StatusCode::FAILURE;
   }
   // Now fit somehow
   double chi2_x = 0;
   double chi2_y = 0;
   std::vector<double> residual_x, residual_y;
   double a1_x = 0;
   double a2_x = 0;
   double a1_y = 0;
   double a2_y = 0;
   bool check = true;

   check = fitVect(v_x, v_xz, v_ex, a1_x, a2_x, chi2_x, residual_x);

   if(!check){
      ATH_MSG_ERROR ( "Fit in X-coordinate failure." );
      return StatusCode::FAILURE;
   }

   check = fitVect(v_y, v_yz, v_ey, a1_y, a2_y, chi2_y, residual_y);
   if(!check){
      ATH_MSG_ERROR ( "Fit in Y-coordinate failure." );
      return StatusCode::FAILURE;
    }

    // Setting the slopes & intercepts for each track //
    ATH_MSG_DEBUG ( "Setting fit parameters of track." );
    ATH_MSG_DEBUG ( "Intercepts: "<<a1_x<<" "<<a1_y );
    ATH_MSG_DEBUG ( "Slopes: "<<a2_x<<" "<<a2_y );

    m_track = CxxUtils::make_unique<TBTrack>(v_x.size(), v_y.size());

    m_track->setUintercept(a1_x);
    m_track->setVintercept(a1_y);
    m_track->setUslope(a2_x);
    m_track->setVslope(a2_y);

    // Setting the residual for plane u //
    for(size_t i = 0; i < v_x.size(); ++i){
      m_track->setResidualu(i, residual_x[i]);
    }

    // Setting the residual for plane v //
    for(size_t i = 0; i < v_y.size(); ++i){
      m_track->setResidualv(i, residual_y[i]);
    }

    ATH_MSG_DEBUG ( "chi2 in X: " << chi2_x );
    ATH_MSG_DEBUG ( "chi2 in Y: " << chi2_y );
    ATH_MSG_DEBUG ( "Setting chi2s of track." );

    m_track->setChi2_u(chi2_x);
    m_track->setChi2_v(chi2_y);

    // Setting the cryo plane (30000 mm in TB coordinate)
    m_track->setCryoHitu(a2_x*30000.+a1_x+m_cryoX);
    m_track->setCryoHitv(a2_y*30000.+a1_y);
    m_track->setCryoHitw(30000.);

    return  StatusCode::SUCCESS;
}

//****************************************************************************
//* Finalize
//****************************************************************************

StatusCode LArTBH6BeamInfo::finalize()
{
   ATH_MSG_INFO ( "LArTBH6BeamInfo::finalize()" );
   return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
bool LArTBH6BeamInfo::fitVect(const dVect &vec, const dVect &vec_z, const dVect &vec_e,
                              double &a1, double &a2,double &chi2, dVect &residual)
///////////////////////////////////////////////////////////////////////////
{
   // v_u = vector of u data points
  // v_w = vector of w data points
  // v_eu = vector of errors in u data points
  // v_ew = vector of errors in w data points
  // a1 and a2 = fit parameters: u = a1 + a2*w

  // *** note that the fit algorithm used  (given in 'Numerical Methods
  // in C' section 15.2) assumes that the w data points are known exactly
  // i.e. that v_ew[i]=0 for all i

  // 'Numerical Methods' notes that the task of fitting a straight
  // line model with errors in both coordinates is "considerably harder"
  // (section 15.3) - but clearly it could be done

  int i;
  double s = 0;
  double su = 0;
  double sww = 0;
  double sw = 0;
  double suw = 0;

  int hitNum = vec.size();
  for(i = 0; i < hitNum; ++i){

    ATH_MSG_DEBUG ( "Position in X: " << vec[i] );
    ATH_MSG_DEBUG ( "Position in Z: " << vec_z[i] );
    ATH_MSG_DEBUG ( "Error in X: " << vec_e[i] );

    s += 1 / (vec_e[i]*vec_e[i]);
    su += vec[i] / (vec_e[i]*vec_e[i]);
    sww += vec_z[i]*vec_z[i] / (vec_e[i]*vec_e[i]);
    sw += vec_z[i] / (vec_e[i]*vec_e[i]);
    suw += vec[i]*vec_z[i] / (vec_e[i]*vec_e[i]);
  }

  const double denom = (s*sww-sw*sw);
  if(denom == 0){
    ATH_MSG_ERROR ( " Invalid denominator" );
    return false;
  }

  const double inv_denom = 1. / denom;
  a1 = (su*sww - sw*suw) * inv_denom;
  a2 = (s*suw - su*sw) * inv_denom;
  ATH_MSG_DEBUG ( "Fit results:" << " intercept = " << a1 << " and slope = " << a2 );

  // Fill residual
  residual.clear();
  for (i = 0; i < hitNum; ++i) {
    residual.push_back(vec[i] - a1 - a2*vec_z[i]);
  }
  // Fill Chi2
  chi2 = 0;
  for(i = 0; i < hitNum; ++i){
    chi2 += (vec[i] - a1 - a2*vec_z[i])*(vec[i] - a1 - a2*vec_z[i])/(vec_e[i]*vec_e[i]);
  }

  return true;
}
