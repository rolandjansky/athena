/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArFill.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

LArFill::LArFill(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
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
  ATH_CHECK( detStore()->retrieve(m_lar_on_id,"LArOnlineID") );
  return StatusCode::SUCCESS;
}

StatusCode LArFill::execute()
{
  ATH_MSG_DEBUG(" In execute() " );

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

  ATH_MSG_DEBUG(" Loop over " << count << " calo cells" );
  return StatusCode::SUCCESS;
}



StatusCode LArFill::stop()
{
  ATH_MSG_DEBUG(" In stop() " );

  ATH_CHECK( detStore()->record(m_adc2dac, m_keyADC2DAC) );
  ATH_CHECK( detStore()->symLink(m_adc2dac, (ILArRamp*)m_adc2dac) );
  ATH_CHECK( detStore()->record(m_dac2ua, m_keyDAC2uA) );
  ATH_CHECK( detStore()->symLink(m_dac2ua, (ILArDAC2uA*)m_dac2ua) );
  ATH_CHECK( detStore()->record(m_ua2mev, m_keyuA2MeV) );
  ATH_CHECK( detStore()->symLink(m_ua2mev, (ILAruA2MeV*)m_ua2mev) );
  
  IAlgTool* algtool = 0;
  ListItem corr(m_ADC2MeVToolName);       
  ATH_CHECK( toolSvc()->retrieveTool(m_ADC2MeVToolName, algtool) );
  m_ADC2MeVtool=dynamic_cast<ILArADC2MeVTool*>(algtool);
  if (!m_ADC2MeVtool ) {
    ATH_MSG_INFO( "Unable to find tool for " << m_ADC2MeVToolName );
    return StatusCode::FAILURE;
  }  

  std::vector<HWIdentifier>::const_iterator it   = m_lar_on_id->channel_begin();
 
  HWIdentifier id2 = *it;
  
  int gain = 1;
  const std::vector<float> result = m_ADC2MeVtool->ADC2MEV(id2,gain);
  
  ATH_MSG_FATAL ( "result = " << result );
  return StatusCode::SUCCESS;
}

