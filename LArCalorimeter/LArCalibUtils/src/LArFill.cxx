/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArFill.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

LArFill::LArFill(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_detStore(0),
  m_lar_on_id(0),
  m_adc2dac(0),
  m_dac2ua(0),
  m_ua2mev(0),
  m_ADC2MeVtool(0)
{
  declareProperty("ADC2MeVTool",m_ADC2MeVToolName);
  declareProperty("keyADC2DAC",m_keyADC2DAC);
  declareProperty("keyDAC2uA",m_keyDAC2uA);
  declareProperty("keyuA2MeV",m_keyuA2MeV);
}

LArFill::~LArFill()
{  }

StatusCode LArFill::initialize()
{
  MsgStream  log(messageService(),name());

  // retrieve detector Store
  StatusCode sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) 
  {
    log << MSG::ERROR
	<< "Unable to retrieve pointer to DetectorStore "
	<< endreq;
    return sc;
  }

  sc = m_detStore->retrieve(m_lar_on_id,"LArOnlineID");
  if (sc.isFailure()) {
    log  << MSG::ERROR << "Unable to retrieve  LArOnlineID from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArFill::execute()
{
  MsgStream  log(messageService(),name());  
  
  log<<MSG::DEBUG<<" In execute() " << endreq;

  std::vector<HWIdentifier>::const_iterator it   = m_lar_on_id->channel_begin();
  std::vector<HWIdentifier>::const_iterator it_e = m_lar_on_id->channel_end();

  m_adc2dac = new LArRampComplete();
  m_dac2ua  = new LArDAC2uAComplete();
  m_ua2mev  = new LAruA2MeVComplete();
  m_adc2dac->initialize(); 
  m_dac2ua->initialize();
  m_ua2mev->initialize();

  std::vector<float> ramp;
  for (unsigned int i=0;i<3;i++){
    const float temp = (float)(i+1.);
    ramp.push_back(temp);
  }
  const float dac2ua = 1.;
  const float ua2mev = 0.5;

  int count = 0;
  for(; it!=it_e;++it){
    count ++;
    const HWIdentifier id = *it;
    m_dac2ua->set(id,dac2ua);
    m_ua2mev->set(id,ua2mev);
    for(unsigned int igain=0;igain<3;igain++){
      m_adc2dac->set(id,igain,ramp);
    }
  }  

  log<<MSG::DEBUG<<" Loop over " << count << " calo cells" << endreq;
 

  return StatusCode::SUCCESS;
}



StatusCode LArFill::stop()
{

  MsgStream  log(messageService(),name());  
  
  log<<MSG::DEBUG<<" In stop() " << endreq;

  StatusCode sc;
  sc = m_detStore->record(m_adc2dac, m_keyADC2DAC);
  sc = m_detStore->symLink(m_adc2dac, (ILArRamp*)m_adc2dac);

  sc = m_detStore->record(m_dac2ua, m_keyDAC2uA);
  sc = m_detStore->symLink(m_dac2ua, (ILArDAC2uA*)m_dac2ua);

  sc = m_detStore->record(m_ua2mev, m_keyuA2MeV);
  sc = m_detStore->symLink(m_ua2mev, (ILAruA2MeV*)m_ua2mev);
  
 //---- retrieve the ADC2MeV tool ----------------
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  IToolSvc* p_toolSvc;// Pointer to Tool Service
  sc = svcLoc->service("ToolSvc",p_toolSvc);
  if (sc.isFailure()) {
    log << MSG::FATAL
	<< " Tool Service not found "
	<< endreq;
    return StatusCode::FAILURE;
  }
  
  IAlgTool* algtool = 0;
  ListItem corr(m_ADC2MeVToolName);       
  sc = p_toolSvc->retrieveTool(m_ADC2MeVToolName, algtool);
  m_ADC2MeVtool=dynamic_cast<ILArADC2MeVTool*>(algtool);
  if (sc.isFailure() || !m_ADC2MeVtool ) {
    log << MSG::INFO
	<< "Unable to find tool for " << m_ADC2MeVToolName
	<< endreq;
    return StatusCode::FAILURE;
  }  


  std::vector<HWIdentifier>::const_iterator it   = m_lar_on_id->channel_begin();
 
  HWIdentifier id2 = *it;
  
  int gain = 1;
  const std::vector<float> result = m_ADC2MeVtool->ADC2MEV(id2,gain);
  
  log << MSG::FATAL << "result = " << result << endreq;

  return StatusCode::SUCCESS;
  
}

