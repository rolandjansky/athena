/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// PixelCalibServiceTest.cxx
// Algorithm to create Pixel Calib objects and place them in Condition DB
// Author Weiming Yao <wmyao@lbl.gov>
/////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "PixelConditionsData/PixelCalibDataColl.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelConditionsServices/IPixelCalibSvc.h" 
#include "PixelCalibServiceTest.h"
#include "GaudiKernel/IToolSvc.h"

static unsigned int rowsFGangedFEI3  =153;   // first ganged pixel row for FEI3
static unsigned int rowsFGangedFEI52  =661;   // first ganged pixel row for FEI52
static unsigned int rowsFGangedFEI51  =331;   // first ganged pixel row for FEI51

static bool isIBL(false);
static bool isITK(false);

PixelCalibServiceTest::PixelCalibServiceTest(const std::string& name, ISvcLocator* pSvcLocator)
    :AthAlgorithm   (name, pSvcLocator),
     // m_log       (msgSvc(), name),
     // m_sgSvc (0),
     //m_detStore  (0), 
     m_calibsvc("PixelCalibSvc",name),
     m_pixman(0),
     m_pixid(0),
     m_setup(0),
     par_rfile(""),
     m_dummy(false),
     par_histf(0)
{
 
  // declare algorithm parameters
  declareProperty("OutputTextFile",par_rfile);
  declareProperty("MakeDummy",m_dummy);
  for(int i =0; i<14; ++i)_myhf[i] =0;
}


PixelCalibServiceTest::~PixelCalibServiceTest(void)
{}

StatusCode PixelCalibServiceTest::initialize() {
  
  msg(MSG::INFO) << "PixelCalibServiceTest::initialize() called" << endreq;
  
  //get storegate pointers (not need for AthAlgorithm classes)
  //if ((StatusCode::SUCCESS!=service("StoreGateSvc",m_sgSvc)) ||
  //   (StatusCode::SUCCESS!=service("DetectorStore",m_detStore))) {
  //  msg(MSG::INFO) << "StoreGate services not found" << endreq;
  //  return StatusCode::FAILURE;
  // }
 
  // Get Pixel manager and ID helper
  if (StatusCode::SUCCESS!= detStore()->retrieve(m_pixman,"Pixel") || 
      m_pixman==0) {
    msg(MSG::FATAL) << "Could not find Pixel manager " << endreq;
    return StatusCode::FAILURE;
  }

  if (StatusCode::SUCCESS!= detStore()->retrieve(m_pixid,"PixelID") ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Pixel manager and helper found ");
  // check pixel geometry here:

  if(m_pixid->wafer_hash_max()>1744)isIBL = true;
  if(m_pixid->wafer_hash_max()>3000)isITK = true;

  InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax;
  itermin = m_pixman->getDetectorElementBegin();
  itermax = m_pixman->getDetectorElementEnd();
  int totpixmodule(0);
  std::ofstream* outfile=0;
  if(msgLvl(MSG::INFO)) msg() << " read PixelCalibData to text file: "
                              << par_rfile << endreq;
  outfile = new std::ofstream(par_rfile.c_str());

  for( iter=itermin; iter !=itermax; ++iter){
    const InDetDD::SiDetectorElement* element = *iter;
    if(element !=0){
      const Identifier ident = element->identify();
      if(m_pixid->is_pixel(ident)){  // OK this Element is included
        // making dump calib file for SLHC
        if(m_dummy&isIBL){
          const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
	  if(design){
	    unsigned int mchips = design->numberOfCircuits();
	    // std::cout<<"I am here "<<m_pixid->barrel_ec(ident)<<","<<m_pixid->layer_disk(ident)<<","<<m_pixid->phi_module(ident)<<","<<m_pixid->eta_module(ident)<<" mchips="<<mchips<<" dio="<<design->numberOfDiodes()<<" columnsrdo="<<design->columnsPerCircuit()<<" rowsrdo="<<design->rowsPerCircuit()<<" columns="<<design->columns()<<" rows="<<design->rows()<<std::endl;
	    if(mchips==8||abs(m_pixid->barrel_ec(ident))==2||(m_pixid->barrel_ec(ident)==0&&m_pixid->layer_disk(ident)>0))mchips *=2; // guess numberOfCircuits()
	    *outfile<<m_pixid->barrel_ec(ident)<<","<<m_pixid->layer_disk(ident)<<","<<m_pixid->phi_module(ident)<<","<<m_pixid->eta_module(ident)<<std::endl;
	    for(int ichip=0; ichip<static_cast<int>(mchips); ++ichip){
	      *outfile<<"I"<<ichip<<" "<<"4160 69 192 5090 4160 69 192 5090 4160 69 192 5090 499 -1501 294329 499 -1501 294329 0.03 0.025"<<std::endl;
	    }
	  }
        }
	++totpixmodule;
      }
    }
  }
  if(msgLvl(MSG::INFO) ) msg() <<" total Pixel module "<<totpixmodule<<endreq;
  // end of checking pixel modules 

  //get Database manager tool
  if (StatusCode::SUCCESS!=m_calibsvc.retrieve() ) {
    msg(MSG::FATAL) << "PixelCalibSvc not found" << endreq;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << " PixelCalibSvc found " << endreq;
  
  //print the options
  msg(MSG::INFO) << " Read from Pixel Conditions database into a text file: " << par_rfile<<endreq;
  if(par_rfile ==""){
    msg(MSG::ERROR) << " It's reading, Output text file is required "<<endreq; 
    return StatusCode::FAILURE; 
  }
  //
  // create a root file
  
  par_histf = new TFile("myoutput.root","RECREATE");
  _myhf[0] = new TH1F("thres","thres",200,3000.,8000.);
  _myhf[1] = new TH1F("sigmath","sigmath",100,0.,500.);
  _myhf[2] = new TH1F("noise","noise",100,0.,500.);
  _myhf[3] = new TH1F("thresin","thresin",200,3000.,8000.);
  _myhf[4] = new TH1F("thres-long","thres-long",200,3000.,8000.);
  _myhf[5] = new TH1F("sigmath-long","sigmath-long",100,0.,500.);
  _myhf[6] = new TH1F("noise-long","noise-long",100,0.,500.);
  _myhf[7] = new TH1F("thresin-long","thresin-long",200,3000.,8000.);
  _myhf[8] = new TH1F("thres-ganged","thres-ganged",200,3000.,8000.);
  _myhf[9] = new TH1F("sigmath-ganged","sigmath-ganged",100,0.,500.);
  _myhf[10] = new TH1F("noise-ganged","noise-ganged",100,0.,500.);
  _myhf[11] = new TH1F("thresin-ganged","thresin-ganged",200,3000.,8000.);
  _myhf[12] = new TH1F("tot-p1","tot-p1",100, -1.,1.);
  _myhf[13] = new TH1F("tot-p2","tot-p2",100, -1.,1.);
  
  //
  return StatusCode::SUCCESS;
}


StatusCode PixelCalibServiceTest::execute() {
  
  //StatusCode sc;
  //
  // at first event:
  // create Conditions objects in the detectorstore
  if(!m_setup){ 
    m_setup = true; 
    std::ofstream* outfile=0;
    if(msgLvl(MSG::INFO)) msg() << " read PixelCalibData to text file: "
	  << par_rfile << endreq;
    outfile = new std::ofstream(par_rfile.c_str());

    InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax;
    if(StatusCode::SUCCESS != detStore()->retrieve(m_pixman, "Pixel") || m_pixman==0){
      if(msgLvl(MSG::FATAL)) msg() << "Could not find Pixel manager "<<endreq;
      return StatusCode::FAILURE;
    }
    else{
      itermin = m_pixman->getDetectorElementBegin();
      itermax = m_pixman->getDetectorElementEnd();

      if (StatusCode::SUCCESS!= detStore()->retrieve(m_pixid,"PixelID") ){
	ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
	return StatusCode::FAILURE;
      }

    }
    int nchips = 0;
    int nobj = 0;
    bool lfst = true;
    bool lfstA = true;
    bool lfstC = true;
    for( iter=itermin; iter !=itermax; ++iter){
      const InDetDD::SiDetectorElement* element = *iter;
      if(element !=0){
	const Identifier ident = element->identify();
	if(m_pixid->is_pixel(ident)){  // OK this Element is included
	  const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
	  if(!design)continue;
	  unsigned int mchips = design->numberOfCircuits();
	  if(mchips==8||abs(m_pixid->barrel_ec(ident))==2||(m_pixid->barrel_ec(ident)==0&&m_pixid->layer_disk(ident)>0))mchips *=2; // guess numberOfCircuits() 
	  //
	    //
	  unsigned int hashID = isIBL?static_cast<unsigned int>(m_pixid->wafer_hash(ident)):( (((m_pixid->barrel_ec(ident) + 2) / 2) << 25 ) + ( m_pixid->layer_disk(ident) << 23) + ( m_pixid->phi_module(ident) << 17) + ((m_pixid->eta_module(ident) + 6) << 13));
	  if(m_dummy){
	    unsigned int dl = 0;
	    if(!isIBL){
              if(m_pixid->barrel_ec(ident)==-2&&lfstA){ // making DBM dummy -4                                                                                                                           
                lfstA = false;
                for(int i = 0; i<3; ++i){
                  for(int j = 0; j<4; ++j){
                    *outfile<<-4<<","<<i<<","<<j<<","<<0<<std::endl;
                    *outfile<<"I"<<0<<" "<<"1200 69 192 1200 1200 69 192 1200 0 0 0 0 941 -1200 800000 941 -1200 800000 0.03 0.025"<<std::endl;
                  }
                }
              }
              if(m_pixid->barrel_ec(ident)==2&&lfstC){// making DBM dummy 4                                                                                                                              
                lfstC =false;
                for(int i = 0; i<3; ++i){
                  for(int j = 0; j<4; ++j){
                    *outfile<<4<<","<<i<<","<<j<<","<<0<<std::endl;
                    *outfile<<"I"<<0<<" "<<"1200 69 192 1200 1200 69 192 1200 0 0 0 0 941 -1200 800000 941 -1200 800000 0.03 0.025"<<std::endl;
                  }
                }
              }
	      if(m_pixid->barrel_ec(ident)==0)dl =1;
	      if(m_pixid->barrel_ec(ident)==0&&m_pixid->layer_disk(ident)==0&&lfst){ // making dummy of IBL
		lfst = false;
		for(int i = 0; i<14; ++i){
		  for(int j =-10; j<10; ++j){
		    //unsigned int dummyID = (2 << 26) +(i << 18) +((j + 10) << 13);
		    int mx = 2;
		    if(j<-6||j>5)mx = 1; 
		    *outfile<<m_pixid->barrel_ec(ident)<<","<<0<<","<<i<<","<<j<<std::endl;
		    for(int ichip=0; ichip<mx; ++ichip){
		      //*outfile<<"I"<<ichip<<" "<<"4160 69 192 5090 4310 165 300 5330 0 0 0 0 499 -1501 294329 499 -1501 294329 0.03 0.025"<<std::endl;
		      //*outfile<<"I"<<ichip<<" "<<"1500 69 192 1500 1500 69 192 1500 0 0 0 0 883 -1500 1600000 883 -1500 1600000 0.03 0.025"<<std::endl;
		      *outfile<<"I"<<ichip<<" "<<"2550 75 120 2550 2550 75 135 2550 0 0 0 0 18 -1445 10000 18 -1445 10000 0.25 0.000025"<<std::endl; // turned from M7 cosmic
		    }
		  }
		}
	      }
	      if(m_calibsvc->hasCalibData(ident)){
		if(nobj%100==0 && msgLvl(MSG::INFO) ) msg()           <<"ith Module:"<<nobj<<"with Identifier:"<<hashID<<endreq;
		++nobj;
		*outfile<<m_pixid->barrel_ec(ident)<<","<<m_pixid->layer_disk(ident)+dl<<","<<m_pixid->phi_module(ident)<<","<<m_pixid->eta_module(ident)<<std::endl;
		for(int ichip=0; ichip<static_cast<int>(mchips); ++ichip){
		++nchips;
		*outfile<<"I"<<ichip<<" "<<m_calibsvc->getThreshold(ident,0,1,ichip)<<" "<<
		  m_calibsvc->getThresholdSigma(ident,0,1,ichip)<<" "<<
		  m_calibsvc->getNoise(ident,0,1,ichip)<<" "<<
		  m_calibsvc->getTimeWalk(ident,0,1,ichip)<<" "<<
		  m_calibsvc->getThreshold(ident,0,0,ichip)<<" "<<
		  m_calibsvc->getThresholdSigma(ident,0,0,ichip)<<" "<<
		  m_calibsvc->getNoise(ident,0,0,ichip)<<" "<<
		  m_calibsvc->getTimeWalk(ident,0,0,ichip)<<" "<<
		  m_calibsvc->getThreshold(ident,153,0,ichip)<<" "<<
		  m_calibsvc->getThresholdSigma(ident,153,0,ichip)<<" "<<
		  m_calibsvc->getNoise(ident,153,0,ichip)<<" "<<
		  m_calibsvc->getTimeWalk(ident,153,0,ichip)<<" "<<
		  m_calibsvc->getQ2TotA(ident,0,1,ichip)<<" "<<
		  m_calibsvc->getQ2TotE(ident,0,1,ichip)<<" "<<
		  m_calibsvc->getQ2TotC(ident,0,1,ichip)<<" "<<
		  m_calibsvc->getQ2TotA(ident,153,0,ichip)<<" "<<
		  m_calibsvc->getQ2TotE(ident,153,0,ichip)<<" "<<
		  m_calibsvc->getQ2TotC(ident,153,0,ichip)<<" "<<
		  m_calibsvc->getTotP1(ident,ichip)<<" "<<
		  m_calibsvc->getTotP2(ident,ichip)<<" "<<std::endl;
		}
	      }
	    }
	  }
	  else{ // normal dump 	      
	    if(m_calibsvc->hasCalibData(ident)){
	      if(nobj%100==0 && msgLvl(MSG::INFO) ) msg()           <<"ith Module:"<<nobj<<"with Identifier:"<<hashID<<endreq;
	      ++nobj;
	      int rowsFGangedFE = rowsFGangedFEI3;
	      if(isITK){
		rowsFGangedFE = rowsFGangedFEI51;
		if(m_pixid->barrel_ec(ident)==0&&m_pixid->layer_disk(ident)==1)rowsFGangedFE = rowsFGangedFEI52;
	      }
	      *outfile<<hashID<<std::endl;
	      int mchipx = m_calibsvc->getNFE(ident);
	      if(mchipx>2){
		for(int ichip=0; ichip<mchipx; ++ichip){
		  ++nchips;		
		  _myhf[0]->Fill(m_calibsvc->getThreshold(ident,0,1,ichip));
		  _myhf[1]->Fill(m_calibsvc->getThresholdSigma(ident,0,1,ichip));
		  _myhf[2]->Fill(m_calibsvc->getNoise(ident,0,1,ichip));
		  _myhf[3]->Fill(m_calibsvc->getTimeWalk(ident,0,1,ichip));
		  _myhf[4]->Fill(m_calibsvc->getThreshold(ident,0,0,ichip));
		  _myhf[5]->Fill(m_calibsvc->getThresholdSigma(ident,0,0,ichip));
		  _myhf[6]->Fill(m_calibsvc->getNoise(ident,0,0,ichip));
		  _myhf[7]->Fill(m_calibsvc->getTimeWalk(ident,0,0,ichip));
		  _myhf[8]->Fill(m_calibsvc->getThreshold(ident,rowsFGangedFE,0,ichip));
		  _myhf[9]->Fill(m_calibsvc->getThresholdSigma(ident,rowsFGangedFE,0,ichip));
		  _myhf[10]->Fill(m_calibsvc->getNoise(ident,rowsFGangedFE,0,ichip));
		  _myhf[11]->Fill(m_calibsvc->getTimeWalk(ident,rowsFGangedFE,0,ichip));
		  _myhf[12]->Fill(m_calibsvc->getTotP1(ident,ichip));
		  _myhf[13]->Fill(m_calibsvc->getTotP2(ident,ichip));
		  // need to be prcise about the type of pixel: 
		  *outfile<<"I"<<ichip<<" "<<m_calibsvc->getThreshold(ident,0,1,ichip)<<" "<<
		    m_calibsvc->getThresholdSigma(ident,0,1,ichip)<<" "<<
		    m_calibsvc->getNoise(ident,0,1,ichip)<<" "<<
		    m_calibsvc->getTimeWalk(ident,0,1,ichip)<<" "<<
		    m_calibsvc->getThreshold(ident,0,0,ichip)<<" "<<
		    m_calibsvc->getThresholdSigma(ident,0,0,ichip)<<" "<<
		    m_calibsvc->getNoise(ident,0,0,ichip)<<" "<<
		    m_calibsvc->getTimeWalk(ident,0,0,ichip)<<" "<<
		    m_calibsvc->getThreshold(ident,rowsFGangedFE,0,ichip)<<" "<<
		    m_calibsvc->getThresholdSigma(ident,rowsFGangedFE,0,ichip)<<" "<<
		    m_calibsvc->getNoise(ident,rowsFGangedFE,0,ichip)<<" "<<
		    m_calibsvc->getTimeWalk(ident,rowsFGangedFE,0,ichip)<<" "<<
		    m_calibsvc->getQ2TotA(ident,0,1,ichip)<<" "<<
		    m_calibsvc->getQ2TotE(ident,0,1,ichip)<<" "<<
		    m_calibsvc->getQ2TotC(ident,0,1,ichip)<<" "<<
		    m_calibsvc->getQ2TotA(ident,rowsFGangedFE,0,ichip)<<" "<<
		    m_calibsvc->getQ2TotE(ident,rowsFGangedFE,0,ichip)<<" "<<
		    m_calibsvc->getQ2TotC(ident,rowsFGangedFE,0,ichip)<<" "<<
		    m_calibsvc->getTotP1(ident,ichip)<<" "<<
		    m_calibsvc->getTotP2(ident,ichip)<<" "<<std::endl;
		}
	      }
	      else{ // FEI4 chips 
		for(int ichip=0; ichip<mchipx; ++ichip){
		  ++nchips;
		  if(mchipx>1){
		    *outfile<<"I"<<ichip<<" "<<m_calibsvc->getThreshold(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getThresholdSigma(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getNoise(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getTimeWalk(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getThreshold(ident,0,0,ichip)<<" "<<
		      m_calibsvc->getThresholdSigma(ident,0,0,ichip)<<" "<<
		      m_calibsvc->getNoise(ident,0,0,ichip)<<" "<<
		      m_calibsvc->getTimeWalk(ident,0,0,ichip)<<" "<<
		      0<<" "<<0<<" "<<0<<" "<<0<<" "<<
		      m_calibsvc->getQ2TotA(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getQ2TotE(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getQ2TotC(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getQ2TotA(ident,0,0,ichip)<<" "<<
		      m_calibsvc->getQ2TotE(ident,0,0,ichip)<<" "<<
		      m_calibsvc->getQ2TotC(ident,0,0,ichip)<<" "<<
		      m_calibsvc->getTotP1(ident,ichip)<<" "<<
		      m_calibsvc->getTotP2(ident,ichip)<<" "<<std::endl;
		  }
		  else{
		    *outfile<<"I"<<ichip<<" "<<m_calibsvc->getThreshold(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getThresholdSigma(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getNoise(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getTimeWalk(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getThreshold(ident,0,0,ichip)<<" "<<
		      m_calibsvc->getThresholdSigma(ident,0,0,ichip)<<" "<<
		      m_calibsvc->getNoise(ident,0,0,ichip)<<" "<<
		      m_calibsvc->getTimeWalk(ident,0,0,ichip)<<" "<<
		      0<<" "<<0<<" "<<0<<" "<<0<<" "<<
		      m_calibsvc->getQ2TotA(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getQ2TotE(ident,1,1,ichip)<<" "<<
		      m_calibsvc->getQ2TotC(ident,1,1,ichip)<<" "<<
		      0<<" "<<0<<" "<<0<<" "<<
		      m_calibsvc->getTotP1(ident,ichip)<<" "<<
		      m_calibsvc->getTotP2(ident,ichip)<<" "<<std::endl;
		  }		  
		}
	      }
	    }
	  }
	}
      }
    }
    outfile->close();
    delete outfile;
    if( msgLvl(MSG::INFO)  ) msg() << "Written "<< nobj <<" PixelCalibData objects" <<
			       " with " << nchips << " chips to text file "<<endreq;
  }
  if( msgLvl(MSG::INFO)  ) msg()  <<" Event execute "<<endreq; 
  //
  for(int i = 0; i<14; ++i)_myhf[i]->Write();
  par_histf->Close();
  //
  return StatusCode::SUCCESS;
}
 
StatusCode PixelCalibServiceTest::finalize() 
{  
  msg(MSG::INFO)<<" PixelCalibServiceTest: finishing "<<endreq; 
  return StatusCode::SUCCESS;
}
