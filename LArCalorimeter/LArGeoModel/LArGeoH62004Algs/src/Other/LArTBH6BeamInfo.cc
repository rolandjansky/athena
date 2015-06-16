/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH62004Algs/LArTBH6BeamInfo.h"

#include "HitManagement/AthenaHitsVector.h"

#include "LArG4TBSimEvent/LArG4H6FrontHitCollection.h"
#include "TBEvent/TBTrack.h"
#include "TBEvent/TBEventInfo.h"

#include "StoreGate/StoreGateSvc.h"

// Gaudi Includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

static const AlgFactory<LArTBH6BeamInfo>  s_factory;
const IAlgFactory& LArTBH6BeamInfoFactory = s_factory;

LArTBH6BeamInfo::LArTBH6BeamInfo(const std::string& name, ISvcLocator* pSvcLocator)
 :Algorithm(name, pSvcLocator),m_cryoX(0.),m_numEv(0),m_Primary(true),m_pcode(999)
{
   declareProperty("HitsContainer",m_HitsCollNames);
   m_HitsCollNames.push_back("LArTBFrontHitCollection");
   declareProperty("PrimaryTrackOnly",m_Primary);
   declareProperty("PrimaryParticle",m_pcode);
}

LArTBH6BeamInfo::~LArTBH6BeamInfo() 
{}

//**************************************************************************** 
//* Initialization 
//****************************************************************************

StatusCode LArTBH6BeamInfo::initialize()
{

   StatusCode sc=StatusCode::SUCCESS;

   MsgStream log(messageService(), name());

   sc = service("StoreGateSvc", m_StoreGate);
   if (sc.isFailure()) {
     log << MSG::ERROR << "Unable to get pointer to StoreGate Service" << endreq;
     return sc;
   }

   if((!m_Primary) && (m_pcode == 999)) {
      log << MSG::ERROR << "Pcode should be in jO, if not PrimaryTrackOnly !" << endreq;
      return StatusCode::FAILURE;
   }
   
// End of Initialization 
   log << MSG::INFO << "LArTBH6BeamInfo initialisation completed" << endreq;
   return sc;
}

//****************************************************************************
//* Execution                                                                 
//****************************************************************************

StatusCode LArTBH6BeamInfo::execute()
{
   StatusCode sc;                                                            
   MsgStream log(messageService(), name());
   log << MSG::INFO << "LArTBH6BeamInfo in execute" << endreq;
   CLHEP::Hep3Vector pos;

   if ( m_numEv == 0 ){
      const TBEventInfo* theEventInfo;
      sc = m_StoreGate->retrieve(theEventInfo,"TBEventInfo");
      if ( sc.isFailure() ) {
         log << MSG::ERROR << "cannot retrieve TBEventInfo from StoreGate" << endreq;
         return StatusCode::FAILURE;
      }
      m_cryoX = theEventInfo->getCryoX();
   }

   typedef AthenaHitsVector<LArG4H6FrontHit> CONTAINER;
   const DataHandle< CONTAINER > hitcoll ;

   std::vector<std::string>::iterator it = m_HitsCollNames.begin();


   dVect v_x;
   dVect v_y;
   dVect v_xz;
   dVect v_yz;
   dVect v_ex;
   dVect v_ey;

// loop hit containers
   for (; it < m_HitsCollNames.end(); ++it) {

      log<<MSG::DEBUG<<" hit container name: "<< *it <<endreq;

      sc = m_StoreGate->retrieve(hitcoll, *it) ;

      if(StatusCode::SUCCESS == sc ) {
	
        CONTAINER::const_iterator f_hit = hitcoll->begin();
        CONTAINER::const_iterator l_hit = hitcoll->end();

        log<<MSG::INFO<<" hit container: "<< *it <<" size: "<<hitcoll->size()<<endreq;

        for ( ; f_hit!=l_hit; ++f_hit) {
            LArG4H6FrontHit* hit = (*f_hit) ;
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

      } else {
        log<<MSG::ERROR<<" unable to retrieve hit container: "<< *it <<endreq;
      }
   }

   if(v_x.size() < 2 || v_y.size() < 2) { // Could not fit
      log<<MSG::DEBUG<<"Could not fit, setting zero. "<<v_x.size()<<"/"<<v_y.size()<<endreq;
      TBTrack *track = new TBTrack(0,0);
      track->setUintercept(0.);
      track->setVintercept(0.);
      track->setUslope(0.);
      track->setVslope(0.);
//      track->setResidualu(0, 0);
//      track->setResidualv(0, 0);
      track->setChi2_u(1000.);
      track->setChi2_v(1000.);
      track->setCryoHitu(0.);
      track->setCryoHitv(0.);
      track->setCryoHitw(30000.);
      sc = m_StoreGate->record(track,"Track");
    
      if ( sc.isFailure( ) ) {
         log << MSG::FATAL << "Cannot record Track" << endreq;
      }
      return sc;
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
      log << MSG::ERROR << "Fit in X-coordinate failure." << endreq;
      return StatusCode::FAILURE;
   }
    
   check = fitVect(v_y, v_yz, v_ey, a1_y, a2_y, chi2_y, residual_y);
   if(!check){
      log << MSG::ERROR << "Fit in Y-coordinate failure." << endreq;
      return StatusCode::FAILURE;
    } 
 
    // Setting the slopes & intercepts for each track //
    log << MSG::DEBUG << "Setting fit parameters of track." << endreq;
    log << MSG::DEBUG << "Intercepts: "<<a1_x<<" "<<a1_y << endreq;
    log << MSG::DEBUG << "Slopes: "<<a2_x<<" "<<a2_y << endreq;
   
    TBTrack *track = new TBTrack(v_x.size(), v_y.size());

    track->setUintercept(a1_x);
    track->setVintercept(a1_y);
    track->setUslope(a2_x);
    track->setVslope(a2_y);

    // Setting the residual for plane u //
    for(int i = 0; i < v_x.size(); ++i){
      track->setResidualu(i, residual_x[i]);
    }
    
    // Setting the residual for plane v //
    for(int i = 0; i < v_y.size(); ++i){
      track->setResidualv(i, residual_y[i]);
    }
    
    log << MSG::DEBUG << "chi2 in X: " << chi2_x << endreq;
    log << MSG::DEBUG << "chi2 in Y: " << chi2_y << endreq;
    log << MSG::DEBUG << "Setting chi2s of track." << endreq;

    track->setChi2_u(chi2_x);
    track->setChi2_v(chi2_y);

    // Setting the cryo plane (30000 mm in TB coordinate)
    track->setCryoHitu(a2_x*30000.+a1_x+m_cryoX);
    track->setCryoHitv(a2_y*30000.+a1_y);
    track->setCryoHitw(30000.);
   
    sc = m_StoreGate->record(track,"Track");
    
    if ( sc.isFailure( ) ) {
      log << MSG::FATAL << "Cannot record Track" << endreq;
      return sc;
    }

  return  StatusCode::SUCCESS;
}

//**************************************************************************** 
//* Finalize                                                                   
//****************************************************************************

StatusCode LArTBH6BeamInfo::finalize()
{
   MsgStream log(messageService(), name());                                   
   log << MSG::INFO << "LArTBH6BeamInfo::finalize()" << endreq;            
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
  
  MsgStream log(msgSvc(), name());

  int i;
  double m_s = 0;
  double m_su = 0;
  double m_sww = 0;
  double m_sw = 0;
  double m_suw = 0;

  int hitNum = vec.size();
  for(i = 0; i < hitNum; ++i){

    log << MSG::DEBUG << "Position in X: " << vec[i] <<endreq;
    log << MSG::DEBUG << "Position in Z: " << vec_z[i] <<endreq;
    log << MSG::DEBUG << "Error in X: " << vec_e[i] <<endreq;

    m_s += 1 / (vec_e[i]*vec_e[i]);
    m_su += vec[i] / (vec_e[i]*vec_e[i]); 
    m_sww += vec_z[i]*vec_z[i] / (vec_e[i]*vec_e[i]);
    m_sw += vec_z[i] / (vec_e[i]*vec_e[i]);
    m_suw += vec[i]*vec_z[i] / (vec_e[i]*vec_e[i]);
  }
 
  double denum = (m_s*m_sww-m_sw*m_sw);
  if(denum == 0){
    log << MSG::ERROR << " Invalid denumerator" << endreq;
    return false;
  }

  a1 = (m_su*m_sww - m_sw*m_suw)/ denum;
  a2 = (m_s*m_suw - m_su*m_sw)/ denum;
  log << MSG::DEBUG << "Fit results:" << " intercept = " << a1 << " and slope = " << a2 << endreq;

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
