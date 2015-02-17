/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __D3PD_TriggerTowerFillerTool_h__
#define __D3PD_TriggerTowerFillerTool_h__
/*
  D3PD::TriggerTowerFillerTool.h
*/
#include <vector>
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/TriggerTower.h"

namespace D3PD {

  class TriggerTowerFillerTool : public D3PD::BlockFillerTool<LVL1::TriggerTower> {
  public:
    TriggerTowerFillerTool(const std::string& type, const std::string& name, 
			    const IInterface* parent);
    ~TriggerTowerFillerTool();
    
    StatusCode book();
    StatusCode fill(const LVL1::TriggerTower& x);

  private:
    // Properties


    // D3PD variables
    float* m_energy_em;
    float* m_energy_had;
    float* m_eta;
    float* m_phi;

    std::vector<int>* m_em_lut; 
    std::vector<int>* m_had_lut; 
    std::vector<int>* m_em_adc; 
    std::vector<int>* m_had_adc; 

    int* m_em_peak ;
    int* m_had_peak ;
    int* m_em_adc_peak ;
    int* m_had_adc_peak ;

  };

}

#endif // __D3PD_TriggerTowerFillerTool_h__

