/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class FixLArElecCalib
   @author Hong Ma <hma@bnl.gov>
   @brief fix electronic calibration constants for MC.

   Purpose:  read in the LAr electronic calib data, and fix them 
             ready to be written out again as a new version.

   This should be used with FixLArElecCalib_jobOptions.py to write a new
   POOL file.

   Implemented fixes:
   
   1. Flag = 1
	Replacing values for barrel in LArDAC2uAMC
	Input barrel_dac2ua_oct06.txt
 
   2. Flag=2
        fix FCAL conditions data for new mapping in release 13.

   3. Flag=3
        replace fsampl for 4.8.2
        June 13,2007: FCAL numbers updated by P. Krieger.
	Junly 10, 2007: updated the number for 13.0.20

   4. Flag=4
        replace uA2MeV DAC2uA with that in dac2ua_ua2mev_rel13.txt

   5. Flag=5
        replace FCAL noise and minbiasRMS from Sven Menke's new number 
        for release 13.

   6  Flag=6
        replace All FCAL noise and ADC2DAC, uA2MeV and DAC2uA.
	<HG/MG>   =  9.96 +/- 0.01
	<MG/LG>    =  9.67 +/- 0.01

   7  Flag=7
        replace sampling fraction for QGSP_BERT from 13.0.30. 

   8  Flag=8
        replace EM/HEC autoCorr, noise, dac2ua, mphys, ramp, shape, ua2mev

   9  Flag=9
        fix FCAL AutoCorr

   10  Flag=10
        fix FCAL ramp.  The scaling factor were inverted. 
	Ramp_M = Ramp_H*9.96 
	Ramp_L = Ramp_H*9.96*9.67 

   11  Flag=11
        Update HEC Ramp 
**/ 


#include "LArConditionsTest/FixLArElecCalib.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "LArTools/LArCablingService.h" 
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "LArRawConditions/LArDAC2uAMC.h"
#include "LArRawConditions/LArShape32MC.h"
#include "LArRawConditions/LAruA2MeVMC.h"
#include "LArRawConditions/LArfSamplMC.h"
#include "LArRawConditions/LArMinBiasMC.h"
#include "LArRawConditions/LArNoiseMC.h"
#include "LArRawConditions/LArRampMC.h"
#include "LArRawConditions/LArAutoCorrMC.h"

#include "CaloTriggerTool/ICaloSuperCellIDTool.h"

#include <fstream>


#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

FixLArElecCalib::FixLArElecCalib(const std::string& name, ISvcLocator* pSvcLocator) : 
  Algorithm(name,pSvcLocator),
  m_fixFlag(0),
  m_detStore(0),
  m_em_idhelper(0),
  m_hec_idhelper(0),
  m_fcal_idhelper(0),
  m_online_idhelper(0),
  m_sem_idhelper(0),
  m_shec_idhelper(0),
  m_sfcal_idhelper(0),
  m_sonline_idhelper(0),
  m_scell_idhelper(0),
  m_cablingSvc(0)
{ 

    declareProperty("FixFlag",      m_fixFlag);
    declareProperty("G4Phys",       m_g4Phys);

}

FixLArElecCalib::~FixLArElecCalib()
{ }

StatusCode FixLArElecCalib::initialize() {
    MsgStream  log(messageService(),name());
  log << MSG::INFO << " in initialize " <<endreq;


  
  StatusCode sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
      log << MSG::ERROR
	  << "Unable to retrieve pointer to DetectorStore "
	  << endreq;
          return sc;
    }
  log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;

  sc=m_detStore->retrieve(m_em_idhelper) ;
  if(sc!=StatusCode::SUCCESS){
       log<< MSG::ERROR<<" Can not find LArEM_ID " <<endreq;
       return sc;
   }

  sc=m_detStore->retrieve(m_hec_idhelper) ;
  if(sc!=StatusCode::SUCCESS){
       log<< MSG::ERROR<<" Can not find LArHEC_ID " <<endreq;
       return sc;
   }

  sc=m_detStore->retrieve(m_fcal_idhelper) ;
  if(sc!=StatusCode::SUCCESS){
       log<< MSG::ERROR<<" Can not find LArFCAL_ID " <<endreq;
       return sc;
   }

  sc=m_detStore->retrieve(m_sem_idhelper) ;
  if(sc!=StatusCode::SUCCESS){
       log<< MSG::ERROR<<" Can not find LArEM_SuperCell_ID " <<endreq;
       return sc;
   }

  sc=m_detStore->retrieve(m_shec_idhelper) ;
  if(sc!=StatusCode::SUCCESS){
       log<< MSG::ERROR<<" Can not find LArHEC_SuperCell_ID " <<endreq;
       return sc;
   }

  sc=m_detStore->retrieve(m_sfcal_idhelper) ;
  if(sc!=StatusCode::SUCCESS){
       log<< MSG::ERROR<<" Can not find LArFCAL_SuperCell_ID " <<endreq;
       return sc;
   }

  sc=m_detStore->retrieve(m_online_idhelper) ;
  if(sc!=StatusCode::SUCCESS){
      log<< MSG::ERROR<<" Can not find LArOnlineID " <<endreq;
      return sc;
  }

  sc=m_detStore->retrieve(m_sonline_idhelper) ;
  if(sc!=StatusCode::SUCCESS){
      log<< MSG::ERROR<<" Can not find LArOnline_SuperCellID " <<endreq;
      return sc;
  }

  IToolSvc* toolSvc;
  if( service("ToolSvc", toolSvc) != StatusCode::SUCCESS){
           log<< MSG::ERROR <<" Failed to get ToolSvc" <<endreq;
           return StatusCode::FAILURE ;
    }

  sc=m_detStore->retrieve(m_scell_idhelper) ;
  if(sc!=StatusCode::SUCCESS){
       log<< MSG::ERROR<<" Can not find CaloCell_SuperCell_ID " <<endreq;
       return sc;
   }

  if(StatusCode::SUCCESS != toolSvc->retrieveTool("LArCablingService",m_cablingSvc) ) {
         log<< MSG::ERROR <<" Failed to get LArCablingService" <<endreq;
         return StatusCode::FAILURE ;
   }

  return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::execute() {

    MsgStream  log(messageService(),name());
  log << MSG::INFO << " in execute  " <<endreq;
    if(m_fixFlag==13)
        return fix13();
  return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::finalize() {

    MsgStream  log(messageService(),name());
   log << MSG::INFO << " in finalize " <<endreq;

    if(m_fixFlag==1) 
	return fix1(); 
    if(m_fixFlag==2) 
	return fix2(); 
    if(m_fixFlag==3) 
	return fix3(); 
    if(m_fixFlag==4) 
	return fix4(); 
    if(m_fixFlag==5) 
	return fix5(); 
    if(m_fixFlag==6) 
	return fix6(); 
    if(m_fixFlag==7) 
	return fix7(); 
    if(m_fixFlag==8) 
	return fix8(); 
    if(m_fixFlag==9)
	return fix9(); 
    if(m_fixFlag==10)
	return fix10(); 
    if(m_fixFlag==11)
	return fix11(); 

    return StatusCode::SUCCESS; 

}

StatusCode FixLArElecCalib::fix1() {


    MsgStream  log(messageService(),name());

   log << MSG::INFO << " in fix1() " <<endreq;


    // Fix1 is for updating the EM DAC2uA, assuming symmetry. 
    // Input should be MC Conditions data with DetDescrVersion=ATLAS-DC3-05

    // Pointer to StoreGate 
    StoreGateSvc * detStore; 
    StatusCode sc = service("DetectorStore", detStore);
    if (sc.isFailure()) {
	log << MSG::ERROR
	    << "Unable to retrieve pointer to DetectorStore "
	    << endreq;
	return sc;
    }
    log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;


   const LArEM_ID* em_idhelper; 
   sc=detStore->retrieve(em_idhelper) ; 
   if(sc!=StatusCode::SUCCESS){ 
     log<< MSG::ERROR<<" Can not find LArEM_ID " <<endreq;
     return sc;
   }

   const LArOnlineID* online_idhelper; 
   sc=detStore->retrieve(online_idhelper) ; 
   if(sc!=StatusCode::SUCCESS){ 
     log<< MSG::ERROR<<" Can not find LArOnlineID " <<endreq;
     return sc;
   }


    // retrieve DetDescrManager and LArCablingService
   IToolSvc* toolSvc;
   if( service("ToolSvc", toolSvc) != StatusCode::SUCCESS){
	log<< MSG::ERROR <<" Failed to get ToolSvc" <<endreq;     
	return StatusCode::FAILURE ; 
    }

   LArCablingService* cablingSvc ; 
   if(StatusCode::SUCCESS != toolSvc->retrieveTool("LArCablingService",cablingSvc) ) {
	log<< MSG::ERROR <<" Failed to get LArCablingService" <<endreq;     
	return StatusCode::FAILURE ; 
    }


   const LArDAC2uAMC * dac2ua_c; 
   sc=detStore->retrieve(dac2ua_c) ; 
   if(sc!=StatusCode::SUCCESS){ 
     log<< MSG::ERROR<<" Can not find DAC2uA" <<endreq;
  	return sc;
   }

   LArDAC2uAMC* dac2ua = const_cast<LArDAC2uAMC*>(dac2ua_c);

   // read in the file

   std::ifstream infile("barrel_dac2ua_oct06.txt") ; 

   int   det,samp,reg,eta; 
   float value ; 
   int n = 0; 
   while ( infile>>det>>samp>>reg>>eta>>value )
    {
	++n; 
	log<<MSG::DEBUG<<" det,samp,reg,eta,value="
	<<det<<" " 
	<<samp<<" " 
	<<reg<<" " 
	<<eta<<" " 
	<<value<<" " <<endreq; 

	Identifier id ; 
	if ( det==1 && samp==1 && reg==0 && eta==0 ){
	   // eta=0 for strip, not connected, but keep it
	   log<<MSG::DEBUG<<" disconnected strip "<<endreq;
 	 id = em_idhelper->disc_channel_id(det,samp,reg,eta,0); 
        }else
 	 id = em_idhelper->channel_id(det,samp,reg,eta,0); 

	HWIdentifier hid = cablingSvc->createSignalChannelID(id);
	const LArDAC2uAComplete::LArCondObj & t = dac2ua->get(hid,0); 
	std::string id_str = online_idhelper->print_to_string(hid); 
	log<<MSG::DEBUG<<" online id = "<<id_str<<endreq; 

	if( t.isEmpty() ) 
	{
	  log<<MSG::ERROR<<" No existing conditions data " <<endreq;
	  // return StatusCode::FAILURE ;
	  continue ; 
	}
	log<<MSG::DEBUG<<" Old DAC2uA = "<< t.m_data<< " " <<endreq; 

	LArDAC2uAComplete::LArCondObj& t2 = const_cast<LArDAC2uAComplete::LArCondObj&>(t); 
	t2.m_data = value ; 

	const LArDAC2uAComplete::LArCondObj & t3 = dac2ua->get(hid,0); 
	log<<MSG::DEBUG<<" New DAC2uA = "<< t3.m_data
<< " " <<endreq; 

    }

   log<<MSG::DEBUG<<"  Number of entries changes =  " <<n <<endreq; 

   return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix2() {


    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " in fix2() " <<endreq;


    // Fix2 is for updating the FCAL conditions data after IdFix7
    // Input should be MC Conditions data with DetDescrVersion=ATLAS-DC3-05

    // Pointer to StoreGate 
    StoreGateSvc * detStore; 
    StatusCode sc = service("DetectorStore", detStore);
    if (sc.isFailure()) {
	log << MSG::ERROR
	    << "Unable to retrieve pointer to DetectorStore "
	    << endreq;
	return sc;
    }
    log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;

    const LArFCAL_ID* fcal_idhelper; 
    sc=detStore->retrieve(fcal_idhelper) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find LArFCAL_ID " <<endreq;
      return sc;
    }
    
    
    const LArOnlineID* online_idhelper; 
    sc=detStore->retrieve(online_idhelper) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find LArOnlineID " <<endreq;
      return sc;
    }
    
    
    // retrieve DetDescrManager and LArCablingService
    IToolSvc* toolSvc;
    if( service("ToolSvc", toolSvc) != StatusCode::SUCCESS){
      log<< MSG::ERROR <<" Failed to get ToolSvc" <<endreq;     
      return StatusCode::FAILURE ; 
    }
    
    LArCablingService* cablingSvc ; 
    if(StatusCode::SUCCESS != toolSvc->retrieveTool("LArCablingService",cablingSvc) ) {
      log<< MSG::ERROR <<" Failed to get LArCablingService" <<endreq;     
      return StatusCode::FAILURE ; 
    }
    
    
    const LArDAC2uAMC * dac2ua_c; 
    sc=detStore->retrieve(dac2ua_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find DAC2uA" <<endreq;
      return sc;
    }
    LArDAC2uAMC* dac2ua = const_cast<LArDAC2uAMC*>(dac2ua_c);
    
    
    const LAruA2MeVMC * ua2mev_c; 
    sc=detStore->retrieve(ua2mev_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find uA2MeV" <<endreq;
      return sc;
    }
    LAruA2MeVMC* ua2mev = const_cast<LAruA2MeVMC*>(ua2mev_c);
    
    const LArRampMC * ramp_c; 
    sc=detStore->retrieve(ramp_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find Ramp" <<endreq;
      return sc;
    }
    LArRampMC* ramp = const_cast<LArRampMC*>(ramp_c);
    
    const LArShape32MC * shape_c; 
    sc=detStore->retrieve(shape_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find Shape" <<endreq;
      return sc;
    }
    LArShape32MC* shape = const_cast<LArShape32MC*>(shape_c);
    
    const LArNoiseMC * noise_c; 
    sc=detStore->retrieve(noise_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find Noise" <<endreq;
      return sc;
    }
    LArNoiseMC* noise = const_cast<LArNoiseMC*>(noise_c);
    
    const LArfSamplMC * fsampl_c; 
    sc=detStore->retrieve(fsampl_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find fSampl" <<endreq;
      return sc;
    }
    LArfSamplMC* fsampl = const_cast<LArfSamplMC*>(fsampl_c);
    
    const LArMinBiasMC * minbias_c; 
    sc=detStore->retrieve(minbias_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find MinBias" <<endreq;
      return sc;
    }
    LArMinBiasMC* minbias = const_cast<LArMinBiasMC*>(minbias_c);
    
    const LArAutoCorrMC * ac_c; 
    sc=detStore->retrieve(ac_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find AutoCorr" <<endreq;
      return sc;
    }
    LArAutoCorrMC* ac = const_cast<LArAutoCorrMC*>(ac_c);
    
    
    for( unsigned int gain=0;gain<3;++gain)
      {
	// loop over all the old data
	LArRampMC::ConstConditionsMapIterator  it = ramp->begin(gain);
	LArRampMC::ConstConditionsMapIterator  it_e = ramp->end(gain);
	
	int n=0;
	for (; it!=it_e;++it)
	  {
	    HWIdentifier hid = it.channelId(); 
	    if( (*it).isEmpty() ) continue;  
	    if(cablingSvc->isOnlineConnected(hid)){
	      continue;
	    }else
	      {
		log<<MSG::DEBUG<<" unconnected channel" 
		   << online_idhelper->print_to_string(hid) <<endreq;
		ac->get(hid,gain) = LArAutoCorrComplete::LArCondObj(); 
		ramp->get(hid,gain) = LArRampComplete::LArCondObj(); 
		shape->get(hid,gain) = LArShape32MC::LArCondObj(); 
		noise->get(hid,gain) = LArNoiseComplete::LArCondObj(); 
		if(gain==0){
		  dac2ua->get(hid,gain) = LArDAC2uAComplete::LArCondObj(); 
		  ua2mev->get(hid,gain) = LAruA2MeVComplete::LArCondObj(); 
		  fsampl->get(hid,gain) = LArfSamplComplete::LArCondObj(); 
		  minbias->get(hid,gain) = LArMinBiasComplete::LArCondObj(); 
		}
		++n ; 
	      }
	    
	  }
	    log<<MSG::DEBUG<<"  Gain="<<gain<<
	      " Number of entries removed =  " <<n <<endreq; 

      }
    

    HWIdentifier hid_mod[3]; 
    hid_mod[0] = online_idhelper->channel_Id(1,1,6,1,33); 
    hid_mod[1] = online_idhelper->channel_Id(1,1,6,10,12); 
    hid_mod[2] = online_idhelper->channel_Id(1,1,6,14,12); 

    // loop over all FCAL channels 
    std::vector<Identifier>::const_iterator it= fcal_idhelper->fcal_begin();
    std::vector<Identifier>::const_iterator it_e= fcal_idhelper->fcal_end();
    int n = 0; 
    for(;it!=it_e; ++it)
	{
	  Identifier id = *it; 
	  int pn = fcal_idhelper->pos_neg(id);
	  if(pn!=2) continue; 
	  int phi = fcal_idhelper->phi(id);
	  if (phi>7) continue; 

	  int module = fcal_idhelper->module(id)-1;

	  HWIdentifier hid = cablingSvc->createSignalChannelID(id);

	  for(unsigned int gain = 0;gain<3;++gain){
	    
		LArRampComplete::LArCondObj& rampP = ramp->get(hid,gain) ;
		if (!rampP.isEmpty()) continue ;

		log<<MSG::DEBUG<<" channel needs repair " 
		   << online_idhelper->print_to_string(hid) <<endreq;
		log<<MSG::DEBUG << fcal_idhelper->print_to_string(id) <<endreq;
		log<<MSG::DEBUG << "module = "<<module <<endreq;

		++n;

		LArRampComplete::LArCondObj ramp_std = ramp->get(hid_mod[module],gain) ;
		//rampP.m_vRamp = ramp_std.m_vRamp;
		ramp->setPdata(hid,ramp_std,gain); 

		//LArAutoCorrComplete::LArCondObj& acP = ac->get(hid,gain) ;
		LArAutoCorrComplete::LArCondObj ac_std = ac->get(hid_mod[module],gain) ;
		//acP.m_vAutoCorr = ac_std.m_vAutoCorr;
		ac->setPdata(hid,ac_std,gain); 

		//LArShapeP& shapeP = shape->get(hid,gain) ;
		LArShape32MC::LArCondObj shape_std = shape->get(hid_mod[module],gain) ;
		//shapeP.m_vShape = shape_std.m_vShape;
		//shapeP.m_vShapeDer = shape_std.m_vShapeDer;
		shape->setPdata(hid,shape_std,gain); 

		//LArNoiseComplete::LArCondObj& noiseP = noise->get(hid,gain) ;
		LArNoiseComplete::LArCondObj noise_std = noise->get(hid_mod[module],gain) ;
		// noiseP.m_Noise = noise_std.m_Noise;
		noise->setPdata(hid,noise_std,gain); 

		if(gain==0){

	 	 //LAruA2MeVComplete::LArCondObj& ua2mevP = ua2mev->get(hid,gain) ;
		 LAruA2MeVComplete::LArCondObj ua2mev_std = ua2mev->get(hid_mod[module],gain) ;
		 //ua2mevP.m_uA2MeV = ua2mev_std.m_uA2MeV;
	 	 ua2mev->setPdata(hid,ua2mev_std,gain); 

		 //LArDAC2uAComplete::LArCondObj& dac2uaP = dac2ua->get(hid,gain) ;
		 LArDAC2uAComplete::LArCondObj dac2ua_std = dac2ua->get(hid_mod[module],gain) ;
		 //dac2uaP.m_DAC2uA = dac2ua_std.m_DAC2uA;
	 	 dac2ua->setPdata(hid,dac2ua_std,gain); 

		 //LArfSamplComplete::LArCondObj& fsamplP = fsampl->get(hid,gain) ;
		 LArfSamplComplete::LArCondObj fsampl_std = fsampl->get(hid_mod[module],gain) ;
		 //fsamplP.m_fSampl = fsampl_std.m_fSampl;
	  	 fsampl->setPdata(hid,fsampl_std,gain); 


		 //LArMinBiasComplete::LArCondObj& minbiasP = minbias->get(hid,gain) ;
		 LArMinBiasComplete::LArCondObj minbias_std = minbias->get(hid_mod[module],gain) ;
		 // minbiasP.m_MinBiasRMS = minbias_std.m_MinBiasRMS;
	 	 minbias->setPdata(hid,minbias_std,gain); 

		}

	}
    }
	
    log<<MSG::DEBUG<< " Number of entries fixed " <<n <<endreq; 
    return StatusCode::SUCCESS;    
}

StatusCode FixLArElecCalib::fix3() {


    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " in fix3() " <<endreq;
    std::string filename ;

    // updated for 13.0.20
    if(m_g4Phys=="QGSP_EMV"){
      filename = "fsampl_1302_qgsp-emv.txt";
    } 
    else
      if( m_g4Phys=="QGSP_BERT"){
	filename = "fsampl_1303_qgsp-bert.txt";
      }
      else {
	log << MSG::ERROR << " wrong option  "<<m_g4Phys <<endreq;
	return StatusCode::FAILURE; 
      }

    // Fix3 is for updating the FCAL conditions data after IdFix7
    // Input should be MC Conditions data with DetDescrVersion=ATLAS-DC3-05

    StatusCode sc = updateEMfSampl(filename ); 
    if(! sc.isSuccess()) return sc;

    sc = updateHADfSampl();
    if(! sc.isSuccess()) return sc;


    return StatusCode::SUCCESS;


}
StatusCode FixLArElecCalib::fix4() {


    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " in fix4() " <<endreq;

    StatusCode sc = updateEM_DACuAMeV( "dac2ua_ua2mev_rel13.txt" ); 
    return sc;

}

StatusCode FixLArElecCalib::updateHADfSampl() {

  MsgStream  log(messageService(),name());

  float fsampl_fcal1,fsampl_fcal2,fsampl_fcal3; 
  float fsampl_hec1,fsampl_hec2;

  if(m_g4Phys=="QGSP_EMV"){
      fsampl_hec1=0.04355;
      fsampl_hec2=0.021775;


      // numbers for G4.8.3
      fsampl_fcal1=0.01366;
      fsampl_fcal2=0.00854;
      fsampl_fcal3=0.01107; 
      // numbers for G4.8.2
      //fsampl_fcal1=0.01412;
      //fsampl_fcal2=0.00894;
      //fsampl_fcal3=0.01149; 
  } 
  else
    if( m_g4Phys=="QGSP_BERT"){
      fsampl_hec1=0.0447; 
      fsampl_hec2=0.02235;

      // for G4.8.3
      fsampl_fcal1=0.01591;
      fsampl_fcal2=0.01144;
      fsampl_fcal3=0.01405; 

      // for G4.8.2
      //fsampl_fcal1=0.01512 ;
      //fsampl_fcal2=0.01086;
      //fsampl_fcal3=0.01352; 
    }
    else {
      log << MSG::ERROR << " wrong option  "<<m_g4Phys <<endreq;
	return StatusCode::FAILURE; 
    }


  const LArfSamplMC * fsampl_c;
  StatusCode sc=m_detStore->retrieve(fsampl_c) ;
  if(sc!=StatusCode::SUCCESS){
    log<< MSG::ERROR<<" Can not find fSampl" <<endreq;
    return sc;
  }

  LArfSamplMC* fsampl = const_cast<LArfSamplMC*>(fsampl_c);

  // loop over all the old data
  LArfSamplMC::ConstConditionsMapIterator  it = fsampl->begin(0);
  LArfSamplMC::ConstConditionsMapIterator  it_e = fsampl->end(0);
  int n_hec=0;
  int n_fcal=0;

  for (; it!=it_e;++it){
    
    HWIdentifier hid = it.channelId(); 
    if( (*it).isEmpty() ) continue;  
    if(!m_cablingSvc->isOnlineConnected(hid)){
	      continue;
    }

    LArfSamplComplete::LArCondObj& t2 = const_cast<LArfSamplComplete::LArCondObj&>(*it);

    Identifier id = m_cablingSvc->cnvToIdentifier(hid);


    if(m_hec_idhelper->is_lar_hec(id)){
      // 
      log<<MSG::INFO<<" HEC Old fsampl = "<< t2.m_fSampl<< " " <<endreq; 
      int sam = m_hec_idhelper->sampling(id); 

      if(sam<=1){
	t2.m_fSampl = fsampl_hec1;
      }else
	{
	  t2.m_fSampl = fsampl_hec2;
	}
      
      const LArfSamplComplete::LArCondObj & t3 = fsampl->get(hid,0); 
      log<<MSG::INFO<<" New fSampl = "<< t3.m_fSampl<< " " <<endreq; 

      ++n_hec;
    }
    if(m_fcal_idhelper->is_lar_fcal(id)){
      // 
      log<<MSG::INFO<<" FCAL Old fsampl = "<< t2.m_fSampl<< " " <<endreq; 
      int sam = m_fcal_idhelper->module(id); 

      if(sam==1){
	t2.m_fSampl = fsampl_fcal1;
      }else
      if(sam==2){
	t2.m_fSampl = fsampl_fcal2;
      }else
      {
	  t2.m_fSampl = fsampl_fcal3;
      }
      
      const LArfSamplComplete::LArCondObj & t3 = fsampl->get(hid,0); 
      log<<MSG::INFO<<" New fSampl = "<< t3.m_fSampl<< " " <<endreq; 

      ++n_fcal;
    }

  }

  log<<MSG::INFO<<" number of hec cells "<<n_hec<<endreq;
  log<<MSG::INFO<<" number of fcal cells "<<n_fcal<<endreq;


  return StatusCode::SUCCESS;
}
StatusCode FixLArElecCalib::updateEMfSampl(const std::string& filename) {

    MsgStream  log(messageService(),name());

   log << MSG::INFO << " in updateEMfSampl() " <<endreq;


    // this method updates the EM fSample, assuming symmetry. 
    // input is the text file.


   const LArfSamplMC * fsampl_c; 
   StatusCode sc=m_detStore->retrieve(fsampl_c) ; 
   if(sc!=StatusCode::SUCCESS){ 
     log<< MSG::ERROR<<" Can not find fSampl" <<endreq;
  	return sc;
   }

   LArfSamplMC* fsampl = const_cast<LArfSamplMC*>(fsampl_c);

   // read in the file

   log<<MSG::INFO<<" opening file "<< filename << endreq;
   std::ifstream infile( filename.c_str() );
   if(! infile.good() ) 
     {
       log<<MSG::ERROR<<" fail to open file "<<endreq;
       return StatusCode::FAILURE; 
     }
   int   det,samp,reg,eta; 
   float value ; 
   int n = 0; 
   while ( infile>>det>>samp>>reg>>eta>>value )
    {
	++n; 
	log<<MSG::INFO<<" det,samp,reg,eta,value="
	<<det<<" " 
	<<samp<<" " 
	<<reg<<" " 
	<<eta<<" " 
	<<value<<" " <<endreq; 

	Identifier id ; 
	if ( det==1 && samp==1 && reg==0 && eta==0 ){
	   // eta=0 for strip, not connected, but keep it
	   log<<MSG::INFO<<" disconnected strip "<<endreq;
 	 id = m_em_idhelper->disc_channel_id(det,samp,reg,eta,0); 
        }else
 	 id = m_em_idhelper->channel_id(det,samp,reg,eta,0); 

	HWIdentifier hid = m_cablingSvc->createSignalChannelID(id);
	const LArfSamplComplete::LArCondObj & t = fsampl->get(hid,0); 
	std::string id_str = m_online_idhelper->print_to_string(hid); 
	log<<MSG::INFO<<" online id = "<<id_str<<endreq; 

	if( t.isEmpty() ) 
	{
	  log<<MSG::ERROR<<" No existing conditions data " <<endreq;
          log<<MSG::ERROR<< m_em_idhelper->print_to_string(id) << endreq;
	  // return StatusCode::FAILURE ;
	  continue ; 
	}
	log<<MSG::INFO<<" Old fsampl = "<< t.m_fSampl<< " " <<endreq; 

	LArfSamplComplete::LArCondObj& t2 = const_cast<LArfSamplComplete::LArCondObj&>(t); 
	t2.m_fSampl = value ; 

	const LArfSamplComplete::LArCondObj & t3 = fsampl->get(hid,0); 
	log<<MSG::INFO<<" New fSampl = "<< t3.m_fSampl<< " " <<endreq; 

    }

   log<<MSG::INFO<<"  Number of entries changes =  " <<n <<endreq; 

   return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::updateEM_DACuAMeV(const std::string& filename) {


   // read in the file

   MsgStream  log(messageService(),name());

   const LArDAC2uAMC * dac2ua_c; 
   StatusCode sc=m_detStore->retrieve(dac2ua_c) ; 
   if(sc!=StatusCode::SUCCESS){ 
     log<< MSG::ERROR<<" Can not find DAC2uA" <<endreq;
  	return sc;
   }

   LArDAC2uAMC* dac2uaMC = const_cast<LArDAC2uAMC*>(dac2ua_c);


   const LAruA2MeVMC * ua2mev_c; 
   sc=m_detStore->retrieve(ua2mev_c) ; 
   if(sc!=StatusCode::SUCCESS){ 
     log<< MSG::ERROR<<" Can not find uA2MeV" <<endreq;
  	return sc;
   }

   LAruA2MeVMC* ua2mevMC = const_cast<LAruA2MeVMC*>(ua2mev_c);

   std::ifstream infile(filename.c_str() ) ; 

   if(! infile.good() ) 
     {
       log<<MSG::ERROR<<" fail to open file "<<endreq;
       return StatusCode::FAILURE; 
     }


   char s[200];
   infile.getline(s,200);
   log<<MSG::INFO<<" first line of the file  "<<s<<endreq;

   int   det,samp,reg,eta;
   int n = 0;
   float dac2ua0, ua2mev0,attenuation,dac2ua,ua2mev;
   while ( infile>>det>>samp>>reg>>eta>>
           dac2ua0>>ua2mev0>>attenuation>>dac2ua>>ua2mev )
     {
        ++n;
	log<<MSG::DEBUG<<" det,samp,reg,eta,values="
	<<det<<" " 
	<<samp<<" " 
	<<reg<<" " 
	<<eta<<" " 
	<<dac2ua<<" "
	<<ua2mev<<" " <<endreq; 

	Identifier id ; 
	if ( det==1 && samp==1 && reg==0 && eta==0 ){
	   // eta=0 for strip, not connected, but keep it
	   log<<MSG::DEBUG<<" disconnected strip "<<endreq;
 	 id = m_em_idhelper->disc_channel_id(det,samp,reg,eta,0); 
        }else
 	 id = m_em_idhelper->channel_id(det,samp,reg,eta,0); 

	HWIdentifier hid = m_cablingSvc->createSignalChannelID(id);

	const LArDAC2uAComplete::LArCondObj & t = dac2uaMC->get(hid,0); 
	std::string id_str = m_online_idhelper->print_to_string(hid); 
	std::string id_str_off = m_em_idhelper->print_to_string(id); 
	log<<MSG::DEBUG<<" online id = "<<id_str<<endreq; 
	log<<MSG::DEBUG<<" offline id = "<<id_str_off<<endreq; 

	if( t.isEmpty() ) 
	{
	  log<<MSG::ERROR<<" No existing conditions data " <<endreq;
	  // return StatusCode::FAILURE ;
	  continue ; 
	}
	log<<MSG::DEBUG<<" Old DAC2uA = "<< t.m_data<< " " <<endreq; 

	LArDAC2uAComplete::LArCondObj& t2 = const_cast<LArDAC2uAComplete::LArCondObj&>(t); 
	t2.m_data= dac2ua ; 

	const LArDAC2uAComplete::LArCondObj & t3 = dac2uaMC->get(hid,0); 
	log<<MSG::DEBUG<<" New DAC2uA = "<< t3.m_data<< " " <<endreq; 

	/**************************/

	const LAruA2MeVComplete::LArCondObj & u = ua2mevMC->get(hid,0); 

	if( u.isEmpty() ) 
	{
	  log<<MSG::ERROR<<" No existing conditions data " <<endreq;
	  // return StatusCode::FAILURE ;
	  continue ; 
	}
	log<<MSG::DEBUG<<" Old uA2MeV = "<< u.m_data<< " " <<endreq; 

	LAruA2MeVComplete::LArCondObj& u2 = const_cast<LAruA2MeVComplete::LArCondObj&>(u); 
	u2.m_data = ua2mev ; 

	const LAruA2MeVComplete::LArCondObj & u3 = ua2mevMC->get(hid,0); 
	log<<MSG::DEBUG<<" New uA2MeV = "<< u3.m_data<< " " <<endreq; 

    }

   log<<MSG::DEBUG<<"  Number of entries changes =  " <<n <<endreq; 

   return StatusCode::SUCCESS;
}



StatusCode FixLArElecCalib::fix5() {


    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " in fix5() " <<endreq;

    // update EM 
    StatusCode sc = updateMinBias("mbrms_em_rel12.txt"); 
    if (!sc.isSuccess()){
      log<< MSG::ERROR<<" failed to write MinBias EM " <<endreq;
      return sc;
    }

    sc = updateMinBias("mbrms_hec_rel12.txt"); 
    if (!sc.isSuccess()){
      log<< MSG::ERROR<<" failed to write MinBias HEC " <<endreq;
      return sc;
    }


    // Fix5 is for updating the FCAL noise and MinBiasRMS data using 
    // Sven Menke's file.

    const LArNoiseMC * noise_c; 
    sc=m_detStore->retrieve(noise_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find Noise" <<endreq;
      return sc;
    }
    LArNoiseMC* noise = const_cast<LArNoiseMC*>(noise_c);
    
    const LArMinBiasMC * minbias_c; 
    sc=m_detStore->retrieve(minbias_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find MinBias" <<endreq;
      return sc;
    }
    LArMinBiasMC* minbias = const_cast<LArMinBiasMC*>(minbias_c);
    
    int ndisc=0;
    int nconn=0;
    for( unsigned int gain=0;gain<3;++gain)
      {
	// loop over all the old data
	LArNoiseMC::ConstConditionsMapIterator  it = noise->begin(gain);
	LArNoiseMC::ConstConditionsMapIterator  it_e = noise->end(gain);
	
	for (; it!=it_e;++it)
	  {
	    HWIdentifier hid = it.channelId(); 
	    if( (*it).isEmpty() ) continue;  

	    Identifier id = m_cablingSvc->cnvToIdentifier(hid);
	    if(!m_fcal_idhelper->is_lar_fcal(id)) continue;

	    if(m_cablingSvc->isOnlineConnected(hid)){
	      ++nconn;
	      continue;
	    }else
	      {
		log<<MSG::ERROR<<" unconnected channel" 
		   << m_online_idhelper->print_to_string(hid) <<endreq;

		++ndisc ; 
	      }
	    
	  }

	log<<MSG::INFO<<"  Gain="<<gain<<
	  " Number of connected and disconnected =  " <<nconn<<" " <<ndisc <<endreq; 
	
      }
    

    int n=0;

    std::string filename("FCal_noise_minbias.txt");
 
    std::ifstream infile(filename.c_str() ) ; 

    if(! infile.good() ) 
      {
	log<<MSG::ERROR<<" fail to open file "<<filename<<endreq;
	return StatusCode::FAILURE; 
     }
    

   char s[200];

   log<<MSG::INFO<<"Opened FCAL file"<<endreq;
   for(int i = 0;i<11;++i){
     infile.getline(s,200);
     log<<MSG::INFO<<s<<endreq;
   }

   std::string str_id;

   float vol,noise_p,noise_h, noise_m,noise_l;

     while ( infile>>str_id >>vol>>noise_p>>noise_h>>noise_m>>noise_l )
     {
       const char* ch_id = str_id.c_str();
       if(str_id.find("A")!=0){
	 log<<MSG::DEBUG<<" skipping string"<<str_id<<endreq; 
         continue;  
       }
       
       int mod,phi,eta;
       sscanf(ch_id, "A%d.%d.%d", &mod,&phi,&eta);
       if(phi>7){
          log<<MSG::DEBUG<<" skipping phi"<<str_id<<" phi="<<phi<<endreq; 
          continue; 
       }
       ++n;

       log<<MSG::INFO<<" Setting channel "<<str_id<<endreq; 
       Identifier id = m_fcal_idhelper->channel_id(2,mod,eta,phi); 
       HWIdentifier hid = m_cablingSvc->createSignalChannelID(id);

       const LArMinBiasComplete::LArCondObj& t1 = minbias->get(hid,0) ;
       LArMinBiasComplete::LArCondObj& t2 = const_cast<LArMinBiasComplete::LArCondObj&>(t1); 
       log<<MSG::INFO<<" minBiasRMS, old new "<<t2.m_MinBiasRMS<<" " <<noise_p<<endreq;
       t2.m_MinBiasRMS = noise_p; 

       const LArNoiseComplete::LArCondObj& noise0 = noise->get(hid,0) ;
       LArNoiseComplete::LArCondObj& u0 = const_cast<LArNoiseComplete::LArCondObj&>(noise0); 
       if(u0.m_Noise!=noise_h) 
	 log<<MSG::INFO<<" noise, old new "<<u0.m_Noise<<" " <<noise_h<<endreq;
       u0.m_Noise=noise_h; 

       const LArNoiseComplete::LArCondObj& noise1 = noise->get(hid,1) ;
       LArNoiseComplete::LArCondObj& u1 = const_cast<LArNoiseComplete::LArCondObj&>(noise1); 
       if(u1.m_Noise!=noise_m) 
	 log<<MSG::INFO<<" noise, old new "<<u1.m_Noise<<" " <<noise_m<<endreq;
       u1.m_Noise=noise_m; 

       const LArNoiseComplete::LArCondObj& noise2 = noise->get(hid,2) ;
       LArNoiseComplete::LArCondObj& u2 = const_cast<LArNoiseComplete::LArCondObj&>(noise2); 
       if(u2.m_Noise!=noise_l) 
	 log<<MSG::INFO<<" noise, old new "<<u2.m_Noise<<" " <<noise_l<<endreq;
       u2.m_Noise=noise_l; 
       
     }

     log<<MSG::INFO<<" number of channels in file ="<<n<<endreq;

     /*
    std::vector<Identifier>::const_iterator it= m_fcal_idhelper->fcal_begin();
    std::vector<Identifier>::const_iterator it_e= m_fcal_idhelper->fcal_end();
    int n2 = 0; 
    for(;it!=it_e; ++it)
	{
	  Identifier id = *it; 
	  int pn = m_fcal_idhelper->pos_neg(id);
	  if(pn!=2) continue; 
	  int phi = m_fcal_idhelper->phi(id);
	  if (phi>7) continue; 

	  int module = m_fcal_idhelper->module(id)-1;
	  int eta2 = m_fcal_idhelper->eta(id);
	  int phi2 = m_fcal_idhelper->phi(id);

	  HWIdentifier hid = m_cablingSvc->createSignalChannelID(id);
	  log<<MSG::INFO<<" mod, phi,eta"<<module<<" "<<phi2<<" "<<eta2<<endreq;
	  ++n2;

	}
     log<<MSG::INFO<<" number of channels in idhelper ="<<n2<<endreq;

     */


     return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::updateMinBias(const std::string& filename) {

    MsgStream  log(messageService(),name());

    log << MSG::INFO << " in updateMinBias(), filename =  "<<filename <<endreq;


    // this method updates the EM MinBias
    // input is the text file.

   const LArMinBiasMC * minbias_c; 
   StatusCode sc=m_detStore->retrieve(minbias_c) ; 
   if(sc!=StatusCode::SUCCESS){ 
     log<< MSG::ERROR<<" Can not find MinBias" <<endreq;
  	return sc;
   }

   LArMinBiasMC* minbias = const_cast<LArMinBiasMC*>(minbias_c);

   // read in the file

   log<<MSG::INFO<<" opening file "<< filename << endreq;
   std::ifstream infile( filename.c_str() );
   if(! infile.good() ) 
     {
       log<<MSG::ERROR<<" fail to open file "<<endreq;
       return StatusCode::FAILURE; 
     }
  
   int lar,tp,det,samp,reg,eta,phi; 
   //tp=1 for EM, 2=HEC 

   float value ; 
   int n = 0; 
   while ( infile>>lar>>tp>>det>>samp>>reg>>eta>>phi>>value )
    {
	++n; 
	log<<MSG::INFO<<" det,samp,reg,eta,value="
	<<det<<" " 
	<<samp<<" " 
	<<reg<<" " 
	<<eta<<" " 
	<<value<<" " <<endreq; 

	Identifier id ; 
	if ( det==1 && samp==1 && reg==0 && eta==0 ){
	   // eta=0 for strip, not connected, but keep it
	   log<<MSG::INFO<<" disconnected strip "<<endreq;
	   id = m_em_idhelper->disc_channel_id(det,samp,reg,eta,0); 
        }else
	  {
	    if (tp==1){
	      id = m_em_idhelper->channel_id(det,samp,reg,eta,0); 
	    }else
	      if(tp==2)
		{
		  id = m_hec_idhelper->channel_id(det,samp,reg,eta,0); 
		}
	      else
		{
		  log<<MSG::ERROR<<" unknown type "<<tp<<endreq;
		  continue;
		}
	  }

	HWIdentifier hid = m_cablingSvc->createSignalChannelID(id);
	const LArMinBiasComplete::LArCondObj & t = minbias->get(hid,0); 
	std::string id_str = m_online_idhelper->print_to_string(hid); 
	log<<MSG::INFO<<" online id = "<<id_str<<endreq; 

	if( t.isEmpty() ) 
	{
	  log<<MSG::ERROR<<" No existing conditions data " <<endreq;
          log<<MSG::ERROR<< m_em_idhelper->print_to_string(id) << endreq;
	  // return StatusCode::FAILURE ;
	  continue ; 
	}
	log<<MSG::INFO<<" Old MinBias = "<< t.m_MinBiasRMS<< " " <<endreq; 

	LArMinBiasComplete::LArCondObj& t2 = const_cast<LArMinBiasComplete::LArCondObj&>(t); 
	t2.m_MinBiasRMS = value ; 

	const LArMinBiasComplete::LArCondObj & t3 = minbias->get(hid,0); 
	log<<MSG::INFO<<" New MinBias = "<< t3.m_MinBiasRMS<< " " <<endreq; 

    }

   log<<MSG::INFO<<"  Number of entries changes =  " <<n <<endreq; 

   return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix6() {

    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " in fix6() " <<endreq;
    // update FCAL noise and gain


    // Fix6 is for updating the FCAL noise and ADC2DAC, uA2MeV and DAC2uA.

    const LArNoiseMC * noise_c; 
    StatusCode sc=m_detStore->retrieve(noise_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find Noise" <<endreq;
      return sc;
    }
    LArNoiseMC* noise = const_cast<LArNoiseMC*>(noise_c);
    

    const LArRampMC * ramp_c; 
    sc=m_detStore->retrieve(ramp_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find Ramp" <<endreq;
      return sc;
    }
    LArRampMC* ramp = const_cast<LArRampMC*>(ramp_c);
    

    const LAruA2MeVMC * ua2MeV_c; 
    sc=m_detStore->retrieve(ua2MeV_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find uA2MeV" <<endreq;
      return sc;
    }
    LAruA2MeVMC* ua2MeV = const_cast<LAruA2MeVMC*>(ua2MeV_c);
    
    const LArDAC2uAMC * dac2uA_c; 
    sc=m_detStore->retrieve(dac2uA_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find DAC2uA" <<endreq;
      return sc;
    }
    LArDAC2uAMC* dac2uA = const_cast<LArDAC2uAMC*>(dac2uA_c);
    
    int ndisc=0;
    int nconn=0;
    for( unsigned int gain=0;gain<3;++gain)
      {
	// loop over all the old data
	LArNoiseMC::ConstConditionsMapIterator  it = noise->begin(gain);
	LArNoiseMC::ConstConditionsMapIterator  it_e = noise->end(gain);
	
	for (; it!=it_e;++it)
	  {
	    HWIdentifier hid = it.channelId(); 
	    if( (*it).isEmpty() ) continue;  

	    Identifier id = m_cablingSvc->cnvToIdentifier(hid);
	    if(!m_fcal_idhelper->is_lar_fcal(id)) continue;

	    if(m_cablingSvc->isOnlineConnected(hid)){
	      ++nconn;
	      continue;
	    }else
	      {
		log<<MSG::ERROR<<" unconnected channel" 
		   << m_online_idhelper->print_to_string(hid) <<endreq;

		++ndisc ; 
	      }
	    
	  }

	log<<MSG::INFO<<"  Gain="<<gain<<
	  " Number of connected and disconnected =  " <<nconn<<" " <<ndisc <<endreq; 
	
      }
    

    int n=0;

    std::string filename("FCal_noise_minbias_adc2mev.txt");
 
    std::ifstream infile(filename.c_str() ) ; 

    if(! infile.good() ) 
      {
	log<<MSG::ERROR<<" fail to open file "<<filename<<endreq;
	return StatusCode::FAILURE; 
     }
    

   char s[200];

   log<<MSG::INFO<<"Opened FCAL file"<<endreq;
   for(int i = 0;i<27;++i){
     infile.getline(s,200);
     log<<MSG::INFO<<s<<endreq;
   }

   std::string str_id;

   float vol,noise_p,noise_h, noise_m,noise_l;
   float ramp_val,ua2MeV_val,dac2uA_val;
   std::string channel_type;
   while ( infile>>str_id >>vol>>noise_p >> noise_h>>noise_m>>noise_l
	   >>channel_type>> ramp_val>>ua2MeV_val>>dac2uA_val )
     {
       const char* ch_id = str_id.c_str();

       if(str_id.substr(0,1)!=std::string("A")){
	 log<<MSG::INFO<<" skipping string"<<str_id<<endreq; 
         continue;  
       }

       int mod,phi,eta;
       sscanf(ch_id, "A%d.%d.%d", &mod,&phi,&eta);
       if(phi>7){
          log<<MSG::INFO<<" skipping phi"<<str_id<<" phi="<<phi<<endreq; 
          continue; 
       }

       ++n;

       log<<MSG::INFO<<" Setting channel "<<str_id<<endreq; 
       Identifier id = m_fcal_idhelper->channel_id(2,mod,eta,phi); 
       HWIdentifier hid = m_cablingSvc->createSignalChannelID(id);

       const LArNoiseComplete::LArCondObj& noise0 = noise->get(hid,0) ;
       LArNoiseComplete::LArCondObj& u0 = const_cast<LArNoiseComplete::LArCondObj&>(noise0); 
       if(u0.m_Noise!=noise_h) 
	 log<<MSG::INFO<<" noise, old new "<<u0.m_Noise<<" " <<noise_h<<endreq;
       u0.m_Noise=noise_h; 

       const LArNoiseComplete::LArCondObj& noise1 = noise->get(hid,1) ;
       LArNoiseComplete::LArCondObj& u1 = const_cast<LArNoiseComplete::LArCondObj&>(noise1); 
       if(u1.m_Noise!=noise_m) 
	 log<<MSG::INFO<<" noise, old new "<<u1.m_Noise<<" " <<noise_m<<endreq;
       u1.m_Noise=noise_m; 

       const LArNoiseComplete::LArCondObj& noise2 = noise->get(hid,2) ;
       LArNoiseComplete::LArCondObj& u2 = const_cast<LArNoiseComplete::LArCondObj&>(noise2); 
       if(u2.m_Noise!=noise_l) 
	 log<<MSG::INFO<<" noise, old new "<<u2.m_Noise<<" " <<noise_l<<endreq;
       u2.m_Noise=noise_l; 
       
       const LArRampComplete::LArCondObj& ramp0_c = ramp->get(hid,0) ;
       const LArRampComplete::LArCondObj& ramp1_c = ramp->get(hid,1) ;
       const LArRampComplete::LArCondObj& ramp2_c = ramp->get(hid,2) ;
       LArRampComplete::LArCondObj& ramp0 = const_cast<LArRampComplete::LArCondObj&>(ramp0_c); 
       LArRampComplete::LArCondObj& ramp1 = const_cast<LArRampComplete::LArCondObj&>(ramp1_c); 
       LArRampComplete::LArCondObj& ramp2 = const_cast<LArRampComplete::LArCondObj&>(ramp2_c); 

       std::vector<float> v_h ;
       v_h.push_back(0);
       v_h.push_back(ramp_val);
       ramp0.m_vRamp=v_h;

       std::vector<float> v_m ;
       v_m.push_back(0);
       v_m.push_back(ramp_val/9.96);
       ramp1.m_vRamp=v_m;

       std::vector<float> v_l ;
       v_l.push_back(0);
       v_l.push_back(ramp_val/9.96/9.67);
       ramp2.m_vRamp=v_l;

       const LAruA2MeVComplete::LArCondObj& ua2MeV0_c = ua2MeV->get(hid,0) ;
       LAruA2MeVComplete::LArCondObj& ua2MeV0 = const_cast<LAruA2MeVComplete::LArCondObj&>(ua2MeV0_c); 
       ua2MeV0.m_data=ua2MeV_val; 

       const LArDAC2uAComplete::LArCondObj& dac2ua0_c = dac2uA->get(hid,0) ;
       LArDAC2uAComplete::LArCondObj& dac2ua0 = const_cast<LArDAC2uAComplete::LArCondObj&>(dac2ua0_c); 
       dac2ua0.m_data = dac2uA_val; 

     }

     log<<MSG::INFO<<" number of channels in file ="<<n<<endreq;

     return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix7() {


    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " in fix7() " <<endreq;
    std::string filename ;

    // updating fSampl from 13.0.30 for QGSP_BERT
    if( m_g4Phys=="QGSP_BERT"){
      filename = "fsampl_1303_qgsp-bert.txt";
    }
    else {
      log << MSG::ERROR << " wrong option  "<<m_g4Phys <<endreq;
      return StatusCode::FAILURE; 
    }


    StatusCode sc = updateEMfSampl(filename ); 
    if(! sc.isSuccess()) return sc;

    return StatusCode::SUCCESS;


}

StatusCode FixLArElecCalib::fix8() {


    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " in fix8() " <<endreq;
    // update EM/HEC 7 data objects.


    StatusCode sc ;
    sc = addMphysOverMcal();
    if(! sc.isSuccess()) return sc;

    //                                                          withGain , nvar
    sc = update_EM_HEC<LArNoiseMC>("noise_em.txt","noise_hec.txt", true, 1 );
    if(! sc.isSuccess()) return sc;

    sc = update_EM_HEC<LArAutoCorrMC>("","autocorr_hec.txt", true, 4 );
    if(! sc.isSuccess()) return sc;

    sc = update_EM_HEC<LArAutoCorrMC>("autocorr_em.txt","", true,5);
    if(! sc.isSuccess()) return sc;
    

    sc = update_EM_HEC<LArRampMC>("ramp_em.txt","ramp_hec.txt", true, 1 );
    if(! sc.isSuccess()) return sc;

    sc = update_EM_HEC<LAruA2MeVMC>("ua2mev_em.txt","ua2mev_hec.txt", false, 1 );
    if(! sc.isSuccess()) return sc;

    sc = update_EM_HEC<LArDAC2uAMC>("dac2ua_em.txt","dac2ua_hec.txt", false, 1 );
    if(! sc.isSuccess()) return sc;

    sc = update_EM_HEC<LArShape32MC>("shape_em.txt","shape_hec.txt", true, 64 );
    if(! sc.isSuccess()) return sc;

    sc = update_EM_HEC<LArMphysOverMcalMC>("mphys_em.txt","", true, 1 );
    if(! sc.isSuccess()) return sc;

    // additional fix for LArDAC2uA and LAruA2MeV
    sc = fixDACuAMeV() ; 
    if(! sc.isSuccess()) return sc;

    return StatusCode::SUCCESS;


}


StatusCode FixLArElecCalib::ReadFile(const std::string& filename, bool EM, bool withGain, int nvar ) {


   MsgStream  log(messageService(),name());

   std::ifstream infile(filename.c_str() ) ; 

   if(! infile.good() ) 
     { log<<MSG::ERROR<<" fail to open file "<<filename << endreq;
       return StatusCode::FAILURE; 
     }

   log<<MSG::INFO<<" Opened file "<<filename << endreq;

   m_cache[0].clear();
   m_cache[1].clear();
   m_cache[2].clear();

   char s[200];
   infile.getline(s,200);
   log<<MSG::INFO<<" first line of the file  "<<s<<endreq;

   int   det=2;
   int   samp,reg,eta;
   int   n = 0;
   int first; 
   while ( infile>>first )
     {
       if (EM) {
	 det = first;
	 infile>>samp>>reg>>eta;
       }else
	 {
	   samp=first;
	   infile>>reg>>eta;
	 }

       int gain=0;
       if(withGain) infile>>gain ; 
       std::vector<float> vfl;
       float x;
       
       for (int i=0;i<nvar;++i)
	 { 
	   infile>>x;
	   vfl.push_back(x);
	 }
        ++n;
	log<<MSG::DEBUG<<" det,samp,reg,eta,values="
	   <<det<<" " 
	   <<samp<<" " 
	   <<reg<<" " 
	   <<eta<<" " 
	   <<endreq; 

	Identifier id ; 
	if (EM)
	{
	  if ( det==1 && samp==1 && reg==0 && eta==0 ){
	   // eta=0 for strip, not connected, but keep it
	   log<<MSG::DEBUG<<" disconnected strip "<<endreq;
	   id = m_em_idhelper->disc_channel_id(det,samp,reg,eta,0); 
	  }else
	    id = m_em_idhelper->channel_id(det,samp,reg,eta,0); 
	}else //HEC
	  {
	    id = m_hec_idhelper->channel_id(2,samp,reg,eta,0); 
	  }
	HWIdentifier hid = m_cablingSvc->createSignalChannelID(id);

        if (gain >= 0 && gain < 3)
          m_cache[gain].push_back(ROW(hid,vfl));
     }

   return StatusCode::SUCCESS; 

}


void FixLArElecCalib::print_object(MsgStream& log, const std::string& msg, const LArNoiseMC::LArCondObj& obj)
{
  if( obj.isEmpty()){
    log<<MSG::INFO<<" LArNoiseMC " << msg << " is empty" <<endreq; 
  }else 
    log<<MSG::DEBUG<<" LArNoiseMC" << msg << obj.m_Noise <<endreq; 
  return ; 
}

void FixLArElecCalib::set_object(LArNoiseMC::LArCondObj& obj, std::vector<float>& v )
{
  if (v.size()!=1)
    {
      std::cout<< " ERROR for LArNoiseMC, size of vector = "<< v.size()<<std::endl; 
      return ; 
    }
  obj.m_Noise = v[0]; 
  return;
} 

void FixLArElecCalib::print_object(MsgStream& log, const std::string& msg, const LArAutoCorrMC::LArCondObj& obj)
{
  if( obj.isEmpty()){
    log<<MSG::INFO<<" LArAutoCorrMC " << msg << " is empty" <<endreq; 
  }else 
    {
      log<<MSG::DEBUG<<" LArAutoCorrMC" << msg ;
      for (unsigned int i =0 ; i<obj.m_vAutoCorr.size();++i)
	log<< " " << obj.m_vAutoCorr[i];
      log<<endreq; 
    }
  return ; 
}

void FixLArElecCalib::set_object(LArAutoCorrMC::LArCondObj& obj, std::vector<float>& v )
{
  if (v.size()<4 )
    {
      std::cout<< " ERROR for LArAutoCorrMC, size of vector = "<< v.size()<<std::endl; 
      return ; 
    }

  obj.m_vAutoCorr.resize(4);
  for (unsigned int i=0;i<4;++i)
   {
     obj.m_vAutoCorr[i] = v[i]; 
   }
  return;
} 

void FixLArElecCalib::print_object(MsgStream& log, const std::string& msg, const LArRampMC::LArCondObj& obj)
{
  if( obj.isEmpty()){
    log<<MSG::INFO<<" LArRampMC " << msg << " is empty" <<endreq; 
  }else 
    {
      log<<MSG::DEBUG<<" LArRampMC" << msg ;
      for (unsigned int i =0 ; i<obj.m_vRamp.size();++i)
	log<< " " << obj.m_vRamp[i];
      log<<endreq; 
    }
  return ; 
}

void FixLArElecCalib::set_object(LArRampMC::LArCondObj& obj, std::vector<float>& v )
{
  if (v.size()!=1 )
    {
      std::cout<< " ERROR for LArRampMC, size of vector = "<< v.size()<<std::endl; 
      return ; 
    }
  obj.m_vRamp.resize(3);
  obj.m_vRamp[0]=0;
  obj.m_vRamp[1]=v[0]; 
  obj.m_vRamp[2]=0; 

  return;
} 

void FixLArElecCalib::print_object(MsgStream& log, const std::string& msg, const LArShape32MC::LArCondObj& obj)
{
  if( obj.isEmpty()){
    log<<MSG::INFO<<" LArShape32MC " << msg << " is empty" <<endreq; 
  }else 
    {
      log<<MSG::DEBUG<<" LArShape" << msg ;
      for (unsigned int i =0 ; i<obj.m_vShape.size();++i)
	log<< " " << obj.m_vShape[i];
      log<<endreq; 

      log<<MSG::DEBUG<<" LArShapeDer" << msg ;
      for (unsigned int i =0 ; i<obj.m_vShapeDer.size();++i)
	log<< " " << obj.m_vShapeDer[i];
      log<<endreq; 
    }
  return ; 
}

void FixLArElecCalib::set_object(LArShape32MC::LArCondObj& obj, std::vector<float>& v )
{
  
  if (v.size()!=64 )
    {
      std::cout<< " ERROR for LArShape32MC, size of vector = "<< v.size()<<std::endl; 
      return ; 
    }

  unsigned int size = v.size()/2; 
  obj.m_vShape.resize(size);
  obj.m_vShapeDer.resize(size);

  for(unsigned int i=0;i<size;++i){
    obj.m_vShape[i]=v[i];
    obj.m_vShapeDer[i]=v[i+size];
  }

  return;
} 





void FixLArElecCalib::print_object(MsgStream& log, const std::string& msg, const LArSingleFloatP& obj)
{
  if( obj.isEmpty()){
    log<<MSG::INFO<<" LArSingleFloatP " << msg << " is empty" <<endreq;
  }else
    log<<MSG::DEBUG<<" LArSingleFloatP " << msg << obj.m_data <<endreq;
  return ;
}

void FixLArElecCalib::set_object(LArSingleFloatP& obj, std::vector<float>& v )
{
  if (v.size()!=1)
    {
      std::cout<< " ERROR for LArSingleFloatP, size of vector = "<< v.size()<<std::endl;
      return ;
    }
  obj.m_data = v[0];
  return;
}




StatusCode FixLArElecCalib::addMphysOverMcal() {
  // add the contain to DetStore.
  // fill 1 for all FCAL/HEC channels

    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " addMphysOverMcal() " <<endreq;
    // create LArMphysOverMcal with FCAL numbers = 1.

    StatusCode sc; 

    LArMphysOverMcalMC* mphys = new LArMphysOverMcalMC() ;
    mphys->setGroupingType(LArConditionsContainerBase::SingleGroup);
    sc = mphys->initialize();
    if (sc != StatusCode::SUCCESS) {
      log << MSG::ERROR << "Could not initialize LArMphysOverMcalMC " << endreq;
      return StatusCode::FAILURE;
    }
    
    sc=m_detStore->record(mphys,"LArMphysOverMcal") ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not record LArMphysOverMcal" <<endreq;
      return sc;
    }
    ILArMphysOverMcal* imphys=0;
    sc=m_detStore->symLink(mphys,imphys); 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not make symlink " <<endreq;
      return sc;
    }

    int n=0;

    std::string filename("FCal_noise_minbias_adc2mev.txt");
 
    std::ifstream infile(filename.c_str() ) ; 

    if(! infile.good() ) 
      {
	log<<MSG::ERROR<<" fail to open file "<<filename<<endreq;
	return StatusCode::FAILURE; 
     }
    

   char s[200];

   log<<MSG::INFO<<"Opened FCAL file"<<endreq;
   for(int i = 0;i<27;++i){
     infile.getline(s,200);
     log<<MSG::INFO<<s<<endreq;
   }

   std::string str_id;

   float vol,noise_p,noise_h, noise_m,noise_l;
   float ramp_val,ua2MeV_val,dac2uA_val;
   std::string channel_type;
   while ( infile>>str_id >>vol>>noise_p >> noise_h>>noise_m>>noise_l
	   >>channel_type>> ramp_val>>ua2MeV_val>>dac2uA_val )
     {
       const char* ch_id = str_id.c_str();

       if(str_id.substr(0,1)!=std::string("A")){
	 log<<MSG::INFO<<" skipping string"<<str_id<<endreq; 
         continue;  
       }

       int mod,phi,eta;
       sscanf(ch_id, "A%d.%d.%d", &mod,&phi,&eta);
       if(phi>7){
          log<<MSG::INFO<<" skipping phi"<<str_id<<" phi="<<phi<<endreq; 
          continue; 
       }

       ++n;

       log<<MSG::INFO<<" Setting channel "<<str_id<<endreq; 
       Identifier id = m_fcal_idhelper->channel_id(2,mod,eta,phi); 
       HWIdentifier hid = m_cablingSvc->createSignalChannelID(id);

       LArMphysOverMcalMC::LArCondObj t; 
       t.m_data =1. ;
       mphys->setPdata(hid,t,0);
       mphys->setPdata(hid,t,1);
       mphys->setPdata(hid,t,2);
     }
       
   log<<MSG::INFO<<" MphysOverMcal added  "<<n<<" FCAL channels"<<endreq;

   bool EM=false ;
   bool withGain=false; 
   int nvar = 1; 
   std::string hec_filename("mphys_hec.txt"); 
   sc = ReadFile(hec_filename,EM,withGain,nvar);
   
   if(sc!=StatusCode::SUCCESS){ 
	     log<< MSG::ERROR<<" Failed to read file "<<hec_filename <<endreq;
	     return sc;
   }
   

   int igain=0;
   VROW::iterator it = m_cache[igain].begin();
   VROW::iterator it_e = m_cache[igain].end();
   
   n=0; 
   for( ;it!=it_e;++it)
     {
       HWIdentifier hid = (*it).first;
       LArMphysOverMcalMC::LArCondObj t; 
       t.m_data =1. ;
       mphys->setPdata(hid,t,0);
       mphys->setPdata(hid,t,1);
       mphys->setPdata(hid,t,2);
       ++n; 

     }

   log<< MSG::ERROR<<" Number of HEC channel added "<<n <<" per gain "<<endreq;

   return StatusCode::SUCCESS ;
}


StatusCode FixLArElecCalib::fixDACuAMeV()
{
   // the old data has -10000000 for invalid. change it to {ERRORCODE = -999};

   MsgStream  log(messageService(),name());

   const LArDAC2uAMC * dac2ua_c; 
   StatusCode sc=m_detStore->retrieve(dac2ua_c) ; 
   if(sc!=StatusCode::SUCCESS){ 
     log<< MSG::ERROR<<" Can not find DAC2uA" <<endreq;
  	return sc;
   }

   LArDAC2uAMC* dac2uaMC = const_cast<LArDAC2uAMC*>(dac2ua_c);


   const LAruA2MeVMC * ua2mev_c; 
   sc=m_detStore->retrieve(ua2mev_c) ; 
   if(sc!=StatusCode::SUCCESS){ 
     log<< MSG::ERROR<<" Can not find uA2MeV" <<endreq;
  	return sc;
   }

   LAruA2MeVMC* ua2mevMC = const_cast<LAruA2MeVMC*>(ua2mev_c);


   int n=0;
   

   LAruA2MeVMC::ConstConditionsMapIterator it = ua2mevMC->begin(0);
   LAruA2MeVMC::ConstConditionsMapIterator it_e = ua2mevMC->end(0);
   
   for(;it!=it_e;++it){
     HWIdentifier hid = it.channelId();
     const LAruA2MeVComplete::LArCondObj & u = (*it);
     
     if( u.m_data> -990)
       {
	 log<<MSG::DEBUG<<" ua2MeV channel OK "<<u.m_data <<endreq;
	 continue ; 
       }
     
     log<<MSG::DEBUG<<" Old uA2MeV = "<< u.m_data<< " " <<endreq; 
     
     LAruA2MeVComplete::LArCondObj& u2 = const_cast<LAruA2MeVComplete::LArCondObj&>(u); 
     u2.m_data= ILAruA2MeV::ERRORCODE ;
     
     const LAruA2MeVComplete::LArCondObj & u3 = ua2mevMC->get(hid,0); 
     log<<MSG::DEBUG<<" New uA2MeV = "<< u3.m_data<< " " <<endreq; 
     
     ++n;
   
   }


   log<<MSG::DEBUG<<"  Number of uA2MeV entries changes =  " <<n <<endreq; 


   for(unsigned int igain=0;igain<3;++igain)
     {
       LArDAC2uAMC::ConstConditionsMapIterator it = dac2uaMC->begin(igain);
       LArDAC2uAMC::ConstConditionsMapIterator it_e = dac2uaMC->end(igain);
       
       n=0;
       for(;it!=it_e;++it){
	 HWIdentifier hid = it.channelId();
	 const LArDAC2uAMC::LArCondObj & u = (*it);
	 
	 if( u.m_data> -990.)
	   {
	     log<<MSG::DEBUG<<" DAC2uA channel OK "<<u.m_data <<endreq;
	     continue ; 
	   }
	 
	 log<<MSG::DEBUG<<" Old DAC2uA = "<< u.m_data<< " " <<endreq; 
	 
	 LArDAC2uAMC::LArCondObj& u2 = const_cast<LArDAC2uAMC::LArCondObj&>(u); 
	 u2.m_data = ILArDAC2uA::ERRORCODE ;
	 
	 const LArDAC2uAMC::LArCondObj & u3 = dac2uaMC->get(hid,igain); 
	 log<<MSG::DEBUG<<" New DACuA2 = "<< u3.m_data << " " <<endreq; 
	 ++n;
       }
       
     }   
   
   log<<MSG::DEBUG<<"  Number of DAC2uA entries changes =  " <<n <<endreq; 
   
   return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix9() {


    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " in fix9(), replace FCAL AutoCorr " <<endreq;


    std::vector<float> fcal_autoCorr[3][3] ; //[module][gain]
    fcal_autoCorr[0][0].push_back( -0.01);
    fcal_autoCorr[0][0].push_back( -0.30);
    fcal_autoCorr[0][0].push_back( -0.09);
    fcal_autoCorr[0][0].push_back( 0.08);

    fcal_autoCorr[0][1].push_back( 0.01);
    fcal_autoCorr[0][1].push_back( -0.07);
    fcal_autoCorr[0][1].push_back( -0.01);
    fcal_autoCorr[0][1].push_back( 0.03);

    fcal_autoCorr[0][2].push_back( 0.2);
    fcal_autoCorr[0][2].push_back( 0.2);
    fcal_autoCorr[0][2].push_back( 0.2);
    fcal_autoCorr[0][2].push_back( 0.2);

    fcal_autoCorr[1][0].push_back( -0.02);
    fcal_autoCorr[1][0].push_back( -0.31);
    fcal_autoCorr[1][0].push_back( -0.06);
    fcal_autoCorr[1][0].push_back(  0.06);

    fcal_autoCorr[1][1].push_back( 0.0);
    fcal_autoCorr[1][1].push_back( -0.08);
    fcal_autoCorr[1][1].push_back( -0.01);
    fcal_autoCorr[1][1].push_back( 0.02);

    fcal_autoCorr[1][2].push_back( 0.02);
    fcal_autoCorr[1][2].push_back( 0.01);
    fcal_autoCorr[1][2].push_back( 0.02);
    fcal_autoCorr[1][2].push_back( 0.02);

    fcal_autoCorr[2][0].push_back( -0.03);
    fcal_autoCorr[2][0].push_back( -0.31);
    fcal_autoCorr[2][0].push_back( -0.05);
    fcal_autoCorr[2][0].push_back( 0.07);

    fcal_autoCorr[2][1].push_back( 0.01);
    fcal_autoCorr[2][1].push_back( -0.07);
    fcal_autoCorr[2][1].push_back( -0.01);
    fcal_autoCorr[2][1].push_back( 0.02);

    fcal_autoCorr[2][2].push_back( 0.02);
    fcal_autoCorr[2][2].push_back( 0.02);
    fcal_autoCorr[2][2].push_back( 0.02);
    fcal_autoCorr[2][2].push_back( 0.02);



    // Fix9 is for updating the FCAL AutoCorr conditions data 
    // Input should be MC Conditions data with DetDescrVersion=ATLAS-CSC-02-00-00

    const LArAutoCorrMC * ac_c; 
    StatusCode   sc =m_detStore->retrieve(ac_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find AutoCorr" <<endreq;
      return sc;
    }
    LArAutoCorrMC* ac = const_cast<LArAutoCorrMC*>(ac_c);
    
    
    for( unsigned int gain=0;gain<3;++gain)
      {
	// loop over all the old data
	LArAutoCorrMC::ConstConditionsMapIterator  it = ac->begin(gain);
	LArAutoCorrMC::ConstConditionsMapIterator  it_e = ac->end(gain);
	
	int n=0;
	for (; it!=it_e;++it)
	  {
	    HWIdentifier hid = it.channelId(); 
	    const LArAutoCorrMC::LArCondObj & u = (*it);
	    if( (*it).isEmpty() ) continue;  
	     
	    Identifier id = m_cablingSvc->cnvToIdentifier(hid);
	    if(! m_fcal_idhelper->is_lar_fcal(id)) continue ;
	    int module = m_fcal_idhelper->module(id);
	    module = module - 1 ; 
	    
	    log<<MSG::DEBUG<<"module= " <<  module << " Old AutoCorr = "
		 << u.m_vAutoCorr[0]<< " "
		 << u.m_vAutoCorr[1]<< " "
		 << u.m_vAutoCorr[2]<< " "
		 << u.m_vAutoCorr[3]<< " "
		 << endreq; 
	       
	    LArAutoCorrMC::LArCondObj& u2 = const_cast<LArAutoCorrMC::LArCondObj&>(u); 
	    u2.m_vAutoCorr = fcal_autoCorr[module][gain] ; 
	       
	    const LArAutoCorrMC::LArCondObj & u3 = ac->get(hid,gain); 
	    log<<MSG::DEBUG<<" New AutoCorr = "
		 << u3.m_vAutoCorr[0]<< " "
		 << u3.m_vAutoCorr[1]<< " "
		 << u3.m_vAutoCorr[2]<< " "
		 << u3.m_vAutoCorr[3]<< " "
		 << endreq; 
	      ++n;

	  }
	  log<<MSG::DEBUG<<"  Gain="<<gain<<
	  " Number of entries modified  =  " <<n <<endreq; 

      }
    
    return StatusCode::SUCCESS;    
}

StatusCode FixLArElecCalib::fix10() {

    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " in fix10() " <<endreq;
    // fix medium and low gain ramp

    const LArRampMC * ramp_c; 
    StatusCode sc=m_detStore->retrieve(ramp_c) ; 
    if(sc!=StatusCode::SUCCESS){ 
      log<< MSG::ERROR<<" Can not find Ramp" <<endreq;
      return sc;
    }
    LArRampMC* ramp = const_cast<LArRampMC*>(ramp_c);
    
    LArRampMC::ConstConditionsMapIterator  it = ramp->begin(0);
    LArRampMC::ConstConditionsMapIterator  it_e = ramp->end(0);
	
    int n=0;
    int ndisc=0;
    for (; it!=it_e;++it)
	  {
	    HWIdentifier hid = it.channelId(); 
	    if( (*it).isEmpty() ) continue;  

	    Identifier id = m_cablingSvc->cnvToIdentifier(hid);
	    if(!m_fcal_idhelper->is_lar_fcal(id)) continue;

	    if( ! m_cablingSvc->isOnlineConnected(hid)){
		log<<MSG::ERROR<<" unconnected channel" 
		   << m_online_idhelper->print_to_string(hid) <<endreq;
		++ndisc ; 
		continue ;
	      }
	    

	    const LArRampComplete::LArCondObj& ramp0_c = ramp->get(hid,0) ;
	    const LArRampComplete::LArCondObj& ramp1_c = ramp->get(hid,1) ;
	    const LArRampComplete::LArCondObj& ramp2_c = ramp->get(hid,2) ;
	    LArRampComplete::LArCondObj& ramp0 = const_cast<LArRampComplete::LArCondObj&>(ramp0_c); 
	    LArRampComplete::LArCondObj& ramp1 = const_cast<LArRampComplete::LArCondObj&>(ramp1_c); 
	    LArRampComplete::LArCondObj& ramp2 = const_cast<LArRampComplete::LArCondObj&>(ramp2_c); 
	    
	    float ramp_high = ramp0.m_vRamp[1]; 
	    
	    std::vector<float> v_m ;
	    v_m.push_back(0);
	    v_m.push_back(ramp_high*9.96);
	    ramp1.m_vRamp=v_m;
	    
	    std::vector<float> v_l ;
	    v_l.push_back(0);
	    v_l.push_back(ramp_high*9.96*9.67);
	    ramp2.m_vRamp=v_l;
	    ++n; 

	    log<<MSG::DEBUG<<" ramp hi,med,low"<< ramp0.m_vRamp[1]<< " "<<ramp1.m_vRamp[1]<<  " " <<
	      ramp2.m_vRamp[1]<<endreq;
	    
	  }

    log<<MSG::INFO<< " Number of channels updted =  " <<n <<endreq; 
    log<<MSG::INFO<< " Number of disconnected =  " <<ndisc <<endreq; 
    
    return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix11() {


    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " in fix8() " <<endreq;
    // update EM/HEC 7 data objects.


    StatusCode sc ;
    

    sc = update_EM_HEC<LArRampMC>("","ramp_hec_june2008.txt", true, 1 );
    if(! sc.isSuccess()) return sc;

    return StatusCode::SUCCESS;


}


StatusCode FixLArElecCalib::fix13() {


    MsgStream  log(messageService(),name());
    
    log << MSG::INFO << " in fix13() " <<endreq;
    // update EM/HEC 7 data objects.

    ToolHandle<ICaloSuperCellIDTool> scidtool("CaloSuperCellIDTool");
    if ( scidtool.retrieve().isFailure() ) {
        log << MSG::ERROR << " Could not retrieve scitool " << endreq;
    }

    std::set<Identifier> scidset;
    std::map<HWIdentifier,int> sslot_schannel_idx;
    std::vector<unsigned int> OrderOnlHash_OffID;
    std::vector<unsigned int> OrderOffHash_OnlID;
    OrderOnlHash_OffID.resize(40000,0);
    OrderOffHash_OnlID.resize(40000,0);

    Identifier::size_type channel_size = m_em_idhelper->channel_hash_max();
    unsigned int min_onl_hash = 999999;
    unsigned int max_onl_hash = 0;
    unsigned int min_off_hash = 999999;
    unsigned int max_off_hash = 0;
    std::ofstream hashes("AllHashChannels_All.txt");
    std::ofstream of("AllChannels_EM.txt");

    for(size_t i = 0; i < channel_size ; i++) {
	Identifier chid = m_em_idhelper->channel_id(i);
	bool isEM = ( m_em_idhelper->is_em_barrel( chid ) || m_em_idhelper->is_em_endcap( chid ) ) ;
	//bool isEM = ( m_em_idhelper->is_em_endcap( chid ) && (m_em_idhelper->sampling( chid )==3) ) ;
        if ( ! isEM ) continue;
        Identifier SCID = scidtool->offlineToSuperCellID(chid);
	if ( scidset.find(SCID) == scidset.end() && (m_scell_idhelper->calo_cell_hash( SCID ).value() < 99999) )
		scidset.insert(SCID);
	else continue;
	HWIdentifier hwid = m_cablingSvc->createSignalChannelID(chid);
	if ( i == 0 ){
        of << "Off ID\t\tSCID\t\tOnl ID\t\tFT\tslot\tB-E pos_neg\tSamp\teta\tphi\tFEB_ID\t\tSHWID\t" << std::endl;
	}
	of << chid.get_identifier32().get_compact() << "\t0x" <<
		SCID.get_identifier32().get_compact() << "\t0x" <<
		hwid.get_identifier32().get_compact() ;
	Identifier regId = m_em_idhelper->region_id( chid );
	int reg = m_em_idhelper->region( chid );
        IdentifierHash regHash = m_em_idhelper->region_hash( regId );
	int samp = m_em_idhelper->sampling( chid );
	int feedthrough = m_online_idhelper->feedthrough( hwid );
	int bar_ec = m_online_idhelper->barrel_ec( hwid );
	int pos_neg = m_online_idhelper->pos_neg( hwid );
	int slot = m_online_idhelper->slot( hwid );
	float etaG = m_em_idhelper->etaGranularity(regHash);
	float phiG = m_em_idhelper->phiGranularity(regHash);
	float eta0 = m_em_idhelper->eta0(regHash);
	float phi0 = m_em_idhelper->phi0(regHash);
	float eta = etaG*m_em_idhelper->eta( chid ) + eta0;
	float phi = phiG*m_em_idhelper->phi( chid ) + phi0;
        char etaChar[10];
        char phiChar[10];
        sprintf(etaChar,"%5.4f",eta);
        sprintf(phiChar,"%5.4f",phi);
	of << std::dec << "\t" << feedthrough << "\t" << slot;
	of << "\t" << bar_ec << "   " << pos_neg << "\t\t" << samp << "\t";
        of << etaChar << "  " << phiChar << "\t0x";
        HWIdentifier feb_id = m_online_idhelper->feb_Id(hwid);
        int sslot=1;
        bool InSlot= m_em_idhelper->is_em_endcap_inner ( chid );
        bool OutSlot= m_em_idhelper->is_em_endcap_outer ( chid )  ;
        bool secondOutSlot=false;
	if ( OutSlot ) {
		if ( (samp==1) && (reg==5) ) secondOutSlot=true;
		if ( (samp==2) && (reg==1) && ( m_em_idhelper->eta( chid )>=39 ) )
				secondOutSlot=true;
		if ( (samp==3) && (reg==0) && ( m_em_idhelper->eta( chid )>=18 ) )
				secondOutSlot=true;
	}
        HWIdentifier sfeb_id = m_sonline_idhelper->feb_Id(bar_ec,pos_neg,feedthrough,sslot);
        
        if ( InSlot ) {
        	sfeb_id = m_sonline_idhelper->feb_Id(bar_ec,pos_neg,feedthrough-1,sslot+1);
	}
        if ( secondOutSlot ) {
        	sfeb_id = m_sonline_idhelper->feb_Id(bar_ec,pos_neg,feedthrough,sslot+1);
	}
        if ( sslot_schannel_idx.find(sfeb_id) == sslot_schannel_idx.end() )
		sslot_schannel_idx[sfeb_id]=0;
	else sslot_schannel_idx[sfeb_id]++;
	HWIdentifier shwid = m_sonline_idhelper->channel_Id(sfeb_id,sslot_schannel_idx[sfeb_id]);
	unsigned int onl_hash = m_sonline_idhelper->channel_Hash( shwid ).value();
	if ( onl_hash > max_onl_hash ) max_onl_hash = onl_hash;
	if ( onl_hash < min_onl_hash ) min_onl_hash = onl_hash;
	unsigned int off_hash = m_scell_idhelper->calo_cell_hash( SCID ).value();
	if ( off_hash > max_off_hash ) max_off_hash = off_hash;
	if ( off_hash < min_off_hash ) min_off_hash = off_hash;
	of << std::hex;
	of << feb_id.get_identifier32().get_compact();
	of << "\t0x" << shwid.get_identifier32().get_compact();
	of << " " << m_online_idhelper->channel_name(hwid) << " " << m_sonline_idhelper->channel_name(shwid);
	of << std::dec;
	of << " " << onl_hash << " " << off_hash;
	of << std::endl;

	if ( m_scell_idhelper->calo_cell_hash( SCID ).value() < 40000 )
	OrderOffHash_OnlID[m_scell_idhelper->calo_cell_hash( SCID ).value()]  =  shwid.get_identifier32().get_compact();
	if ( m_sonline_idhelper->channel_Hash( shwid ).value() < 40000 )
	OrderOnlHash_OffID[m_sonline_idhelper->channel_Hash( shwid ).value()]  =  SCID.get_identifier32().get_compact();
    }
    of << "Hash check.  Onl : " << min_onl_hash << " " << max_onl_hash;
    of << ";   Off : " << min_off_hash << " " << max_off_hash << std::endl;;
    of.close();

    channel_size = m_hec_idhelper->channel_hash_max();
    min_onl_hash = 999999;
    max_onl_hash = 0;
    min_off_hash = 999999;
    max_off_hash = 0;
    std::ofstream of1("AllChannels_HEC.txt");

    for(size_t i = 0; i < channel_size ; i++) {
        Identifier chid = m_hec_idhelper->channel_id(i);
        bool isEM = ( m_hec_idhelper->is_lar_hec( chid )  ) ;
        if ( ! isEM ) continue;
        Identifier SCID = scidtool->offlineToSuperCellID(chid);
        if ( scidset.find(SCID) == scidset.end() )
                scidset.insert(SCID);
        else continue;
        HWIdentifier hwid = m_cablingSvc->createSignalChannelID(chid);
        if ( i == 0 ){
        of1 << "Off ID\t\tSCID\t\tOnl ID\t\tFT\tslot\tB-E pos_neg\tSamp\teta\tphi\tFEB_ID\t\tSHWID\t" << std::endl;
        }
        of1 << chid.get_identifier32().get_compact() << "\t0x" <<
                SCID.get_identifier32().get_compact() << "\t0x" <<
                hwid.get_identifier32().get_compact() ;
        Identifier regId = m_hec_idhelper->region_id( chid );
        IdentifierHash regHash = m_hec_idhelper->region_hash( regId );
        int samp = m_hec_idhelper->sampling( chid );
        int feedthrough = m_online_idhelper->feedthrough( hwid );
        int bar_ec = m_online_idhelper->barrel_ec( hwid );
        int pos_neg = m_online_idhelper->pos_neg( hwid );
        int slot = m_online_idhelper->slot( hwid );
        float etaG = m_hec_idhelper->etaGranularity(regHash);
        float phiG = m_hec_idhelper->phiGranularity(regHash);
        float eta0 = m_hec_idhelper->eta0(regHash);
        float phi0 = m_hec_idhelper->phi0(regHash);
        float eta = etaG*m_hec_idhelper->eta( chid ) + eta0;
        float phi = phiG*m_hec_idhelper->phi( chid ) + phi0;
        char etaChar[10];
        char phiChar[10];
        sprintf(etaChar,"%5.4f",eta);
        sprintf(phiChar,"%5.4f",phi);
        of1 << std::dec << "\t" << feedthrough << "\t" << slot;
        of1 << "\t" << bar_ec << "   " << pos_neg << "\t\t" << samp << "\t";
        of1 << etaChar << "  " << phiChar << "\t0x";
        HWIdentifier feb_id = m_online_idhelper->feb_Id(hwid);
        int sslot=1;
        int subtract=0;
        HWIdentifier sfeb_id = m_sonline_idhelper->feb_Id(bar_ec,pos_neg,feedthrough,sslot);
        if ( sslot_schannel_idx.find(sfeb_id) == sslot_schannel_idx.end() )
                sslot_schannel_idx[sfeb_id]=0;
        else sslot_schannel_idx[sfeb_id]++;
        if ( (feedthrough==6) && (bar_ec==1) && (slot>9) ) {
                sfeb_id = m_sonline_idhelper->feb_Id(bar_ec,pos_neg,feedthrough,sslot+1);
                subtract=192;
        }
        HWIdentifier shwid = m_sonline_idhelper->channel_Id(sfeb_id,sslot_schannel_idx[sfeb_id]-subtract);
        unsigned int onl_hash = m_sonline_idhelper->channel_Hash( shwid ).value();
        if ( onl_hash > max_onl_hash ) max_onl_hash = onl_hash;
        if ( onl_hash < min_onl_hash ) min_onl_hash = onl_hash;
        //unsigned int off_hash = m_shec_idhelper->channel_hash( SCID ).value();
        unsigned int off_hash = m_scell_idhelper->calo_cell_hash( SCID ).value();
        if ( off_hash > max_off_hash ) max_off_hash = off_hash;
        if ( off_hash < min_off_hash ) min_off_hash = off_hash;
        of1 << std::hex;
        of1 << feb_id.get_identifier32().get_compact();
        of1 << "\t0x" << shwid.get_identifier32().get_compact();
        of1 << " " << m_online_idhelper->channel_name(hwid) << " " << m_sonline_idhelper->channel_name(shwid);
        of1 << std::dec;
        of1 << " " << onl_hash << " " << off_hash;
        of1 << std::endl;

        OrderOffHash_OnlID[m_scell_idhelper->calo_cell_hash( SCID ).value()]  =  shwid.get_identifier32().get_compact();
        OrderOnlHash_OffID[m_sonline_idhelper->channel_Hash( shwid ).value()]  =  SCID.get_identifier32().get_compact();
    }

    channel_size = m_fcal_idhelper->channel_hash_max();
    min_onl_hash = 999999;
    max_onl_hash = 0;
    min_off_hash = 999999;
    max_off_hash = 0;
    std::ofstream of3("AllChannels_FCAL.txt");

    for(size_t i = 0; i < channel_size ; i++) {
        Identifier chid = m_fcal_idhelper->channel_id(i);
        bool isEM = ( m_fcal_idhelper->is_lar_fcal( chid )  ) ;
        if ( ! isEM ) continue;
        Identifier SCID = scidtool->offlineToSuperCellID(chid);
        if ( scidset.find(SCID) == scidset.end() )
                scidset.insert(SCID);
        else continue;
        HWIdentifier hwid = m_cablingSvc->createSignalChannelID(chid);
        if ( i == 0 ){
        of3 << "Off ID\t\tSCID\t\tOnl ID\t\tFT\tslot\tB-E pos_neg\tSamp\teta\tphi\tFEB_ID\t\tSHWID\t" << std::endl;
        }
        of3 << chid.get_identifier32().get_compact() << "\t0x" <<
                SCID.get_identifier32().get_compact() << "\t0x" <<
                hwid.get_identifier32().get_compact() ;
        int samp = 0;
        int feedthrough = m_online_idhelper->feedthrough( hwid );
        int bar_ec = m_online_idhelper->barrel_ec( hwid );
        int pos_neg = m_online_idhelper->pos_neg( hwid );
        int slot = m_online_idhelper->slot( hwid );
        of3 << std::dec << "\t" << feedthrough << "\t" << slot;
        of3 << "\t" << bar_ec << "   " << pos_neg << "\t\t" << samp << "\t";
        HWIdentifier feb_id = m_online_idhelper->feb_Id(hwid);
        int sslot=1;
        HWIdentifier sfeb_id = m_sonline_idhelper->feb_Id(bar_ec,pos_neg,feedthrough,sslot);
        if ( (feedthrough==6) && (bar_ec==1) && (slot>9) ) {
                sfeb_id = m_sonline_idhelper->feb_Id(bar_ec,pos_neg,feedthrough,sslot+1);
        }
        if ( sslot_schannel_idx.find(sfeb_id) == sslot_schannel_idx.end() )
                sslot_schannel_idx[sfeb_id]=0;
        else sslot_schannel_idx[sfeb_id]++;
        HWIdentifier shwid = m_sonline_idhelper->channel_Id(sfeb_id,sslot_schannel_idx[sfeb_id]);
        unsigned int onl_hash = m_sonline_idhelper->channel_Hash( shwid ).value();
        if ( onl_hash > max_onl_hash ) max_onl_hash = onl_hash;
        if ( onl_hash < min_onl_hash ) min_onl_hash = onl_hash;
        unsigned int off_hash = m_scell_idhelper->calo_cell_hash( SCID ).value();
        if ( off_hash > max_off_hash ) max_off_hash = off_hash;
        if ( off_hash < min_off_hash ) min_off_hash = off_hash;
        of3 << std::hex;
        of3 << feb_id.get_identifier32().get_compact();
        of3 << "\t0x" << shwid.get_identifier32().get_compact();
        of3 << " " << m_online_idhelper->channel_name(hwid) << " " << m_sonline_idhelper->channel_name(shwid);
        of3 << std::dec;
        of3 << " " << onl_hash << " " << off_hash;
        of3 << std::endl;

        OrderOffHash_OnlID[m_scell_idhelper->calo_cell_hash( SCID ).value()]  =  shwid.get_identifier32().get_compact();
        OrderOnlHash_OffID[m_sonline_idhelper->channel_Hash( shwid ).value()]  =  SCID.get_identifier32().get_compact();
    }

    of3 << "Hash check.  Onl : " << min_onl_hash << " " << max_onl_hash;
    of3 << ";   Off : " << min_off_hash << " " << max_off_hash << std::endl;;
    of3.close();



    const uint32_t onlHashMax=m_sonline_idhelper->channelHashMax(); 
    coral::AttributeListSpecification* spec_onOff = new coral::AttributeListSpecification();
    spec_onOff->extend("OnlineHashToOfflineId", "blob");
    spec_onOff->extend<unsigned>("version");
    AthenaAttributeList* al_onOff = new AthenaAttributeList(*spec_onOff);
    coral::Blob& blobOnOff=(*al_onOff)["OnlineHashToOfflineId"].data<coral::Blob>();
    (*al_onOff)["version"].setValue(0U);
    blobOnOff.resize(onlHashMax*sizeof(uint32_t));
    uint32_t* pBlobOnOff=static_cast<uint32_t*>(blobOnOff.startingAddress());
    unsigned nConn=0;

    const uint32_t emptyId=Identifier().get_identifier32().get_compact();
    for(size_t ii=0;ii<onlHashMax;ii++) {
      if (OrderOnlHash_OffID[ii] != 0) {
	pBlobOnOff[ii]=OrderOnlHash_OffID[ii];
	nConn++;
      }
      else
	pBlobOnOff[ii]=emptyId;
    }

    log << MSG::INFO << "HashMax=" << onlHashMax << ", connected=" << nConn << endreq;

    StatusCode sc=m_detStore->record(al_onOff,"/LAR/IdentifierOfl/OnOffIdMap_SC");
    if (sc.isFailure()){
      log << MSG::ERROR << "Failed to record LArCablingMap" << endreq;
      return sc;
    }

    

    hashes << "idx \t Off2OnlId \t Onl2OffId" << std::endl;
    for(size_t ii=0;ii<40000;ii++)
      if ( (OrderOffHash_OnlID[ii] != 0) || ( OrderOnlHash_OffID[ii] != 0 ) )
	hashes << std::dec << ii << std::hex << " \t " << OrderOffHash_OnlID[ii]  << " \t\t " << OrderOnlHash_OffID[ii]  << std::endl;
    hashes.close();

    return StatusCode::SUCCESS;
}

