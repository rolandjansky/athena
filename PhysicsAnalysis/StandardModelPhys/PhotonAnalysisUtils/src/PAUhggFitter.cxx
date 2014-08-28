/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// PAUhggFitter.cxx, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////////////////
// author: Marcello Fanti, marcello.fanti@mi.infn.it
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////

#include "CaloEvent/CaloCluster.h"

// Trk
//#include "TrkParameters/Perigee.h"
//#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/TrackParameters.h"
#include "PhotonAnalysisUtils/PAUhggFitter.h"
#include "PhotonAnalysisUtils/IPAUhggFitter.h"
//#include "VxVertex/VxContainer.h"	
//#include "VxVertex/VxCandidate.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
//#include "TH1.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include "PathResolver/PathResolver.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "InDetBeamSpotService/IBeamCondSvc.h" 


using HepGeom::Point3D;


// constructor
PAUhggFitter::PAUhggFitter(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t,n,p) {
  declareInterface<IPAUhggFitter>(this);
  //
  declareProperty("MaxChi2perDOF", m_chi2worst = 4 ) ;
  declareProperty("MinFitPoints",  m_nfitmin = 2 ) ;
  declareProperty("UsePS",          m_usePS=true) ;
  declareProperty("UseDummyVertex", m_useDummyVertex=true) ;
  declareProperty("UseCombinedFit", m_useCombinedFit=true) ;
  declareProperty("ExtrapolationTool",m_extrapolator); 
  declareProperty("beamCondSvcName",m_beamCondSvcName="BeamCondSvc");
  m_beamCondSvc = 0;
}

// destructor
PAUhggFitter::~PAUhggFitter()
{}

// Athena standard methods

// initialize
StatusCode PAUhggFitter::initialize() {
  MsgStream log(msgSvc(), name());

  m_PI = acos(-1.0) ;
  StatusCode sc = GetErrors();
  if ( sc.isFailure() ) return sc ;

  /***   to extrapolate perigee to an other vertex   ***/
  if ( m_extrapolator.retrieve().isFailure() ) {
    log << MSG::FATAL << "Failed to retrieve tool " << m_extrapolator << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved tool " << m_extrapolator << endreq;
  } 

  /* Get BeamCondSvc */
  sc = service(m_beamCondSvcName,m_beamCondSvc);
  if (sc.isFailure() || m_beamCondSvc == 0) {
    log << MSG::INFO << "Could not find BeamCondSvc. Will use (0,0,0) if no vertex is given and extrapolation is needed." << endreq;
    m_beamCondSvc = 0;
  }

  log <<  MSG::INFO  << name() <<" initialize() successful" << endreq;    
  return StatusCode::SUCCESS;
}

// finalize
StatusCode PAUhggFitter::finalize() {
  MsgStream log(msgSvc(), name());
  log <<  MSG::INFO  << name() <<" finalize() successful" << endreq;
  return StatusCode::SUCCESS;
}

bool PAUhggFitter::FitPAU(float& Zrec, float& ZrecRes, float& eta1rec, float& eta2rec) {
  MsgStream log(msgSvc(), name());

  m_ZVertexRec.resize(2) ;
  m_ZVertexRecRes.resize(2) ;
  m_NewRecoEta.resize(2) ;
  m_NewRecoTheta.resize(2) ;

  m_NewRecoEtaSeparate.clear() ;

  // first, fit 2 photons separately, IDVert is given from outside:
  log << MSG::VERBOSE << "--- Fitting photons ---" << endreq ;
  static const bool FillHistos = true ;
  for ( unsigned jphot=0 ; jphot<2 ; ++jphot )  SinglePhotonFit(jphot,FillHistos) ;
  if ( m_fit_ok[0] && m_fit_ok[1] )   m_NewRecoEtaSeparate = m_NewRecoEta ; // copy etas from separate fits
  log << MSG::VERBOSE << "... single fit: " << m_fit_ok[0] << " " << m_fit_ok[1] << endreq ;
  for ( unsigned u=0 ; u<2 ; ++u ) {
    if ( m_fit_ok[u] ) log << MSG::VERBOSE << "... Zrec[" << u << "] = " << m_ZVertexRec[u] 
			  << "\t +/- " << m_ZVertexRecRes[u] << endreq ;
  }
  //
  bool status = false ;
  if ( m_fit_ok[0] && m_fit_ok[1] ) { // both fits were successfull, now combine!
    combineZ() ;
    if ( m_useCombinedFit ) {
      m_useIDVert = true ;
      for ( unsigned jphot=0 ; jphot<2 ; ++jphot )  SinglePhotonFit(jphot) ;
      log << MSG::VERBOSE << "... combined fit: " << m_fit_ok[0] << " " << m_fit_ok[1] << endreq ;
      for ( unsigned u=0 ; u<2 ; ++u ) {
	if ( m_fit_ok[u] ) log << MSG::VERBOSE << "... Zrec[" << u << "] = " << m_ZVertexRec[u] 
			       << "\t +/- " << m_ZVertexRecRes[u] << endreq ;
      }
      combineZ() ;
    } else {
      log << MSG::VERBOSE << "You chose not to do combined fit" << endreq ;
    }
    Zrec    = m_IDVert ;
    ZrecRes = m_IDVertRes ;
    eta1rec = m_NewRecoEta[0] ;
    eta2rec = m_NewRecoEta[1] ;
    status = ( m_fit_ok[0] && m_fit_ok[1] ) ;
  } else if ( m_fit_ok[0] ) { // only photon-0 was ok, use it
    Zrec    = m_ZVertexRec[0] ;
    ZrecRes = m_ZVertexRecRes[0] ;
    eta1rec = m_NewRecoEta[0] ;
    eta2rec = -99999 ;
  } else if ( m_fit_ok[1] ) { // only photon-1 was ok, use it
    Zrec    = m_ZVertexRec[1] ;
    ZrecRes = m_ZVertexRecRes[1] ;
    eta2rec = m_NewRecoEta[1] ;
    eta1rec = -99999 ;
  } else { // both fits were bad, can't do anything!
    Zrec = ZrecRes = eta1rec = eta2rec = -99999 ;
  }
  log << MSG::VERBOSE << "... exiting FitPAU, status=" << status << endreq ;
  return status ;
}

void PAUhggFitter::combineZ() {
  float sumZ=0, sumW=0 ;
  for ( unsigned i=0 ; i<2 ; ++i ) {
    float w = 1./pow(m_ZVertexRecRes[i],2) ;
    sumW += w ;
    sumZ += w * m_ZVertexRec[i] ;
  }
  if ( m_useIDVert ) {
    float w = 1./pow(m_IDVertRes,2) ;
    sumW += w ;
    sumZ += w * m_IDVert ;
  }
  m_IDVert    = sumZ / sumW ;
  m_IDVertRes = 1. / sqrt(sumW) ;
}

bool PAUhggFitter::AssignRZ(int jphot,std::vector<float>& r,std::vector<float>& z,std::vector<float>& weight) {
  MsgStream log(msgSvc(), name());
  assert( jphot==0 || jphot==1 ) ;
  r.resize(0) ;
  z.resize(0) ;
  weight.resize(0) ;
  m_p_where.resize(0) ;
  // points from EMC layers:
  log << MSG::VERBOSE << "...... photon n.r " << jphot << endreq ;
  for ( unsigned layer=0 ; layer<3 ; ++layer ) {
    float R, Z ;
    if( gimeRZ(layer, m_etas[jphot][layer], R, Z ) ) {
      r.push_back(R) ;
      z.push_back(Z) ;
      m_p_where.push_back(layer) ;
      int ibin = (int) floor( 100 * fabs(m_etas[jphot][layer]) / 2.5 ) ;
      float err = 9.999e10 ;
      if ( ibin>=0 && ibin<100 ) {
	switch ( layer ) {
	case 0 : err = m_errors_ps[ibin]    ; break ;
	case 1 : err = m_errors_strip[ibin] ; break ;
	case 2 : err = m_errors_mid[ibin]   ; break ;
	}
      }
      weight.push_back( pow(err,-2) ) ;
      log << MSG::VERBOSE << "...... adding layer " << layer << " R=" << R << " Z=" << Z << " err=" << err << endreq ;
    }
  }
  // primary vertex from ID:
  if ( m_useIDVert ) {
    r.push_back(0) ;
    z.push_back(m_IDVert) ;
    m_p_where.push_back(PrimVtx) ;
    weight.push_back( pow(m_IDVertRes,-2) ) ;
    log << MSG::VERBOSE << "...... adding primary vertex, Z=" << m_IDVert << " err=" << m_IDVertRes << endreq ;
  }
  // point from conversion, if any:
  if ( m_useConversion && m_isConv[jphot] ) {
    r.push_back(m_Rconv[jphot]) ;
    z.push_back(m_Zconv[jphot]) ;
    m_p_where.push_back(Conv) ;
    float err = 5.4 ;
    if ( m_Rconv[jphot] < 200 )  err = 0.3 ;
    weight.push_back( pow(err,-2) ) ;
    log << MSG::VERBOSE << "...... adding conv. vertex, R=" << m_Rconv[jphot] << " Z=" << m_Zconv[jphot] << " err=" << err << endreq ;
  }
  return true ;
}

bool PAUhggFitter::SinglePhotonFit(int jphot, bool FillHistos) {
  if ( false ) std::cout << FillHistos << std::endl ; // dummy, to avoid warning
  assert( jphot==0 || jphot==1 ) ;
  std::vector<float> r, z, weight ;
  AssignRZ(jphot,r,z,weight) ;
  // keep track of which points we wanted to use
  std::vector<int> p_where0 = m_p_where ;
  // ok, let's fit!
  float zvrec, zvrecres, etarec, thetarec ;
  const bool rej = true ;
  if ( inEMBarrel( m_etas[jphot][2] ) ) {
    m_fit_ok[jphot] = FitDir(r,z,weight,rej) ;
    zvrec     = m_a ;
    zvrecres  = m_sigmaa ;
    thetarec  = -(atan(m_b)-m_PI/2) ;
    etarec    = -log(tan(thetarec/2.));
  } else {
    m_fit_ok[jphot] = FitDir(z,r,weight,rej) ;
    if(fabs(m_b)>.00000001 && fabs(m_a)>.00000001){
      zvrec     = -m_a/m_b ;
      zvrecres  = fabs(zvrec) * sqrt( pow(m_sigmaa/m_a,2) + pow(m_sigmab/m_b,2) + 2*m_covab/(m_a*m_b) ) ;
      thetarec  = -(atan(1./m_b)-m_PI/2.);
      etarec = -log(tan(thetarec/2.));
    }
    else{
      zvrec     = -9999.;
      zvrecres  = -9999.;
      thetarec  = -9999.;
      etarec    = -9999.;
    }
  }
  m_ZVertexRec[jphot]        = zvrec ;
  m_ZVertexRecRes[jphot]     = zvrecres ;
  m_NewRecoEta[jphot]        = etarec ;
  m_NewRecoTheta[jphot]      = thetarec ;
  m_chi2[jphot]              = m_chiquad ;
  return m_fit_ok[jphot] ;
}

void PAUhggFitter::killWorst(std::vector<float>& x, std::vector<float>& y, std::vector<float>& w, unsigned iworst) {
  assert( x.size()==y.size() && x.size()==w.size() ) ;
  std::vector<float> x1,y1,w1 ;
  std::vector<int> pw1 ;
  for ( unsigned i=0 ; i<x.size() ; ++i ) {
    if ( i != iworst ) {
      x1.push_back(x[i]) ;
      y1.push_back(y[i]) ;
      w1.push_back(w[i]) ;
      pw1.push_back(m_p_where[i]) ;
    }
  }
  x = x1 ;
  y = y1 ;
  w = w1 ;
  m_p_where = pw1 ;
}

bool PAUhggFitter::getEtaSinglePhoton(float& eta1,float& eta2) const {
  if ( m_NewRecoEtaSeparate.size() == 2 ) {
    eta1 = m_NewRecoEtaSeparate[0] ;
    eta2 = m_NewRecoEtaSeparate[1] ;
    return true ;
  }
  eta1 = eta2 = -99999 ;
  return false ;
}


//============================================================================
//
// This method is used to compute the fit given r and z positions
//
//============================================================================

bool PAUhggFitter::FitDir( std::vector<float> x, std::vector<float> y, std::vector<float> weight, bool rej ) {
  MsgStream log(msgSvc(), name());

  assert( x.size()==y.size() && x.size()==weight.size() ) ; 
  m_nFitPoints = x.size();

  float
    sumw   = 0 ,
    sumwx  = 0 ,
    sumwx2 = 0 ,
    sumwy  = 0 ,
    sumwy2 = 0 ,
    sumwxy = 0 ;
  for ( unsigned i = 0; i < m_nFitPoints; i++){
    sumw   += weight[i];
    sumwx  += x[i] * weight[i];
    sumwy  += y[i] * weight[i];
    sumwx2 += pow(x[i],2) * weight[i];
    sumwy2 += pow(y[i],2) * weight[i];
    sumwxy += x[i] * y[i] * weight[i];
  }
 
  float delta = sumw * sumwx2 - pow(sumwx,2);

  // these are the evaluated variables:
  m_a = (sumwx2 * sumwy - sumwx * sumwxy) / delta;
  m_b = (sumw * sumwxy - sumwx * sumwy) / delta;
  m_sigmaa = sqrt(sumwx2 / delta);
  m_sigmab = sqrt(sumw / delta);
  m_covab  = - sumwx / delta ;
  //---

  m_iworst = 0 ;
  m_chi2point.clear();
  m_chiquad = 0;
  for ( unsigned i = 0 ;  i < m_nFitPoints; i++){
    m_chi2point.push_back( pow(m_a+m_b*x[i]-y[i],2) * weight[i] );
    m_chiquad += m_chi2point[i];   
    if ( m_chi2point[i] > m_chi2point[m_iworst] ) m_iworst = i ;
  }

  if ( m_chiquad < m_chi2worst*(m_nFitPoints-2) ) { // good fit
    log << MSG::VERBOSE << "......... good fit, chi2/ndf=" << m_chiquad/(m_nFitPoints-2) << ", points: " << m_nFitPoints << endreq ;
    return true ;
  } else if ( m_nFitPoints == 2 ) {
    log << MSG::VERBOSE << "......... fit through 2 points" << endreq ;
    return true ;
  } else if ( m_nFitPoints<=m_nfitmin || !rej ) { // bad fit, but cannot reject points
    log << MSG::VERBOSE << "......... bad fit, chi2/ndf=" << m_chiquad/(m_nFitPoints-2) << endreq ;
    if ( !rej )  log << MSG::VERBOSE << "          (rejection is off!)" << endreq ;
    else         log << MSG::VERBOSE << "          (left with only " << m_nFitPoints << " points)" << endreq ;
    return false ;
  } else { // try to reject worst point and recursively reiterate
    log << MSG::VERBOSE << "......... reject point " << m_iworst << ", chi2/ndf=" << m_chiquad/(m_nFitPoints-2) << endreq ;
    killWorst(x,y,weight,m_iworst) ;
    return FitDir(x,y,weight,rej) ;
  }
}


bool PAUhggFitter::gimeRZ(int layer, float eta, float& R, float& Z) const {
  MsgStream log(msgSvc(), name());
  const float etaBarrelEnd = 1.475 ;
  const float etaAB        = 0.8 ;
  const float eta2PS       = 1.8 ;
  const float eta2EM       = 2.5 ;
  R = Z = -9999. ;
  float aeta = fabs(eta) ;
  if ( aeta > eta2EM ) {
    log << MSG::DEBUG << "gimeRZ: out of EM, eta = " << eta << " layer " << layer << endreq ;
    return false ;
  }
  float sign = (eta>=0) ? 1 : -1 ;
  float Sinh = sinh(eta) ;
  switch ( layer ) {
  case 0 : // PS -----------------------------------------
    if ( aeta < etaBarrelEnd ) {
      R = 144.3 ;
      Z = R * Sinh ;
    } else if ( aeta < eta2PS ) {
      Z = 362.5 * sign ;
      R = Z / Sinh ;
    } else {
      log << MSG::DEBUG << "Geometry::gimeRZ: out of PS, eta = " << eta << endreq ;
      return false ;
    }
    break ;
  case 1 : // Strips -------------------------------------
    if ( aeta < etaAB ) {
      R = 156.78 - 1.8975*aeta - 1.7668*aeta*aeta;
      Z = R * Sinh ;
    } else if ( aeta < etaBarrelEnd ) {
      R = 150.32 + 7.1716*aeta - 4.1008*aeta*aeta;
      Z = R * Sinh ;
    } else {
      Z = 376. * sign ;
      R = Z / Sinh ;
    }
    break ;
  case 2 : // Middle -------------------------------------
    if ( aeta < etaAB ) {
      R = 169.71 - 1.5311*aeta - 6.4153*aeta*aeta;
      Z = R * Sinh ;
    } else if ( aeta < etaBarrelEnd ) {
      R = 173.91 - 7.5648*aeta - 1.8501*aeta*aeta ;
      Z = R * Sinh ;
    } else {
      Z = 388. * sign ;
      R = Z / Sinh ;
    }
    break ;
  default: 
    log << MSG::ERROR << "gimeRZ: ERROR: layer " << layer << " not known!" << endreq ;
    return false ;
  } ;
  R *= 10 ; // convert to mm
  Z *= 10 ; // convert to mm
  return true ;
}


bool PAUhggFitter::inEMBarrel( float eta ) const {
  return ( fabs(eta) < 1.475 ) ;
}

float PAUhggFitter::EtaFromVertex(int layer, float eta, float Zvtx) const {
  float R, Z ;
  if ( ! gimeRZ(layer,eta,R,Z) ) return -99999. ;
  return asinh( (Z-Zvtx) / R ) ;
}


StatusCode PAUhggFitter::GetErrors() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Loading errors..." << endreq ;
  std::string line;
  //char num[15];
  std::string datmidname=PathResolver::find_file ("dzmid.dat", "DATAPATH");
  if (datmidname == "")
    {
      log << MSG::FATAL << "Could not locate dzmid.dat  data file" << endreq ;
      return  StatusCode::FAILURE;
    }
  const char * datmidfile= datmidname.c_str() ;
  std::ifstream myfilemid (datmidfile);
  if (myfilemid.is_open())  {
    int i =0;
    while (! myfilemid.eof() && i<100 ) {
      myfilemid >> m_errors_mid[i];
      i++;
    }
    myfilemid.close();
    log << MSG::INFO << "errors on Middle loaded!" << endreq ;
  } else {
    log << MSG::ERROR << "could not open file dzmid.dat" << endreq ;
    return StatusCode::FAILURE ;
  }

  std::string datstrname=PathResolver::find_file ("dzstr.dat", "DATAPATH");
  if (datstrname == "")
    {
      log << MSG::FATAL << "Could not locate dzstr.dat  data file" << endreq ;
      return  StatusCode::FAILURE;
    }

  const char * datstrfile= datstrname.c_str() ;
  std::ifstream myfilestr (datstrfile);
  if (myfilestr.is_open()) {      
    int i = 0 ;
    while (! myfilestr.eof() && i<100 ) {
      myfilestr >> m_errors_strip[i];
      i++;
    }
    myfilestr.close();
    log << MSG::INFO << "errors on Strips loaded!" << endreq ;
  } else {
    log << MSG::ERROR << "could not open file dzstr.dat" << endreq ;
    return StatusCode::FAILURE ;
  }
  

  std::string datpsname=PathResolver::find_file ("dzps.dat", "DATAPATH");
  if (datpsname == "")
    {
      log << MSG::FATAL << "Could not locate dzps.dat  data file" << endreq ;
      return  StatusCode::FAILURE;
    }

  const char * datpsfile= datpsname.c_str() ;
  std::ifstream myfileps (datpsfile);
  if (myfileps.is_open()) {      
    int i = 0 ;
    while (! myfileps.eof() && i<100 ) {
      myfileps >> m_errors_ps[i] ;
      i++;
    }
    myfileps.close();
    log << MSG::INFO << "errors on PS loaded!" << endreq ;
  }   else {
    log << MSG::ERROR << "could not open file dzps.dat" << endreq ;
    return StatusCode::FAILURE ;
  }

  if ( ! m_usePS ) {
    for ( unsigned i=0 ; i<100 ; ++i ) m_errors_ps[i] = +9.9999e+10 ;
    log << MSG::INFO << "You chose not to use Presampler, errors are set to +9.9999e+10" << endreq ;
  }

  return StatusCode::SUCCESS ;

}

void PAUhggFitter::SetEtas(const CaloCluster* clus1,const CaloCluster* clus2) {
  MsgStream log(msgSvc(), name());
  const CaloCluster* clus[2] ;
  clus[0] = clus1 ;
  clus[1] = clus2 ;
  for ( unsigned i=0 ; i<2 ; ++i ) {
    m_etas[i].clear() ;
    if ( clus[i] != NULL ) {
      for ( unsigned u=0 ; u<3 ; ++u ) m_etas[i].push_back( clus[i]->etaBE(u) ) ;
      log << MSG::VERBOSE << "PAUhggFitter::SetEtas : gamma" << i+1 << " : " ;
      for ( unsigned u=0 ; u<3 ; ++u ) log << "\t eta" << u << "=" << m_etas[i][u] ;
      log << endreq ;
    } else {
      log << MSG::WARNING << "PAUhggFitter::SetEtas : gamma" << i+1 << " has NULL cluster " << endreq ;
      for ( unsigned u=0 ; u<3 ; ++u ) m_etas[i].push_back( -99999 ) ;
    }
  }
  // reset primary vertex info:
  m_useIDVert = true ;
  m_IDVert    = 0 ;
  m_IDVertRes = 56 ; //mm
  if ( ! m_useDummyVertex ) {
    m_IDVertRes = +9.9999e+10 ;
    log << MSG::VERBOSE << "You chose not to use dummy primary vertex, its error is set to +9.9999e+10" << endreq ;
  }
  // reset conversion/perigee info:
  m_useConversion = false ;
  m_isConv[0] = false ;
  m_isConv[1] = false ;
}

void PAUhggFitter::SetPrimVertexInfo(float Zinput,float DZinput) {
  MsgStream log(msgSvc(), name());
  m_useIDVert = true;
  m_IDVert    = Zinput;
  m_IDVertRes = DZinput;
  log << MSG::VERBOSE << "PAUhggFitter::SetPrimVertexInfo: set primary vertex to Z=" << Zinput << " sigma=" << DZinput << endreq ;
}

void PAUhggFitter::SetConversion(unsigned gamma_index, const Trk::VxCandidate* conv) {
  MsgStream log(msgSvc(), name());
  if ( gamma_index<1 || gamma_index>2 ) {
    log << MSG::WARNING << "PAUhggFitter::SetConversion called with index=" << gamma_index << " --- must be 1 or 2" << endreq ;
    return ;
  }
  if ( conv == NULL ) {
    log << MSG::WARNING << "PAUhggFitter::SetConversion called with NULL conversion for index=" << gamma_index << endreq ;
    return ;
  }
  unsigned iGam = gamma_index - 1 ;
  m_useConversion = true ;
  m_isConv[iGam] = true ;
  m_Rconv[iGam] = sqrt( pow(conv->recVertex().position()[0],2) + pow(conv->recVertex().position()[1],2) ) ;
  m_Zconv[iGam] = conv->recVertex().position()[2] ;
  log << MSG::VERBOSE << "PAUhggFitter::SetConversion: set conversion for gamma" << gamma_index << " at R=" << m_Rconv[iGam]
      << " Z=" << m_Zconv[iGam] << endreq ;
}

void PAUhggFitter::SetTrack(unsigned gamma_index, const Rec::TrackParticle* trk) {
  MsgStream log(msgSvc(), name());
  if ( gamma_index<1 || gamma_index>2 ) {
    log << MSG::WARNING << "PAUhggFitter::SetTrack called with index=" << gamma_index << " --- must be 1 or 2" << endreq ;
    return ;
  }
  if ( trk == NULL ) {
    log << MSG::WARNING << "PAUhggFitter::SetTrack called with NULL conversion for index=" << gamma_index << endreq ;
    return ;
  }
  unsigned iGam = gamma_index - 1 ;
  m_useConversion = true ;
  m_isConv[iGam] = true ;
  m_Rconv[iGam] = 0 ;


  const Trk::TrackParameters* oldPerigee = trk->measuredPerigee();
  //notused? float trkz0=std::fabs(trk->measuredPerigee()->parameters()[Trk::z0]);
  float beamSpotX = 0.;
  float beamSpotY = 0.;
  float beamSpotZ = 0.;
  if (m_beamCondSvc) {
    Amg::Vector3D bpos = m_beamCondSvc->beamPos();
    beamSpotX = bpos.x();
    beamSpotY = bpos.y();
    beamSpotZ = bpos.z();
  }
  Amg::Vector3D newPerigeePosition(beamSpotX, beamSpotY, beamSpotZ);
  Trk::PerigeeSurface surface(newPerigeePosition);
  const Trk::TrackParameters* newPerigee = m_extrapolator->extrapolate(*oldPerigee,surface);
  float new_trkz0 = -9999.;
  if (newPerigee) new_trkz0 = newPerigee->parameters()[Trk::z0];
  //std::cout << "----->old z0 = " << trkz0 << " - new z0 = " << new_trkz0 << std::endl;
  

  if (fabs(new_trkz0+9999)<1e-5) m_Zconv[iGam] = trk->measuredPerigee()->parameters()[Trk::z0] ;
  else m_Zconv[iGam] = new_trkz0;

  log << MSG::VERBOSE << "PAUhggFitter::SetTrack: set track perigee for gamma" << gamma_index << " at R=" << m_Rconv[iGam]
      << " Z=" << m_Zconv[iGam] << endreq ;
}

/////////////////////////////////////////////////

void PAUhggFitter::SetEtas(std::vector<float> eta1input , std::vector<float> eta2input ){
  m_etas[0] = eta1input ;
  m_etas[1] = eta2input ;
}

void PAUhggFitter::SetPrimVertexInfo(bool usevtx,float Zinput,float DZinput) {
  m_useIDVert = usevtx;
  m_IDVert    = Zinput;
  m_IDVertRes = DZinput;
}

void PAUhggFitter::SetConvInfo(bool usecv,bool conv1,float rcv1,float zcv1,bool conv2,float rcv2,float zcv2){
  m_useConversion = usecv;
  m_isConv[0] = conv1;
  m_isConv[1] = conv2;
  m_Rconv[0] = rcv1;
  m_Zconv[0] = zcv1;
  m_Rconv[1] = rcv2;
  m_Zconv[1] = zcv2; 
}

