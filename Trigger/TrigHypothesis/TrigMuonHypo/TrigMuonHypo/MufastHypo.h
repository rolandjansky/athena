/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUFASTHYPO_H 
#define TRIG_MUFASTHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace MufastHypoConsts {
enum ECRegions{ Bulk, WeakBFieldA, WeakBFieldB };
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class MufastHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
  public:
    MufastHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~MufastHypo();
    
    HLT::ErrorCode hltInitialize();    
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
    HLT::ErrorCode hltFinalize();
    
    
  private:
    
    float getLocalPhi(float, float, float) const;
    MufastHypoConsts::ECRegions whichECRegion(const float eta, const float phi) const;
    
    // Properties:
    std::vector<float> m_ptBins;
    std::vector<float> m_ptThresholds;
    BooleanProperty m_acceptAll;
    BooleanProperty m_selectPV;
    float m_Z_PV;
    float m_R_PV;
    
    float m_ptThresholdForECWeakBRegionA;
    float m_ptThresholdForECWeakBRegionB;

    // Other members:   
    StoreGateSvc* m_storeGate;
    
    std::vector<float>::size_type m_bins;
    float m_fex_pt;
    float m_fex_eta;
    float m_fex_phi;
    
    float m_x_at_station;
    float m_y_at_station;
    float m_z_at_station;
    float m_x_at_beam;
    float m_z_at_beam;
    
    
};

#endif
