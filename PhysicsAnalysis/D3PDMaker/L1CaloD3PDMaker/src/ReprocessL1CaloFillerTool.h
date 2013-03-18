// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
  * @file L1CaloD3PDMaker/ReprocessL1CaloFillerTool.h
  * @author John Morris <john.morris@cern.ch>
  * @date  May 2011
  * @brief Block filler tool for reprocessed L1Calo information.
  * @brief L1Calo settings can be changed and the entire system re-run
  * @brief from ADC values all the way to RoIs
  * @brief This D3PD algorithm records the reprocessed L1Calo output
  * @brief It is designed to be run many times per event (i.e. - test 20 different configs!)
  */

#ifndef _L1CALOD3PDMAKER_REPROCESSL1CALOFILLERTOOL_H_
#define _L1CALOD3PDMAKER_REPROCESSL1CALOFILLERTOOL_H_


#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/VoidBlockFillerTool.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"

#include "TrigT1CaloToolInterfaces/IL1RoITools.h"
#include "TrigT1CaloToolInterfaces/IL1JetElementTools.h"
#include "TrigT1CaloToolInterfaces/IL1CPHitsTools.h"
#include "TrigT1CaloToolInterfaces/IL1EmTauTools.h"
#include "TrigT1CaloToolInterfaces/IL1JetTools.h"
#include "TrigT1CaloToolInterfaces/IL1EtTools.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"

#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/CPMRoICollection.h"
#include "TrigT1CaloEvent/CPMHitsCollection.h"
#include "TrigT1CaloUtils/CPAlgorithm.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"

#include "TrigT1CaloEvent/JetElementCollection.h"

#include <vector>


namespace D3PD{

  class ReprocessL1CaloFillerTool : public BlockFillerTool<void>
  {
  public:
    ReprocessL1CaloFillerTool (const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

    /// Standard Gaudi initialize method.
    virtual StatusCode initialize();

    virtual StatusCode book();
    virtual StatusCode fill();

  protected:

    StatusCode loadTools();
    float AtlasPhi(const float& L1CaloPhi) const;

    void ppm(const TriggerTowerCollection* tts);

    void cpm(const TriggerTowerCollection* tts,std::vector< EmTau_ROI > rois);
    void cpmhits(const CPMRoICollection* cpmrois);

    void jem(const TriggerTowerCollection* tts);


    
    void rois_emTau(const std::vector< EmTau_ROI > rois);
    void rois_jet(const std::vector< Jet_ROI > rois);
    void rois_energy(const std::vector< EnergySum_ROI > rois);
    

  private:

    ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools> m_tools;
    ToolHandle<LVL1::IL1RoITools>                     m_roiTool;
    ToolHandle<LVL1::IL1JetElementTools>              m_JetElementTool;
    ToolHandle<LVL1::IL1CPHitsTools>                  m_CpHitsTool;
    ToolHandle<LVL1::IL1EmTauTools>                   m_EmTauTool;
    ToolHandle<LVL1::IL1JetTools>                     m_JetTool;
    ToolHandle<LVL1::IL1EtTools>                      m_EtTool;
     
    std::string m_triggerTowerLocation;
    std::string m_reprocessName;

    bool m_doPPM;
    bool m_doCPM;
    bool m_doJEM;
    bool m_doCMM;
    bool m_doROI;

    // *********************** PPM ***********************
    std::vector<unsigned int>*          m_ppm_em_coolId;
    std::vector< std::vector<int> >*    m_ppm_em_LUT;
    std::vector<int>*                   m_ppm_em_Energy;

    std::vector<unsigned int>*          m_ppm_had_coolId;
    std::vector< std::vector<int> >*    m_ppm_had_LUT;
    std::vector<int>*                   m_ppm_had_Energy;

    // *********************** CPM ***********************
    // CPM Tower
    int*                                m_cpmtower_n;
    std::vector<int>*                   m_cpmtower_emEnergy;
    std::vector<int>*                   m_cpmtower_hadEnergy;
    std::vector<std::vector<int> >*     m_cpmtower_emEnergyVec;
    std::vector<std::vector<int> >*     m_cpmtower_hadEnergyVec;
    std::vector<int>*                   m_cpmtower_peak;
    std::vector<int>*                   m_cpmtower_emError;
    std::vector<int>*                   m_cpmtower_hadError;
    std::vector<std::vector<int> >*     m_cpmtower_emErrorVec;
    std::vector<std::vector<int> >*     m_cpmtower_hadErrorVec;
    std::vector<float>*                 m_cpmtower_eta;
    std::vector<float>*                 m_cpmtower_l1caloPhi;
    std::vector<float>*                 m_cpmtower_atlasPhi;

    // CPM RoI
    int*                                m_cpmroi_n;
    std::vector<int>*                   m_cpmroi_crate;
    std::vector<int>*                   m_cpmroi_cpm;
    std::vector<int>*                   m_cpmroi_chip;
    std::vector<int>*                   m_cpmroi_location;
    std::vector<int>*                   m_cpmroi_hits;
    std::vector<int>*                   m_cpmroi_error;
    std::vector<uint32_t>*              m_cpmroi_roiWord;
    std::vector<float>*                 m_cpmroi_eta;
    std::vector<float>*                 m_cpmroi_l1caloPhi;
    std::vector<float>*                 m_cpmroi_atlasPhi;
    std::vector<int>*                   m_cpmroi_core;
    std::vector<int>*                   m_cpmroi_emClus;
    std::vector<int>*                   m_cpmroi_tauClus;
    std::vector<int>*                   m_cpmroi_emIsol;
    std::vector<int>*                   m_cpmroi_hadIsol;
    std::vector<int>*                   m_cpmroi_hadVeto;
    std::vector<unsigned int>*          m_cpmroi_isEtMax;

    // CPM Hits
    int*                                m_cpmhits_n;
    std::vector<int>*                   m_cpmhits_crate;
    std::vector<int>*                   m_cpmhits_module;
    std::vector<int>*                   m_cpmhits_peak;
    std::vector<unsigned int>*          m_cpmhits_hitWord0;
    std::vector<unsigned int>*          m_cpmhits_hitWord1;
    std::vector<std::vector <unsigned int> >*  m_cpmhits_hitsVec0;
    std::vector<std::vector <unsigned int> >*  m_cpmhits_hitsVec1;

    // *********************** JEM ***********************
    // Jet Element
    int*                                m_jem_jetel_n;
    std::vector<float>*                 m_jem_jetel_eta;
    std::vector<float>*                 m_jem_jetel_l1caloPhi;
    std::vector<float>*                 m_jem_jetel_atlasPhi;
    std::vector<int>*                   m_jem_jetel_emEnergy;
    std::vector<int>*                   m_jem_jetel_hadEnergy;
    std::vector<int>*                   m_jem_jetel_energy;
    std::vector<unsigned int>*          m_jem_jetel_isSaturated;
    std::vector<unsigned int>*          m_jem_jetel_isEmSaturated;
    std::vector<unsigned int>*          m_jem_jetel_isHadSaturated;
    std::vector<int>*                   m_jem_jetel_emError;
    std::vector<int>*                   m_jem_jetel_hadError;
    std::vector<int>*                   m_jem_jetel_linkError;
    std::vector<int>*                   m_jem_jetel_peak;
    std::vector<std::vector<int> >*     m_jem_jetel_emEnergyVec;
    std::vector<std::vector<int> >*     m_jem_jetel_hadEnergyVec;
    std::vector<std::vector<int> >*     m_jem_jetel_emErrorVec;
    std::vector<std::vector<int> >*     m_jem_jetel_hadErrorVec;
    std::vector<std::vector<int> >*     m_jem_jetel_linkErrorVec;

    // *********************** CMM ***********************
    // 

    // ROIs
    // EmTau ROI
    int*                                m_roi_emtau_n;
    std::vector<uint32_t>*              m_roi_emtau_roiWord;
    std::vector<float>*                 m_roi_emtau_core;
    std::vector<float>*                 m_roi_emtau_emClus;
    std::vector<float>*                 m_roi_emtau_tauClus;
    std::vector<float>*                 m_roi_emtau_emIsol;
    std::vector<float>*                 m_roi_emtau_hadIsol;
    std::vector<float>*                 m_roi_emtau_hadCore;
    std::vector<float>*                 m_roi_emtau_eta;
    std::vector<float>*                 m_roi_emtau_phi;

    // Jet ROI
    int*                                m_roi_jet_n;
    std::vector<float>*                 m_roi_jet_roiWord;
    std::vector<float>*                 m_roi_jet_et4x4;
    std::vector<float>*                 m_roi_jet_et6x6;
    std::vector<float>*                 m_roi_jet_et8x8;
    std::vector<float>*                 m_roi_jet_eta;
    std::vector<float>*                 m_roi_jet_phi;

    // Energy ROI
    uint32_t*                           m_roi_energy_roiWord0;
    uint32_t*                           m_roi_energy_roiWord1;
    uint32_t*                           m_roi_energy_roiWord2;
    float*                              m_roi_energy_eXmiss;
    float*                              m_roi_energy_eYmiss;    
    float*                              m_roi_energy_energyT;
    unsigned int*                       m_roi_energy_overflowX;
    unsigned int*                       m_roi_energy_overflowY;    
    unsigned int*                       m_roi_energy_overflowT;    
  };
  
} //end of namespace
#endif
