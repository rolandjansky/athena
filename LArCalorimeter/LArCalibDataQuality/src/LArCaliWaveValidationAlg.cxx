/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibDataQuality/LArCaliWaveValidationAlg.h"
#include <cmath>

LArCaliWaveValidationAlg::LArCaliWaveValidationAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  LArCaliWaveValidationBase(name,pSvcLocator) {
  
  declareProperty("TimeShiftDetection",m_timeShift=false,
		  "Enable detection of Pulse timeShift");
  declareProperty("AmplitudeTolerance",m_ampToleranceInit,
		  "CaloCellGroup of allowed deviation of the amplitude (in permills)");
  declareProperty("CaliWaveFWHMTolerance",m_fwhmToleranceInit,
		  "CaloCellGroup of llowed deviation of the FWHM (in permills)");
  declareProperty("AmplitudeToleranceFEB",m_ampToleranceFEBInit,
		  "CaloCellGroup of llowed deviation of the amplitude (average over one FEB, in permills)");
  declareProperty("CaliWaveFWHMToleranceFEB",m_fwhmToleranceFEBInit,
		  "CaloCellGroup of allowed deviation of the FWHM (average over one FEB, in permills)");

  m_ampGlobalRef=0.;
  m_fwhmGlobalRef=0.;
  m_ampGlobalVal=0.;
  m_fwhmGlobalVal=0.;
  m_nEntriesGlobal=0;
}


StatusCode LArCaliWaveValidationAlg::preLoop() {
  m_ampGlobalRef=0.;
  m_fwhmGlobalRef=0.;
  m_ampGlobalVal=0.;
  m_fwhmGlobalVal=0.;
  m_nEntriesGlobal=0;
  bool stat;

  //Initialize CaloCellGroups:

  ATH_MSG_INFO ( "Initialize Amplitude Tolerances (CaloCellGroup)" ) ;
  stat=m_ampTolerance.setDefinition(m_caloId,m_ampToleranceInit,msg());
  if (!stat) {
    ATH_MSG_ERROR ( "Failed to initialize CaloCellGroup of amplitude tolerances!" ) ;
    return StatusCode::FAILURE;
  }
  if (m_ampTolerance.getDefaults().size()!=3) {
    ATH_MSG_ERROR ( "Configuration error: Expected three amplitude tolerance values (one per gain), got " 
                    << m_ampTolerance.getDefaults().size() ) ;
    return StatusCode::FAILURE;
  }
  if (this->msgLvl(MSG::DEBUG)) m_ampTolerance.printDef();//for debugging....


  
  ATH_MSG_INFO ( "Initialize FWHM Tolerances (CaloCellGroup)" ) ;
  stat=m_fwhmTolerance.setDefinition(m_caloId,m_fwhmToleranceInit,msg());
  if (!stat) {
    ATH_MSG_ERROR ( "Failed to initialize CaloCellGroup of FWHM tolerances!" ) ;
    return StatusCode::FAILURE;
  }
  if (m_fwhmTolerance.getDefaults().size()!=3) {
    ATH_MSG_ERROR ( "Configuration error: Expected three FWHM tolerance values (one per gain), got " 
                    << m_fwhmTolerance.getDefaults().size() ) ;
    return StatusCode::FAILURE;
  }
  if (this->msgLvl(MSG::DEBUG)) m_fwhmTolerance.printDef();//for debugging....


  
  ATH_MSG_INFO ( "Initialize FEB Amplitude Tolerances (CaloCellGroup)" ) ;
  stat=m_ampToleranceFEB.setDefinition(m_caloId,m_ampToleranceFEBInit,msg());
  if (!stat) {
    ATH_MSG_ERROR ( "Failed to initialize CaloCellGroup of FEB amplitude tolerances!" ) ;
    return StatusCode::FAILURE;
  }
  if (m_ampToleranceFEB.getDefaults().size()!=3) {
    ATH_MSG_ERROR ( "Configuration error: Expected three FEB amplitude tolerance values (one per gain), got " 
                    << m_ampToleranceFEB.getDefaults().size() ) ;
    return StatusCode::FAILURE;
  }
  if (this->msgLvl(MSG::DEBUG))  m_ampToleranceFEB.printDef();//for debugging....


  
  ATH_MSG_INFO ( "Initialize FEB FWHM Tolerances (CaloCellGroup)" ) ;
  stat=m_fwhmToleranceFEB.setDefinition(m_caloId,m_fwhmToleranceFEBInit,msg());
  if (!stat) {
    ATH_MSG_ERROR ( "Failed to initialize CaloCellGroup of FEB FWHM tolerances!" ) ;
    return StatusCode::FAILURE;
  }
  if (m_fwhmToleranceFEB.getDefaults().size()!=3) {
    ATH_MSG_ERROR ( "Configuration error: Expected three FEB FWHM tolerance values (one per gain), got " 
                    << m_fwhmToleranceFEB.getDefaults().size() ) ;
    return StatusCode::FAILURE;
  }
  if (this->msgLvl(MSG::DEBUG)) m_fwhmToleranceFEB.printDef();//for debugging....

  return StatusCode::SUCCESS;
}

bool LArCaliWaveValidationAlg::validateChannel(const LArCondObj& ref, const LArCondObj& val, const HWIdentifier chid, const int gain) {

  if (gain<0 || gain>2) {
     ATH_MSG_ERROR ( "Unexpected gain value " << gain ) ;
     return false;
  }

  double ampVal=0;
  double ampRef=0;

  float fwhmVal=0;
  float fwhmRef=0;

  float TmaxVal=0;
  float TmaxRef=0;

  float TMaxshift=5.;


  const Identifier id=m_larCablingSvc->cnvToIdentifier(chid);

  const float ampTolerance=m_ampTolerance.valuesForCell(id)[gain];
  const float fwhmTolerance=m_fwhmTolerance.valuesForCell(id)[gain];
  
  if (val.size()>0 && ref.size()>0){
	  
	  //We assume that the caliwavevec has only one entry.
           const LArWave& wave=val[0];
           const LArWave& waver=ref[0];
	   TmaxVal=m_waveHelper.getMax(wave);
	   TmaxRef=m_waveHelper.getMax(waver);
	   ampVal=m_waveHelper.getMaxAmp(wave);
	   ampRef=m_waveHelper.getMaxAmp(waver);
	   fwhmVal=wave.getDt()*m_waveHelper.getWidth(wave);
	   fwhmRef=waver.getDt()*m_waveHelper.getWidth(waver);
  }

  const HWIdentifier febid=m_onlineHelper->feb_Id(chid);
    

  DataPerFEB* dataPerFEB=&(m_vDataPerFEB.back());
  if (m_vDataPerFEB.size()==0 || dataPerFEB->febid!=febid) {//Got to new FEB
    m_vDataPerFEB.push_back(DataPerFEB(chid,febid,gain));
    dataPerFEB=&(m_vDataPerFEB.back());
  }

  dataPerFEB->ampVal+=ampVal;
  dataPerFEB->ampRef+=ampRef;
  dataPerFEB->fwhmVal+=fwhmVal;
  dataPerFEB->fwhmRef+=fwhmRef;
  ++(dataPerFEB->nEntries);


  m_ampGlobalVal+=ampVal;
  m_fwhmGlobalVal+=fwhmVal;
  m_ampGlobalRef+=ampRef;
  m_fwhmGlobalRef+=fwhmRef;
  ++m_nEntriesGlobal;

  bool retval=true;
  if (fabs(TmaxVal-TmaxRef)> TMaxshift && m_timeShift==true) {
    retval=false;
    if (m_nFailedValidation<m_maxmessages)
      msg() << m_myMsgLvl << "Shifted! " << channelDescription(chid,gain)  << " Tmax: " << TmaxVal << " ( " << TmaxRef << " ) " << endreq;

  }

 
  if (1000*fabs(ampVal-ampRef)/ampRef > ampTolerance || 1000*fabs(fwhmVal-fwhmRef)/fwhmRef > fwhmTolerance) {
    retval=false;
    if (m_nFailedValidation<m_maxmessages) {
      msg().precision(2);
      msg().setf(std::ios::fixed,std::ios::floatfield); 
      msg() <<  this->m_myMsgLvl << "Deviating! " << channelDescription(chid,gain) << " Amp: " << ampVal << "( " << ampRef 
	       << ", " << 100.*(ampVal-ampRef)/ampRef << " %)" 
	       << " FWHM: " << fwhmVal << " ( " << fwhmRef << ", " << 100*(fwhmVal-fwhmRef)/fwhmVal << " %)" << endreq;
      ATH_MSG_DEBUG ( "Amplitude FEB tolerance: " << ampTolerance << ", FWHM FEB tolerance: " << fwhmTolerance ) ;
    }
  }
  if (!retval && m_nFailedValidation==m_maxmessages)
    msg() <<  this->m_myMsgLvl << "Channel deviation message has already been printed " << m_maxmessages << " times. Now silent..." << endreq;
  
  return retval;
}


bool LArCaliWaveValidationAlg::febSummary() {

  unsigned nBadFebs=0;

  msg().precision(3);
  msg().setf(std::ios::fixed,std::ios::floatfield); 

  std::vector<DataPerFEB>::iterator it=m_vDataPerFEB.begin();
  std::vector<DataPerFEB>::iterator it_e=m_vDataPerFEB.end();
  for (;it!=it_e;++it) {
    DataPerFEB& dataPerFeb=*it;
    dataPerFeb.ampVal/=dataPerFeb.nEntries;
    dataPerFeb.ampRef/=dataPerFeb.nEntries;
    dataPerFeb.fwhmVal/=dataPerFeb.nEntries;
    dataPerFeb.fwhmRef/=dataPerFeb.nEntries;

    const Identifier id=m_larCablingSvc->cnvToIdentifier(dataPerFeb.chid);
    const float& ampToleranceFEB=m_ampToleranceFEB.valuesForCell(id)[dataPerFeb.gain];
    const float& fwhmToleranceFEB=m_fwhmToleranceFEB.valuesForCell(id)[dataPerFeb.gain];
    
    if (fabs(dataPerFeb.ampVal-dataPerFeb.ampRef)/dataPerFeb.ampRef*1000>ampToleranceFEB  ||  
	fabs(dataPerFeb.fwhmVal-dataPerFeb.fwhmRef)/dataPerFeb.fwhmRef*1000>fwhmToleranceFEB) {
      msg() << m_myMsgLvl << "Deviating! " <<channelDescription(dataPerFeb.febid,dataPerFeb.gain,true)<< "Average Amp: " << dataPerFeb.ampVal << " (" << dataPerFeb.ampRef << ")" 
	       << " FWHM: " << dataPerFeb.fwhmVal << " (" << dataPerFeb.fwhmRef << ")" << endreq;
      ++nBadFebs;
      ATH_MSG_DEBUG ( "Amplitude FEB tolerance: " << ampToleranceFEB << ", FWHM FEB tolerance: " << fwhmToleranceFEB ) ;
    }
  }

  if (nBadFebs) {
    ATH_MSG_ERROR ( "Found " << nBadFebs << " out of " << m_vDataPerFEB.size() << " FEBs deviating from reference" ) ;
    return false;
  }
  else {
    ATH_MSG_INFO ( "All " << m_vDataPerFEB.size() << " FEBs withing given tolerance." ) ;
    return true;
  }
}
StatusCode LArCaliWaveValidationAlg::summary() {
 StatusCode sc=StatusCode::SUCCESS;
  //1nd step: Check the FEB-averages:
  if (m_doFebAverages && !febSummary())
    sc=StatusCode::RECOVERABLE;
  //2st step: Call the summary method from base-class (single-channel summary)
  if (!LArCaliWaveValidationBase::summary().isSuccess())
    sc=StatusCode::RECOVERABLE;
  //3rd step: Check the gobal averages:
  if (m_nEntriesGlobal) {
    m_ampGlobalVal/=m_nEntriesGlobal;
    m_ampGlobalRef/=m_nEntriesGlobal;
    m_fwhmGlobalVal/=m_nEntriesGlobal;
    m_fwhmGlobalRef/=m_nEntriesGlobal;
  }
  ATH_MSG_INFO ( "Global amplitude average: " << m_ampGlobalVal << " Reference:" << m_ampGlobalRef
                 << " Deviation:" << (m_ampGlobalVal-m_ampGlobalRef)/m_ampGlobalRef*1000 <<" permille" ) ;
  ATH_MSG_INFO ( "Gobal FWHM average: " << m_fwhmGlobalVal << " Reference:" << m_fwhmGlobalRef
                 << " Deviation:" << (m_fwhmGlobalVal-m_fwhmGlobalRef)/m_fwhmGlobalRef*1000 <<" permille" ) ;
  return sc;
}
