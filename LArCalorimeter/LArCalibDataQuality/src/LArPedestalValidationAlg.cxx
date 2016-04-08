/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibDataQuality/LArPedestalValidationAlg.h"
#include <cmath>

LArPedestalValidationAlg::LArPedestalValidationAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  LArPedestalValidationBase(name,pSvcLocator) {

  declareProperty("PedestalTolerance",m_pedToleranceInit,
		  "CaloCellGroup of allowed deviation of the pedestal (in ADC counts)");
  declareProperty("PedestalRMSTolerance",m_rmsToleranceInit,
		  "CaloCellGroup of allowed deviation of the pedestal RMS (in ADC counts)");

  declareProperty("PedestalToleranceFEB",m_pedToleranceFEBInit,
		  "CaloCellGroup of allowed deviation of the pedestal (average over one FEB, in ADC counts)");
  declareProperty("PedestalRMSToleranceFEB",m_rmsToleranceFEBInit,
		  "CaloCellGroup Allowed deviation of the pedestal RMS (average over one FEB, in ADC counts)");
  m_pedGlobalRef=0.;
  m_rmsGlobalRef=0.;
  m_pedGlobalVal=0.;
  m_rmsGlobalVal=0.;
  m_nEntriesGlobal=0;
}


StatusCode LArPedestalValidationAlg::preLoop() {
  m_pedGlobalRef=0.;
  m_rmsGlobalRef=0.;
  m_pedGlobalVal=0.;
  m_rmsGlobalVal=0.;
  m_nEntriesGlobal=0;

  bool stat;
  
  
  ATH_MSG_INFO ( "Initialize Pedestal Tolerances (CaloCellGroup)" ) ;
  stat=m_pedTolerance.setDefinition(m_caloId,m_pedToleranceInit,msg());
  if (!stat) {
    ATH_MSG_ERROR ( "Failed to initialize CaloCellGroup of pedestal tolerances!" ) ;
    return StatusCode::FAILURE;
  }
  if (m_pedTolerance.getDefaults().size()!=3) {
    ATH_MSG_ERROR ( "Configuration error: Expected three pedestal tolerance values (one per gain), got " 
                    << m_pedTolerance.getDefaults().size() ) ;
    return StatusCode::FAILURE;
  }
  if (this->msgLvl(MSG::DEBUG)) m_pedTolerance.printDef();//for debugging....
  
  
  ATH_MSG_INFO ( "Initialize Pedestal RMS Tolerances (CaloCellGroup)" ) ;
  stat=m_rmsTolerance.setDefinition(m_caloId,m_rmsToleranceInit,msg());
  if (!stat) {
    ATH_MSG_ERROR ( "Failed to initialize CaloCellGroup of rms tolerances!" ) ;
    return StatusCode::FAILURE;
  }
  if (m_rmsTolerance.getDefaults().size()!=3) {
    ATH_MSG_ERROR ( "Configuration error: Expected three rms tolerance values (one per gain), got " 
                    << m_rmsTolerance.getDefaults().size() ) ;
    return StatusCode::FAILURE;
  }
  if (this->msgLvl(MSG::DEBUG)) m_rmsTolerance.printDef();//for debugging...


  ATH_MSG_INFO ( "Initialize FEB Pedestal Tolerances (CaloCellGroup)" ) ;
  stat=m_pedToleranceFEB.setDefinition(m_caloId,m_pedToleranceFEBInit,msg());
  if (!stat) {
    ATH_MSG_ERROR ( "Failed to initialize CaloCellGroup of pedestal tolerances!" ) ;
    return StatusCode::FAILURE;
  }
  if (m_pedToleranceFEB.getDefaults().size()!=3) {
    ATH_MSG_ERROR ( "Configuration error: Expected three pedestal tolerance values (one per gain), got " 
                    << m_pedToleranceFEB.getDefaults().size() ) ;
    return StatusCode::FAILURE;
  }
  if (this->msgLvl(MSG::DEBUG)) m_pedToleranceFEB.printDef();//for debugging....
  
  ATH_MSG_INFO ( "Initialize FEB Pedestal RMS Tolerances (CaloCellGroup)" ) ;
  stat=m_rmsToleranceFEB.setDefinition(m_caloId,m_rmsToleranceFEBInit,msg());
  if (!stat) {
    ATH_MSG_ERROR ( "Failed to initialize CaloCellGroup of rms tolerances!" ) ;
    return StatusCode::FAILURE;
  }
  if (m_rmsToleranceFEB.getDefaults().size()!=3) {
    ATH_MSG_ERROR ( "Configuration error: Expected three rms tolerance values (one per gain), got " 
                    << m_rmsToleranceFEB.getDefaults().size() ) ;
    return StatusCode::FAILURE;
  }
  if (this->msgLvl(MSG::DEBUG)) m_rmsToleranceFEB.printDef();//for debugging...

  return StatusCode::SUCCESS;
}

bool LArPedestalValidationAlg::validateChannel(const LArCondObj& ref, const LArCondObj& val, const HWIdentifier chid, const int gain) {

  if (gain<0 || gain>2) {
     ATH_MSG_ERROR ( "Unexpected gain value " << gain ) ;
     return false;
  }

  const float& pedVal=val.m_Pedestal;
  const float& pedRef=ref.m_Pedestal;

  const float& rmsVal=val.m_PedestalRMS;
  const float& rmsRef=ref.m_PedestalRMS;

  const Identifier id=m_larCablingSvc->cnvToIdentifier(chid);

  const float pedTolerance=m_pedTolerance.valuesForCell(id)[gain];
  const float rmsTolerance=m_rmsTolerance.valuesForCell(id)[gain];
  

  const HWIdentifier febid=m_onlineHelper->feb_Id(chid);

  DataPerFEB* dataPerFEB=&(m_vDataPerFEB.back());
  if (m_vDataPerFEB.size()==0 || dataPerFEB->febid!=febid) {//Got to new FEB
    m_vDataPerFEB.push_back(DataPerFEB(chid,febid,gain));
    dataPerFEB=&(m_vDataPerFEB.back());
  }

  dataPerFEB->pedVal+=pedVal;
  dataPerFEB->pedRef+=pedRef;
  dataPerFEB->rmsVal+=rmsVal;
  dataPerFEB->rmsRef+=rmsRef;
  ++(dataPerFEB->nEntries);


  m_pedGlobalVal+=pedVal;
  m_rmsGlobalVal+=rmsVal;
  m_pedGlobalRef+=pedRef;
  m_rmsGlobalRef+=rmsRef;
  ++m_nEntriesGlobal;


  if (fabs(pedVal-pedRef)> pedTolerance ||
      fabs(rmsVal-rmsRef)> rmsTolerance){
    if (m_nFailedValidation<m_maxmessages) {
      msg().precision(2);
      msg().setf(std::ios::fixed,std::ios::floatfield); 
      msg() <<  this->m_myMsgLvl << "Deviating! " << channelDescription(chid,gain) << " Ped: " << val.m_Pedestal 
            << " (" << ref.m_Pedestal << ", " << val.m_Pedestal-ref.m_Pedestal  << " ADC)" 
            << " RMS: " << val.m_PedestalRMS << " (" << ref.m_PedestalRMS << ", " 
            << ((val.m_PedestalRMS-ref.m_PedestalRMS)/ref.m_PedestalRMS)*100 << "%)" << endreq;
    
      ATH_MSG_DEBUG ( "Pedestal Tolerance: " <<  pedTolerance << " RMS Tolerance:" <<  rmsTolerance ) ;
    }
    if (m_nFailedValidation==m_maxmessages)
      msg() <<  this->m_myMsgLvl << "Channel deviation message has already been printed " << m_maxmessages << " times. Now silent..." << endreq;
    return false;
  }
  else
    return true;
}


bool LArPedestalValidationAlg::febSummary() {

  unsigned nBadFebs=0;

  msg().precision(3);
  msg().setf(std::ios::fixed,std::ios::floatfield); 

  std::vector<DataPerFEB>::iterator it=m_vDataPerFEB.begin();
  std::vector<DataPerFEB>::iterator it_e=m_vDataPerFEB.end();
  for (;it!=it_e;++it) {
    DataPerFEB& dataPerFeb=*it;
    dataPerFeb.pedVal/=dataPerFeb.nEntries;
    dataPerFeb.pedRef/=dataPerFeb.nEntries;
    dataPerFeb.rmsVal/=dataPerFeb.nEntries;
    dataPerFeb.rmsRef/=dataPerFeb.nEntries;


    const Identifier id=m_larCablingSvc->cnvToIdentifier(dataPerFeb.chid);
    const float& pedToleranceFEB=m_pedToleranceFEB.valuesForCell(id)[dataPerFeb.gain];
    const float& rmsToleranceFEB=m_rmsToleranceFEB.valuesForCell(id)[dataPerFeb.gain];

    if (fabs(dataPerFeb.pedVal-dataPerFeb.pedRef)>pedToleranceFEB ||
	fabs(dataPerFeb.rmsVal-dataPerFeb.rmsRef)>rmsToleranceFEB) {
   
      msg() <<  m_myMsgLvl << "Deviating!" <<channelDescription(dataPerFeb.febid, dataPerFeb.gain, true)
            << "Average Ped: " << dataPerFeb.pedVal << " (" << dataPerFeb.pedRef << ")" 
            << " RMS: " << dataPerFeb.rmsVal << " (" << dataPerFeb.rmsRef << ")" << endreq;
      ATH_MSG_DEBUG ( "Pdestal FEB Tolerance: " <<  pedToleranceFEB << " RMS FEB Tolerance:" <<  rmsToleranceFEB ) ;
      ++nBadFebs;
    }
  }

  if (nBadFebs) {
    msg() <<  m_myMsgLvl << "Found " << nBadFebs << " out of " << m_vDataPerFEB.size() << " FEBs devating from reference" << endreq;
    return false;
  }
  else {
    ATH_MSG_INFO ( "All " << m_vDataPerFEB.size() << " FEBs withing given tolerance." ) ;
    return true;
  }
}
StatusCode LArPedestalValidationAlg::summary() {
  StatusCode sc=StatusCode::SUCCESS;
  //1nd step: Check the FEB-averages:
  if (m_doFebAverages && !febSummary())
    sc=StatusCode::RECOVERABLE;

  //2st step: Call the summary method from base-class (single-channel summary)
  if (!LArPedestalValidationBase::summary().isSuccess())
    sc=StatusCode::RECOVERABLE;

  //3rd step: Check the gobal averages:
  if (m_nEntriesGlobal) {
    m_pedGlobalVal/=m_nEntriesGlobal;
    m_pedGlobalRef/=m_nEntriesGlobal;
    m_rmsGlobalVal/=m_nEntriesGlobal;
    m_rmsGlobalRef/=m_nEntriesGlobal;
  }
  msg().precision(3);
  msg().setf(std::ios::fixed,std::ios::floatfield); 
  ATH_MSG_INFO ( "Global pedestal average: " << m_pedGlobalVal << " Reference:" << m_pedGlobalRef
                 << " Deviation:" << m_pedGlobalVal-m_pedGlobalRef ) ;
  ATH_MSG_INFO ( "Global elec noise average: " << m_rmsGlobalVal << " Reference:" << m_rmsGlobalRef
                 << " Deviation:" << m_rmsGlobalVal-m_rmsGlobalRef ) ;
  return sc;
}
