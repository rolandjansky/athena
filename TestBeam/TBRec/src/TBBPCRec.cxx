/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "PathResolver/PathResolver.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <iostream>
#include <fstream>
#include <math.h>

#include "TBRec/TBBPCRec.h"

//#include "GaudiKernel/AlgFactory.h"
// static const AlgFactory<TBBPCRec> s_factory;
// const IAlgFactory& TBBPCRecFactory = s_factory;





TBBPCRec::TBBPCRec(const std::string& name,
				 ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator),
  m_StoreGate(0)
 {
  // job options

   declareProperty("SGkey",  m_SGkey="BPCRawCont");
   declareProperty("SGrecordkey",  m_SGrecordkey="BPCCont");
   declareProperty("CalibFileName",  m_calib_filename="H6BPCCalib.txt");

   declareProperty("BPCnames",  m_bpc_names);
   declareProperty("BPCcalibX",  m_bpc_calibX);
   declareProperty("BPCcalibY",  m_bpc_calibY);
   declareProperty("BPCleftright",  m_bpc_leftright);
   declareProperty("BPCupdown",  m_bpc_updown);
   declareProperty("BPCudoffset",  m_bpc_udoffset);
   declareProperty("BPClroffset",  m_bpc_lroffset);

   declareProperty("BPCinvX",  m_bpc_invX);
   declareProperty("BPCinvY",  m_bpc_invY);
   m_runnumber = 0;

   declareProperty("TDCmin", m_tdccutmin);
   declareProperty("TDCmax", m_tdccutmax);
   declareProperty("TDClrcut", m_tdccutlr);
   declareProperty("TDCudcut", m_tdccutud);
   declareProperty("BPCRotation", m_rotation);
 }

TBBPCRec::~TBBPCRec()
{ }

StatusCode
TBBPCRec::initialize()
{
  ///////////////////////
  // Allocate Services //
  ///////////////////////

  // message service
  MsgStream log(messageService(),name());
  StatusCode sc;
  
  sc = service( "StoreGateSvc", m_StoreGate);
  
  if( sc.isFailure() ) {
    log << MSG::FATAL << name() 
  	<< ": Unable to locate Service StoreGateSvc" << endreq;
    return sc;
  } 
  return sc;
}

StatusCode
TBBPCRec::execute()
{

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////
  MsgStream log(messageService(),name());
  StatusCode sc;
  
  log << MSG::DEBUG << "In execute()" << endreq;

  // Get run number and get new calib constants -----------------------------
  unsigned int thisrun=0;
  EventID *thisEvent;           //EventID is a part of EventInfo
  const EventInfo* thisEventInfo;
  sc=m_StoreGate->retrieve(thisEventInfo);
  if (sc!=StatusCode::SUCCESS){
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

  // Reconstruct BPC :
  TBBPCRawCont * bpcrawCont;
  sc = m_StoreGate->retrieve(bpcrawCont, m_SGkey);
  if (sc.isFailure()){
    log << MSG::DEBUG << "TBObjectReco: Retrieval of "<<m_SGkey<<" failed" << endreq;
  }else {
    log << MSG::DEBUG << "TBBPCRec : Retrieval of "<<m_SGkey<<" succeed : cont size=" << bpcrawCont->size()<< endreq;

    TBBPCCont * bpcCont = new TBBPCCont();
    TBBPCRawCont::const_iterator it_bc   = bpcrawCont->begin();
    TBBPCRawCont::const_iterator last_bc   = bpcrawCont->end();

    // Loop over BPC
    for(;it_bc != last_bc;it_bc++){
      const TBBPCRaw * bpcraw= (*it_bc);
      std::string name = bpcraw->getDetectorName();
      unsigned int ind=0;
      // Find calibration index for this BPC
      while(ind<m_bpc_names.size()) 
	{
	  if(name==m_bpc_names[ind]) break; 
	  else ind++;
	}
      if(ind==m_bpc_names.size()){log<<MSG::ERROR<< "No calibrations for BPC" <<name<<endreq;continue;}

      // build new BPC
      TBBPC * bpc = new TBBPC(name);
      // hit position
      float xpos = m_bpc_calibX[ind]*(bpcraw->getTDCLeft()-m_bpc_leftright[ind]*bpcraw->getTDCRight()+m_bpc_lroffset[ind]);
      // PL at least true for BPC5 y = -y!
      float ypos = -(m_bpc_calibY[ind]*(bpcraw->getTDCUp()-m_bpc_updown[ind]*bpcraw->getTDCDown()+m_bpc_udoffset[ind]));
      

      // perform rotation
      // PWK 29/06/05 only if m_rotation is set

      float xposR;
      float yposR;

      if (m_rotation.size()>0) {
         xposR = xpos*cos(m_rotation[ind]) - ypos*sin(m_rotation[ind]);
         yposR = xpos*sin(m_rotation[ind]) + ypos*cos(m_rotation[ind]);
      } else {
	 xposR = xpos;
	 yposR = ypos;
      }

      log << MSG::DEBUG << "BPC " << name << " PosX  = " << xpos  << " PosY  = " <<ypos   << endreq;
      log << MSG::DEBUG << "BPC " << name << " PosXR = " << xposR << " PosYR = " << yposR << endreq;

      bpc->setXPos(xposR);
      bpc->setYPos(yposR);
   

     // pulse
      bpc->setXPulse(1.*bpcraw->getADCHorizontal());
      bpc->setYPulse(1.*bpcraw->getADCVertical());

 
      // addition September 27 2004
      // let, in addition to the existing conditions, XPosOverflow and YPosOverflow
      // be 'true' if either of the left/right or up/down TDC counts falls outside
      // the range 5 -> 2047
     
      // this way, overflows indicate undesirable conditions in general

      // must find out what causes a real overflow

      bool xcut = false;
      bool ycut = false;


      //PWK 29/06/05 set xcut, ycut true only if TDCmin, TDCmax properties set 

      if (m_tdccutmin.size()>0 && m_tdccutmax.size()>0) {       
      
	if (bpcraw->getTDCLeft()>m_tdccutmax[ind] || bpcraw->getTDCLeft()<m_tdccutmin[ind]) {xcut = true;}
	if (bpcraw->getTDCRight()>m_tdccutmax[ind] || bpcraw->getTDCRight()<m_tdccutmin[ind]) {xcut = true;}

	if (bpcraw->getTDCUp()>m_tdccutmax[ind] || bpcraw->getTDCUp()<m_tdccutmin[ind]) {ycut = true;}
	if (bpcraw->getTDCDown()>m_tdccutmax[ind] || bpcraw->getTDCDown()<m_tdccutmin[ind]) {ycut = true;}
      
      }

      if (xcut == true)
	log << MSG::INFO << "TDC left/right out of range " << endreq;
  
      if (ycut == true)
        log << MSG::INFO << "TDC up/down out of range " << endreq;
 
      //PWK 29/06/05 set xcut, ycut true only if TDClrcut, TDCudcut properties set 

      if (m_tdccutlr.size()>0) {
	if (bpcraw->getTDCLeft()+bpcraw->getTDCRight() < m_tdccutlr[ind]) 
	  {
	    xcut = true;
	    log << MSG::INFO << "TDC left+right sum below cutoff for BPC " << ind << endreq;
	  }
      }
      if (m_tdccutud.size()>0) {
	if (bpcraw->getTDCUp()+bpcraw->getTDCDown() < m_tdccutud[ind]) 
	  {
	    ycut = true;
	    log << MSG::INFO << "TDC up+down sum below cutoff for BPC " << ind << endreq;
	  }
      }

      // Overflows
      bpc->setXPosOverflow(bpcraw->isOverflow(TBBPCRaw::tdcLeft)||bpcraw->isOverflow(TBBPCRaw::tdcRight)||xcut);    
      bpc->setYPosOverflow(bpcraw->isOverflow(TBBPCRaw::tdcUp)||bpcraw->isOverflow(TBBPCRaw::tdcDown)||ycut);
      bpc->setXPulseOverflow(bpcraw->isOverflow(TBBPCRaw::adcHorizontal));
      bpc->setYPulseOverflow(bpcraw->isOverflow(TBBPCRaw::adcVertical));


      bpcCont->push_back(bpc);
    }

    sc = m_StoreGate->record(bpcCont,m_SGrecordkey);
    if ( sc.isFailure( ) ) {
      log << MSG::FATAL << "Cannot record BPCCont" << endreq;
    }
  }

  if ( sc.isFailure( ) ) {
     setFilterPassed(false);
  } else {
     setFilterPassed(true);
  }

  return StatusCode::SUCCESS;

}


StatusCode 
TBBPCRec::finalize()
{

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////


  return StatusCode::SUCCESS;
}

StatusCode TBBPCRec::getnewcalib()
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
  // bpcnumber calibX calibY leftright updown lroffset udoffset invX invY

  MsgStream log(messageService(),name());
  log << MSG::DEBUG << "Get new calibs for run " << m_runnumber<< endreq;
  
  int bpcnumber= m_bpc_names.size();

  m_bpc_calibX.clear(); m_bpc_calibX.resize(bpcnumber);
  m_bpc_calibY.clear(); m_bpc_calibY.resize(bpcnumber);
  m_bpc_leftright.clear(); m_bpc_leftright.resize(bpcnumber);
  m_bpc_updown.clear();    m_bpc_updown.resize(bpcnumber);
  m_bpc_udoffset.clear();  m_bpc_udoffset.resize(bpcnumber);
  m_bpc_lroffset.clear();  m_bpc_lroffset.resize(bpcnumber);
  m_bpc_invX.clear(); m_bpc_invX.resize(bpcnumber);
  m_bpc_invY.clear(); m_bpc_invY.resize(bpcnumber);

  int pos;

  std::ifstream calibfile;
  std::string filename = PathResolver::find_file (m_calib_filename.c_str(), "DATAPATH");
  calibfile.open(filename.c_str());
  if(!calibfile.good()){
    log << MSG::WARNING << " Problem with file named "<< m_calib_filename << " in $DATAPATH" << endreq;
    return StatusCode::FAILURE;
  } else {
     log << MSG::DEBUG << " file " << filename << " opened" << endreq;
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
      calibfile >> m_bpc_calibX[j];
      calibfile >> m_bpc_calibY[j];
      calibfile >> m_bpc_leftright[j];
      calibfile >> m_bpc_updown[j];
      calibfile >> m_bpc_lroffset[j];
      calibfile >> m_bpc_udoffset[j];
      calibfile >> m_bpc_invX[j];
      calibfile >> m_bpc_invY[j];      
      log << MSG::DEBUG << bpcn << " "<<m_bpc_calibX[j];    
      log << MSG::DEBUG << " "<<  m_bpc_calibY[j];    
      log << MSG::DEBUG << " "<< m_bpc_leftright[j]; 
      log << MSG::DEBUG << " "<< m_bpc_updown[j];    
      log << MSG::DEBUG << " "<< m_bpc_lroffset[j];  
      log << MSG::DEBUG << " "<< m_bpc_udoffset[j];  
      log << MSG::DEBUG << " "<< m_bpc_invX[j];      
      log << MSG::DEBUG << " "<< m_bpc_invY[j] << endreq;      

    }
  
  calibfile.close();
  
  return StatusCode::SUCCESS;
  
}
