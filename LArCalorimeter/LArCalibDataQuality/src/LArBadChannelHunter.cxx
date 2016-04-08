/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibDataQuality/LArBadChannelHunter.h"

#include "LArElecCalib/ILArPedestal.h"
#include "LArRawConditions/LArCaliWaveContainer.h"
#include "LArRawConditions/LArWaveHelper.h"
#include "LArRawConditions/LArWave.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArTools/LArCablingService.h"
#include "LArRecConditions/LArBadChanBitPacking.h"
#include "StoreGate/StoreGateSvc.h"

//#include <sstream> 
#include <fstream>
#include <cmath>

LArBadChannelHunter::LArBadChannelHunter(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator),
  m_onlineId(0),
  m_caloId(0),
  m_badChannelTool("LArBadChanTool"),
  m_larCablingSvc("LArCablingService"), 
  m_avgType(FEB) {

  declareProperty("BadChannelTool",	m_badChannelTool, 	"BadChannelTool to be used");
  declareProperty("PedestalKey",	m_pedKey="", 		"Key of the pedestal container");
  declareProperty("CaliWaveKey",	m_caliWaveKey="",	"Key of the CaliWave container");

  declareProperty("OutFileName",	m_outFileName="",	"File for text output");
  declareProperty("OutOnlyNew",		m_outOnlyNew=false,	"remove all known bad channels in output list");
  declareProperty("AverageType",	m_avgTypeProp="FEB",	"Reference options: average over 'FEB' or 'PHI'");
  declareProperty("UndoCorrections",	m_undoCorr=false,	"go back to unpatched values for cali-waves");

  declareProperty("CutType",		m_cutType="SIG",	"Cut criteria: bad if deviation > X 'SIG' (sigma) or 'PER' (percent)");
  declareProperty("RecalcPer",		m_recalcPer=0.03,	"Condition cuts to do recalculation, <=0 means NO recalculation");


//////
  declareProperty("LowNoiseThresholdHG",m_lowNoiseTh[CaloGain::LARHIGHGAIN]=6.0,
		  "Channels with pedestal rms x sigma or percent above average are tagged 'low noise' (in high gain) ");
  declareProperty("LowNoiseThresholdMG",m_lowNoiseTh[CaloGain::LARMEDIUMGAIN]=6.0,
		  "Channels with pedestal rms x sigma or percent above average are tagged 'low noise' (in medium gain) ");
  declareProperty("LowNoiseThresholdLG",m_lowNoiseTh[CaloGain::LARLOWGAIN]=6.0,
		  "Channels with pedestal rms x sigma or percent above average are tagged 'low noise' (in low gain) ");

  declareProperty("HighNoiseThresholdHG",m_highNoiseTh[CaloGain::LARHIGHGAIN]=50.0,
		  "Channels with pedestal rms x sigma or percent above average are tagged 'High noise' (in high gain) ");
  declareProperty("HighNoiseThresholdMG",m_highNoiseTh[CaloGain::LARMEDIUMGAIN]=50.0,
		  "Channels with pedestal rms x sigma or percent above average are tagged 'High noise' (in medium gain) ");
  declareProperty("HighNoiseThresholdLG",m_highNoiseTh[CaloGain::LARLOWGAIN]=50.0,
		  "Channels with pedestal rms x sigma or percent above average are tagged 'High noise' (in low gain) ");

//////
  declareProperty("DeadThresholdAmpHG",m_amplTh[CaloGain::LARHIGHGAIN]=60.0,
		  "Channels with amplitudes x sigma or percent above the average are tagged as 'deadReadout' or 'deadCalib' (H gain)");
  declareProperty("DeadThresholdAmpMG",m_amplTh[CaloGain::LARMEDIUMGAIN]=60.0,
		  "Channels with amplitudes x sigma or percent above the average are tagged as 'deadReadout' or 'deadCalib' (M gain)");
  declareProperty("DeadThresholdAmpLG",m_amplTh[CaloGain::LARLOWGAIN]=60.0,
		  "Channels with amplitudes x sigma or percent above the average are tagged as 'deadReadout' or 'deadCalib' (L gain)");

  declareProperty("DistortThresholdAmpHG",m_distampTh[CaloGain::LARHIGHGAIN]=5.0,
		  "Channels with a width x sigma or percent above the average are tagged as 'distorted' (H gain)");
  declareProperty("DistortThresholdAmpMG",m_distampTh[CaloGain::LARMEDIUMGAIN]=5.0,
		  "Channels with a width x sigma or percent above the average are tagged as 'distorted' (M gain)");
  declareProperty("DistortThresholdAmpLG",m_distampTh[CaloGain::LARLOWGAIN]=5.0,
		  "Channels with a width x sigma or percent above the average are tagged as 'distorted' (L gain)");

//////
  declareProperty("DeadThresholdWidHG",m_widTh[CaloGain::LARHIGHGAIN]=100.0,
		  "Channels with a width x sigma or percent above the average are tagged as 'deadReadout' or 'deadCalib' (H gain)");
  declareProperty("DeadThresholdWidMG",m_widTh[CaloGain::LARMEDIUMGAIN]=100.0,
		  "Channels with a width x sigma or percent above the average are tagged as 'deadReadout' or 'deadCalib' (M gain)");
  declareProperty("DeadThresholdWidLG",m_widTh[CaloGain::LARLOWGAIN]=100.0,
		  "Channels with a width x sigma or percent above the average are tagged as 'deadReadout' or 'deadCalib' (L gain)");

  declareProperty("DistortThresholdWidHG",m_distwidTh[CaloGain::LARHIGHGAIN]=4.0,
		  "Channels with a width x sigma or percent above the average are tagged as 'distorted' (H gain)");
  declareProperty("DistortThresholdWidMG",m_distwidTh[CaloGain::LARMEDIUMGAIN]=4.0,
		  "Channels with a width x sigma or percent above the average are tagged as 'distorted' (M gain)");
  declareProperty("DistortThresholdWidLG",m_distwidTh[CaloGain::LARLOWGAIN]=4.0,
		  "Channels with a width x sigma or percent above the average are tagged as 'distorted' (L gain)");

//////
  declareProperty("DistortThresholdTmaxHG",m_tmaxampTh[CaloGain::LARHIGHGAIN]=5.0,
		  "Distorted with a TamxAmpl x sigma or percent above the average are tagged as 'distorted' (H gain)");
  declareProperty("DistortThresholdTmaxMG",m_tmaxampTh[CaloGain::LARMEDIUMGAIN]=5.0,
		  "Distorted with a TamxAmpl x sigma or percent above the average are tagged as 'distorted' (M gain)");
  declareProperty("DistortThresholdTmaxLG",m_tmaxampTh[CaloGain::LARLOWGAIN]=5.0,
		  "Distorted with a TamxAmpl x sigma or percent above the average are tagged as 'distorted' (L gain)");
}

//////
StatusCode LArBadChannelHunter::initialize() {
  if (m_avgTypeProp=="FEB") 
    m_avgType=FEB;
  else if (m_avgTypeProp=="PHI") 
    m_avgType=PHI;
  else {
    ATH_MSG_ERROR ( "Unknown AverageType " << m_avgTypeProp << ". Allowed values are 'FEB' or 'PHI'" ) ;
    return StatusCode::FAILURE;
  }

  for (int i=0; i < CaloGain::LARNGAIN; i++) {
    if (m_lowNoiseTh[i]>=m_highNoiseTh[i]) {
      ATH_MSG_ERROR ( "Low noise threshold (" << m_lowNoiseTh << ") is supposed to lower than the high noise threshold (" 
                      << m_highNoiseTh << ")" ) ;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

//////
StatusCode LArBadChannelHunter::stop() {
  ATH_CHECK( detStore()->retrieve(m_onlineId, "LArOnlineID") );
  ATH_CHECK( detStore()->retrieve(m_caloId, "CaloCell_ID") );
  ATH_CHECK( m_badChannelTool.retrieve() );
  ATH_CHECK( m_larCablingSvc.retrieve() );

  const ILArPedestal* pedestal = nullptr;
  ATH_CHECK( detStore()->retrieve(pedestal,m_pedKey) );

  const LArCaliWaveContainer* caliwave = nullptr;
  ATH_CHECK( detStore()->retrieve(caliwave,m_caliWaveKey) );

  if (m_undoCorr) {
    LArCaliWaveContainer* caliwavecomplete= const_cast<LArCaliWaveContainer*>(caliwave);
    ATH_MSG_INFO ( "Undo caliwave corrections now." ) ;
    caliwavecomplete->undoCorrections();
  }


////////////////////////////////
  LArWaveHelper waveHelper;

  //HWIdentifier lastfeb=HWIdentifier(0);
  //Map of Averages per region (defined by getSymId)
  std::map<unsigned,Average> averageMap;  
  //List of all individual values
  std::vector<CellData> cellData;

  std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
  std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();

  for(; itOnId!=itOnIdEnd;++itOnId){
    const HWIdentifier chid = *itOnId;
    if (!m_larCablingSvc->isOnlineConnected(chid)) continue;
    //const HWIdentifier febid= m_onlineId->feb_Id(chid);
    float ped=pedestal->pedestalRMS(chid,0);
    const LArCaliWaveVec& cwv=caliwave->get(chid,0);
    if (cwv.size()==0 || ped <=(1.0+LArElecCalib::ERRORCODE)) continue; //Want at least high-gain ped and wave 
    const float ampl=waveHelper.getMaxAmp(cwv[0]);
    const float wid =waveHelper.getWidth(cwv[0])*cwv[0].getDt();  
    const float tmax=waveHelper.getMax(cwv[0])*cwv[0].getDt();
    
    CellData thisCellData;
    thisCellData.m_chid=chid;
    thisCellData.m_ampl=ampl;
    thisCellData.m_wid =wid;
    thisCellData.m_tmax=tmax;

    const unsigned regId=getSymId(chid);
    Average& avreg=averageMap[regId];

    for(unsigned igain=CaloGain::LARHIGHGAIN; igain<CaloGain::LARNGAIN; igain++) {
      //get Pedestal RMS
      float ped=pedestal->pedestalRMS(chid,igain);
      if (ped >= (1.0+LArElecCalib::ERRORCODE)) {
	avreg.m_avPedRMS[igain]+=ped;
	avreg.m_avPedRMSSD[igain]+=(ped*ped);
	avreg.m_nPed[igain]++;
	avreg.m_vmedPedRMS[igain].push_back(ped);
	thisCellData.m_pedRMS[igain]=ped;
      }
    }//end loop over gains

    avreg.m_avAmpl[0]+=ampl;
    avreg.m_avAmplSD[0]+=ampl*ampl;
    avreg.m_nAmpls[0]++;
    avreg.m_vmedAmpl[0].push_back(ampl);

    avreg.m_avWid[0]+=wid;
    avreg.m_avWidSD[0]+=wid*wid;
    avreg.m_nWids[0]++;	
    avreg.m_vmedWid[0].push_back(wid);

    avreg.m_avTmax[0]+=tmax;
    avreg.m_avTmaxSD[0]+=tmax*tmax;
    avreg.m_nTmaxs[0]++;
    avreg.m_vmedTmax[0].push_back(tmax);

    cellData.push_back(thisCellData);
  }//End loop over cells

//////
  std::map<unsigned,Average>::iterator itMap=averageMap.begin();
  std::map<unsigned,Average>::iterator itMap_e=averageMap.end();
  for(;itMap!=itMap_e;itMap++) {
    itMap->second.finish(m_recalcPer);
  }


////////////////////////////////
  //Keep track to bad cells ...
  typedef LArBadChannelEnum::ProblemType PROB_t;
  typedef std::vector<std::pair<HWIdentifier,LArBadChannel> > BCV_t;
  
  BCV_t badChanVec;
  LArBadChanBitPacking packing;
  PROB_t lownoiseProb[CaloGain::LARNGAIN], highnoiseProb[CaloGain::LARNGAIN];
  lownoiseProb[CaloGain::LARHIGHGAIN]=LArBadChannelEnum::lowNoiseHGBit; 
  lownoiseProb[CaloGain::LARMEDIUMGAIN]=LArBadChannelEnum::lowNoiseMGBit; 
  lownoiseProb[CaloGain::LARLOWGAIN]=LArBadChannelEnum::lowNoiseLGBit; 

  highnoiseProb[CaloGain::LARHIGHGAIN]=LArBadChannelEnum::highNoiseHGBit; 
  highnoiseProb[CaloGain::LARMEDIUMGAIN]=LArBadChannelEnum::highNoiseMGBit; 
  highnoiseProb[CaloGain::LARLOWGAIN]=LArBadChannelEnum::highNoiseLGBit; 

  typedef std::pair<unsigned,std::vector<size_t> > goodAndBad_t;
  typedef std::map<HWIdentifier,goodAndBad_t> goodAndBadMap_t; //the key is the calibLineID

  goodAndBadMap_t calibLineMap;

  //Second loop: Find bad channels:
  std::vector<CellData>::const_iterator itcells=cellData.begin();
  std::vector<CellData>::const_iterator itcells_e=cellData.end();

  for (;itcells!=itcells_e;++itcells) { /// beginning of the loop of channels
    const CellData& thisCellData=*itcells;
    const HWIdentifier chid = thisCellData.m_chid;

    unsigned regId=getSymId(chid);
    ATH_MSG_VERBOSE ( "Checking " << channelDescription(chid) ) ;
    std::map<unsigned,Average>::const_iterator febit=averageMap.find(regId);
    if (febit==averageMap.end()) continue;
    const Average& avreg=febit->second;

    LArBadChannel problem(0); /// bad bit newly found 
    LArBadChannel bc = m_badChannelTool->status(chid); /// known bad bit

    for(unsigned igain=CaloGain::LARHIGHGAIN; igain<CaloGain::LARNGAIN; igain++) {
	if (avreg.m_nPed[igain]>2) {//Check if have average ped-rms for this cell & gain
	  const float rms=thisCellData.m_pedRMS[igain];
	  float lowCut_rms=0.; /// for low noisy cells
	  float higCut_rms=0.; /// for high noisy cells
	  if ( m_cutType=="SIG" ) {
	    lowCut_rms=m_lowNoiseTh[igain]*avreg.m_avPedRMSSD[igain];
	    higCut_rms=m_highNoiseTh[igain]*avreg.m_avPedRMSSD[igain];
	  }  
	  else if ( m_cutType=="PER") { /// 0.01 is used to scale "PER" to the same order of magnitude to "SIG"
	    lowCut_rms=m_lowNoiseTh[igain]*avreg.m_avPedRMS[igain]*0.01;
	    higCut_rms=m_highNoiseTh[igain]*avreg.m_avPedRMS[igain]*0.01;
	  }
	    
	  if (rms==-1)
	    ATH_MSG_ERROR ( "No Pedestal found for " << channelDescription(chid,igain) ) ;
	  else {
	    ATH_MSG_VERBOSE ( "PedRMS, gain: " << igain << ":" << rms << " Average: " 
                              << avreg.m_avPedRMS[igain] << " Median: " << avreg.m_medPedRMS ) ; 
	    //// channels ABOVE threshold, NOT fabs()!
	    if ( (rms-avreg.m_avPedRMS[igain]) > higCut_rms ) {
	      packing.setBit(highnoiseProb[igain],problem);
	    } 
	    else if ( (rms-avreg.m_avPedRMS[igain]) > lowCut_rms ) {
	      packing.setBit(lownoiseProb[igain],problem);
	    }
	    //// channel information output, all channels with a deviation (fabs() here) or known BC. 
	    if (fabs(rms-avreg.m_avPedRMS[igain]) > lowCut_rms || (bc.lowNoiseHG()||bc.highNoiseHG()) ) {
	      std::string my_status;	 
	      if (fabs(rms-avreg.m_avPedRMS[igain]) < lowCut_rms && (bc.lowNoiseHG()||bc.highNoiseHG())) 
	        my_status="BCN "; //// Known BCs are not thought to be bad this time. 	
	      else		
	        my_status=(bc.lowNoiseHG()||bc.highNoiseHG()) ? "BC " : "NEW ";

	      ATH_MSG_INFO( my_status << channelDescription(chid,igain) 
                            << " RMS: " << rms << " ( " << avreg.m_avPedRMS[igain] << " , " 
                            << float(int(10000*(rms-avreg.m_avPedRMS[igain])/avreg.m_avPedRMS[igain]))/100 <<" %) " << ", #Sig: " 
                            << float(int(100*(rms-avreg.m_avPedRMS[igain])/avreg.m_avPedRMSSD[igain]))/100 
                            << " ( " << avreg.m_avPedRMSSD[igain] << " ) " ) ;	    
	    }// end bad channels information output
	  } //end if(rms==-1)
	}//end if have ped rms for this cell & gain
    }//end loop over gains

////
      if (avreg.m_nAmpls[0]>2 && avreg.m_nWids[0]>2 && avreg.m_nTmaxs[0]>2) {//Check if we have average ampls for this cell & gain
	const float ampl=thisCellData.m_ampl;
	const float wid=thisCellData.m_wid;
	const float tmax=thisCellData.m_tmax;

	float lowCut_amp=0.;
	float higCut_amp=0.;
	float lowCut_wid=0.;
	float higCut_wid=0.;
	float Cut_tmax=0.;

	if ( m_cutType=="SIG" ) { /////
	  lowCut_amp=m_distampTh[0]*avreg.m_avAmplSD[0];
	  higCut_amp=m_amplTh[0]*avreg.m_avAmplSD[0];
	  lowCut_wid=m_distwidTh[0]*avreg.m_avWidSD[0];
	  higCut_wid=m_widTh[0]*avreg.m_avWidSD[0];
	  Cut_tmax=m_tmaxampTh[0]*avreg.m_avTmaxSD[0];		    
	}  
	else if ( m_cutType=="PER") {
	  lowCut_amp=m_distampTh[0]*avreg.m_avAmpl[0]*0.01;
	  higCut_amp=m_amplTh[0]*avreg.m_avAmpl[0]*0.01;
	  lowCut_wid=m_distwidTh[0]*avreg.m_avWid[0]*0.01;
	  higCut_wid=m_widTh[0]*avreg.m_avWid[0]*0.01;
	  Cut_tmax=m_tmaxampTh[0]*avreg.m_avTmax[0]*0.01;	    
	} // end if loop
	

	if (ampl==-1 || wid==-1) {
	  ATH_MSG_INFO ( "No Amplitude or Width found for " << channelDescription(chid,0) ) ;
	  packing.setBit(LArBadChannelEnum::deadReadoutBit,problem);
	}
	else {
	  ATH_MSG_VERBOSE ( "Ampl gain: "<< 0<< ":"<< ampl<< " Average: " << avreg.m_avAmpl[0]) ; 
	  //// larger deviation: dead
	  if (fabs(ampl-avreg.m_avAmpl[0])>higCut_amp || fabs(wid-avreg.m_avWid[0])>higCut_wid) { 
	    packing.setBit(LArBadChannelEnum::deadReadoutBit,problem);
	  } 
	  /// smaller deviation: distorted
	  else if (fabs(ampl-avreg.m_avAmpl[0])>lowCut_amp || fabs(wid-avreg.m_avWid[0])>lowCut_wid){ 
            packing.setBit(LArBadChannelEnum::distortedBit,problem);
	  }
	  /// checking TmaxAmp, Not mixed with MaxAmp and Width 
	  else if (fabs(tmax-avreg.m_avTmax[0])>Cut_tmax) {
            packing.setBit(LArBadChannelEnum::distortedBit,problem);	  
//	    std::string my_status=(bc.good()) ? "NEW " : "BC ";
//            log << MSG::INFO << my_status << channelDescription(chid,0) << " Tmax: " << tmax << " ( " << avreg.m_avTmax[0] << " , " 
//	    	<< float(int(10000*(tmax-avreg.m_avTmax[0])/avreg.m_avTmax[0]))/100 << " %) "
//            	<< " #Sig:" << float(int(100*(tmax-avreg.m_avTmax[0])/avreg.m_avTmaxSD[0]))/100 
//		<< " ( " << avreg.m_avTmaxSD[0] << " ) " << endreq;
	  }
	  /// channel information output
//	  if (fabs(ampl-avreg.m_avAmpl[0])>lowCut_amp || fabs(wid-avreg.m_avWid[0])>lowCut_wid) {
	  /// Only dead or distorted, or short known BCs are considered below.
	  if ( problem.deadReadout()||problem.distorted() || bc.deadReadout()||bc.deadCalib()||bc.deadPhys()||bc.distorted()||bc.shortProblem() ){
	    std::string my_status;
	    if ( !(problem.deadReadout()||problem.distorted())
	    	&&(bc.deadReadout()||bc.deadCalib()||bc.deadPhys()||bc.distorted()||bc.shortProblem()) )
	    	my_status="BCN ";
	    else	
		my_status=(bc.deadReadout()||bc.deadCalib()||bc.deadPhys()||bc.distorted()||bc.shortProblem()) ? "BC " : "NEW ";
		
	    ATH_MSG_INFO ( my_status << channelDescription(chid,0) 
	        << " Amp: " << ampl << " ( " << avreg.m_avAmpl[0] << " , " 
	    	<< float(int(10000*(ampl-avreg.m_avAmpl[0])/avreg.m_avAmpl[0]))/100 << " %) " << " #Sig: " 
		<< float(int(100*(ampl-avreg.m_avAmpl[0])/avreg.m_avAmplSD[0]))/100 << " ( " << avreg.m_avAmplSD[0] <<" ) "
	    	<< " FWHM: " << wid << " ( " << avreg.m_avWid[0] << " , " 
		<< float(int(10000*(wid-avreg.m_avWid[0])/avreg.m_avWid[0]))/100 << " %) " << " #Sig: " 
		<< float(int(100*(wid-avreg.m_avWid[0])/avreg.m_avWidSD[0]))/100 
		<< " ( " << avreg.m_avWidSD[0] << " ) "
		<< " Tmax: " << tmax << " ( " << avreg.m_avTmax[0] << " , " 
		<< float(int(10000*(tmax-avreg.m_avTmax[0])/avreg.m_avTmax[0]))/100 << " %) " << " #Sig:" 
		<< float(int(100*(tmax-avreg.m_avTmax[0])/avreg.m_avTmaxSD[0]))/100 
                           ) ;
	  }
	}
      }//end if have amplitude for this cell


      const std::vector<HWIdentifier>& cLids=m_larCablingSvc->calibSlotLine(chid);
      for (unsigned i=0;i<cLids.size();i++) {
	goodAndBad_t& gb=calibLineMap[cLids[i]];
	if (problem.deadReadout()||problem.distorted())
	  gb.second.push_back(badChanVec.size()); /// index of bc
	else
	  gb.first++;
      }
      if (!problem.good()) badChanVec.push_back(std::make_pair(chid,problem));

  }//end loop over channels


  goodAndBadMap_t::const_iterator itCalib=calibLineMap.begin();
  goodAndBadMap_t::const_iterator itCalib_e=calibLineMap.end();
  for(;itCalib!=itCalib_e;itCalib++) {
//    const HWIdentifier& cLid=itCalib->first;
    const goodAndBad_t& gb=itCalib->second;
    for (unsigned i=0;i<gb.second.size();i++) {
    if (gb.first==0) {
      ATH_MSG_INFO ( "All channels belonging to calibLine " << channelDescription(badChanVec[gb.second[i]].first) 
                     << " don't respond to pulses. Assume bad calib line." ) ;
	packing.setBit(LArBadChannelEnum::deadReadoutBit,badChanVec[gb.second[i]].second,false);
	packing.setBit(LArBadChannelEnum::distortedBit,badChanVec[gb.second[i]].second,false);
	packing.setBit(LArBadChannelEnum::deadCalibBit,badChanVec[gb.second[i]].second,true);
      }//end loop over all channels belonging to this calibline
    }
  }//end loop over calibline vector

  if (m_outFileName.size()) {
    std::ofstream outfile(m_outFileName.c_str());
    if (!outfile.is_open()) {
      ATH_MSG_ERROR ( "Failed to open output file " << m_outFileName << ". No output will be written." ) ;
    }
    else {
      BCV_t::const_iterator bcvit=badChanVec.begin();
      BCV_t::const_iterator bcvit_e=badChanVec.end();
      for(;bcvit!=bcvit_e;++bcvit) {
	const HWIdentifier chid=bcvit->first;
	const LArBadChannel bc=bcvit->second;
	const HWIdentifier cLid=m_larCablingSvc->calibSlotLine(chid)[0];
	const LArBadChannel m_bc=m_badChannelTool->status(chid);
	std::string my_ps=(m_bc.good())? "NEW " : "BC ";
	if (!m_bc.good()&&m_outOnlyNew) continue; // just new
	  outfile << m_onlineId->barrel_ec(chid) << " " 
	  	  << m_onlineId->pos_neg(chid) << " "
		  << m_onlineId->feedthrough(chid) << " "
		  << m_onlineId->slot(chid) << " "
		  << m_onlineId->channel(chid) << " "
		  << m_onlineId->channel(cLid) << " "
		  << packing.stringStatus(bc) << " "
		  << my_ps << std::endl;
//		  << packing.stringStatus(bc) << std::endl;
      }
      outfile.close();
    } //end if out file open
  } // end if have out file name
  return StatusCode::SUCCESS; 
}

const std::string LArBadChannelHunter::channelDescription(const HWIdentifier& chid, 
							  const unsigned gain) const {
							  
/// the format of output is changed a bit to be consistent with the output format of 
/// validation tools
  std::ostringstream output;

  if (gain<=2) 
    output << "Gain:"<< gain << " ";

  output << "Channel [";
//  output << " id=0x" << std::hex <<  chid.get_compact() << std::dec << " [";
  if (m_onlineId->barrel_ec(chid)==0) 
    output << "Bar";
  else
    output << "End";
      
//  output << ",Side:";
  if (m_onlineId->pos_neg(chid)==0)
    output << ",C";
  else
    output << ",A";

  output << ",FT:"  << m_onlineId->feedthrough(chid) 
	 << ",Sl:" << m_onlineId->slot(chid);

  output << ",Ch:" << m_onlineId->channel(chid);
  if (!m_onlineId->isCalibration(chid)) {
    try {
      if (m_larCablingSvc->isOnlineConnected(chid)) {
	if (m_onlineId->isFCALchannel(chid))
	  output << ",FCAL";
	if (m_onlineId->isHECchannel(chid))
	  output << ",HEC";
	if (m_onlineId->isEMBchannel(chid))
	  output << ",EMB";
	if (m_onlineId->isEMECchannel(chid))
	  output << ",EMEC";
//	const Identifier id=m_larCablingSvc->cnvToIdentifier(chid);
//        output << ",Samp:" << m_caloId->sampling(id); 
      //Could add Eta, phi....
      }//end if is connected
      else
        output << ",disconnected";
    }
    catch (LArID_Exception&) {}
  }//end if !isCalibration
   output << "]"; 
  return output.str();
}



LArBadChannelHunter::Average::Average() {
  for(unsigned igain=CaloGain::LARHIGHGAIN; igain<CaloGain::LARNGAIN; igain++){
    m_nPed[igain]=0;
    m_avPedRMS[igain]=0;
    m_avPedRMSSD[igain]=0;

    m_nAmpls[igain]=0;
    m_avAmpl[igain]=0.;
    m_avAmplSD[igain]=0.;

    m_nWids[igain]=0;
    m_avWid[igain]=0.;
    m_avWidSD[igain]=0.;

    m_nTmaxs[igain]=0;
    m_avTmax[igain]=0.;
    m_avTmaxSD[igain]=0.;

    m_medPedRMS[igain]=0;
    m_medAmpl[igain]=0;
    m_medWid[igain]=0;
    m_medTmax[igain]=0;
  }
}


void LArBadChannelHunter::Average::finish(float my_recalcPer) {
  for(unsigned igain=CaloGain::LARHIGHGAIN; igain<CaloGain::LARNGAIN; igain++) {

    if (m_nPed[igain]) {
      m_avPedRMS[igain]/=m_nPed[igain];
      //sample standard deviation of average
      m_avPedRMSSD[igain]=(m_avPedRMSSD[igain]-(m_nPed[igain]*m_avPedRMS[igain]*m_avPedRMS[igain]))/(m_nPed[igain]);
      m_avPedRMSSD[igain]=sqrt(m_avPedRMSSD[igain]);
      //Find median
      std::sort(m_vmedPedRMS[igain].begin(),m_vmedPedRMS[igain].end());
      m_medPedRMS[igain]=m_vmedPedRMS[igain].at(m_vmedPedRMS[igain].size()/2);
    }
    if (m_nAmpls[igain]) {
      m_avAmpl[igain]/=m_nAmpls[igain];
      //sample standard deviation of average
      m_avAmplSD[igain]=(m_avAmplSD[igain]-(m_nAmpls[igain]*m_avAmpl[igain]*m_avAmpl[igain]))/(m_nAmpls[igain]);
      m_avAmplSD[igain]=sqrt(m_avAmplSD[igain]);
      //Find median
      std::sort(m_vmedAmpl[igain].begin(),m_vmedAmpl[igain].end());
      m_medAmpl[igain]=m_vmedAmpl[igain].at(m_vmedAmpl[igain].size()/2);
    }
    if (m_nWids[igain]) {
      m_avWid[igain]/=m_nWids[igain];
      //sample standard deviation of average
      m_avWidSD[igain]=(m_avWidSD[igain]-(m_nWids[igain]*m_avWid[igain]*m_avWid[igain]))/(m_nWids[igain]);
      m_avWidSD[igain]=sqrt(m_avWidSD[igain]);
      //Find median
      std::sort(m_vmedWid[igain].begin(),m_vmedWid[igain].end());
      m_medWid[igain]=m_vmedWid[igain].at(m_vmedWid[igain].size()/2);
    }
    if (m_nTmaxs[igain]) {
      m_avTmax[igain]/=m_nTmaxs[igain];
      //sample standard deviation of average
      m_avTmaxSD[igain]=(m_avTmaxSD[igain]-(m_nTmaxs[igain]*m_avTmax[igain]*m_avTmax[igain]))/(m_nTmaxs[igain]);
      m_avTmaxSD[igain]=sqrt(m_avTmaxSD[igain]);
      //Find median
      std::sort(m_vmedTmax[igain].begin(),m_vmedTmax[igain].end());
      m_medTmax[igain]=m_vmedTmax[igain].at(m_vmedTmax[igain].size()/2);
    }


    ///// here we try to re-calculate the mean and rms values based on the central part of 
    ///// vector of all entries with the purpose of reducing the impact of the deviating GROUPS
   
  if (my_recalcPer>=0.){  
   //// pedrms
   if ( m_avPedRMSSD[igain]>my_recalcPer*m_avPedRMS[igain]) {
    float re_avPed=0.; float re_avPedSD=0.; int re_nPed=0;
    std::vector<float>::iterator p_loop = m_vmedPedRMS[igain].begin();
    std::vector<float>::iterator p_loop_e=m_vmedPedRMS[igain].end();
    for (;p_loop!=p_loop_e; ++p_loop){
      if ( fabs(*p_loop-m_medPedRMS[igain])<my_recalcPer*m_medPedRMS[igain] ){
         re_avPed+=*p_loop;
         re_avPedSD+=*p_loop*(*p_loop);
         re_nPed++;
      }
    }
    if (re_nPed > 0) {
      re_avPed/=re_nPed;
      re_avPedSD=(re_avPedSD- re_nPed*re_avPed*re_avPed)/(re_nPed);
    }
    re_avPedSD=sqrt(re_avPedSD);
    std::cout <<" PedRMS mean && RMS are recalc: Orig (recalc) are: "<< m_avPedRMS[igain]<< " ( "<< re_avPed 
	      <<" ) and "<< m_avPedRMSSD[igain]<<" ( "<<re_avPedSD<<" )"<<std::endl;
    m_avPedRMS[igain]=re_avPed; m_avPedRMSSD[igain]=re_avPedSD; 
   }
   ///// ampl
   if ( m_avAmplSD[igain]>my_recalcPer*m_avAmpl[igain] ) {
    float re_avAmpl=0.; float re_avAmplSD=0.; int   re_nAmpl=0;
    std::vector<float>::iterator re_loop = m_vmedAmpl[igain].begin();
    std::vector<float>::iterator re_loop_e=m_vmedAmpl[igain].end();
    for (;re_loop!=re_loop_e; ++re_loop){
      if ( fabs(*re_loop-m_medAmpl[igain])<my_recalcPer*m_medAmpl[igain] ){
	 re_avAmpl+=*re_loop;
	 re_avAmplSD+=*re_loop*(*re_loop);
	 re_nAmpl++;
      }
    }
    if (re_nAmpl > 0) {
      re_avAmpl/=re_nAmpl;
      re_avAmplSD=(re_avAmplSD- re_nAmpl*re_avAmpl*re_avAmpl)/(re_nAmpl);
    }
    re_avAmplSD=sqrt(re_avAmplSD);
    std::cout <<" MaxAmp mean && RMS are recalc: Orig (recalc) are: "<< m_avAmpl[igain]<< " ( "<< re_avAmpl
              <<" ) and "<< m_avAmplSD[igain]<<" ( "<<re_avAmplSD<<" )"<<std::endl;
    m_avAmpl[igain]=re_avAmpl; m_avAmplSD[igain]=re_avAmplSD;
   }
   //// width
   if ( m_avWidSD[igain]>my_recalcPer*m_avWid[igain] ) {
    float re_avWid=0.; float re_avWidSD=0.; int   re_nWid=0; 
    std::vector<float>::iterator w_loop = m_vmedWid[igain].begin();
    std::vector<float>::iterator w_loop_e=m_vmedWid[igain].end();	
    for (;w_loop!=w_loop_e; ++w_loop){
      if ( fabs(*w_loop-m_medWid[igain])<my_recalcPer*m_medWid[igain] ){
         re_avWid+=*w_loop;
         re_avWidSD+=*w_loop*(*w_loop);
         re_nWid++;
      }
    }
    if (re_nWid > 0) {
      re_avWid/=re_nWid;
      re_avWidSD=(re_avWidSD- re_nWid*re_avWid*re_avWid)/(re_nWid);
    }
    re_avWidSD=sqrt(re_avWidSD);
    std::cout <<"   FWHM mean && RMS are recalc: Orig (recalc) are: "<< m_avWid[igain]<< " ( "<< re_avWid
	      <<" ) and "<< m_avWidSD[igain]<<" ( "<<re_avWidSD<<" )"<<std::endl;
    m_avWid[igain]=re_avWid; m_avWidSD[igain]=re_avWidSD;
   }
   ////TmaxAmp
   if ( m_avTmaxSD[igain]>my_recalcPer*m_avTmax[igain] ) { 
    float re_avTmax=0.; float re_avTmaxSD=0.; int   re_nTmax=0;
    std::vector<float>::iterator t_loop = m_vmedTmax[igain].begin();
    std::vector<float>::iterator t_loop_e=m_vmedTmax[igain].end();
    for (;t_loop!=t_loop_e; ++t_loop){
      if ( fabs(*t_loop-m_medTmax[igain])<my_recalcPer*m_medTmax[igain] ){
         re_avTmax+=*t_loop;
         re_avTmaxSD+=*t_loop*(*t_loop);
         re_nTmax++;
      }
    }
    if (re_nTmax > 0) {
      re_avTmax/=re_nTmax;
      re_avTmaxSD=(re_avTmaxSD- re_nTmax*re_avTmax*re_avTmax)/(re_nTmax);
    }
    re_avTmaxSD=sqrt(re_avTmaxSD);
    std::cout <<"TmaxAmp mean && RMS are recalc: Orig (recalc) are: "<< m_avTmax[igain]<< " ( "<< re_avTmax
	      <<" ) and "<< m_avTmaxSD[igain]<<" ( "<<re_avTmaxSD<<" )"<<std::endl;
    m_avTmax[igain]=re_avTmax; m_avTmaxSD[igain]=re_avTmaxSD;
   } 
  } 

  }//end loop over gains
}



unsigned  LArBadChannelHunter::getSymId(const HWIdentifier chid) const {
  if (m_avgType==FEB)
    return m_onlineId->feb_Id(chid).get_identifier32().get_compact();
  else {
    const unsigned caloRegionHashMax=m_caloId->calo_region_hash_max();
    const Identifier id=m_larCablingSvc->cnvToIdentifier(chid);
    const Identifier regid=m_caloId->region_id(id);
    const unsigned reghash=m_caloId->calo_region_hash(regid);
    const int eta=m_caloId->eta(id);
  


    if (eta<0 || eta==CaloCell_ID::NOT_VALID || regid==CaloCell_ID::NOT_VALID ||reghash==CaloCell_ID::NOT_VALID || caloRegionHashMax!=64) {
      std::cout << "ERROR unexpected idetifier:"  
		<< std::hex << " id=0x" << id << " regionId=0x" << regid 
		<< std::dec << " reghash=" << reghash << " eta=" << eta
		<< " caloRegionHashMax=" << caloRegionHashMax 
		<< std::endl;
      assert(0);
    }
    const unsigned retval= (eta<<8) | reghash;
    return retval;
  }
}
