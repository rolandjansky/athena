/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  D3PD::TriggerTowerFillerTool.cxx
*/
#include "TriggerTowerFillerTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AthenaKernel/errorcheck.h"
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"

using namespace std;

namespace D3PD {

  TriggerTowerFillerTool::TriggerTowerFillerTool(const std::string& type, 
						   const std::string& name, 
						   const IInterface* parent) : 
    BlockFillerTool<LVL1::TriggerTower>(type, name, parent)
  {
    book().ignore(); // Avoid coverity warnings.
  }

  TriggerTowerFillerTool::~TriggerTowerFillerTool() {
  }
  
  StatusCode TriggerTowerFillerTool::book() {
    CHECK(addVariable("em_E", m_energy_em));
    CHECK(addVariable("had_E", m_energy_had));

    CHECK(addVariable("em_peak", m_em_peak));
    CHECK(addVariable("had_peak", m_had_peak));

    CHECK(addVariable("em_adc_peak", m_em_adc_peak));
    CHECK(addVariable("had_adc_peak", m_had_adc_peak));

    CHECK(addVariable("em_lut", m_em_lut));
    CHECK(addVariable("had_lut", m_had_lut));

    CHECK(addVariable("em_adc", m_em_adc));
    CHECK(addVariable("had_adc", m_had_adc));

    CHECK(addVariable("eta", m_eta));
    CHECK(addVariable("phi", m_phi));

    return StatusCode::SUCCESS;
  }

  StatusCode TriggerTowerFillerTool::fill(const LVL1::TriggerTower& x) {
      *m_energy_em = x.emEnergy();
      *m_energy_had = x.hadEnergy();
      *m_eta = x.eta();
      *m_phi = x.phi();

      *m_em_lut = x.emLUT();
      *m_had_lut = x.hadLUT();

      *m_em_adc = x.emADC();
      *m_had_adc = x.hadADC();

      *m_em_peak = x.emPeak();
      *m_had_peak = x.hadPeak();

      *m_em_adc_peak = x.emADCPeak();
      *m_had_adc_peak = x.hadADCPeak();

    return StatusCode::SUCCESS;
  }


}
