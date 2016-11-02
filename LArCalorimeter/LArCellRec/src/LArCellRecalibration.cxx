/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellRec/LArCellRecalibration.h" 
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloGain.h"


LArCellRecalibration::LArCellRecalibration (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
  CaloCellCorrection(type, name, parent),
  m_adc2mevTool("LArADC2MevToolDefault"),
  m_adc2mevToolOnline("LArADC2MeVToolOnline"),
  m_useInterceptOld(false),
  m_useInterceptNew(true)
{ 
  declareInterface<CaloCellCorrection>(this); 
  declareProperty("adc2MeVTool",m_adc2mevTool);
  declareProperty("adc2MeVToolOnline",m_adc2mevToolOnline);
  declareProperty("useIntercept",m_useInterceptNew);
  declareProperty("useInterceptOnline",m_useInterceptOld);
}
                                                                                

LArCellRecalibration::~LArCellRecalibration() {}


StatusCode LArCellRecalibration::initialize() {
  ATH_MSG_INFO( " initialization "  );
  ATH_CHECK( m_adc2mevTool.retrieve() );
  ATH_CHECK( m_adc2mevToolOnline.retrieve() );
  return StatusCode::SUCCESS;
}


void LArCellRecalibration::MakeCorrection(CaloCell* theCell) {

  Identifier id = theCell->ID();
  CaloGain::CaloGain gain = theCell->gain();
  float newEnergy=theCell->energy();
  const std::vector<float>& adc2mev_new = m_adc2mevTool->ADC2MEV(id,gain);
  const std::vector<float>& adc2mev_old = m_adc2mevToolOnline->ADC2MEV(id,gain);

  if (adc2mev_old[1]>0 && adc2mev_new[1]>0) {
    float adc =  newEnergy/adc2mev_old[1];
    if (m_useInterceptOld && gain != CaloGain::LARHIGHGAIN) adc = adc - adc2mev_old[0];

    newEnergy = 0.;
    float adcPower = adc;
    for (unsigned int i=1;i<adc2mev_new.size();i++) {
       newEnergy += adc2mev_new[i]*adcPower;
       adcPower = adcPower*adc; 
    }
    if (m_useInterceptNew && gain != CaloGain::LARHIGHGAIN) newEnergy = newEnergy + adc2mev_new[0];
  }

  theCell->setEnergy(newEnergy);
  return;
}
