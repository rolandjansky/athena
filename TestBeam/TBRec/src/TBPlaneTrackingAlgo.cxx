/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "PathResolver/PathResolver.h"

#include "TBRec/TBPlaneTrackingAlgo.h"
#include "TBEvent/TBHitPlaneCont.h"
#include "TBEvent/TBBPCCont.h"
#include "TBEvent/TBHitPlane.h"
#include "TBEvent/TBTrack.h"
#include "TBEvent/TBEventInfo.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

class ISvcLocator;

TBPlaneTrackingAlgo::TBPlaneTrackingAlgo(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{
  MsgStream log(msgSvc(), "TBPlaneTrackingAlgo" );
  declareProperty("TestAlgo",m_testAlgo=false);
  declareProperty("CalibFileName",  m_calib_filename="H6HitCalib.txt");
  declareProperty("BPCnames",  m_bpc_names);
  declareProperty("BPCposX",  m_bpc_posX);
  declareProperty("BPCposY",  m_bpc_posY);
  declareProperty("BPCposZX",  m_bpc_posZX);
  declareProperty("BPCposZY",  m_bpc_posZY);
  declareProperty("BPCerrposX",  m_bpc_errposX);
  declareProperty("BPCerrposY",  m_bpc_errposY);
  declareProperty("BPCerrposZX",  m_bpc_errposZX);
  declareProperty("BPCerrposZY",  m_bpc_errposZY);
  declareProperty("BPCerrmeasX",  m_bpc_errmeasX);
  declareProperty("BPCerrmeasY",  m_bpc_errmeasY);
  
}

////////////////////////////////////////////
StatusCode TBPlaneTrackingAlgo::initialize()
////////////////////////////////////////////
{
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  
  sc = service( "StoreGateSvc", m_StoreGate);
  
  if( sc.isFailure() ) {
    log << MSG::FATAL << name() 
  	<< ": Unable to locate Service StoreGateSvc" << endreq;
    return sc;
  } 
  

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////
StatusCode TBPlaneTrackingAlgo::execute()
/////////////////////////////////////////
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Executing TBPlaneTracking algorithm" << endreq;
  
  if(m_testAlgo == true){
    log << MSG::WARNING << "TBPlaneTrackingAlgo: "
	<< "Using FillRandomHit method to get data" 
	<< endreq;
    FillRandomHit();
  }

  // Check if this is not a physics event and exit OK
  // retrieve Event Info
  const TBEventInfo* theEventInfo;
  StatusCode checkOut = m_StoreGate->retrieve(theEventInfo,"TBEventInfo");
  if ( checkOut.isFailure() )
    {
      log << MSG::ERROR
	  << "cannot retrieve TBEventInfo from StoreGate"
	  << endreq;
      return StatusCode::FAILURE;
    }
  int evtType = theEventInfo->getEventType();
  log << MSG::DEBUG << "Event Type found " << evtType << endreq;
  if (evtType != 1)  return StatusCode::SUCCESS;

  StatusCode sc1;

  // Get run number and get new calib constants -----------------------------
  unsigned int thisrun=0;
  EventID *thisEvent;           //EventID is a part of EventInfo
  const EventInfo* thisEventInfo;
  sc1=m_StoreGate->retrieve(thisEventInfo);
  if (sc1!=StatusCode::SUCCESS){
    log << MSG::WARNING << "No EventInfo object found! Can't read run number!" << endreq;
    log << MSG::WARNING << "     => can't get calib constant. Exit" << endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  else
    {
      thisEvent=thisEventInfo->event_ID();
      thisrun = thisEvent->run_number();
    }

  if(thisrun != m_runnumber)
    {
      m_runnumber= thisrun;
      getnewcalib();
    }
  // ------------------------------------------------------------------------



  sc1 = buildHits();
  
//   sc1 = m_StoreGate->retrieve(m_hitPlaneCont_u,"HitPlaneCont_U");
//   sc2 = m_StoreGate->retrieve(m_hitPlaneCont_v,"HitPlaneCont_V");
  
  if(sc1.isFailure()){
    log << MSG::WARNING << "TBPlaneTrackingAlgo: Retrieval of HitPlane failed" 
	<< endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  } else {

    int hitPlaneNumU, hitPlaneNumV;
    hitPlaneNumU = m_hitPlaneCont_u.size();
    hitPlaneNumV = m_hitPlaneCont_v.size();
    
    if( (hitPlaneNumU < 2) || (hitPlaneNumV < 2) ){
      log << MSG::WARNING << "TBPlaneTrackingAlgo: "
	  << "Not enough hits in one or both planes, " 
	  << "Cannot make track."
	  << endreq;
      
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    }

    // Setting the chi2's for the track //
    double chi2_u = 0;
    double chi2_v = 0;
    std::vector<double> residual_u, residual_v;
    double a1_u = 0;
    double a2_u = 0;
    double a1_v = 0;
    double a2_v = 0;
    bool check = true;

    check = fitPlane(&m_hitPlaneCont_u, a1_u, a2_u, chi2_u, residual_u);
    if(check == false){
      log << MSG::ERROR << "TBPlaneTrackingAlgo: "
	  << "Fit failure." 
	  << endreq;
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    }
    
    check = fitPlane(&m_hitPlaneCont_v, a1_v, a2_v, chi2_v, residual_v);
    if(check == false){
      log << MSG::ERROR << "TBPlaneTrackingAlgo: "
	  << "Fit failure." 
	  << endreq;
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    } 
 
    // Setting the slopes & intercepts for each track //
    log << MSG::DEBUG << "Setting fit parameters of track." << endreq;
   
    TBTrack *track = new TBTrack(hitPlaneNumU, hitPlaneNumV);
    track->setUintercept(a1_u);
    track->setVintercept(a1_v);
    track->setUslope(a2_u);
    track->setVslope(a2_v);

    // Setting the residual for plane u //
    for(int i = 0; i < hitPlaneNumU; i++){
      track->setResidualu(i, residual_u[i]);
    }
    
    // Setting the residual for plane v //
    for(int i = 0; i < hitPlaneNumV; i++){
      track->setResidualv(i, residual_v[i]);
    }
    
    log << MSG::DEBUG << "chi2 in u: " << chi2_u << endreq;
    log << MSG::DEBUG << "chi2 in v: " << chi2_v << endreq;
    log << MSG::DEBUG << "Setting chi2s of track." << endreq;

    track->setChi2_u(chi2_u);
    track->setChi2_v(chi2_v);
   
    // Record track info. into storeGate //
    StatusCode sc = m_StoreGate->record(track,"Track");
    
    if ( sc.isFailure( ) ) {
      log << MSG::FATAL << "Cannot record Track" << endreq;
    }
  }
  
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////
StatusCode TBPlaneTrackingAlgo::finalize()
//////////////////////////////////////////
{
  MsgStream log(msgSvc(), name());	
  log << MSG::INFO << "Finalizing TBTracking algorithm" << endreq;
  return StatusCode::SUCCESS; 
}

///////////////////////////////////////////////////////////////////////////
bool TBPlaneTrackingAlgo::fitPlane(const TBHitPlaneCont * hitPlaneCont,
double &a1, double &a2,double &chi2, std::vector<double> &residual)
///////////////////////////////////////////////////////////////////////////
{
  MsgStream log(msgSvc(), name());

  // Get number of hits in plane //
  int hitPlaneNum = hitPlaneCont->size();
  
  log << MSG::DEBUG << "The hit plane container size is: "
      << hitPlaneNum 
      << endreq;
  
  TBHitPlaneCont::const_iterator it_hit = hitPlaneCont->begin();
  TBHitPlaneCont::const_iterator last_hit = hitPlaneCont->end();
  
  std::vector<double> vec_u;
  std::vector<double> vec_w;
  std::vector<double> err_vec_u;
  std::vector<double> err_vec_w;

  for (; it_hit!=last_hit; it_hit++) { 
    log << MSG::DEBUG << "Position in u: " << (*it_hit)->getPosu() <<endreq;
    log << MSG::DEBUG << "Position in w: " << (*it_hit)->getPosw() <<endreq;
    log << MSG::DEBUG << "Error in u: " << (*it_hit)->getErroru() <<endreq;
    log << MSG::DEBUG << "Error in w: " << (*it_hit)->getErrorw() << endreq;
    vec_u.push_back((*it_hit)->getPosu());
    vec_w.push_back((*it_hit)->getPosw());
    err_vec_u.push_back((*it_hit)->getErroru());
    err_vec_w.push_back((*it_hit)->getErrorw());
  }
  
  if(vec_u.size() != vec_w.size()){
    log << MSG::ERROR << "TBPlaneTrackingAlgo: Invalid hits" << endreq;
    return false;
  }

  bool check;
//  check = fitHits(vec_u, vec_w, err_vec_u, err_vec_w, a1, a2);
  check = fitHits(vec_u, vec_w, err_vec_u, a1, a2);
  if(check == false){
    log << MSG::ERROR << "TBPlaneTrackingAlgo: Invalid denumerator" << endreq;
    return false;
  }

//  chi2 = getChi2(vec_u, vec_w, err_vec_u, err_vec_w, a1, a2);
  chi2 = getChi2(vec_u, vec_w, err_vec_u, a1, a2);

  log << MSG::DEBUG << "Fit results:" 
      << " intercept = " << a1 
      << " and slope = " << a2 
      << endreq;
  
  // Must be wrong //
  for (int i = 0; i < hitPlaneNum; i++) {
    residual.push_back(getResidual(vec_u[i],vec_w[i],a1, a2));
  }
  return true;
}

/////////////////////////////////////////////////////////////////
bool TBPlaneTrackingAlgo::fitHits(const std::vector<double> & v_u,
				  const std::vector<double> & v_w,
				  const std::vector<double> & v_eu,
//				  const std::vector<double> & v_ew, 
				  double &a1, double &a2)
/////////////////////////////////////////////////////////////////
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
  
  double m_s = 0;
  double m_su = 0;
  double m_sww = 0;
  double m_sw = 0;
  double m_suw = 0;

  int numHits = v_u.size();
  for(int i = 0; i < numHits; i++){
    m_s += 1 / (v_eu[i]*v_eu[i]);
    m_su += v_u[i] / (v_eu[i]*v_eu[i]); 
    m_sww += v_w[i]*v_w[i] / (v_eu[i]*v_eu[i]);
    m_sw += v_w[i] / (v_eu[i]*v_eu[i]);
    m_suw += v_u[i]*v_w[i] / (v_eu[i]*v_eu[i]);
  }
 
  double denum = (m_s*m_sww-m_sw*m_sw);
  if(denum == 0){
    return false;
  }

  a1 = (m_su*m_sww - m_sw*m_suw)/ denum;
  a2 = (m_s*m_suw - m_su*m_sw)/ denum;
  return true;
}
/////////////////////////////////////////////////////////////////////////////
double TBPlaneTrackingAlgo::getResidual(const double &u, const double &w
					, const double &a1, const double &a2)
/////////////////////////////////////////////////////////////////////////////
{
  return (u - a1 - a2*w);
}

///////////////////////////////////////////////////////////////////
double TBPlaneTrackingAlgo::getChi2(const std::vector<double> &v_u,
                                    const std::vector<double> &v_w,
				    const std::vector<double> &v_eu,
//				    const std::vector<double> &v_ew, 
				    const double &a1, const double &a2)
///////////////////////////////////////////////////////////////////
{ 
  // *** as in fitHits() above, the chi squared calculation below
  // (taken from 'Numerical Methods in C' section 15.2) 
  // assumes that the w data points are known exactly i.e. that 
  // v_ew[i]=0 for all i
  
  int numX = v_u.size();
  double chi2 = 0;
  
  for(int i = 0; i < numX; i++){
    chi2 += (v_u[i] - a1 - a2*v_w[i])*(v_u[i] - a1 - a2*v_w[i])/(v_eu[i]*v_eu[i]);;
  }  
  return chi2;
}


/////////////////////////////////////////
void TBPlaneTrackingAlgo::FillRandomHit()
/////////////////////////////////////////
{

  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "Starting FillRandom" <<endreq;
 
  TBHitPlaneCont * hitPlaneCont_U = new TBHitPlaneCont(); 
  TBHitPlaneCont * hitPlaneCont_V = new TBHitPlaneCont();
  
  TBHitPlane *hitPaneU1 = new TBHitPlane();
  hitPaneU1->setPosu(1);  hitPaneU1->setPosw(1);
  hitPlaneCont_U->push_back(hitPaneU1);
  

  TBHitPlane *hitPaneU2 = new TBHitPlane();
  hitPaneU2->setPosu(2);  hitPaneU2->setPosw(2);
  hitPlaneCont_U->push_back(hitPaneU2);

  StatusCode sc1 = m_StoreGate->record(hitPlaneCont_U,"HitPlaneCont_U");
  if ( sc1.isFailure( ) ) {
    log << MSG::FATAL << "Cannot record HitPlaneCont_U" << endreq;
  }

  TBHitPlane *hitPaneV1 = new TBHitPlane();
  hitPaneV1->setPosu(1);  hitPaneV1->setPosw(1);
  hitPlaneCont_V->push_back(hitPaneV1);

  TBHitPlane *hitPaneV2 = new TBHitPlane();
  hitPaneV2->setPosu(2);  hitPaneV2->setPosw(2);
  hitPlaneCont_V->push_back(hitPaneV2);

  StatusCode sc2 = m_StoreGate->record(hitPlaneCont_V,"HitPlaneCont_V");
  if ( sc2.isFailure( ) ) {
    log << MSG::FATAL << "Cannot record HitPlaneCont_V" << endreq;
  }
 
  log << MSG::DEBUG << "FillRandom Done" <<endreq;
 
}

/////////////////////////////////////////
StatusCode TBPlaneTrackingAlgo::buildHits()
/////////////////////////////////////////
{

  MsgStream log(messageService(),name());
  StatusCode sc;
  log << MSG::DEBUG << " In buildHits() " << endreq;
  m_hitPlaneCont_u.clear();
  m_hitPlaneCont_v.clear();

  TBBPCCont * bpcCont;
  sc = m_StoreGate->retrieve(bpcCont, "BPCCont");
  if (sc.isFailure()){
    log << MSG::DEBUG << "Retrieval of BPCCont failed" << endreq;
    
  }else {

    TBBPCCont::const_iterator it_bc   = bpcCont->begin();
    TBBPCCont::const_iterator last_bc   = bpcCont->end();

  // Loop over BPC
  for(;it_bc != last_bc;it_bc++){
    const TBBPC * bpc = (*it_bc);
    std::string name = bpc->getDetectorName();
    log << MSG::DEBUG << " Hits in BPC "<< name << endreq;
    // Find calibration index for this BPC
    unsigned int ind=0;
    while(ind<m_bpc_names.size()) 
	{
	  if(name==m_bpc_names[ind]) break; 
	  else ind++;
	}
    if(ind==m_bpc_names.size()){log<<MSG::ERROR<< "No calibrations for BPC" <<name<<endreq;continue;}
    
    log << MSG::DEBUG << " BPC number "<< ind << endreq;
   
    
    if(!(bpc->isXPosOverflow()||bpc->isXPulseOverflow())){
      TBHitPlane * hitu = new TBHitPlane();
      float w = m_bpc_posZX[ind];
      //      float u =m_bpc_posX[ind] + m_bpc_invX[ind]*bpc->getXPos();
      float u =m_bpc_posX[ind] + bpc->getXPos();
      log << MSG::DEBUG << "BPC" << ind << "X =" << u << endreq;
      hitu->setPosu(u);  hitu->setPosw(w);
      float uerr=sqrt(m_bpc_errposX[ind]*m_bpc_errposX[ind]+m_bpc_errmeasX[ind]*m_bpc_errmeasX[ind]);
      hitu->setErroru(uerr);  hitu->setErrorw(m_bpc_errposZX[ind]);
      hitu->setValidFlag(bpc->isXPosOverflow()&&bpc->isYPosOverflow());
      m_hitPlaneCont_u.push_back(hitu);
    }

    if(!(bpc->isYPosOverflow()||bpc->isYPulseOverflow())){
      TBHitPlane * hitv = new TBHitPlane();
      float w = m_bpc_posZY[ind];
      //      float v =m_bpc_posY[ind] + m_bpc_invY[ind]*bpc->getYPos();
      float v =m_bpc_posY[ind] + bpc->getYPos();
      log << MSG::DEBUG << "BPC" << ind << "Y =" << v << endreq;
      hitv->setPosu(v);  hitv->setPosw(w);
      float verr=sqrt(m_bpc_errposY[ind]*m_bpc_errposY[ind]+m_bpc_errmeasY[ind]*m_bpc_errmeasY[ind]);
      hitv->setErroru(verr);  hitv->setErrorw(m_bpc_errposZY[ind]);
      hitv->setValidFlag(bpc->isXPosOverflow()&&bpc->isYPosOverflow());
      m_hitPlaneCont_v.push_back(hitv);
    }


    }
  } // BPC

  return sc;

}


StatusCode TBPlaneTrackingAlgo::getnewcalib()
{
  // Get calib constant from an ASCII file with the following structure :
  // 
  // runnumber
  // bpcnumber1 coeff1 coeff2 ... coeff8
  // bpcnumber2 coeff1 coeff2 ... coeff8
  // ...
  // bpcnumber6 coeff1 coeff2 ... coeff8
  // runnumber
  // ...
  //
  // coeff must have the following order :
  // bpcnumber posX posY posZX posZY errposX errposY errposZX errposZY errmeasX errmeasY

  MsgStream log(messageService(),name());
  log << MSG::DEBUG << "Get new calibs for run " << m_runnumber<< endreq;
  
  int bpcnumber= m_bpc_names.size();

  m_bpc_posX.clear(); m_bpc_posX.resize(bpcnumber);
  m_bpc_posY.clear(); m_bpc_posY.resize(bpcnumber);
  m_bpc_posZX.clear(); m_bpc_posZX.resize(bpcnumber);
  m_bpc_posZY.clear(); m_bpc_posZY.resize(bpcnumber);
  m_bpc_errposX.clear(); m_bpc_errposX.resize(bpcnumber);
  m_bpc_errposY.clear(); m_bpc_errposY.resize(bpcnumber);
  m_bpc_errposZX.clear(); m_bpc_errposZX.resize(bpcnumber);
  m_bpc_errposZY.clear(); m_bpc_errposZY.resize(bpcnumber);
  m_bpc_errmeasX.clear(); m_bpc_errmeasX.resize(bpcnumber);
  m_bpc_errmeasY.clear(); m_bpc_errmeasY.resize(bpcnumber);

  int pos;

  std::ifstream calibfile;
  std::string filename = PathResolver::find_file (m_calib_filename.c_str(), "DATAPATH");
  calibfile.open(filename.c_str());
  if(!calibfile.good()){
    log << MSG::WARNING << " Problem with file named "<< m_calib_filename << " in $DATAPATH" << endreq;
    return StatusCode::FAILURE;
  } else {
     log << MSG::DEBUG << " File: "<< filename << " opened" << endreq;
  }
  unsigned int runnumber;
  calibfile >> runnumber;
  pos = calibfile.tellg();
  log << MSG::DEBUG << " Run number "<< runnumber << endreq;
  while((runnumber<m_runnumber)&&(!calibfile.eof()))
    {
      runnumber=0;
      pos = calibfile.tellg();
      // discard next lines
      for(int j=0;j<bpcnumber+1;j++) calibfile.ignore(5000,'\n');
      // check next runnumber
      calibfile >> runnumber;
      if(runnumber==0) { log << MSG::DEBUG << "empty line"<<endreq;calibfile.clear();break;} // reached an empty line : exit.
      log << MSG::DEBUG << " Run number "<< runnumber << endreq;
    }
  
  // Now we found the good set of constant (the ones following pos)
  if(runnumber==m_runnumber)  pos = calibfile.tellg();
  log << MSG::DEBUG << " Pos = "<< pos << endreq;
  calibfile.seekg(pos);
  log << MSG::DEBUG << " Will use the following constants :" << endreq;
  for(int j=0;j<bpcnumber;j++) 
    {
      int bpcn;
      calibfile >> bpcn; 
      calibfile >> m_bpc_posX[j];
      calibfile >> m_bpc_posY[j];
      calibfile >> m_bpc_posZX[j];
      calibfile >> m_bpc_posZY[j];
      calibfile >> m_bpc_errposX[j];
      calibfile >> m_bpc_errposY[j];
      calibfile >> m_bpc_errposZX[j];
      calibfile >> m_bpc_errposZY[j];
      calibfile >> m_bpc_errmeasX[j];
      calibfile >> m_bpc_errmeasY[j];

      log << MSG::DEBUG << bpcn << " "<<m_bpc_posX[j];    
      log << MSG::DEBUG << " "<< m_bpc_posY[j];    
      log << MSG::DEBUG << " "<< m_bpc_posZX[j];   
      log << MSG::DEBUG << " "<< m_bpc_posZY[j];   
      log << MSG::DEBUG << " "<< m_bpc_errposX[j]; 
      log << MSG::DEBUG << " "<< m_bpc_errposY[j]; 
      log << MSG::DEBUG << " "<< m_bpc_errposZX[j];  
      log << MSG::DEBUG << " "<< m_bpc_errposZY[j];
      log << MSG::DEBUG << " "<< m_bpc_errmeasX[j];
      log << MSG::DEBUG << " "<< m_bpc_errmeasY[j]<<endreq;

				    

				    
	
    }
  
  calibfile.close();

  return StatusCode::SUCCESS;  
  
}
