/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// L1 objects
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

// xAOD
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
// =============================================

#include "PprMonitorAlgorithm.h"


PprMonitorAlgorithm::PprMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
}

StatusCode PprMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("PprMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);
  ATH_MSG_DEBUG("m_xAODTriggerTowerContainerName "<< m_xAODTriggerTowerContainerName); 
  ATH_MSG_DEBUG("m_TT_ADC_HitMap_Thresh " << m_TT_ADC_HitMap_Thresh);

  // we initialise all the containers that we need
  ATH_CHECK( m_xAODTriggerTowerContainerName.initialize() );
  
  // retrieve any tools if needed
  //ATH_CHECK(myTool.retrieve());
  
 
  return AthMonitorAlgorithm::initialize();
}

StatusCode PprMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG("PprMonitorAlgorithm::fillHistograms");


  // Contains the variables to monitor
  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;

  // Retrieve event info from SG
  
  uint32_t bunchCrossing = 0;
  bunchCrossing = ctx.eventID().bunch_crossing_id();
  ATH_MSG_DEBUG("BCID: " << bunchCrossing);
  
  // Retrieve Trigger Towers from SG
  SG::ReadHandle<xAOD::TriggerTowerContainer> triggerTowerTES(m_xAODTriggerTowerContainerName, ctx);
  if(!triggerTowerTES.isValid()){
    ATH_MSG_ERROR("No Trigger Tower container found in TES  "<< m_xAODTriggerTowerContainerName); 
    return StatusCode::FAILURE;
  }

  // Create a vector of trigger towers with quantities to be monitored
  std::vector<MonitorTT> vecMonTT_EM;  // EM towers
  std::vector<MonitorTT> vecMonTT_HAD; // HAD towers
  std::vector<MonitorTT> vecMonTT;     // All towers

  // Loop over trigger tower container
  for (const xAOD::TriggerTower* tt : *triggerTowerTES) {
   
    // Create the trigger tower objects and calculate scaled phi
    // Fill the required number of phi bins based on tower granularity
    ATH_CHECK( fillPPMTowerEtaPhi(tt, vecMonTT_EM, vecMonTT_HAD, vecMonTT) );     
        
  } // End of loop over triggerTowerTES container    


  // Variables to monitor 
 
  // EM towers
  // eta for 2-d eta-phi maps (contains "duplicate" entries to account for granularity of multiple phi bins in forward region)
  auto etaTT_EM = Monitored::Collection("etaTT_EM", vecMonTT_EM, []( const auto &emTower ){return emTower.tower->eta();});
  variables.push_back(etaTT_EM);

  auto phiTT_2d_EM = Monitored::Collection("phiTT_2d_EM", vecMonTT_EM, []( const auto &emTower ){return emTower.phiScaled;});
  variables.push_back(phiTT_2d_EM); 

  auto phiTT_1d_EM = Monitored::Collection("phiTT_1d_EM", vecMonTT_EM, []( const auto &emTower ){return emTower.phi1d;});
  variables.push_back(phiTT_1d_EM);  

  auto cpET_EM = Monitored::Collection("cpET_EM", vecMonTT_EM, []( const auto &emTower ){return emTower.tower->cpET();});
  variables.push_back(cpET_EM);  

  auto jepET_EM = Monitored::Collection("jepET_EM", vecMonTT_EM, []( const auto &emTower ){return emTower.jepET;});
  variables.push_back(jepET_EM);

  auto maxADC_EM = Monitored::Collection("maxADC_EM", vecMonTT_EM, []( const auto &emTower ){return emTower.maxADC;});
  variables.push_back(maxADC_EM);

  auto adcPeak_EM = Monitored::Collection("adcPeak_EM", vecMonTT_EM, []( const auto &emTower ){return emTower.tower->adcPeak();});
  variables.push_back(adcPeak_EM);

  // HAD towers
  auto etaTT_HAD = Monitored::Collection("etaTT_HAD", vecMonTT_HAD, []( const auto &hadTower ){return hadTower.tower->eta();});
  variables.push_back(etaTT_HAD);

  auto phiTT_2d_HAD = Monitored::Collection("phiTT_2d_HAD", vecMonTT_HAD, []( const auto &hadTower ){return hadTower.phiScaled;});
  variables.push_back(phiTT_2d_HAD);

  auto phiTT_1d_HAD = Monitored::Collection("phiTT_1d_HAD", vecMonTT_HAD, []( const auto &hadTower ){return hadTower.phi1d;});
  variables.push_back(phiTT_1d_HAD);

  auto cpET_HAD = Monitored::Collection("cpET_HAD", vecMonTT_HAD, []( const auto &hadTower ){return hadTower.tower->cpET();});
  variables.push_back(cpET_HAD);

  auto jepET_HAD = Monitored::Collection("jepET_HAD", vecMonTT_HAD, []( const auto &hadTower ){return hadTower.jepET;});
  variables.push_back(jepET_HAD);
 
  auto maxADC_HAD = Monitored::Collection("maxADC_HAD", vecMonTT_HAD, []( const auto &hadTower ){return hadTower.maxADC;});
  variables.push_back(maxADC_HAD);

  auto adcPeak_HAD = Monitored::Collection("adcPeak_HAD", vecMonTT_HAD, []( const auto &hadTower ){return hadTower.tower->adcPeak();});
  variables.push_back(adcPeak_HAD);


  // Cutmasks (EM)
  std::vector<int> vec_EM_noDuplicates = {};
  std::vector<int> vec_EM_timeslice = {};
  std::vector<int> vec_EM_maxSlice = {};
  std::vector<int> vec_EM_maxSlice_noDuplicates = {};
  std::vector<int> vec_EM_cpET_0 = {};             // includes "duplicate" towers for phi maps
  std::vector<int> vec_EM_cpET_0_noDuplicates = {}; // no duplicates: for plots not binned in phi
  std::vector<int> vec_EM_cpET_5 = {};
  std::vector<int> vec_EM_cpET_5_noDuplicates = {};
  std::vector<int> vec_EM_jepET_0 = {};
  std::vector<int> vec_EM_jepET_0_noDuplicates = {};
  std::vector<int> vec_EM_jepET_5 = {};
  std::vector<int> vec_EM_jepET_5_noDuplicates = {};
  std::vector<int> vec_cpET_5 = {};
  std::vector<int> vec_jepET_5 = {};
  
  // Weights
  std::vector<int> vec_EM_ADC = {};

  // For average ADC plots
  std::vector<double> vec_EM_maxADCPlus1 = {};

  std::vector<uint32_t> vec_cp_BCID = {};
  std::vector<uint32_t> vec_jep_BCID = {};

  for (auto& emTower : vecMonTT_EM) {
  
    // -------- LUT --------
    int cpET = (emTower.tower)->cpET();
    int jepET = emTower.jepET;
    bool isDuplicate = emTower.isDuplicate;
    vec_cp_BCID.push_back(bunchCrossing);
    vec_jep_BCID.push_back(bunchCrossing);

    ATH_MSG_DEBUG("cpET: " << cpET << " jepET: " << jepET);

    // Fill the cutmasks for EM LUT-CP and LUT-JEP energy distributions 
    vec_EM_noDuplicates.push_back(!isDuplicate);
    vec_EM_cpET_0.push_back(cpET > 0);                                  // For phi distributions / maps
    vec_EM_cpET_0_noDuplicates.push_back((cpET > 0) && !isDuplicate);   // For plots not binned in phi
    vec_EM_cpET_5.push_back(cpET > 5);
    vec_EM_cpET_5_noDuplicates.push_back((cpET > 5) && !isDuplicate);
    vec_cpET_5.push_back((cpET > 5) && !isDuplicate);
    vec_EM_jepET_0.push_back(jepET > 0);
    vec_EM_jepET_0_noDuplicates.push_back((jepET > 0) && !isDuplicate);
    vec_EM_jepET_5.push_back(jepET > 5);
    vec_EM_jepET_5_noDuplicates.push_back((jepET > 5) && !isDuplicate); 
    vec_jepET_5.push_back((jepET > 5) && !isDuplicate);      

    // -------- ADC hitmaps per timeslice --------
    unsigned int tslice = (emTower.tower)->adcPeak();
    unsigned int adcSize = ((emTower.tower)->adc()).size();
    
    ATH_MSG_DEBUG("tslice: " << tslice << " adcSize: " << adcSize);

    if (tslice < adcSize) {
      const int ADC = ((emTower.tower)->adc())[tslice];
      vec_EM_timeslice.push_back(ADC > m_TT_ADC_HitMap_Thresh);
      vec_EM_ADC.push_back(ADC);
      ATH_MSG_DEBUG("ADC: " << ADC);
    }
    else {
      vec_EM_timeslice.push_back(0);
      vec_EM_ADC.push_back(0);
    }
  
    // -------- Timing of FADC signal --------
    
    double max = emTower.maxADC;
    vec_EM_maxSlice.push_back(max >= 0.);
    vec_EM_maxSlice_noDuplicates.push_back((max >= 0.) && !isDuplicate);
    vec_EM_maxADCPlus1.push_back(max + 1.);
 
  } // End loop over vector of EM towers 

  // Cutmasks (HAD)
  std::vector<int> vec_HAD_noDuplicates = {};
  std::vector<int> vec_HAD_timeslice = {};
  std::vector<int> vec_HAD_maxSlice = {};
  std::vector<int> vec_HAD_maxSlice_noDuplicates = {};
  std::vector<int> vec_HAD_cpET_0 = {};             // includes "duplicate" towers for phi maps
  std::vector<int> vec_HAD_cpET_0_noDuplicates = {}; // no duplicates: for plots not binned in phi
  std::vector<int> vec_HAD_cpET_5 = {};
  std::vector<int> vec_HAD_cpET_5_noDuplicates = {};
  std::vector<int> vec_HAD_jepET_0 = {};
  std::vector<int> vec_HAD_jepET_0_noDuplicates = {};
  std::vector<int> vec_HAD_jepET_5 = {};
  std::vector<int> vec_HAD_jepET_5_noDuplicates = {};


  // HAD weights 
  std::vector<int> vec_HAD_ADC = {};

  // For average ADC plots 
  std::vector<double> vec_HAD_maxADCPlus1 = {};

  for (auto& hadTower : vecMonTT_HAD) {

    // -------- LUT --------
    int cpET = (hadTower.tower)->cpET();
    int jepET = hadTower.jepET;
    bool isDuplicate = hadTower.isDuplicate;
    vec_cp_BCID.push_back(bunchCrossing);
    vec_jep_BCID.push_back(bunchCrossing);

    if (cpET > 0) ATH_MSG_DEBUG("HAD tower cpET: " << cpET << " jepET: " << jepET);

    // Fill the cutmasks for HAD LUT-CP and LUT-JEP energy distributions 
    vec_HAD_noDuplicates.push_back(!isDuplicate);
    vec_HAD_cpET_0.push_back(cpET > 0);                                  // For phi distributions / maps
    vec_HAD_cpET_0_noDuplicates.push_back((cpET > 0) && !isDuplicate);   // For plots not binned in phi
    vec_HAD_cpET_5.push_back(cpET > 5);
    vec_HAD_cpET_5_noDuplicates.push_back((cpET > 5) && !isDuplicate);
    vec_cpET_5.push_back((cpET > 5) && !isDuplicate);
    vec_HAD_jepET_0.push_back(jepET > 0);
    vec_HAD_jepET_0_noDuplicates.push_back((jepET > 0) && !isDuplicate);
    vec_HAD_jepET_5.push_back(jepET > 5);
    vec_HAD_jepET_5_noDuplicates.push_back((jepET > 5) && !isDuplicate);
    vec_jepET_5.push_back((jepET > 5) && !isDuplicate);

    // -------- ADC hitmaps per timeslice -------- 
    unsigned int tslice = (hadTower.tower)->adcPeak();
    unsigned int adcSize = ((hadTower.tower)->adc()).size();

    ATH_MSG_DEBUG("tslice: " << tslice << " adcSize: " << adcSize);

    if (tslice < adcSize) {
      const int ADC = ((hadTower.tower)->adc())[tslice];
      vec_HAD_timeslice.push_back(ADC > m_TT_ADC_HitMap_Thresh);
      vec_HAD_ADC.push_back(ADC);
      ATH_MSG_DEBUG("ADC: " << ADC);
    }
    else {
      vec_HAD_timeslice.push_back(0);
      vec_HAD_ADC.push_back(0);
    }
    ATH_MSG_DEBUG(" mask: " << vec_HAD_timeslice.back());

    // -------- Timing of FADC signal --------

    double max = hadTower.maxADC;
    vec_HAD_maxSlice.push_back(max >= 0.);
    vec_HAD_maxSlice_noDuplicates.push_back((max >= 0.) && !isDuplicate);
    vec_HAD_maxADCPlus1.push_back(max + 1.);

  } // End loop over vector of HAD towers 
 

  // Define additional monitored variables (BCID)
  auto cp_BCID = Monitored::Collection("cp_BCID", vec_cp_BCID);
  variables.push_back(cp_BCID);

  auto jep_BCID = Monitored::Collection("jep_BCID", vec_jep_BCID);
  variables.push_back(jep_BCID);
  
  // Define additional monitored variables (EM) 
  auto maxADCPlus1_EM = Monitored::Collection("maxADCPlus1_EM", vec_EM_maxADCPlus1);
  variables.push_back(maxADCPlus1_EM);

  // Define additional monitored variables (HAD)
  auto maxADCPlus1_HAD = Monitored::Collection("maxADCPlus1_HAD", vec_HAD_maxADCPlus1);
  variables.push_back(maxADCPlus1_HAD);

  // Define cutmask
  auto mask_cpET_5 = Monitored::Collection("mask_cpET_5", vec_cpET_5);
  variables.push_back(mask_cpET_5);

  auto mask_jepET_5 = Monitored::Collection("mask_jepET_5", vec_jepET_5);
  variables.push_back(mask_jepET_5);

  // Define the cutmasks (EM)
  auto mask_EM_noDuplicates = Monitored::Collection("mask_EM_noDuplicates", vec_EM_noDuplicates);
  variables.push_back(mask_EM_noDuplicates);  

  auto mask_EM_timeslice = Monitored::Collection("mask_EM_timeslice", vec_EM_timeslice);
  variables.push_back(mask_EM_timeslice);
 
  auto mask_EM_maxSlice = Monitored::Collection("mask_EM_maxSlice", vec_EM_maxSlice);
  variables.push_back(mask_EM_maxSlice);
 
  auto mask_EM_maxSlice_noPhi = Monitored::Collection("mask_EM_maxSlice_noPhi", vec_EM_maxSlice_noDuplicates);
  variables.push_back(mask_EM_maxSlice_noPhi); 

  auto emTT_ADC = Monitored::Collection("emTT_ADC", vec_EM_ADC);
  variables.push_back(emTT_ADC);

  auto mask_EM_cpET_0_phiBins = Monitored::Collection("mask_EM_cpET_0_phiBins", vec_EM_cpET_0);
  variables.push_back(mask_EM_cpET_0_phiBins);

  auto mask_EM_cpET_0_noPhi = Monitored::Collection("mask_EM_cpET_0_noPhi", vec_EM_cpET_0_noDuplicates);
  variables.push_back(mask_EM_cpET_0_noPhi);

  auto mask_EM_cpET_5_phiBins = Monitored::Collection("mask_EM_cpET_5_phiBins", vec_EM_cpET_5);
  variables.push_back(mask_EM_cpET_5_phiBins);

  auto mask_EM_cpET_5_noPhi = Monitored::Collection("mask_EM_cpET_5_noPhi", vec_EM_cpET_5_noDuplicates);
  variables.push_back(mask_EM_cpET_5_noPhi);

  auto mask_EM_jepET_0_phiBins = Monitored::Collection("mask_EM_jepET_0_phiBins", vec_EM_jepET_0);
  variables.push_back(mask_EM_jepET_0_phiBins);

  auto mask_EM_jepET_0_noPhi = Monitored::Collection("mask_EM_jepET_0_noPhi", vec_EM_jepET_0_noDuplicates);
  variables.push_back(mask_EM_jepET_0_noPhi);

  auto mask_EM_jepET_5_phiBins = Monitored::Collection("mask_EM_jepET_5_phiBins", vec_EM_jepET_5);
  variables.push_back(mask_EM_jepET_5_phiBins);
  
  auto mask_EM_jepET_5_noPhi = Monitored::Collection("mask_EM_jepET_5_noPhi", vec_EM_jepET_5_noDuplicates);
  variables.push_back(mask_EM_jepET_5_noPhi);


  // Define the cutmasks (HAD)
  auto mask_HAD_noDuplicates = Monitored::Collection("mask_HAD_noDuplicates", vec_HAD_noDuplicates);
  variables.push_back(mask_HAD_noDuplicates);

  auto mask_HAD_timeslice = Monitored::Collection("mask_HAD_timeslice", vec_HAD_timeslice);
  variables.push_back(mask_HAD_timeslice);

  auto mask_HAD_maxSlice = Monitored::Collection("mask_HAD_maxSlice", vec_HAD_maxSlice);
  variables.push_back(mask_HAD_maxSlice);

  auto mask_HAD_maxSlice_noPhi = Monitored::Collection("mask_HAD_maxSlice_noPhi", vec_HAD_maxSlice_noDuplicates);
  variables.push_back(mask_HAD_maxSlice_noPhi);

  auto hadTT_ADC = Monitored::Collection("hadTT_ADC", vec_HAD_ADC);
  variables.push_back(hadTT_ADC);

  auto mask_HAD_cpET_0_phiBins = Monitored::Collection("mask_HAD_cpET_0_phiBins", vec_HAD_cpET_0);
  variables.push_back(mask_HAD_cpET_0_phiBins);

  auto mask_HAD_cpET_0_noPhi = Monitored::Collection("mask_HAD_cpET_0_noPhi", vec_HAD_cpET_0_noDuplicates);
  variables.push_back(mask_HAD_cpET_0_noPhi);

  auto mask_HAD_cpET_5_phiBins = Monitored::Collection("mask_HAD_cpET_5_phiBins", vec_HAD_cpET_5);
  variables.push_back(mask_HAD_cpET_5_phiBins);

  auto mask_HAD_cpET_5_noPhi = Monitored::Collection("mask_HAD_cpET_5_noPhi", vec_HAD_cpET_5_noDuplicates);
  variables.push_back(mask_HAD_cpET_5_noPhi);

  auto mask_HAD_jepET_0_phiBins = Monitored::Collection("mask_HAD_jepET_0_phiBins", vec_HAD_jepET_0);
  variables.push_back(mask_HAD_jepET_0_phiBins);

  auto mask_HAD_jepET_0_noPhi = Monitored::Collection("mask_HAD_jepET_0_noPhi", vec_HAD_jepET_0_noDuplicates);
  variables.push_back(mask_HAD_jepET_0_noPhi);

  auto mask_HAD_jepET_5_phiBins = Monitored::Collection("mask_HAD_jepET_5_phiBins", vec_HAD_jepET_5);
  variables.push_back(mask_HAD_jepET_5_phiBins);

  auto mask_HAD_jepET_5_noPhi = Monitored::Collection("mask_HAD_jepET_5_noPhi", vec_HAD_jepET_5_noDuplicates);
  variables.push_back(mask_HAD_jepET_5_noPhi);


  fill(m_packageName, variables);
  variables.clear();
  return StatusCode::SUCCESS;
}

StatusCode PprMonitorAlgorithm::fillPPMTowerEtaPhi( const xAOD::TriggerTower_v2* tt, 
                                                    std::vector<MonitorTT> &vecMonTT_EM,  
                                                    std::vector<MonitorTT> &vecMonTT_HAD,  
                                                    std::vector<MonitorTT> &vecMonTT) const
{
  // Geometry
  const int layer = tt->layer(); // 0 = EM, 1 = HAD
  const double eta = tt->eta();
  const double absEta = std::fabs(eta);
  const double phi = tt->phi();
  double phiMod = phi * m_phiScaleTT;
  
  // LUT JEP
  int jepET = 0;
  const std::vector<uint_least8_t> jepETvec = tt->lut_jep();
  if (jepETvec.size() > 0) jepET = tt->jepET();
  
  // To remove duplicates when filling multiple bins in phi
  bool isDuplicate = false; 
 
  // ADC timeslice
  const std::vector<short unsigned int> &ADC( tt->adc() );
  double max = recTime(ADC, m_EMFADCCut); 
  
  // Offsets for filling multiple phi bins depending on TT granularity in eta
  std::vector<double> offset32 = {1.5, 0.5, -0.5, -1.5};
  std::vector<double> offset25 = {0.5, -0.5};  

  // Offsets for filling 1D phi plots
  double phi1d = phi;

  if (absEta > 3.2) {
   
    phiMod = std::floor(phiMod/4)*4. + 2.;
    // Fill four bins in phi  
    for (int i = 0; i < 4; i++) {
      MonitorTT monTT;
      monTT.tower = tt;
      monTT.phiScaled = phiMod + offset32[i];
      monTT.phi1d = phi + offset32[i]/m_phiScaleTT;
      monTT.jepET = jepET;
      if(i != 0) isDuplicate = true;
      monTT.isDuplicate = isDuplicate;   
      monTT.maxADC = max;     
 
      vecMonTT.push_back(monTT);
      if (layer == 0) vecMonTT_EM.push_back(monTT);
      if (layer == 1) vecMonTT_HAD.push_back(monTT);
      
      ATH_MSG_DEBUG("layer: " << layer << " eta: " << eta << " phi: " << phi << " scaled phi: " << monTT.phiScaled << " 1d phi: " << phi1d << " max: " << max <<  " duplicate: " << monTT.isDuplicate);
    }
  }
  else if (absEta > 2.5) {
    phiMod = std::floor(phiMod/2)*2. + 1.;
    // Fill two bins in phi
    for (int i = 0; i < 2; i++) {
      MonitorTT monTT;
      monTT.tower = tt;
      monTT.phiScaled = phiMod + offset25[i];
      monTT.phi1d = phi + offset25[i] / m_phiScaleTT;
      monTT.jepET = jepET;
      if(i != 0) isDuplicate = true;
      monTT.isDuplicate = isDuplicate;
      monTT.maxADC = max;

      vecMonTT.push_back(monTT);
      if (layer == 0) vecMonTT_EM.push_back(monTT);
      if (layer == 1) vecMonTT_HAD.push_back(monTT);
      ATH_MSG_DEBUG("layer: " << layer << " eta: " << eta << " phi: " << phi << " scaled phi: " << monTT.phiScaled << " 1d phi: " << phi1d << " max: " << max << " duplicate: " << monTT.isDuplicate);
    }
  }
  else {
    // Fill one phi bin
    MonitorTT monTT;
    monTT.tower = tt;
    monTT.phiScaled = phiMod;
    monTT.phi1d = phi;
    monTT.jepET = jepET;
    monTT.isDuplicate = false;
    monTT.maxADC = max;

    vecMonTT.push_back(monTT);
    if (layer == 0) vecMonTT_EM.push_back(monTT);
    if (layer == 1) vecMonTT_HAD.push_back(monTT);
    ATH_MSG_DEBUG("layer: " << layer << " eta: " << eta << " phi: " << phi << " scaled phi: " << monTT.phiScaled << " 1d phi: " << phi1d << " max: " << max << " duplicate: " << monTT.isDuplicate);
  }   
    
  return StatusCode::SUCCESS; 
}

double PprMonitorAlgorithm::recTime(const std::vector<short unsigned int> &vFADC, int cut) const {

  int max = -1;
  const int slices = vFADC.size();
  if (slices > 0) {
    max = 0.;
    int maxAdc = vFADC[0];
    for (int sl = 1; sl < slices; ++sl) {
      if (vFADC[sl] > maxAdc) {
        maxAdc = vFADC[sl];
        max = sl;
      } else if (vFADC[sl] == maxAdc)
        max = -1;
    }
    if (maxAdc == 0)
      max = -1;
  }
  if (max >= 0) {
    int slbeg = max - 2;
    if (slbeg < 0)
      slbeg = 0;
    int slend = max + 3;
    if (slend > slices)
      slend = slices;
    int sum = 0;
    int min = 999999;
    for (int sl = slbeg; sl < slend; ++sl) {
      int val = vFADC[sl];
      if (val < m_TT_ADC_Pedestal)
        val = m_TT_ADC_Pedestal;
      sum += val;
      if (val < min)
        min = val;
    }
    sum -= (slend - slbeg) * min;
    if (sum <= cut)
      max = -1;
  }

  return double(max);
}














