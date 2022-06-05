/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

   12  Flag=12
        Fill the LArMinBiasMC and LArMinBiasAverageMC from ntuple created by LArMinBiasAlg (in CaloCondPhysAlgs)   
   13  Flag=13
       Filling the OnlOflMap for SC   

   14  Flag=14
        Fill the LArMinBiasAverageMC type in LArPileupAverage folder for BCID correction from ntuple created by LArMinBiasAlg (in CaloCondPhysAlgs)   

   15 Flag=15
        Scale the MinBiasAverage by constant   

   16 Flag=16
         Fix the EMEC-A FT 7 slot 11 OnOfIdMap cable swap

   17 Flag=17
         replace All shape

   18 Flag=18
         Fix the calib. line mapping for the EMF, where all channels in EMB-A FT 0 has CL=channel

**/ 


#include "FixLArElecCalib.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"
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
#include "LArRawConditions/LArMinBiasAverageMC.h"
#include "LArRawConditions/LArNoiseMC.h"
#include "LArRawConditions/LArRampMC.h"
#include "LArRawConditions/LArAutoCorrMC.h"

#include "CaloDetDescr/ICaloSuperCellIDTool.h"

#include <fstream>
#include "TTree.h"
#include "TFile.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

FixLArElecCalib::FixLArElecCalib(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator),
  m_fixFlag(0),
  m_em_idhelper(0),
  m_hec_idhelper(0),
  m_fcal_idhelper(0),
  m_online_idhelper(0),
  m_sem_idhelper(0),
  m_shec_idhelper(0),
  m_sfcal_idhelper(0),
  m_sonline_idhelper(0),
  m_scell_idhelper(0)
{ 

    declareProperty("FixFlag",      m_fixFlag);
    declareProperty("G4Phys",       m_g4Phys);
    declareProperty("InputFile",    m_infile="");

}

StatusCode FixLArElecCalib::initialize() {
  ATH_MSG_INFO ( " in initialize " );
  
  ATH_CHECK( detStore()->retrieve(m_em_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_hec_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_fcal_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_sem_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_shec_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_sfcal_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_online_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_sonline_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_scell_idhelper) );

  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_CLKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::execute() {

  const EventContext& ctx = Gaudi::Hive::currentContext();  

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, ctx};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_WARNING( "Do not have cabling mapping from key " << m_cablingKey.key() );
  }
  if(m_fixFlag==13)
    return fix13(cabling);

  return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::stop() {

   ATH_MSG_INFO ( " in stop " );

  const EventContext& ctx = Gaudi::Hive::currentContext();  

   SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, ctx};
   const LArOnOffIdMapping* cabling{*cablingHdl};
   if(!cabling) {
     ATH_MSG_WARNING( "Do not have cabling mapping from key " << m_cablingKey.key() );
   }
   SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_CLKey, ctx};
   const LArCalibLineMapping *clCont {*clHdl};
   if(!clCont) {
     ATH_MSG_ERROR( "Do not have calib line mapping !!!" );
     return StatusCode::FAILURE;
   }


    if(m_fixFlag==1) 
	return fix1(cabling); 
    if(m_fixFlag==2) 
	return fix2(cabling); 
    if(m_fixFlag==3) 
	return fix3(cabling); 
    if(m_fixFlag==4) 
	return fix4(cabling); 
    if(m_fixFlag==5) 
	return fix5(cabling); 
    if(m_fixFlag==6) 
	return fix6(cabling); 
    if(m_fixFlag==7) 
	return fix7(cabling); 
    if(m_fixFlag==8) 
	return fix8(cabling); 
    if(m_fixFlag==9)
	return fix9(cabling); 
    if(m_fixFlag==10)
	return fix10(cabling); 
    if(m_fixFlag==11)
	return fix11(cabling); 
    if(m_fixFlag==12)
	return fix12(cabling); 
    if(m_fixFlag==13)
	return fix13(cabling); 
    if(m_fixFlag==14)
	return fix14(cabling); 
    if(m_fixFlag==15)
	return fix15(); 
    if(m_fixFlag==16)
	return fix16(); 
    if(m_fixFlag==17)
	return fix17(cabling); 
    if(m_fixFlag==18)
	return fix18(clCont); 

    return StatusCode::SUCCESS; 

}

StatusCode FixLArElecCalib::fix1(const LArOnOffIdMapping *cabling) {

   ATH_MSG_INFO ( " in fix1() " );

   // Fix1 is for updating the EM DAC2uA, assuming symmetry. 
   // Input should be MC Conditions data with DetDescrVersion=ATLAS-DC3-05

   // Pointer to StoreGate 
   const LArEM_ID* em_idhelper = nullptr;
   ATH_CHECK( detStore()->retrieve(em_idhelper) );

   const LArOnlineID* online_idhelper = nullptr;
   ATH_CHECK( detStore()->retrieve(online_idhelper) );

   // retrieve DetDescrManager
   const IToolSvc* toolSvc = nullptr;
   ATH_CHECK( service("ToolSvc", toolSvc) );

   LArDAC2uAMC * dac2ua = nullptr;
   ATH_CHECK( detStore()->retrieve(dac2ua) );

   // read in the file

   std::ifstream infile("barrel_dac2ua_oct06.txt") ; 
   if(! infile.good() ) 
     {
       ATH_MSG_ERROR(" fail to open file ");
       return StatusCode::FAILURE; 
     }


   int   det,samp,reg,eta; 
   float value ; 
   int n = 0; 
   while ( infile>>det>>samp>>reg>>eta>>value )
    {
	++n; 
	ATH_MSG_DEBUG(" det,samp,reg,eta,value="
                      <<det<<" " 
                      <<samp<<" " 
                      <<reg<<" " 
                      <<eta<<" " 
                      <<value<<" " );

	Identifier id ; 
	if ( det==1 && samp==1 && reg==0 && eta==0 ){
	   // eta=0 for strip, not connected, but keep it
           // Removed disconnected channels: 03/2016 RDS 
           // ATH_MSG_DEBUG(" disconnected strip ");
 	   // id = em_idhelper->disc_channel_id(det,samp,reg,eta,0); 
        }else
 	 id = em_idhelper->channel_id(det,samp,reg,eta,0); 

	HWIdentifier hid = cabling->createSignalChannelID(id);
	LArDAC2uAComplete::LArCondObj & t = dac2ua->get(hid,0); 
	ATH_MSG_DEBUG(" online id = "<<online_idhelper->print_to_string(hid));

	if( t.isEmpty() ) 
	{
	  ATH_MSG_WARNING(" No existing conditions data " );
	  continue ; 
	}
	ATH_MSG_DEBUG(" Old DAC2uA = "<< t.m_data<< " " );

	t.m_data = value ; 

	const LArDAC2uAComplete::LArCondObj & t3 = dac2ua->get(hid,0); 
	ATH_MSG_DEBUG(" New DAC2uA = "<< t3.m_data << " " );

    }

   ATH_MSG_DEBUG("  Number of entries changes =  " <<n );
   return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix2(const LArOnOffIdMapping *cabling) {

    ATH_MSG_INFO ( " in fix2() " );

    // Fix2 is for updating the FCAL conditions data after IdFix7
    // Input should be MC Conditions data with DetDescrVersion=ATLAS-DC3-05

    const LArFCAL_ID* fcal_idhelper = nullptr;
    ATH_CHECK( detStore()->retrieve(fcal_idhelper) );
    
    const LArOnlineID* online_idhelper = nullptr;
    ATH_CHECK( detStore()->retrieve(online_idhelper) );
    
    // retrieve DetDescrManager 
    IToolSvc* toolSvc = nullptr;
    ATH_CHECK( service("ToolSvc", toolSvc) );
    
    LArDAC2uAMC * dac2ua = nullptr;
    ATH_CHECK( detStore()->retrieve(dac2ua) );
    
    LAruA2MeVMC * ua2mev = nullptr;
    ATH_CHECK( detStore()->retrieve(ua2mev) );
    
    LArRampMC * ramp = nullptr;
    ATH_CHECK( detStore()->retrieve(ramp) );
    
    LArShape32MC * shape = nullptr;
    ATH_CHECK( detStore()->retrieve(shape) );
    
    LArNoiseMC * noise = nullptr;
    ATH_CHECK( detStore()->retrieve(noise) );
    
    LArfSamplMC * fsampl = nullptr;
    ATH_CHECK( detStore()->retrieve(fsampl) );
    
    LArMinBiasMC * minbias = nullptr;
    ATH_CHECK( detStore()->retrieve(minbias) );
    
    LArAutoCorrMC * ac = nullptr;
    ATH_CHECK( detStore()->retrieve(ac) );
    
    
    for( unsigned int gain=0;gain<3;++gain)
      {
	// loop over all the old data
	LArRampMC::ConditionsMapIterator  it = ramp->begin(gain);
	LArRampMC::ConditionsMapIterator  it_e = ramp->end(gain);
	
	int n=0;
	for (; it!=it_e;++it)
	  {
	    HWIdentifier hid = it.channelId(); 
	    if( (*it).isEmpty() ) continue;  
	    if(cabling->isOnlineConnected(hid)){
	      continue;
	    }else
	      {
		ATH_MSG_DEBUG(" unconnected channel" 
                              << online_idhelper->print_to_string(hid) );
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
        ATH_MSG_DEBUG("  Gain="<<gain<<
                      " Number of entries removed =  " <<n );
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

	  HWIdentifier hid = cabling->createSignalChannelID(id);

	  for(unsigned int gain = 0;gain<3;++gain){
	    
		LArRampComplete::LArCondObj& rampP = ramp->get(hid,gain) ;
		if (!rampP.isEmpty()) continue ;

		ATH_MSG_DEBUG(" channel needs repair " 
                              << online_idhelper->print_to_string(hid) );
		ATH_MSG_DEBUG ( fcal_idhelper->print_to_string(id) );
		ATH_MSG_DEBUG ( "module = "<<module );

		++n;

		LArRampComplete::LArCondObj ramp_std = ramp->get(hid_mod[module],gain) ;
		ramp->setPdata(hid,ramp_std,gain); 

		LArAutoCorrComplete::LArCondObj ac_std = ac->get(hid_mod[module],gain) ;
		ac->setPdata(hid,ac_std,gain); 

		LArShape32MC::LArCondObj shape_std = shape->get(hid_mod[module],gain) ;
		shape->setPdata(hid,shape_std,gain); 

		LArNoiseComplete::LArCondObj noise_std = noise->get(hid_mod[module],gain) ;
		noise->setPdata(hid,noise_std,gain); 

		if(gain==0){

		 LAruA2MeVComplete::LArCondObj ua2mev_std = ua2mev->get(hid_mod[module],gain) ;
	 	 ua2mev->setPdata(hid,ua2mev_std,gain); 

		 LArDAC2uAComplete::LArCondObj dac2ua_std = dac2ua->get(hid_mod[module],gain) ;
	 	 dac2ua->setPdata(hid,dac2ua_std,gain); 

		 LArfSamplComplete::LArCondObj fsampl_std = fsampl->get(hid_mod[module],gain) ;
	  	 fsampl->setPdata(hid,fsampl_std,gain); 

		 LArMinBiasComplete::LArCondObj minbias_std = minbias->get(hid_mod[module],gain) ;
	 	 minbias->setPdata(hid,minbias_std,gain); 

		}

	}
    }
	
    ATH_MSG_DEBUG( " Number of entries fixed " <<n );
    return StatusCode::SUCCESS;    
}

StatusCode FixLArElecCalib::fix3(const LArOnOffIdMapping *cabling) {

    ATH_MSG_INFO ( " in fix3() " );
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
	ATH_MSG_ERROR ( " wrong option  "<<m_g4Phys );
	return StatusCode::FAILURE; 
      }

    // Fix3 is for updating the FCAL conditions data after IdFix7
    // Input should be MC Conditions data with DetDescrVersion=ATLAS-DC3-05
    
    ATH_CHECK( updateEMfSampl(filename, cabling ) );
    ATH_CHECK( updateHADfSampl(cabling) );
    return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix4(const LArOnOffIdMapping *cabling) {
  ATH_MSG_INFO ( " in fix4() " );
  return updateEM_DACuAMeV( "dac2ua_ua2mev_rel13.txt", cabling ); 
}

StatusCode FixLArElecCalib::updateHADfSampl(const LArOnOffIdMapping *cabling) {

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
      ATH_MSG_ERROR ( " wrong option  "<<m_g4Phys );
      return StatusCode::FAILURE; 
    }


  const LArfSamplMC * fsampl_c = nullptr;
  ATH_CHECK( detStore()->retrieve(fsampl_c) );

  LArfSamplMC* fsampl = const_cast<LArfSamplMC*>(fsampl_c);

  // loop over all the old data
  LArfSamplMC::ConditionsMapIterator  it = fsampl->begin(0);
  LArfSamplMC::ConditionsMapIterator  it_e = fsampl->end(0);
  int n_hec=0;
  int n_fcal=0;

  for (; it!=it_e;++it){
    
    HWIdentifier hid = it.channelId(); 
    if( (*it).isEmpty() ) continue;  
    if(!cabling->isOnlineConnected(hid)){
	      continue;
    }

    LArfSamplComplete::LArCondObj& t2 = const_cast<LArfSamplComplete::LArCondObj&>(*it);

    Identifier id = cabling->cnvToIdentifier(hid);


    if(m_hec_idhelper->is_lar_hec(id)){
      ATH_MSG_INFO(" HEC Old fsampl = "<< t2.m_fSampl<< " " );
      int sam = m_hec_idhelper->sampling(id); 

      if(sam<=1){
	t2.m_fSampl = fsampl_hec1;
      }else
	{
	  t2.m_fSampl = fsampl_hec2;
	}
      
      const LArfSamplComplete::LArCondObj & t3 = fsampl->get(hid,0); 
      ATH_MSG_INFO(" New fSampl = "<< t3.m_fSampl<< " " );

      ++n_hec;
    }
    if(m_fcal_idhelper->is_lar_fcal(id)){
      ATH_MSG_INFO(" FCAL Old fsampl = "<< t2.m_fSampl<< " " );
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
      ATH_MSG_INFO(" New fSampl = "<< t3.m_fSampl<< " " );

      ++n_fcal;
    }

  }

  ATH_MSG_INFO(" number of hec cells "<<n_hec);
  ATH_MSG_INFO(" number of fcal cells "<<n_fcal);

  return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::updateEMfSampl(const std::string& filename, const LArOnOffIdMapping *cabling) {

   ATH_MSG_INFO ( " in updateEMfSampl() " );

   // this method updates the EM fSample, assuming symmetry. 
   // input is the text file.

   const LArfSamplMC * fsampl_c = nullptr;
   ATH_CHECK( detStore()->retrieve(fsampl_c) );
   LArfSamplMC* fsampl = const_cast<LArfSamplMC*>(fsampl_c);

   // read in the file

   ATH_MSG_INFO(" opening file "<< filename );
   std::ifstream infile( filename.c_str() );
   if(! infile.good() ) 
     {
       ATH_MSG_ERROR(" fail to open file ");
       return StatusCode::FAILURE; 
     }
   int   det,samp,reg,eta; 
   float value ; 
   int n = 0; 
   while ( infile>>det>>samp>>reg>>eta>>value )
    {
	++n; 
	ATH_MSG_INFO(" det,samp,reg,eta,value="
                     <<det<<" " 
                     <<samp<<" " 
                     <<reg<<" " 
                     <<eta<<" " 
                     <<value<<" " );

	Identifier id ; 
	if ( det==1 && samp==1 && reg==0 && eta==0 ){
	   // eta=0 for strip, not connected, but keep it
           // Removed disconnected channels: 03/2016 RDS 
           // ATH_MSG_INFO(" disconnected strip ");
 	   // id = m_em_idhelper->disc_channel_id(det,samp,reg,eta,0); 
        }else
 	 id = m_em_idhelper->channel_id(det,samp,reg,eta,0); 

	HWIdentifier hid = cabling->createSignalChannelID(id);
	const LArfSamplComplete::LArCondObj & t = fsampl->get(hid,0); 
	std::string id_str = m_online_idhelper->print_to_string(hid); 
	ATH_MSG_INFO(" online id = "<<id_str);

	if( t.isEmpty() ) 
	{
	  ATH_MSG_ERROR(" No existing conditions data " );
          ATH_MSG_ERROR( m_em_idhelper->print_to_string(id) );
	  // return StatusCode::FAILURE ;
	  continue ; 
	}
	ATH_MSG_INFO(" Old fsampl = "<< t.m_fSampl<< " " );

	LArfSamplComplete::LArCondObj& t2 = const_cast<LArfSamplComplete::LArCondObj&>(t); 
	t2.m_fSampl = value ; 

	const LArfSamplComplete::LArCondObj & t3 = fsampl->get(hid,0); 
	ATH_MSG_INFO(" New fSampl = "<< t3.m_fSampl<< " " );
    }

   ATH_MSG_INFO("  Number of entries changes =  " <<n );
   return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::updateEM_DACuAMeV(const std::string& filename, const LArOnOffIdMapping *cabling) {
   // read in the file

   const LArDAC2uAMC * dac2ua_c = nullptr;
   ATH_CHECK( detStore()->retrieve(dac2ua_c) );
   LArDAC2uAMC* dac2uaMC = const_cast<LArDAC2uAMC*>(dac2ua_c);

   const LAruA2MeVMC * ua2mev_c = nullptr;
   ATH_CHECK( detStore()->retrieve(ua2mev_c) );
   LAruA2MeVMC* ua2mevMC = const_cast<LAruA2MeVMC*>(ua2mev_c);

   std::ifstream infile(filename.c_str() ) ; 

   if(! infile.good() ) 
     {
       ATH_MSG_ERROR(" fail to open file ");
       return StatusCode::FAILURE; 
     }


   char s[200];
   infile.getline(s,200);
   ATH_MSG_INFO(" first line of the file  "<<s);

   int   det,samp,reg,eta;
   int n = 0;
   float dac2ua0, ua2mev0,attenuation,dac2ua,ua2mev;
   while ( infile>>det>>samp>>reg>>eta>>
           dac2ua0>>ua2mev0>>attenuation>>dac2ua>>ua2mev )
     {
        ++n;
	ATH_MSG_DEBUG(" det,samp,reg,eta,values="
                      <<det<<" " 
                      <<samp<<" " 
                      <<reg<<" " 
                      <<eta<<" " 
                      <<dac2ua<<" "
                      <<ua2mev<<" " );

	Identifier id ; 
	if ( det==1 && samp==1 && reg==0 && eta==0 ){
	   // eta=0 for strip, not connected, but keep it
           // Removed disconnected channels: 03/2016 RDS 
           // ATH_MSG_DEBUG(" disconnected strip ");
 	   // id = m_em_idhelper->disc_channel_id(det,samp,reg,eta,0); 
        }else
 	 id = m_em_idhelper->channel_id(det,samp,reg,eta,0); 

	HWIdentifier hid = cabling->createSignalChannelID(id);

	const LArDAC2uAComplete::LArCondObj & t = dac2uaMC->get(hid,0); 
	std::string id_str = m_online_idhelper->print_to_string(hid); 
	std::string id_str_off = m_em_idhelper->print_to_string(id); 
	ATH_MSG_DEBUG(" online id = "<<id_str);
	ATH_MSG_DEBUG(" offline id = "<<id_str_off);

	if( t.isEmpty() ) 
	{
	  ATH_MSG_WARNING(" No existing conditions data " );
	  continue ; 
	}
	ATH_MSG_DEBUG(" Old DAC2uA = "<< t.m_data<< " " );

	LArDAC2uAComplete::LArCondObj& t2 = const_cast<LArDAC2uAComplete::LArCondObj&>(t); 
	t2.m_data= dac2ua ; 

	const LArDAC2uAComplete::LArCondObj & t3 = dac2uaMC->get(hid,0); 
	ATH_MSG_DEBUG(" New DAC2uA = "<< t3.m_data<< " " );

	/**************************/

	const LAruA2MeVComplete::LArCondObj & u = ua2mevMC->get(hid,0); 

	if( u.isEmpty() ) 
	{
	  ATH_MSG_ERROR(" No existing conditions data " );
	  // return StatusCode::FAILURE ;
	  continue ; 
	}
	ATH_MSG_DEBUG(" Old uA2MeV = "<< u.m_data<< " " );

	LAruA2MeVComplete::LArCondObj& u2 = const_cast<LAruA2MeVComplete::LArCondObj&>(u); 
	u2.m_data = ua2mev ; 

	const LAruA2MeVComplete::LArCondObj & u3 = ua2mevMC->get(hid,0); 
	ATH_MSG_DEBUG(" New uA2MeV = "<< u3.m_data<< " " );

    }

   ATH_MSG_DEBUG("  Number of entries changes =  " <<n );
   return StatusCode::SUCCESS;
}



StatusCode FixLArElecCalib::fix5(const LArOnOffIdMapping *cabling) {
    
    ATH_MSG_INFO ( " in fix5() " );

    // update EM 
    ATH_CHECK( updateMinBias("mbrms_em_rel12.txt", cabling) );
    ATH_CHECK( updateMinBias("mbrms_hec_rel12.txt", cabling) );

    // Fix5 is for updating the FCAL noise and MinBiasRMS data using 
    // Sven Menke's file.

    const LArNoiseMC * noise_c = nullptr;
    ATH_CHECK( detStore()->retrieve(noise_c) );
    LArNoiseMC* noise = const_cast<LArNoiseMC*>(noise_c);
    
    const LArMinBiasMC * minbias_c = nullptr;
    ATH_CHECK( detStore()->retrieve(minbias_c) );
    LArMinBiasMC* minbias = const_cast<LArMinBiasMC*>(minbias_c);
    
    int ndisc=0;
    int nconn=0;
    for( unsigned int gain=0;gain<3;++gain)
      {
	// loop over all the old data
	LArNoiseMC::ConditionsMapIterator  it = noise->begin(gain);
	LArNoiseMC::ConditionsMapIterator  it_e = noise->end(gain);
	
	for (; it!=it_e;++it)
	  {
	    HWIdentifier hid = it.channelId(); 
	    if( (*it).isEmpty() ) continue;  

	    Identifier id = cabling->cnvToIdentifier(hid);
	    if(!m_fcal_idhelper->is_lar_fcal(id)) continue;

	    if(cabling->isOnlineConnected(hid)){
	      ++nconn;
	    }else {
		ATH_MSG_ERROR(" unconnected channel" 
                              << m_online_idhelper->print_to_string(hid) );

		++ndisc ; 
	    }
	    
	  }

	ATH_MSG_INFO("  Gain="<<gain<<
                     " Number of connected and disconnected =  " <<nconn<<" " <<ndisc );
	
      }
    

    int n=0;

    std::string filename("FCal_noise_minbias.txt");
 
    std::ifstream infile(filename.c_str() ) ; 

    if(! infile.good() ) 
      {
	ATH_MSG_ERROR(" fail to open file "<<filename);
	return StatusCode::FAILURE; 
     }
    

   char s[200];

   ATH_MSG_INFO("Opened FCAL file");
   for(int i = 0;i<11;++i){
     infile.getline(s,200);
     ATH_MSG_INFO(s);
   }

   std::string str_id;

   float vol,noise_p,noise_h, noise_m,noise_l;

     while ( infile>>str_id >>vol>>noise_p>>noise_h>>noise_m>>noise_l )
     {
       const char* ch_id = str_id.c_str();
       if(str_id.find("A")!=0){
	 ATH_MSG_DEBUG(" skipping string"<<str_id);
         continue;  
       }
       
       int mod,phi,eta;
       std::sscanf(ch_id, "A%d.%d.%d", &mod,&phi,&eta);
       if(phi>7){
         ATH_MSG_DEBUG(" skipping phi"<<str_id<<" phi="<<phi);
          continue; 
       }
       ++n;

       ATH_MSG_INFO(" Setting channel "<<str_id);
       Identifier id = m_fcal_idhelper->channel_id(2,mod,eta,phi); 
       HWIdentifier hid = cabling->createSignalChannelID(id);

       const LArMinBiasComplete::LArCondObj& t1 = minbias->get(hid,0) ;
       LArMinBiasComplete::LArCondObj& t2 = const_cast<LArMinBiasComplete::LArCondObj&>(t1); 
       ATH_MSG_INFO(" minBiasRMS, old new "<<t2.m_MinBiasRMS<<" " <<noise_p);
       t2.m_MinBiasRMS = noise_p; 

       LArNoiseComplete::LArCondObj& u0 = const_cast<LArNoiseComplete::LArCondObj&>(noise->get(hid,0)); 
       if(u0.m_Noise!=noise_h) 
	 ATH_MSG_INFO(" noise, old new "<<u0.m_Noise<<" " <<noise_h);
       u0.m_Noise=noise_h; 

       LArNoiseComplete::LArCondObj& u1 = const_cast<LArNoiseComplete::LArCondObj&>(noise->get(hid,1)); 
       if(u1.m_Noise!=noise_m) 
	 ATH_MSG_INFO(" noise, old new "<<u1.m_Noise<<" " <<noise_m);
       u1.m_Noise=noise_m; 

       LArNoiseComplete::LArCondObj& u2 = const_cast<LArNoiseComplete::LArCondObj&>(noise->get(hid,2)); 
       if(u2.m_Noise!=noise_l) 
	 ATH_MSG_INFO(" noise, old new "<<u2.m_Noise<<" " <<noise_l);
       u2.m_Noise=noise_l; 
       
     }

     ATH_MSG_INFO(" number of channels in file ="<<n);

     return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::updateMinBias(const std::string& filename, const LArOnOffIdMapping *cabling) {

   ATH_MSG_INFO ( " in updateMinBias(), filename =  "<<filename );

   // this method updates the EM MinBias
   // input is the text file.

   const LArMinBiasMC * minbias_c = nullptr;
   ATH_CHECK( detStore()->retrieve(minbias_c) );
   LArMinBiasMC* minbias = const_cast<LArMinBiasMC*>(minbias_c);

   // read in the file

   ATH_MSG_INFO(" opening file "<< filename );
   std::ifstream infile( filename.c_str() );
   if(! infile.good() ) 
     {
       ATH_MSG_ERROR(" fail to open file ");
       return StatusCode::FAILURE; 
     }
  
   int lar,tp,det,samp,reg,eta,phi; 
   //tp=1 for EM, 2=HEC 

   float value ; 
   int n = 0; 
   while ( infile>>lar>>tp>>det>>samp>>reg>>eta>>phi>>value )
    {
	++n; 
	ATH_MSG_INFO(" det,samp,reg,eta,value="
                     <<det<<" " 
                     <<samp<<" " 
                     <<reg<<" " 
                     <<eta<<" " 
                     <<value<<" " );

	Identifier id ; 
	if ( det==1 && samp==1 && reg==0 && eta==0 ){
	   // eta=0 for strip, not connected, but keep it
           // Removed disconnected channels: 03/2016 RDS 
           // ATH_MSG_INFO(" disconnected strip ");
	   // id = m_em_idhelper->disc_channel_id(det,samp,reg,eta,0); 
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
		  ATH_MSG_ERROR(" unknown type "<<tp);
		  continue;
		}
	  }

	HWIdentifier hid = cabling->createSignalChannelID(id);
	const LArMinBiasComplete::LArCondObj & t = minbias->get(hid,0); 
	ATH_MSG_INFO(" online id = "<<m_online_idhelper->print_to_string(hid));

	if( t.isEmpty() ) 
	{
	  ATH_MSG_ERROR(" No existing conditions data " );
          ATH_MSG_ERROR( m_em_idhelper->print_to_string(id) );
	  // return StatusCode::FAILURE ;
	  continue ; 
	}
	ATH_MSG_INFO(" Old MinBias = "<< t.m_MinBiasRMS<< " " );

	LArMinBiasComplete::LArCondObj& t2 = const_cast<LArMinBiasComplete::LArCondObj&>(t); 
	t2.m_MinBiasRMS = value ; 

	const LArMinBiasComplete::LArCondObj & t3 = minbias->get(hid,0); 
	ATH_MSG_INFO(" New MinBias = "<< t3.m_MinBiasRMS<< " " );

    }

   ATH_MSG_INFO("  Number of entries changes =  " <<n );
   return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix6(const LArOnOffIdMapping *cabling) {

    ATH_MSG_INFO ( " in fix6() " );

    // update FCAL noise and gain
    // Fix6 is for updating the FCAL noise and ADC2DAC, uA2MeV and DAC2uA.

    const LArNoiseMC * noise_c = nullptr;
    ATH_CHECK( detStore()->retrieve(noise_c) );
    LArNoiseMC* noise = const_cast<LArNoiseMC*>(noise_c);

    const LArRampMC * ramp_c = nullptr;
    ATH_CHECK( detStore()->retrieve(ramp_c) );
    LArRampMC* ramp = const_cast<LArRampMC*>(ramp_c);

    const LAruA2MeVMC * ua2MeV_c = nullptr;
    ATH_CHECK( detStore()->retrieve(ua2MeV_c) );
    LAruA2MeVMC* ua2MeV = const_cast<LAruA2MeVMC*>(ua2MeV_c);
    
    const LArDAC2uAMC * dac2uA_c = nullptr;
    ATH_CHECK( detStore()->retrieve(dac2uA_c) );
    LArDAC2uAMC* dac2uA = const_cast<LArDAC2uAMC*>(dac2uA_c);
    
    int ndisc=0;
    int nconn=0;
    for( unsigned int gain=0;gain<3;++gain)
      {
	// loop over all the old data
	LArNoiseMC::ConditionsMapIterator  it = noise->begin(gain);
	LArNoiseMC::ConditionsMapIterator  it_e = noise->end(gain);
	
	for (; it!=it_e;++it)
	  {
	    HWIdentifier hid = it.channelId(); 
	    if( (*it).isEmpty() ) continue;  

	    Identifier id = cabling->cnvToIdentifier(hid);
	    if(!m_fcal_idhelper->is_lar_fcal(id)) continue;

	    if(cabling->isOnlineConnected(hid)){
	      ++nconn;
	      continue;
	    }else
	      {
		ATH_MSG_ERROR(" unconnected channel" 
                              << m_online_idhelper->print_to_string(hid) );

		++ndisc ; 
	      }
	    
	  }

	ATH_MSG_INFO("  Gain="<<gain<<
                     " Number of connected and disconnected =  " <<nconn<<" " <<ndisc );
	
      }
    

    int n=0;

    std::string filename("FCal_noise_minbias_adc2mev.txt");
    std::ifstream infile(filename.c_str() ) ; 

    if(! infile.good() ) 
      {
	ATH_MSG_ERROR(" fail to open file "<<filename);
	return StatusCode::FAILURE; 
     }
    

   char s[200];

   ATH_MSG_INFO("Opened FCAL file");
   for(int i = 0;i<27;++i){
     infile.getline(s,200);
     ATH_MSG_INFO(s);
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
	 ATH_MSG_INFO(" skipping string"<<str_id);
         continue;  
       }

       int mod,phi,eta;
       sscanf(ch_id, "A%d.%d.%d", &mod,&phi,&eta);
       if(phi>7){
          ATH_MSG_INFO(" skipping phi"<<str_id<<" phi="<<phi);
          continue; 
       }

       ++n;

       ATH_MSG_INFO(" Setting channel "<<str_id);
       Identifier id = m_fcal_idhelper->channel_id(2,mod,eta,phi); 
       HWIdentifier hid = cabling->createSignalChannelID(id);

       const LArNoiseComplete::LArCondObj& noise0 = noise->get(hid,0) ;
       LArNoiseComplete::LArCondObj& u0 = const_cast<LArNoiseComplete::LArCondObj&>(noise0); 
       if(u0.m_Noise!=noise_h) 
	 ATH_MSG_INFO(" noise, old new "<<u0.m_Noise<<" " <<noise_h);
       u0.m_Noise=noise_h; 

       const LArNoiseComplete::LArCondObj& noise1 = noise->get(hid,1) ;
       LArNoiseComplete::LArCondObj& u1 = const_cast<LArNoiseComplete::LArCondObj&>(noise1); 
       if(u1.m_Noise!=noise_m) 
	 ATH_MSG_INFO(" noise, old new "<<u1.m_Noise<<" " <<noise_m);
       u1.m_Noise=noise_m; 

       const LArNoiseComplete::LArCondObj& noise2 = noise->get(hid,2) ;
       LArNoiseComplete::LArCondObj& u2 = const_cast<LArNoiseComplete::LArCondObj&>(noise2); 
       if(u2.m_Noise!=noise_l) 
	 ATH_MSG_INFO(" noise, old new "<<u2.m_Noise<<" " <<noise_l);
       u2.m_Noise=noise_l; 
       
       LArRampComplete::LArCondObj& ramp0 = const_cast<LArRampComplete::LArCondObj&>(ramp->get(hid,0)); 
       LArRampComplete::LArCondObj& ramp1 = const_cast<LArRampComplete::LArCondObj&>(ramp->get(hid,1)); 
       LArRampComplete::LArCondObj& ramp2 = const_cast<LArRampComplete::LArCondObj&>(ramp->get(hid,2)); 

       ramp0.m_vRamp={0., ramp_val};

       ramp1.m_vRamp={0., ramp_val/(float)9.96};

       ramp2.m_vRamp={0., ramp_val/(float)9.96/(float)9.67};

       LAruA2MeVComplete::LArCondObj& ua2MeV0 = const_cast<LAruA2MeVComplete::LArCondObj&>(ua2MeV->get(hid,0)); 
       ua2MeV0.m_data=ua2MeV_val; 

       LArDAC2uAComplete::LArCondObj& dac2ua0 = const_cast<LArDAC2uAComplete::LArCondObj&>(dac2uA->get(hid,0)); 
       dac2ua0.m_data = dac2uA_val; 

     }

     ATH_MSG_INFO(" number of channels in file ="<<n);
     return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix7(const LArOnOffIdMapping *cabling) {

    ATH_MSG_INFO ( " in fix7() " );
    std::string filename ;

    // updating fSampl from 13.0.30 for QGSP_BERT
    if( m_g4Phys=="QGSP_BERT"){
      filename = "fsampl_1303_qgsp-bert.txt";
    }
    else {
      ATH_MSG_ERROR ( " wrong option  "<<m_g4Phys );
      return StatusCode::FAILURE; 
    }

    ATH_CHECK( updateEMfSampl(filename, cabling ) );
    return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::fix8(const LArOnOffIdMapping *cabling) {

    ATH_MSG_INFO ( " in fix8() " );
    // update EM/HEC 7 data objects.

    ATH_CHECK( addMphysOverMcal(cabling) );

    //                                                          withGain , nvar
    ATH_CHECK( update_EM_HEC<LArNoiseMC>("noise_em.txt","noise_hec.txt", cabling, true, 1 ) );
    ATH_CHECK( update_EM_HEC<LArAutoCorrMC>("","autocorr_hec.txt", cabling, true, 4 ) );

    ATH_CHECK( update_EM_HEC<LArAutoCorrMC>("autocorr_em.txt","", cabling, true,5) );
    ATH_CHECK( update_EM_HEC<LArRampMC>("ramp_em.txt","ramp_hec.txt", cabling, true, 1 ) );
    ATH_CHECK( update_EM_HEC<LAruA2MeVMC>("ua2mev_em.txt","ua2mev_hec.txt", cabling, false, 1 ) );
    ATH_CHECK( update_EM_HEC<LArDAC2uAMC>("dac2ua_em.txt","dac2ua_hec.txt", cabling, false, 1 ) );
    ATH_CHECK( update_EM_HEC<LArShape32MC>("shape_em.txt","shape_hec.txt", cabling, true, 64 ) );
    ATH_CHECK( update_EM_HEC<LArMphysOverMcalMC>("mphys_em.txt","", cabling, true, 1 ) );

    // additional fix for LArDAC2uA and LAruA2MeV
    ATH_CHECK( fixDACuAMeV() );
    return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::ReadFile(const std::string& filename, const LArOnOffIdMapping *cabling, bool EM, bool withGain, int nvar ) {

   std::ifstream infile(filename.c_str() ) ; 

   if(! infile.good() ) 
     {
       ATH_MSG_ERROR(" fail to open file "<<filename );
       return StatusCode::FAILURE; 
     }

   ATH_MSG_INFO(" Opened file "<<filename );

   m_cache[0].clear();
   m_cache[1].clear();
   m_cache[2].clear();

   char s[200];
   infile.getline(s,200);
   ATH_MSG_INFO(" first line of the file  "<<s);

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
	ATH_MSG_DEBUG(" det,samp,reg,eta,values="
                      <<det<<" " 
                      <<samp<<" " 
                      <<reg<<" " 
                      <<eta<<" "  );

	Identifier id ; 
	if (EM)
	{
	  if ( det==1 && samp==1 && reg==0 && eta==0 ){
	   // eta=0 for strip, not connected, but keep it
           // Removed disconnected channels: 03/2016 RDS 
           ATH_MSG_DEBUG(" disconnected strip ");
	  }else
	    id = m_em_idhelper->channel_id(det,samp,reg,eta,0); 
	}else //HEC
	  {
	    id = m_hec_idhelper->channel_id(2,samp,reg,eta,0); 
	  }
	HWIdentifier hid = cabling->createSignalChannelID(id);

        if (gain >= 0 && gain < 3)
          m_cache[gain].push_back(ROW_t(hid,vfl));
     }

   return StatusCode::SUCCESS; 
}

StatusCode FixLArElecCalib::ReadFileAll(const std::string& filename, const LArOnOffIdMapping *cabling, bool withGain, int nvar ) {

   std::ifstream infile(filename.c_str() ) ; 

   if(! infile.good() ) 
     {
       ATH_MSG_ERROR(" fail to open file "<<filename );
       return StatusCode::FAILURE; 
     }

   ATH_MSG_INFO(" Opened file "<<filename );

   m_cache[0].clear();
   m_cache[1].clear();
   m_cache[2].clear();

   char s[200];
   infile.getline(s,200);
   ATH_MSG_INFO(" first line of the file  "<<s);

   int   det,samp,reg,eta;
   int   n = 0;
   int first; 
   while ( infile>>first>>samp>>reg>>eta ) {
       Identifier id ; 
       int gain=0;
       float x;
       if(first <= 2) { // Calo 0,1,2 is EM
          det = first+1;
	  if ( det==1 && samp==1 && reg==0 && eta==0 ){
	   // eta=0 for strip, not connected, but keep it
           // Removed disconnected channels: 03/2016 RDS 
           ATH_MSG_DEBUG(" disconnected strip ");
	   // id = m_em_idhelper->disc_channel_id(det,samp,reg,eta,0); 
	  }else
	    id = m_em_idhelper->channel_id(det,samp,reg,eta,0); 
       } else if (first == 3) { // HEC
	  id = m_hec_idhelper->channel_id(2,samp,reg,eta,0); 
       } else if (first == 4) { // FCAL
          id = m_fcal_idhelper->channel_id(2,samp+1,eta,0);
       } else {
          ATH_MSG_WARNING(" Wrong calo number "<<first<<" skipping !!!");
          if(withGain) infile>>gain ; 
          for (int i=0;i<nvar;++i) infile>>x;
          continue;
       }
       if(withGain) infile>>gain ; 
       std::vector<float> vfl;
       
       for (int i=0;i<nvar;++i)
	 { 
	   infile>>x;
	   vfl.push_back(x);
	 }
        ++n;
	ATH_MSG_DEBUG(" first, det,samp,reg,eta,values="
                      <<first<<"  " <<det<<" " <<samp<<" " <<reg<<" " <<eta<<" "  );

	HWIdentifier hid = cabling->createSignalChannelID(id);

        if (gain >= 0 && gain < 3) m_cache[gain].push_back(ROW_t(hid,vfl));
        else {
           ATH_MSG_WARNING(" Wrong gain " <<gain<<" skipping !");
        }
     }// while 

   return StatusCode::SUCCESS; 
}

void FixLArElecCalib::print_object(const std::string& msg, const LArNoiseMC::LArCondObj& obj)
{
  if( obj.isEmpty()){
    ATH_MSG_INFO(" LArNoiseMC " << msg << " is empty" );
  }else 
    ATH_MSG_DEBUG(" LArNoiseMC" << msg << obj.m_Noise );
  return ; 
}

void FixLArElecCalib::set_object(LArNoiseMC::LArCondObj& obj, const std::vector<float>& v )
{
  if (v.size()!=1)
    {
      ATH_MSG_ERROR ("for LArNoiseMC, size of vector = "<< v.size() <<" differs from one");
      return ; 
    }
  obj.m_Noise = v[0]; 
  return;
} 

void FixLArElecCalib::print_object(const std::string& s, const LArAutoCorrMC::LArCondObj& obj)
{
  if( obj.isEmpty()){
    ATH_MSG_INFO(" LArAutoCorrMC " << s << " is empty" );
  }else 
    {
      msg()<<MSG::DEBUG<<" LArAutoCorrMC" << s ;
      for (unsigned int i =0 ; i<obj.m_vAutoCorr.size();++i)
	msg()<< " " << obj.m_vAutoCorr[i];
      msg()<<endmsg; 
    }
  return ; 
}

void FixLArElecCalib::set_object(LArAutoCorrMC::LArCondObj& obj, const std::vector<float>& v )
{
  if (v.size()<4 )
    {
      ATH_MSG_ERROR( "for LArAutoCorrMC, size of vector = "<< v.size()<<" is less then 4" );
      return ; 
    }

  obj.m_vAutoCorr.resize(4);
  std::copy_n(v.begin(), 4, obj.m_vAutoCorr.begin());
  return;
} 

void FixLArElecCalib::print_object(const std::string& s, const LArRampMC::LArCondObj& obj)
{
  if( obj.isEmpty()){
    ATH_MSG_INFO(" LArRampMC " << s << " is empty" );
  }else 
    {
      msg()<<MSG::DEBUG<<" LArRampMC" << s ;
      for (unsigned int i =0 ; i<obj.m_vRamp.size();++i)
	msg()<< " " << obj.m_vRamp[i];
      msg()<<endmsg; 
    }
  return ; 
}

void FixLArElecCalib::set_object(LArRampMC::LArCondObj& obj, const std::vector<float>& v )
{
  if (v.size()!=1 )
    {
      ATH_MSG_ERROR ("for LArRampMC, size of vector = "<< v.size());
      return ; 
    }
  obj.m_vRamp.resize(3);
  obj.m_vRamp[0]=0;
  obj.m_vRamp[1]=v[0]; 
  obj.m_vRamp[2]=0; 

  return;
} 

void FixLArElecCalib::print_object(const std::string& s, const LArShape32MC::LArCondObj& obj)
{
  if( obj.isEmpty()){
    ATH_MSG_INFO(" LArShape32MC " << s << " is empty" );
  }else 
    {
      msg()<<MSG::DEBUG<<" LArShape" << s ;
      for (unsigned int i =0 ; i<obj.m_vShape.size();++i)
	msg()<< " " << obj.m_vShape[i];
      msg()<<endmsg; 

      msg()<<MSG::DEBUG<<" LArShapeDer" << s ;
      for (unsigned int i =0 ; i<obj.m_vShapeDer.size();++i)
	msg()<< " " << obj.m_vShapeDer[i];
      msg()<<endmsg; 
    }
  return ; 
}

void FixLArElecCalib::set_object(LArShape32MC::LArCondObj& obj, const std::vector<float>& v )
{
  
  if (v.size()!=64 && v.size()!=32 )
    {
      ATH_MSG_ERROR ("for LArShape32MC, size of vector = "<< v.size());
      return ; 
    }

  unsigned int size = 32; 
  obj.m_vShape.resize(size);
  obj.m_vShapeDer.resize(size);

  for(unsigned int i=0;i<size;++i){
    obj.m_vShape[i]=v[i];
    if(v.size() == 64 ) obj.m_vShapeDer[i]=v[i+size]; else obj.m_vShapeDer[i]=0.;
  }

  return;
} 





void FixLArElecCalib::print_object(const std::string& msg, const LArSingleFloatP& obj)
{
  if( obj.isEmpty()){
    ATH_MSG_INFO(" LArSingleFloatP " << msg << " is empty" );
  }else
    ATH_MSG_DEBUG(" LArSingleFloatP " << msg << obj.m_data );
  return ;
}

void FixLArElecCalib::set_object(LArSingleFloatP& obj, const std::vector<float>& v )
{
  if (v.size()!=1)
    {
      ATH_MSG_ERROR( "for LArSingleFloatP, size of vector = "<< v.size() );
      return ;
    }
  obj.m_data = v[0];
  return;
}




StatusCode FixLArElecCalib::addMphysOverMcal(const LArOnOffIdMapping *cabling) {
  // add the contain to DetStore.
  // fill 1 for all FCAL/HEC channels

    ATH_MSG_INFO ( " addMphysOverMcal() " );
    // create LArMphysOverMcal with FCAL numbers = 1.

    std::unique_ptr<LArMphysOverMcalMC> mphys = std::make_unique<LArMphysOverMcalMC>(); 
    mphys->setGroupingType(LArConditionsContainerBase::SingleGroup);
    ATH_CHECK( mphys->initialize() );

    int n=0;

    std::string filename("FCal_noise_minbias_adc2mev.txt");
 
    std::ifstream infile(filename.c_str() ) ; 

    if(! infile.good() ) 
      {
	ATH_MSG_ERROR(" fail to open file "<<filename);
	return StatusCode::FAILURE; 
     }
    

   char s[200];

   ATH_MSG_INFO("Opened FCAL file");
   for(int i = 0;i<27;++i){
     infile.getline(s,200);
     ATH_MSG_INFO(s);
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
	 ATH_MSG_INFO(" skipping string"<<str_id);
         continue;  
       }

       int mod,phi,eta;
       sscanf(ch_id, "A%d.%d.%d", &mod,&phi,&eta);
       if(phi>7){
          ATH_MSG_INFO(" skipping phi"<<str_id<<" phi="<<phi);
          continue; 
       }

       ++n;

       ATH_MSG_INFO(" Setting channel "<<str_id);
       Identifier id = m_fcal_idhelper->channel_id(2,mod,eta,phi); 
       HWIdentifier hid = cabling->createSignalChannelID(id);

       LArMphysOverMcalMC::LArCondObj t; 
       t.m_data =1. ;
       mphys->setPdata(hid,t,0);
       mphys->setPdata(hid,t,1);
       mphys->setPdata(hid,t,2);
     }
       
   ATH_MSG_INFO(" MphysOverMcal added  "<<n<<" FCAL channels");

   bool EM=false ;
   bool withGain=false; 
   int nvar = 1; 
   std::string hec_filename("mphys_hec.txt"); 
   ATH_CHECK( ReadFile(hec_filename,cabling,EM,withGain,nvar) );
   
   int igain=0;
   n=0; 
   for( ROW_t &it : m_cache[igain])
     {
       HWIdentifier hid = it.first;
       LArMphysOverMcalMC::LArCondObj t; 
       t.m_data =1. ;
       mphys->setPdata(hid,t,0);
       mphys->setPdata(hid,t,1);
       mphys->setPdata(hid,t,2);
       ++n; 

     }

   ATH_CHECK( detStore()->record(std::move(mphys),"LArMphysOverMcal") );
   ILArMphysOverMcal* imphys=nullptr;
   ATH_CHECK (detStore()->symLink(mphys.get(),imphys) );

   ATH_MSG_ERROR(" Number of HEC channel added "<<n <<" per gain ");
   return StatusCode::SUCCESS ;
}


StatusCode FixLArElecCalib::fixDACuAMeV()
{
   // the old data has -10000000 for invalid. change it to {ERRORCODE = -999};

   const LArDAC2uAMC * dac2ua_c = nullptr;
   ATH_CHECK( detStore()->retrieve(dac2ua_c) );
   LArDAC2uAMC* dac2uaMC = const_cast<LArDAC2uAMC*>(dac2ua_c);

   const LAruA2MeVMC * ua2mev_c = nullptr;
   ATH_CHECK( detStore()->retrieve(ua2mev_c) );
   LAruA2MeVMC* ua2mevMC = const_cast<LAruA2MeVMC*>(ua2mev_c);

   int n=0;

   LAruA2MeVMC::ConditionsMapIterator it = ua2mevMC->begin(0);
   LAruA2MeVMC::ConditionsMapIterator it_e = ua2mevMC->end(0);
   
   for(;it!=it_e;++it){
     HWIdentifier hid = it.channelId();
     const LAruA2MeVComplete::LArCondObj & u = (*it);
     
     if( u.m_data> -990)
       {
	 ATH_MSG_DEBUG(" ua2MeV channel OK "<<u.m_data );
	 continue ; 
       }
     
     ATH_MSG_DEBUG(" Old uA2MeV = "<< u.m_data<< " " );
     
     LAruA2MeVComplete::LArCondObj& u2 = const_cast<LAruA2MeVComplete::LArCondObj&>(u); 
     u2.m_data= ILAruA2MeV::ERRORCODE ;
     
     const LAruA2MeVComplete::LArCondObj & u3 = ua2mevMC->get(hid,0); 
     ATH_MSG_DEBUG(" New uA2MeV = "<< u3.m_data<< " " );
     
     ++n;
   }

   ATH_MSG_DEBUG("  Number of uA2MeV entries changes =  " <<n );

   for(unsigned int igain=0;igain<3;++igain)
     {
       LArDAC2uAMC::ConditionsMapIterator it = dac2uaMC->begin(igain);
       LArDAC2uAMC::ConditionsMapIterator it_e = dac2uaMC->end(igain);
       
       n=0;
       for( ;it!=it_e;++it){
	 HWIdentifier hid = it.channelId();
	 const LArDAC2uAMC::LArCondObj & u = (*it);
	 
	 if( u.m_data> -990.)
	   {
	     ATH_MSG_DEBUG(" DAC2uA channel OK "<<u.m_data );
	     continue ; 
	   }
	 
	 ATH_MSG_DEBUG(" Old DAC2uA = "<< u.m_data<< " " );
	 
	 LArDAC2uAMC::LArCondObj& u2 = const_cast<LArDAC2uAMC::LArCondObj&>(u); 
	 u2.m_data = ILArDAC2uA::ERRORCODE ;
	 
	 const LArDAC2uAMC::LArCondObj & u3 = dac2uaMC->get(hid,igain); 
	 ATH_MSG_DEBUG(" New DACuA2 = "<< u3.m_data << " " );
	 ++n;
       }
       
     }   
   
   ATH_MSG_DEBUG("  Number of DAC2uA entries changes =  " <<n );
   return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix9(const LArOnOffIdMapping *cabling) {

    ATH_MSG_INFO ( " in fix9(), replace FCAL AutoCorr " );

    std::vector<float> fcal_autoCorr[3][3] ; //[module][gain]
    fcal_autoCorr[0][0] = {-0.01, -0.30, -0.09, 0.08};
    fcal_autoCorr[0][1] = { 0.01, -0.07, -0.01, 0.03};
    fcal_autoCorr[0][2] = { 0.2, 0.2, 0.2, 0.2};
    fcal_autoCorr[1][0] = { -0.02, -0.31, -0.06, 0.06};
    fcal_autoCorr[1][1] = { 0.0, -0.08, -0.01, 0.02};
    fcal_autoCorr[1][2] = { 0.02, 0.01, 0.02, 0.02};
    fcal_autoCorr[2][0] = { -0.03, -0.31, -0.05, 0.07};
    fcal_autoCorr[2][1] = { 0.01, -0.07, -0.01, 0.02};
    fcal_autoCorr[2][2] = { 0.02, 0.02, 0.02, 0.02};

    // Fix9 is for updating the FCAL AutoCorr conditions data 
    // Input should be MC Conditions data with DetDescrVersion=ATLAS-CSC-02-00-00

    const LArAutoCorrMC * ac_c = nullptr;
    ATH_CHECK( detStore()->retrieve(ac_c) );
    LArAutoCorrMC* ac = const_cast<LArAutoCorrMC*>(ac_c);
    
    
    for( unsigned int gain=0;gain<3;++gain)
      {
	// loop over all the old data
	LArAutoCorrMC::ConditionsMapIterator  it = ac->begin(gain);
	LArAutoCorrMC::ConditionsMapIterator  it_e = ac->end(gain);
	
	int n=0;
	for (; it!=it_e;++it)
	  {
	    HWIdentifier hid = it.channelId(); 
	    const LArAutoCorrMC::LArCondObj & u = (*it);
	    if( (*it).isEmpty() ) continue;  
	     
	    Identifier id = cabling->cnvToIdentifier(hid);
	    if(! m_fcal_idhelper->is_lar_fcal(id)) continue ;
	    int module = m_fcal_idhelper->module(id) - 1;
	    
	    ATH_MSG_DEBUG("module= " <<  module << " Old AutoCorr = "
		 << u.m_vAutoCorr[0]<< " "
		 << u.m_vAutoCorr[1]<< " "
		 << u.m_vAutoCorr[2]<< " "
		 << u.m_vAutoCorr[3]<< " " );
	       
	    LArAutoCorrMC::LArCondObj& u2 = const_cast<LArAutoCorrMC::LArCondObj&>(u); 
	    u2.m_vAutoCorr = fcal_autoCorr[module][gain] ; 
	       
	    const LArAutoCorrMC::LArCondObj & u3 = ac->get(hid,gain); 
	    ATH_MSG_DEBUG(" New AutoCorr = "
		 << u3.m_vAutoCorr[0]<< " "
		 << u3.m_vAutoCorr[1]<< " "
		 << u3.m_vAutoCorr[2]<< " "
		 << u3.m_vAutoCorr[3]<< " " );
	      ++n;

	  }
        ATH_MSG_DEBUG("  Gain="<<gain<<
                      " Number of entries modified  =  " <<n );
      }
    
    return StatusCode::SUCCESS;    
}


StatusCode FixLArElecCalib::fix10(const LArOnOffIdMapping *cabling) {

    ATH_MSG_INFO ( " in fix10() " );
    // fix medium and low gain ramp

    const LArRampMC * ramp_c = nullptr;
    ATH_CHECK( detStore()->retrieve(ramp_c) );
    LArRampMC* ramp = const_cast<LArRampMC*>(ramp_c);
    
    LArRampMC::ConditionsMapIterator  it = ramp->begin(0);
    LArRampMC::ConditionsMapIterator  it_e = ramp->end(0);
	
    int n=0;
    int ndisc=0;
    for (; it!=it_e;++it)
	  {
	    HWIdentifier hid = it.channelId(); 
	    if( (*it).isEmpty() ) continue;  

	    Identifier id = cabling->cnvToIdentifier(hid);
	    if(!m_fcal_idhelper->is_lar_fcal(id)) continue;

	    if( ! cabling->isOnlineConnected(hid)){
              ATH_MSG_ERROR(" unconnected channel" 
                            << m_online_idhelper->print_to_string(hid) );
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

	    ATH_MSG_DEBUG(" ramp hi,med,low"<< ramp0.m_vRamp[1]<< " "<<ramp1.m_vRamp[1]<<  " " <<
                          ramp2.m_vRamp[1]);
	    
	  }

    ATH_MSG_INFO( " Number of channels updted =  " <<n );
    ATH_MSG_INFO( " Number of disconnected =  " <<ndisc );
    
    return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix11(const LArOnOffIdMapping *cabling) {

    ATH_MSG_INFO ( " in fix8() " );
    // update EM/HEC 7 data objects.

    ATH_CHECK( update_EM_HEC<LArRampMC>("","ramp_hec_june2008.txt", cabling, true, 1 ) );
    return StatusCode::SUCCESS;
}


StatusCode FixLArElecCalib::fix13(const LArOnOffIdMapping *cabling) {

    ATH_MSG_INFO ( " in fix13() " );
    // update EM/HEC 7 data objects.

    ToolHandle<ICaloSuperCellIDTool> scidtool("CaloSuperCellIDTool");
    if ( scidtool.retrieve().isFailure() ) {
      ATH_MSG_ERROR ( " Could not retrieve scitool " );
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
        if ( ! isEM ) continue;
        Identifier SCID = scidtool->offlineToSuperCellID(chid);
	if ( scidset.find(SCID) == scidset.end() && (m_scell_idhelper->calo_cell_hash( SCID ).value() < 99999) )
		scidset.insert(SCID);
	else continue;
	HWIdentifier hwid = cabling->createSignalChannelID(chid);
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

    of1 << "Off ID\t\tSCID\t\tOnl ID\t\tFT\tslot\tB-E pos_neg\tSamp\teta\tphi\tFEB_ID\t\tSHWID\t" << std::endl;
    for(size_t i = 0; i < channel_size ; i++) {
        Identifier chid = m_hec_idhelper->channel_id(i);
        if ( ! ( m_hec_idhelper->is_lar_hec( chid )  ) ) continue;
        Identifier SCID = scidtool->offlineToSuperCellID(chid);
        if ( scidset.find(SCID) == scidset.end() )
                scidset.insert(SCID);
        else continue;
        HWIdentifier hwid = cabling->createSignalChannelID(chid);
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

    of3 << "Off ID\t\tSCID\t\tOnl ID\t\tFT\tslot\tB-E pos_neg\tSamp\teta\tphi\tFEB_ID\t\tSHWID\t" << std::endl;
    for(size_t i = 0; i < channel_size ; i++) {
        Identifier chid = m_fcal_idhelper->channel_id(i);
        if ( ! ( m_fcal_idhelper->is_lar_fcal( chid )  ) ) continue;
        Identifier SCID = scidtool->offlineToSuperCellID(chid);
        if ( scidset.find(SCID) == scidset.end() )
                scidset.insert(SCID);
        else continue;
        HWIdentifier hwid = cabling->createSignalChannelID(chid);
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
    coral::AttributeListSpecification *spec_onOff = new coral::AttributeListSpecification();
    spec_onOff->extend("OnlineHashToOfflineId", "blob");
    spec_onOff->extend<unsigned>("version");
    std::unique_ptr<AthenaAttributeList> al_onOff = std::make_unique<AthenaAttributeList>(*spec_onOff);
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

    ATH_MSG_INFO ( "HashMax=" << onlHashMax << ", connected=" << nConn );

    ATH_CHECK( detStore()->record(std::move(al_onOff),"/LAR/IdentifierOfl/OnOffIdMap_SC") );

    hashes << "idx \t Off2OnlId \t Onl2OffId" << std::endl;
    for(size_t ii=0;ii<40000;ii++)
      if ( (OrderOffHash_OnlID[ii] != 0) || ( OrderOnlHash_OffID[ii] != 0 ) )
	hashes << std::dec << ii << std::hex << " \t " << OrderOffHash_OnlID[ii]  << " \t\t " << OrderOnlHash_OffID[ii]  << std::endl;
    hashes.close();

    return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::fix12(const LArOnOffIdMapping *cabling) {
    
   ATH_MSG_INFO ( " in fix12() " );

   // Fix12 is for filling the  MinBiasRMS and MinBiasAverage  from ntuple

   // Will try to regenerate from scratch
   // Create new container 
   std::unique_ptr<LArMinBiasMC> minbias = std::make_unique<LArMinBiasMC>();
   ATH_CHECK( minbias->setGroupingType("Single", msg()) );
   ATH_CHECK( minbias->initialize() );
   std::unique_ptr<LArMinBiasAverageMC> minbias_av = std::make_unique<LArMinBiasAverageMC>();
   ATH_CHECK( minbias_av->setGroupingType("Single", msg()) );
   ATH_CHECK( minbias_av->initialize() );
   //
   std::unique_ptr<TFile> fin= std::make_unique<TFile>("ntuple_av.root");
   TTree *tin=dynamic_cast<TTree*>(fin->Get("m_tree"));
   int           ncell;
   int           identifier[2862];   
   int           layer[2862];   
   int           region[2862]; 
   int           ieta[2862];   
   float         eta[2862];   
   double        average[2862];   
   double        rms[2862];   
   TBranch        *b_ncell;   //!
   TBranch        *b_identifier;   //!
   TBranch        *b_layer;   //!
   TBranch        *b_region;   //!
   TBranch        *b_ieta;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_average;   //!
   TBranch        *b_rms;   //!
   tin->SetMakeClass(1);
   tin->SetBranchAddress("ncell", &ncell, &b_ncell);
   tin->SetBranchAddress("identifier", identifier, &b_identifier);
   tin->SetBranchAddress("layer", layer, &b_layer);
   tin->SetBranchAddress("region", region, &b_region);
   tin->SetBranchAddress("ieta", ieta, &b_ieta);
   tin->SetBranchAddress("eta", eta, &b_eta);
   tin->SetBranchAddress("average", average, &b_average);
   tin->SetBranchAddress("rms", rms, &b_rms);
   tin->GetEntry(0);


   for(int icell=0; icell<ncell; ++icell)  {

       Identifier32 id32(identifier[icell]); 
       Identifier id(id32);
       HWIdentifier hid = cabling->createSignalChannelID(id);

       minbias->set(hid, rms[icell]);
       minbias_av->set(hid, average[icell]);

     }

     ATH_MSG_INFO(" number of channels in intuple ="<<ncell);

     ATH_CHECK( detStore()->record(std::move(minbias),"LArMinBias") );
     ATH_CHECK( detStore()->symLink(minbias.get(), dynamic_cast<ILArMinBias*>(minbias.get())) );
     ATH_MSG_INFO ( "Stored container " << minbias->totalNumberOfConditions() << " conditions, key LArMinBias " );
     ATH_CHECK( detStore()->record(std::move(minbias_av),"LArMinBiasAverage") );
     ATH_CHECK( detStore()->symLink(minbias_av.get(), dynamic_cast<ILArMinBiasAverage*>(minbias_av.get())) );
     ATH_MSG_INFO ( "Stored container " << minbias_av->totalNumberOfConditions() << " conditions, key LArMinBiasAverage " );

     return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::fix14(const LArOnOffIdMapping *cabling) {
    
    ATH_MSG_INFO ( " in fix14() " );

    // Fix14 is for filling the  LArPileupAverage  from ntuple

    const ILArfSampl* larfSampl = nullptr;
    ATH_CHECK( detStore()->retrieve(larfSampl,"") );
    // Will try to regenerate from scratch
   // Create new container 
   std::unique_ptr<LArMinBiasAverageMC> minbias_av = std::make_unique<LArMinBiasAverageMC>();
   ATH_CHECK( minbias_av->setGroupingType("Single", msg()) );
   ATH_CHECK( minbias_av->initialize() );
   //
   std::unique_ptr<TFile> fin= std::make_unique<TFile>("ntuple_av.root");
   TTree *tin=dynamic_cast<TTree*>(fin->Get("m_tree"));
   int           ncell;
   int           identifier[2862];   
   int           layer[2862];   
   int           region[2862]; 
   int           ieta[2862];   
   float         eta[2862];   
   double        average[2862];   
   double        rms[2862];   
   TBranch        *b_ncell;   //!
   TBranch        *b_identifier;   //!
   TBranch        *b_layer;   //!
   TBranch        *b_region;   //!
   TBranch        *b_ieta;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_average;   //!
   TBranch        *b_rms;   //!
   tin->SetMakeClass(1);
   tin->SetBranchAddress("ncell", &ncell, &b_ncell);
   tin->SetBranchAddress("identifier", identifier, &b_identifier);
   tin->SetBranchAddress("layer", layer, &b_layer);
   tin->SetBranchAddress("region", region, &b_region);
   tin->SetBranchAddress("ieta", ieta, &b_ieta);
   tin->SetBranchAddress("eta", eta, &b_eta);
   tin->SetBranchAddress("average", average, &b_average);
   tin->SetBranchAddress("rms", rms, &b_rms);
   tin->GetEntry(0);


   for(int icell=0; icell<ncell; ++icell)  {

       Identifier32 id32(identifier[icell]); 
       Identifier id(id32);
       HWIdentifier hid = cabling->createSignalChannelID(id);
       float fsampl = larfSampl->FSAMPL(hid);
       minbias_av->set(hid, 6.31*average[icell]/fsampl);

     }

     ATH_MSG_INFO(" number of channels in intuple ="<<ncell);

     ATH_CHECK( detStore()->record(std::move(minbias_av),"LArPileupAverage") );
     ATH_CHECK( detStore()->symLink(minbias_av.get(), dynamic_cast<ILArMinBiasAverage*>(minbias_av.get())) );
     ATH_MSG_INFO ( "Stored container " << minbias_av->totalNumberOfConditions() << " conditions, key LArPileupAverage " );

     return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::fix15() {

   ATH_MSG_INFO ( " in fix15  " );

   // this method updates the EM MinBias
   // input is the text file.

   const LArMinBiasAverageMC * minbias_c = nullptr;
   ATH_CHECK( detStore()->retrieve(minbias_c,"LArPileupAverage") );
   LArMinBiasAverageMC* minbias = const_cast<LArMinBiasAverageMC*>(minbias_c);


   // here some iterator loop is needed
   int n=0;

   LArMinBiasAverageMC::ConditionsMapIterator it = minbias->begin(0);
   LArMinBiasAverageMC::ConditionsMapIterator it_e = minbias->end(0);
   
   for(;it!=it_e;++it){
     HWIdentifier hid = it.channelId();
     const LArMinBiasAverageComplete::LArCondObj & u = (*it);
     
     ATH_MSG_INFO(" Old pileup = "<< u.m_MinBiasAverage<< " " );
     
     LArMinBiasAverageComplete::LArCondObj& u2 = const_cast<LArMinBiasAverageComplete::LArCondObj&>(u); 
     
     u2.m_MinBiasAverage *= 1.0838; // correction value


     const LArMinBiasAverageComplete::LArCondObj & t3 = minbias->get(hid,0); 
     ATH_MSG_INFO(" New Pileup = "<< t3.m_MinBiasAverage<< " " );

    }

   ATH_MSG_INFO("  Number of entries changes =  " <<n );
   return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::fix16() {

    ATH_MSG_INFO ( " in fix16() " );

    std::string onOffIdKey="/LAR/Identifier/OnOffIdMap";
    const AthenaAttributeList *attrOnOff=nullptr;   
    StatusCode sc=detStore()->retrieve(attrOnOff,onOffIdKey);
    if (sc.isFailure()) {
         ATH_MSG_ERROR("Failed to read AthenaAttributeList with key " << onOffIdKey);
         return StatusCode::FAILURE;
    }
    const coral::Blob& blobOnOff=(*attrOnOff)["OnlineHashToOfflineId"].data<coral::Blob>();
    unsigned nChan=blobOnOff.size()/sizeof(uint32_t);
    uint32_t* pBlobOnOff=const_cast<uint32_t*>(static_cast<const uint32_t*>(blobOnOff.startingAddress()));


    if (nChan!=m_online_idhelper->channelHashMax()) {
          ATH_MSG_WARNING("Number of channels read from DB (" << nChan << ") does not match online hash max (" 
                 <<m_online_idhelper->channelHashMax() << ")");
          nChan=std::min(nChan,static_cast<unsigned int>(m_online_idhelper->channelHashMax()));
    }
    std::vector<std::pair<unsigned int, unsigned int> > swapindex(8, std::make_pair(nChan+10,nChan+10)); // here we store, which indexes to swap
    // loop over all online hashes
    for (unsigned i=0;i<nChan;++i) {
        const HWIdentifier hwid = m_online_idhelper->channel_Id(IdentifierHash(i));
        if(m_online_idhelper->isEMECchannel(hwid) && m_online_idhelper->pos_neg(hwid) == 1 && m_online_idhelper->feedthrough(hwid) == 7 && m_online_idhelper->slot(hwid) == 11) { //EMEC-A FT 7, Slot 11
           if(m_online_idhelper->channel(hwid)>=96 && m_online_idhelper->channel(hwid)<=103) swapindex[m_online_idhelper->channel(hwid)-96].first=i;
           if(m_online_idhelper->channel(hwid)>=112 && m_online_idhelper->channel(hwid)<=129) swapindex[m_online_idhelper->channel(hwid)-112].second=i;
        }
    }
    for(unsigned i=0; i<8; ++i) {
    }
    // now do swaping
    for(unsigned i=0; i<8; ++i) {
       // check consistency of swapindex
       if (swapindex[i].first > nChan || swapindex[i].second > nChan ) {
          ATH_MSG_ERROR("Problem in swapindex: "<< i << " : " << swapindex[i].first << " " << swapindex[i].second);
          return StatusCode::FAILURE;
       }
       uint32_t id = pBlobOnOff[swapindex[i].first];
       pBlobOnOff[swapindex[i].first] = pBlobOnOff[swapindex[i].second];
       pBlobOnOff[swapindex[i].second] = id;
    }
    // and now record the changed blob to SG
    ATH_CHECK(detStore()->record(const_cast<AthenaAttributeList *>(attrOnOff),"/LAR/Identifier/OnOffIdMapNew"));

    return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::fix17(const LArOnOffIdMapping *cabling) {

    ATH_MSG_INFO ( " in fix17() " );

    //                                                          withGain , nvar
    ATH_CHECK( update_All<LArShape32MC>(m_infile, cabling, true, 32 ) );

    return StatusCode::SUCCESS;
}

StatusCode FixLArElecCalib::fix18(const LArCalibLineMapping *cabling) {

    ATH_MSG_INFO ( " in fix18() " );

    const uint32_t onlHashMax=m_online_idhelper->channelHashMax();

    coral::AttributeListSpecification* spec_calib = new coral::AttributeListSpecification();
    spec_calib->extend("OnlineHashToCalibIds", "blob");
    spec_calib->extend<unsigned>("version");
    std::unique_ptr<AthenaAttributeList> al_calib = std::make_unique<AthenaAttributeList>(*spec_calib);
    coral::Blob& blobCalib=(*al_calib)["OnlineHashToCalibIds"].data<coral::Blob>();
    (*al_calib)["version"].setValue(0U);
    blobCalib.resize(onlHashMax*sizeof(uint32_t)*4); //Bigger than necessary 

    spec_calib->release();
    // cppcheck-suppress memleak
    spec_calib = nullptr;

    uint32_t* pBlobCalib=static_cast<uint32_t*>(blobCalib.startingAddress());
    size_t calibIndex=0;


    // loop over all online hashes
    for (unsigned i=0;i<onlHashMax;++i) {
        const HWIdentifier hwid = m_online_idhelper->channel_Id(IdentifierHash(i));

        const std::vector<HWIdentifier>& calibIDs=cabling->calibSlotLine(hwid);
        const size_t nCalibLines=calibIDs.size();

        if (m_online_idhelper->isEMBchannel(hwid) &&  m_online_idhelper->pos_neg(hwid) == 1 && m_online_idhelper->feedthrough(hwid) == 0 ) { // EMB-A FT 0
           if(nCalibLines > 1) { //should not be in our channels
              ATH_MSG_ERROR("Wrong number of CL in our channels");
              return StatusCode::FAILURE;
           }
           if(nCalibLines==0) {
             pBlobCalib[calibIndex++]=0;
           } else {
              const HWIdentifier cmodule = m_online_idhelper->calib_module_Id(calibIDs[0]);
              const HWIdentifier newcl = m_online_idhelper->calib_channel_Id(cmodule,  m_online_idhelper->channel(hwid) );       
              if(!m_online_idhelper->isCalibration(newcl)) {
                  ATH_MSG_ERROR("Something wrong, new CL not a calibration");
                  ATH_MSG_ERROR("Channel: "<<hwid.get_identifier32().get_compact()<<" old CL: "<< calibIDs[0].get_identifier32().get_compact()<<" new CL: "<<newcl.get_identifier32().get_compact());
                 return StatusCode::FAILURE;
              }
              ATH_MSG_INFO("Channel: "<<hwid.get_identifier32().get_compact()<<" old CL: "<< calibIDs[0].get_identifier32().get_compact()<<" new CL: "<<newcl.get_identifier32().get_compact());
              pBlobCalib[calibIndex++]=1;
              pBlobCalib[calibIndex++]=newcl.get_identifier32().get_compact();
           }
        } else {
           pBlobCalib[calibIndex++]=nCalibLines;
           for(uint32_t iCalib=0;iCalib<nCalibLines;++iCalib) 
             pBlobCalib[calibIndex++]=calibIDs[iCalib].get_identifier32().get_compact();
        }
    }
    blobCalib.resize(calibIndex*sizeof(uint32_t)); //Size down to actual size
    ATH_MSG_INFO( "BlobSize CalibId:" << calibIndex);

    // and now record the blob to SG
    ATH_CHECK(detStore()->record(std::move(al_calib),"/LAR/Identifier/CalibIdMap_EMF"));

    return StatusCode::SUCCESS;
}
