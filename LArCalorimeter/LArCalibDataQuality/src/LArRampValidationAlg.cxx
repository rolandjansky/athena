/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibDataQuality/LArRampValidationAlg.h"
#include <cmath>

LArRampValidationAlg::LArRampValidationAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  LArRampValidationBase(name,pSvcLocator),
  m_hasRawRampContainer(false),
  m_rawRampContainer(0),
  m_rampGlobalVal(0),
  m_rmsGlobalVal(0),
  m_rampGlobalRef(0),
  m_rmsGlobalRef(0),
  m_nEntriesGlobal(0)
{
  declareProperty("KeyList", m_contKey);
  declareProperty("RawRampTimeMinADC",m_rawrampTimeADC=100,
		  "Minimal ADC, where the RampTime is computed");
  declareProperty("RampTolerance",m_toleranceInit,
		  "Allowed deviation of ramp (%)");
  declareProperty("RampToleranceFEB",m_toleranceInitFEB,
		  "Allowed deviation of ramp (average over one FEB,");
  declareProperty("RawRampTimeTolerance",m_rawrampTimeTolerance=1.,
		  "Allowed deviation of the reconstructed time (ns)");
}


StatusCode LArRampValidationAlg::preLoop() {

  //Initialize CellGroup object storing the threhsolds:
  if (m_tolerance.setDefinition(m_caloId,m_toleranceInit,msg())==false) {
    ATH_MSG_ERROR ( "Failed to initialize CaloCellGroup of thresholds!" ) ;
    return StatusCode::FAILURE;
  }

  if (m_tolerance.getDefaults().size()!=3) {
    ATH_MSG_ERROR ( "Expected three values in CaloCellGroup of thresholds for three gains!" ) ;
     return StatusCode::FAILURE;
  }
  if (this->msgLvl(MSG::DEBUG)) m_tolerance.printDef();//for debugging....


  //Initialize CellGroup object storing the threhsolds:
  if (m_toleranceFEB.setDefinition(m_caloId,m_toleranceInitFEB,msg())==false) {
    ATH_MSG_ERROR ( "Failed to initialize CaloCellGroup of thresholds!" ) ;
    return StatusCode::FAILURE;
  }

  if (m_toleranceFEB.getDefaults().size()!=3) {
    ATH_MSG_ERROR ( "Expected three values in CaloCellGroup of thresholds for three gains!" ) ;
     return StatusCode::FAILURE;
  }
  if (this->msgLvl(MSG::DEBUG)) m_toleranceFEB.printDef();//for debugging....


  m_rampGlobalRef=0.;
  m_rampGlobalVal=0.;
  m_nEntriesGlobal=0;
  m_rawRampContainer=NULL;

  // Retrieve Raw Ramps Container
  std::vector<std::string>::const_iterator key_it=m_contKey.begin();
  std::vector<std::string>::const_iterator key_it_e=m_contKey.end();
  m_hasRawRampContainer = false;
  for (;key_it!=key_it_e;key_it++) {
    StatusCode sc=detStore()->retrieve(m_rawRampContainer,*key_it);
    if (sc!=StatusCode::SUCCESS || !m_rawRampContainer) {
      ATH_MSG_WARNING ( "Unable to retrieve LArRawRampContainer with key " << *key_it ) ;
    } 
    else {
      ATH_MSG_DEBUG ( "Got LArRawRampContainer with key " << *key_it ) ;
      m_hasRawRampContainer = true;
    }
  }
  if (!m_hasRawRampContainer) 
    ATH_MSG_WARNING ( "No LArRawRampContainer found. Only fitted ramp will be tested " ) ;
  
  // Check Raw Ramps
  if(m_hasRawRampContainer){

    ATH_MSG_DEBUG ( "Checking LArRawRampContainer with key " << *key_it ) ;

    LArRawRampContainer::const_iterator cont_it=m_rawRampContainer->begin();
    LArRawRampContainer::const_iterator cont_it_e=m_rawRampContainer->end();
    for (;cont_it!=cont_it_e;cont_it++) {
      const std::vector<LArRawRamp::RAMPPOINT_t>& singleRamp=(*cont_it)->theRamp(); 
      for (unsigned int DACIndex=0; DACIndex<singleRamp.size(); DACIndex++) {

        ATH_MSG_DEBUG ( "DAC Index:" << DACIndex 
                        << " DAC value : " << singleRamp[DACIndex].DAC 
                        << " ADC value : " << singleRamp[DACIndex].ADC 
                        << " Time " << singleRamp[DACIndex].TimeMax )  ;

	// Check point with DAC > m_rawrampTimeDAC where DeltaT is meaningful
	if(singleRamp[DACIndex].ADC > m_rawrampTimeADC){
	  if (fabs(singleRamp[DACIndex].TimeMax) > m_rawrampTimeTolerance){ 
	    msg().setf(std::ios::fixed,std::ios::floatfield); 
	    msg().precision(2);
	    msg() << m_myMsgLvl << "Deviating! " << channelDescription((*cont_it)->channelID(),(*cont_it)->gain()) << " DeltaT=" << singleRamp[DACIndex].TimeMax << " DAC = " << singleRamp[DACIndex].DAC << endreq;
	  }
	  break; //Stop loop after testing the m_rawrampTimeDAC DAC point
	}

      } //end DAC points loop
    } // end channels loop
  }
  return StatusCode::SUCCESS;
}

bool LArRampValidationAlg::validateChannel(const LArCondObj& ref, const LArCondObj& val, const HWIdentifier chid, const int gain) {


  HWIdentifier febid=m_onlineHelper->feb_Id(chid);
  Identifier offlineID = m_larCablingSvc->cnvToIdentifier(chid);

  ++m_nEntriesGlobal;

  // Store average Ramp value per FEB
  DataPerFEB* dataPerFEB=&(m_vDataPerFEB.back());
  if (m_vDataPerFEB.size()==0 || dataPerFEB->febid!=febid) {//Got to new FEB
    m_vDataPerFEB.push_back(DataPerFEB(chid,febid,gain)); //Need to remember chid of one connected channel
    dataPerFEB=&(m_vDataPerFEB.back());
  }
  dataPerFEB->rampVal+=val.m_vRamp[1];
  dataPerFEB->rampRef+=ref.m_vRamp[1];
  ++(dataPerFEB->nEntries);

  m_rampGlobalVal+=val.m_vRamp[1];
  m_rampGlobalRef+=ref.m_vRamp[1];

  // Retrieve layer/eta 
  int layer = 0; int pos_neg = 0; int eta = 0 ; int region = 0;
 
  if(m_onlineHelper->isHECchannel(chid)){
    layer = m_hecId->sampling(offlineID);
    pos_neg = m_hecId->pos_neg(offlineID);
    region =  m_hecId->region(offlineID);  
    eta = m_hecId->eta(offlineID); 
  }
  if(m_onlineHelper->isFCALchannel(chid)){
    layer = m_fcalId->module(offlineID);
    pos_neg = m_fcalId->pos_neg(offlineID) ;
    region = 0;  
    eta = m_fcalId->eta(offlineID); 
  }
  if(m_onlineHelper->isEMECchannel(chid) || m_onlineHelper->isEMBchannel(chid)){
    layer = m_emId->sampling(offlineID);
    pos_neg = m_emId->barrel_ec(offlineID);
    region =  m_emId->region(offlineID);  
    eta = m_emId->eta(offlineID); 
  }

  // Store avg Ramp value per sector : pos_neg/region/layer/eta (average over phi)
  DataPerSector* dataPerSector=&(m_vDataPerSector.back());
  if (m_vDataPerSector.size()==0 || dataPerSector->pos_neg!=pos_neg || dataPerSector->region!=region
      || dataPerSector->layer!=layer  || dataPerSector->eta!=eta || dataPerSector->gain!=gain) {//Got to new sector
    m_vDataPerSector.push_back(DataPerSector(pos_neg,region,layer,eta,gain));
    dataPerSector=&(m_vDataPerSector.back());
  }
  dataPerSector->rampVal+=val.m_vRamp[1];
  //dataPerSector->rampRef+=ref.m_vRamp[1];
  ++(dataPerSector->nEntries);



  // Check individual channel for Ramp deviation
  const float& tolerance=m_tolerance.valuesForCell(offlineID)[gain];


  if (fabs(val.m_vRamp[1]-ref.m_vRamp[1])/ref.m_vRamp[1] > tolerance){
    if (m_nFailedValidation<m_maxmessages) {
      std::stringstream devMsg;
      devMsg.setf(std::ios::fixed,std::ios::floatfield); 
      devMsg.precision(3);
      devMsg << "Deviating! " << channelDescription(chid,gain) << " Ramp: " << val.m_vRamp[1] << " (" << ref.m_vRamp[1] << ", ";
      devMsg.precision(2);
      devMsg << 100*(val.m_vRamp[1]-ref.m_vRamp[1])/ref.m_vRamp[1] << "%)";
      msg() << this->m_myMsgLvl << devMsg.str() << endreq;
      ATH_MSG_DEBUG ( "Ramp Tolerance: " << tolerance ) ;
    }
    else if ( m_nFailedValidation==m_maxmessages)
      msg() <<  this->m_myMsgLvl << "Channel deviation message has already been printed " << m_maxmessages << " times. Now silent..." << endreq;
    
    return false;
  } 
  else 
    return true;

  
}

bool LArRampValidationAlg::febSummary() {

  // FEBs
  unsigned nBadFebs=0;
  std::vector<DataPerFEB>::iterator it=m_vDataPerFEB.begin();
  std::vector<DataPerFEB>::iterator it_e=m_vDataPerFEB.end();
  for (;it!=it_e;++it) {
    DataPerFEB& dataPerFeb=*it;
    dataPerFeb.rampVal/=dataPerFeb.nEntries;
    dataPerFeb.rampRef/=dataPerFeb.nEntries;

    ATH_MSG_DEBUG ( " nb of channels = "  << dataPerFeb.nEntries 
                    << " for FEB " << channelDescription(dataPerFeb.febid,true) ) ;  

    //Get offline identifier of channel 0 of this FEB, should be good enough ...
    const Identifier id=m_larCablingSvc->cnvToIdentifier(dataPerFeb.febid);
    const float& tolerance=m_toleranceFEB.valuesForCell(id)[dataPerFeb.gain];
    
    if (fabs(dataPerFeb.rampVal-dataPerFeb.rampRef)/dataPerFeb.rampRef > tolerance){
      msg().precision(3);
      msg().setf(std::ios::fixed,std::ios::floatfield); 
      msg() << m_myMsgLvl << "Deviating! " << channelDescription(dataPerFeb.febid,dataPerFeb.gain,true) << "Average Ramp: " 
            << dataPerFeb.rampVal << " (reference: " << dataPerFeb.rampRef << ")" << endreq;
      ATH_MSG_DEBUG ( "Ramp FEB average tolerance: " << tolerance ) ;
      ++nBadFebs;
    }
  }
  
  if (nBadFebs) {
    ATH_MSG_ERROR ( "Found " << nBadFebs << " out of " << m_vDataPerFEB.size() 
                    << " FEBs deviating from reference" ) ;
    return false;
  } else {
    ATH_MSG_INFO ( "All " << m_vDataPerFEB.size() 
                   << " FEBs withing given tolerance." ) ;
    return true;
  }
  /*
  // Sectors
  std::vector<DataPerSector>::iterator it2=m_vDataPerSector.begin();
  std::vector<DataPerSector>::iterator it2_e=m_vDataPerSector.end();
  for (;it2!=it2_e;++it2) {
    DataPerSector& dataPerSector=*it2;
    dataPerSector.rampVal/=dataPerSector.nEntries;
    //dataPerSector.rampRef/=dataPerSector.nEntries;

    (*m_log) << MSG::DEBUG << " nb of channels = "  << dataPerSector.nEntries 
	     << " for Sector : gain " <<  dataPerSector.gain 
	     << " pos_neg " <<  dataPerSector.pos_neg 
	     << " region " << dataPerSector.region
	     << " layer " << dataPerSector.layer 
	     << " eta " << dataPerSector.eta << endreq;  
  }
  */
}

bool LArRampValidationAlg::deviateFromAvg(const LArCondObj& val, const HWIdentifier chid, const int gain) {

 Identifier offlineID = m_larCablingSvc->cnvToIdentifier(chid);

  // Retrieve layer/eta 
  int layer = 0; int pos_neg = 0; int eta = 0 ; int region = 0;
 
  if(m_onlineHelper->isHECchannel(chid)){
    layer = m_hecId->sampling(offlineID);
    pos_neg = m_hecId->pos_neg(offlineID);
    region =  m_hecId->region(offlineID);  
    eta = m_hecId->eta(offlineID); 
  }
  if(m_onlineHelper->isFCALchannel(chid)){
    layer = m_fcalId->module(offlineID);
    pos_neg = m_fcalId->pos_neg(offlineID) ;
    region = 0;  
    eta = m_fcalId->eta(offlineID); 
  }
  if(m_onlineHelper->isEMECchannel(chid) || m_onlineHelper->isEMBchannel(chid)){
    layer = m_emId->sampling(offlineID);
    pos_neg = m_emId->barrel_ec(offlineID);
    region =  m_emId->region(offlineID);  
    eta = m_emId->eta(offlineID); 
  }

  // Loop over all sectors (instead of using a find() function )
  std::vector<DataPerSector>::iterator it2=m_vDataPerSector.begin();
  std::vector<DataPerSector>::iterator it2_e=m_vDataPerSector.end();
  for (;it2!=it2_e;++it2) {
    DataPerSector& dataPerSector=*it2;
    if (dataPerSector.pos_neg!=pos_neg || 
	dataPerSector.region!=region   || 
	dataPerSector.layer!=layer     || 
	dataPerSector.eta!=eta         ||
	dataPerSector.gain!=gain) continue;

    if (dataPerSector.rampVal == 0 ){
      ATH_MSG_ERROR ( "Found Sector with Ramp Average equals to zero" ) ;
      ATH_MSG_ERROR ( "Sector : pos_neg " <<  dataPerSector.pos_neg << " region " << dataPerSector.region
                      << " layer " << dataPerSector.layer << " eta " << dataPerSector.eta ) ;     
      return false;
    }else{
      float ratio = val.m_vRamp[1]/dataPerSector.rampVal;      
      if ( ratio > 2.){
	msg() << m_myMsgLvl << "!!! Deviating Sector channel =  " <<channelDescription(chid,dataPerSector.gain) << "Ramp: " << val.m_vRamp[1] << " (Average Sector Ramp: " << dataPerSector.rampRef << ")" << endreq;
	return false;
      }
    }
  }// end sector loop

  return true;
}

StatusCode LArRampValidationAlg::summary() {
  StatusCode sc=StatusCode::SUCCESS;
  //1nd step: Check the FEB-averages:
  if (m_doFebAverages && !febSummary())
    sc=StatusCode::RECOVERABLE;
  //2nd step: Call the summary method from base-class (single-channel summary)
  if (!LArRampValidationBase::summary().isSuccess())
    sc=StatusCode::RECOVERABLE;
  //3rd step: Check the gobal averages:
  if (m_nEntriesGlobal) {
    m_rampGlobalVal/=m_nEntriesGlobal;
    m_rampGlobalRef/=m_nEntriesGlobal;
    m_rmsGlobalVal/=m_nEntriesGlobal;
    m_rmsGlobalRef/=m_nEntriesGlobal;
  }
  ATH_MSG_INFO ( "Gobal ramp average: " << m_rampGlobalVal << " Reference: " << m_rampGlobalRef
                 << " Deviation: " << m_rampGlobalVal-m_rampGlobalRef ) ;

  return sc;
}
