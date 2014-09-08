/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArTimeChecker.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "TBEvent/TBPhase.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArRamp.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArOFCTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"


LArTimeChecker::LArTimeChecker (const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_DataLocation("FREE"),m_AdcCut(1300)
{
  declareProperty("DataLocation", m_DataLocation );
  declareProperty("ADCCut", m_AdcCut );
}


StatusCode LArTimeChecker::initialize(){
  return StatusCode::SUCCESS;
}



StatusCode LArTimeChecker::execute() {
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  
  const LArDigitContainer* larDigitContainer=NULL;
  const ILArPedestal* larPedestal;
  const ILArOFC* larOFC;
  
  std::vector<short> seed_samples;
  HWIdentifier seed_cell;
  CaloGain::CaloGain gain2 = CaloGain::LARHIGHGAIN;
  short maxSample=0;
  
  sc=evtStore()->retrieve(larDigitContainer,m_DataLocation);
  if(sc.isFailure()) {
    log << MSG::ERROR << "Can't retrieve LArDigitContainer with key " <<m_DataLocation << " from StoreGate." << endreq;
    return StatusCode::FAILURE;
  }
  
  sc=detStore()->retrieve(larPedestal);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Can't retrieve LArPedestal from Conditions Store" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc=detStore()->retrieve(larOFC);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Can't retrieve LArOFC from Conditions Store" << endreq;
    return StatusCode::FAILURE;
  }
  

  LArDigitContainer::const_iterator cell_it=larDigitContainer->begin();
  LArDigitContainer::const_iterator cell_it_e=larDigitContainer->end();


  for (;cell_it!=cell_it_e;cell_it++) {

    const std::vector<short>& samples=(*cell_it)->samples();
    //const unsigned nSamples=samples.size(); 
    const HWIdentifier chid=(*cell_it)->channelID();
    const CaloGain::CaloGain gain=(*cell_it)->gain();
      
    if(samples[2]>maxSample)
      {
	maxSample=samples[2];
	seed_cell=chid;
	seed_samples=samples;
	gain2=gain;
      }
    }
 
  if (seed_samples[2]<m_AdcCut)
    return StatusCode::SUCCESS;
  

  float tmin=999.;
  int phase=-1;
  float pedestal=larPedestal->pedestal(seed_cell,gain2);
  if (pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
     log << MSG::WARNING << "invalid pedestal for cell " << seed_cell << endreq;       
     pedestal = seed_samples[0];
  }

 
  for(int tbin=0;tbin<25;tbin++){
    float time=0;
    float adc_reco=0;
    
    ILArOFC::OFCRef_t ofc_b=larOFC->OFC_b(seed_cell,gain2,tbin);
    ILArOFC::OFCRef_t ofc_a=larOFC->OFC_a(seed_cell,gain2,tbin);
    
    if(ofc_a.size()==0||ofc_b.size()==0)
      {
	log<<MSG::INFO<<"OFC not found for this channel with phase "<<tbin<<endreq;
	continue;
      } 

    const unsigned nSamples=seed_samples.size();
    for(unsigned j=0;j<nSamples-1;j++){
      time+=(seed_samples[j]-pedestal)*ofc_b.at(j);
      adc_reco+=(seed_samples[j]-pedestal)*ofc_a.at(j);
    }
    time/=adc_reco;
    //std::cout << "tbin=" << tbin << " time=" << time;
    if (fabs(time)<fabs(tmin)) {
      tmin=time;
      phase=tbin;
      //std::cout << " PHASE=" << phase;
    }
    //std::cout << std::endl;
  }
  //std::cout << "Phase=" << phase << std::endl;
  //TBPhase(float phase,short phaseind) 
  float peakTime=phase+tmin;
  TBPhase* theTBPhase = new TBPhase(peakTime,phase);
  
  sc = evtStore()->record(theTBPhase, "TBPhase");
  if (sc.isFailure( )) {
    log << MSG::FATAL << "Cannot record TBPhase" << endreq;
    return StatusCode::FAILURE ;
  }
  std::cout << "Found phase=" << phase << " tmin=" << tmin << " at cell 0x" << std::hex << seed_cell.get_compact() 
	    << std::dec << std::endl;
  return StatusCode::SUCCESS;
}


