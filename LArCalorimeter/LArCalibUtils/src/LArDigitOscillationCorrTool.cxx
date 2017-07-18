/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArDigitOscillationCorrTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArH6Oscillation.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "CaloIdentifier/CaloIdManager.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>


using CLHEP::hertz;
using CLHEP::ns;


LArDigitOscillationCorrTool::LArDigitOscillationCorrTool(const std::string& type, 
							 const std::string& name, 
							 const IInterface* parent) 
  : AthAlgTool(type, name, parent) , 
    m_priority(1400), 
    m_nSigma(3.0), 
    m_eventPhase(0),
    m_omega(1.024e6*hertz),
    m_emId(0), m_fcalId(0), m_hecId(0), m_lar_on_id(0),
    m_cablingService("LArCablingService")
{
  declareInterface<ILArDigitOscillationCorrTool>(this);
  declareProperty("BeginRunPriority",m_priority);
  declareProperty("SignalCutInSigma",m_nSigma);
  declareProperty("Omega",m_omega);
}

StatusCode LArDigitOscillationCorrTool::initialize()
{
  ATH_MSG_DEBUG ( "LArDigitOscillationCorrTool initialize() begin" );
  
  ATH_CHECK( detStore()->retrieve(m_lar_on_id,"LArOnlineID") );
  
  const CaloIdManager *caloIdMgr=CaloIdManager::instance();
  m_emId   = caloIdMgr->getEM_ID();
  m_fcalId = caloIdMgr->getFCAL_ID();
  m_hecId  = caloIdMgr->getHEC_ID();
  
  ATH_CHECK( m_cablingService.retrieve() );
  
  IIncidentSvc* incSvc = nullptr;
  ATH_CHECK( service("IncidentSvc", incSvc) );
  
  //start listening to "BeginRun"
  incSvc->addListener(this, "BeginRun", m_priority);
  
  ATH_MSG_DEBUG ( "LArDigitOscillationCorrTool initialize() end" );
  return StatusCode::SUCCESS;
}

StatusCode LArDigitOscillationCorrTool::calculateEventPhase(const LArDigitContainer &theDC)
{
  ATH_MSG_DEBUG ( "In execute: calculate event phase" );
  
  //Pointer to conditions data objects to get pedetestals
  ATH_MSG_DEBUG ( "Retrieving pedestal " );
  const ILArPedestal* larPedestal=nullptr;
  ATH_CHECK( detStore()->retrieve(larPedestal) );
  
  //Pointer to conditions data objects to get channel phases
  ATH_MSG_DEBUG ( "Retrieving channel phases and amplituides " );
  const ILArH6Oscillation* larH6Oscillations = nullptr;
  ATH_CHECK( detStore()->retrieve(larH6Oscillations) );
  unsigned int iHECChan(0);
  std::vector<std::vector<short> > theSamples;
  std::vector<float> thePedestals;
  std::vector<double> theChannelAmplitudes;
  std::vector<double> theChannelPhases;
  std::vector<float> theRMSValues;
  
  //Now all data is available, start loop over Digit Container
  ATH_MSG_DEBUG ( "Loop over Digit Container with size <" << theDC.size() << ">"  );
  
  for (unsigned int i=0;i<theDC.size();i++) {
    //Get data from LArDigit
    const LArDigit *theDigit = theDC[i];
    const std::vector<short>& samples  = theDigit->samples();
    const HWIdentifier        chid  = theDigit->channelID();
    const CaloGain::CaloGain  gain     = theDigit->gain();
    
    // use only HEC channels
    const Identifier id = m_cablingService->cnvToIdentifier(chid);    
    bool isHEC = false;
    
    if (m_hecId->is_lar_hec(id)) {
      isHEC = true;
    }

    if ( isHEC) {
      float DBpedestalRMS=larPedestal->pedestalRMS(chid,gain);
      if (DBpedestalRMS <= (1.0+LArElecCalib::ERRORCODE)) {
	ATH_MSG_DEBUG ( "No pedestal RMS found for this cell. Exiting ...." );
	return StatusCode::FAILURE;
      }
      //    log << MSG::DEBUG << "Retriving channelPhase " << endmsg;
      const double& DBchannelPhase=larH6Oscillations->channelPhase(chid);
      //     log << MSG::DEBUG << "Retriving channelAmplitude " << endmsg;
      const double& DBchannelAmplitude=larH6Oscillations->channelAmplitude(chid);
      
      if( DBpedestalRMS > 0 && DBchannelAmplitude>0 ) { 
	theRMSValues.push_back(DBpedestalRMS);
	theSamples.push_back(samples);
	theChannelPhases.push_back(DBchannelPhase);
	theChannelAmplitudes.push_back(DBchannelAmplitude);
	float DBpedestal=larPedestal->pedestal(chid,gain);
	if (DBpedestal >= (1.0+LArElecCalib::ERRORCODE))
	  thePedestals.push_back(DBpedestal);
	else {
	  ATH_MSG_DEBUG ( "No valid pedestal found for this cell. Exiting ...." );
	  return StatusCode::FAILURE;
	}
	iHECChan++;
      }
    }
  }
  
  ATH_MSG_DEBUG ( "Start calculating the event phase" );
  double lchimin = -1;  
  int nTotSamples = 0;
  for(double myEventPhase = -M_PI; myEventPhase<M_PI; myEventPhase+=0.1) {
    double lchitest = 0;
    for (unsigned int i=0;i<iHECChan;i++) {
      unsigned int nSamples = theSamples[i].size();
      for(unsigned int j=0;j<nSamples;j++) {
	// exclude all samples (and the previous and next sample) which 
	//have more than nSigma worth of absolute signal 
	if ( j == 0 || !(fabs(theSamples[i][j] - theSamples[i][0]) > m_nSigma*theRMSValues[i] 
			 || ( j > 0 && fabs(theSamples[i][j-1] - theSamples[i][0]) > m_nSigma*theRMSValues[i])
			 || ( j < nSamples-1 && fabs(theSamples[i][j+1] - theSamples[i][0]) > m_nSigma*theRMSValues[i])) ) {
	  
	  if ( lchimin < 0 )
	    nTotSamples ++;
	  lchitest += pow((theSamples[i][j] - thePedestals[i] 
			   - theChannelAmplitudes[i]*sin(j*25*ns*m_omega
							 +theChannelPhases[i]+myEventPhase))
			  /theRMSValues[i],2);
	}
      }
    }
    if ( lchitest < lchimin || lchimin < 0 ) {
      lchimin = lchitest;
      m_eventPhase = myEventPhase;
    }   
  }
  
  ATH_MSG_DEBUG ( "Ending eventphase calculation. Phase = <" << m_eventPhase << ">, Number of Channels used = <" << iHECChan << ">, Average Number of Samples Used = <"
                  << (iHECChan > 0 ? (double)nTotSamples/iHECChan : 0)
                  << ">" );
  return StatusCode::SUCCESS;
}


StatusCode LArDigitOscillationCorrTool::correctLArDigits(LArDigitContainer &theDC)
{
  ATH_MSG_DEBUG ( "In execute: correct LAr Digit" );
  
  //Pointer to conditions data objects to get channel phases
  const ILArH6Oscillation* larH6Oscillations = nullptr;
  ATH_CHECK( detStore()->retrieve(larH6Oscillations) );
  
  std::vector<double> theChannelAmplitudes;
  std::vector<double> theChannelPhases;
  
  //Now all data is available, start loop over Digit Container
  ATH_MSG_DEBUG ( "Loop over Digit Container " );
  
  for (unsigned int i=0;i<theDC.size();i++) {
    //Get data from LArDigit
    LArDigit * theDigit = theDC[i];
    const std::vector<short>& samples  = theDigit->samples();
    unsigned int              nSamples = samples.size(); 
    const HWIdentifier        chid     = theDigit->channelID();
    const CaloGain::CaloGain  gain     = theDigit->gain();
    
    // correct only HEC channels
    const Identifier id = m_cablingService->cnvToIdentifier(chid);    
    bool isHEC = false;
    
    if (m_hecId->is_lar_hec(id)) {
      isHEC = true;
    }
    
    if ( isHEC) {
      const double& DBchannelPhase=larH6Oscillations->channelPhase(chid);
      const double& DBchannelAmplitude=larH6Oscillations->channelAmplitude(chid);
      ATH_MSG_DEBUG ( "The HWId is " << chid 
                      << ", the offline Id is " << m_hecId->show_to_string(id,0,'/') 
                      << ", the ChannelAmplitude is " <<  DBchannelAmplitude 
                      << ", the ChannelPhase is " <<  DBchannelPhase );
      ATH_MSG_DEBUG  ( "m_omega value is " << m_omega );
      if( DBchannelAmplitude>0 ) { 
	std::vector<short> new_samples(nSamples);
	for(unsigned int j=0;j<nSamples;j++)  
	  new_samples[j] = (short)(samples[j] + 0.5 
				   - DBchannelAmplitude*sin(j*25*ns*m_omega+DBchannelPhase+m_eventPhase));
	HWIdentifier new_id(chid);
	CaloGain::CaloGain new_gain(gain);
	LArDigit * theNewDigit = new LArDigit(new_id,new_gain,new_samples);
	theDC[i] = theNewDigit;
      }
    }
  }
  return StatusCode::SUCCESS;
}


void LArDigitOscillationCorrTool::handle(const Incident& /* inc*/ )
{
  ATH_MSG_DEBUG ( "LArDigitOscillationCorrTool handle()" );
  
  this->retrieveDB();
  return;
}

// *** retrieve subfactors from the DB *** 
StatusCode LArDigitOscillationCorrTool::retrieveDB()
{
  ATH_MSG_DEBUG ( "in retrieveDB() " );
  return StatusCode::SUCCESS;
}


