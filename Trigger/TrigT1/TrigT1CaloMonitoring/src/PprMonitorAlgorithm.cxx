/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

  ATH_MSG_DEBUG("PprMonitorAlgorithm::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);
  ATH_MSG_DEBUG("m_xAODTriggerTowerContainerName "<< m_xAODTriggerTowerContainerName); 
  ATH_MSG_DEBUG("m_TT_ADC_HitMap_Thresh " << m_TT_ADC_HitMap_Thresh);

  // we initialise all the containers that we need
  ATH_CHECK( AthMonitorAlgorithm::initialize() );
  ATH_CHECK( m_xAODTriggerTowerContainerName.initialize() );
 
  // retrieve any tools if needed
  //ATH_CHECK(myTool.retrieve());
 
  // Initialize the groups for GenericMonitoringArrays
  std::vector<std::string> partitionsEM = {"LArFCAL1C", "LArEMECC", "LArOverlapC", "LArEMBC", "LArEMBA", "LArOverlapA", "LArEMECA", "LArFCAL1A"};
  m_groupTimeslice_EM = Monitored::buildToolMap<int>(m_tools, "groupTimeslice_EM", partitionsEM);

  std::vector<std::string> partitionsHAD = {"LArFCAL23C", "LArHECC", "TileEBC", "TileLBC", "TileLBA", "TileEBA", "LArHECA", "LArFCAL23A"}; 
  m_groupTimeslice_HAD = Monitored::buildToolMap<int>(m_tools, "groupTimeslice_HAD", partitionsHAD);

  return StatusCode::SUCCESS;
}

StatusCode PprMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG("PprMonitorAlgorithm::fillHistograms");

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
  // Create the trigger tower objects and calculate scaled phi
  for (const xAOD::TriggerTower* tt : *triggerTowerTES) {
    ATH_CHECK( fillPPMTowerEtaPhi(tt, vecMonTT_EM, vecMonTT_HAD, vecMonTT) );     
  }

  
  // Loop over the trigger tower objects and fill the histograms 
 
  for (auto& myTower : vecMonTT) {
    ATH_MSG_DEBUG("PprMonitorAlgorithm looping over TTs"); 
    // -------- LUT --------
    int cpET = (myTower.tower)->cpET();
    int jepET = myTower.jepET;
    int layer = (myTower.tower)->layer();
    bool isEM = (layer == 0);
    bool isHAD = (layer == 1);     
    std::string layerName = (layer == 0) ? "EM" : "HAD";
    
    ATH_MSG_DEBUG("isEM " << isEM << " isHAD " << isHAD << " layerName " << layerName);
    ATH_MSG_DEBUG("cpET: " << cpET << " jepET: " << jepET);

    // The variables to plot
    auto eta_TT = Monitored::Scalar<double>("eta_TT", myTower.tower->eta());
    auto cpET_TT  = Monitored::Scalar<int>("cpET_TT", cpET);
    auto jepET_TT = Monitored::Scalar<int>("jepET_TT", jepET);
    auto BCID  = Monitored::Scalar<uint32_t>("BCID", bunchCrossing);

    // The cutmasks
    auto mask_EM         = Monitored::Scalar<bool>("mask_EM", isEM);
    auto mask_HAD        = Monitored::Scalar<bool>("mask_HAD", isHAD);
    auto mask_cpET_0     = Monitored::Scalar<bool>("mask_cpET_0", cpET > 0 );
    auto mask_jepET_0    = Monitored::Scalar<bool>("mask_jepET_0", jepET > 0 );
    auto mask_cpET_5     = Monitored::Scalar<bool>("mask_cpET_5", cpET > 5);
    auto mask_jepET_5    = Monitored::Scalar<bool>("mask_jepET_5", jepET > 5);

    // Fill LUT histograms (layer-independent)
    std::string groupName = "groupLUTCP";
    fill(groupName, BCID, mask_cpET_5); // ppm_1d_tt_lutcp_LutPerBCN

    groupName = "groupLUTJEP";
    fill(groupName, BCID, mask_jepET_5); // ppm_1d_tt_lutjep_LutPerBCN

    // Layer-dependent LUT-CP plots (EM or HAD)
    groupName = "groupLUTCP_";
    groupName.append(layerName);
    ATH_MSG_DEBUG("Filling group: " << groupName);   
 
    // Fill LUT-CP eta and ET distributions
    // ppm_em_1d_tt_lutcp_Eta, ppm_had_1d_tt_lutcp_Eta
    // ppm_em_1d_tt_lutcp_Et, ppm_had_1d_tt_lutcp_Et 
    fill(groupName, eta_TT, cpET_TT, mask_cpET_0);      
    
    // Fill LUT-CP phi distributions
    // ppm_em_1d_tt_lutcp_Phi, ppm_had_1d_tt_lutcp_Phi
    if (cpET > 0) {
      ATH_CHECK( fillPPMPhi(myTower, groupName) );  
    
      // Fill LUT-CP eta-phi maps 
      // ppm_em_2d_etaPhi_tt_lutcp_AverageEt, ppm_had_2d_etaPhi_tt_lutcp_AverageEt 
      if (cpET > 5) {
        ATH_CHECK( fillPPMEtaVsPhi(myTower, groupName, "cpET_TT_2D", cpET) ); 
      }
    }

    // LUT-JEP  
    groupName = "groupLUTJEP_";
    groupName.append(layerName);
    ATH_MSG_DEBUG("Filling group: " << groupName); 
   
    // Fill LUT-JEP eta and ET distributions
    // ppm_em_1d_tt_lutjep_Eta, ppm_had_1d_tt_lutjep_Eta
    // ppm_em_1d_tt_lutjep_Et, ppm_had_1d_tt_lutjep_Et 
    fill(groupName, eta_TT, jepET_TT, mask_jepET_0); // LUT-JEP eta and ET distributions   

    // Fill LUT-JEP phi distributions
    // ppm_em_1d_tt_lutjep_Phi, ppm_had_1d_tt_lutjep_Phi
    if (jepET > 0) {
      ATH_CHECK( fillPPMPhi(myTower, groupName) );  
    
      // Fill LUT-JEP eta-phi maps 
      // ppm_em_2d_etaPhi_tt_lutjep_AverageEt, ppm_had_2d_etaPhi_tt_lutjep_AverageEt 
      if (jepET > 5) {
        ATH_CHECK( fillPPMEtaVsPhi(myTower, groupName, "jepET_TT_2D", jepET) ); 
      }
    }


    // -------- ADC hitmaps per timeslice --------
    unsigned int tslice = (myTower.tower)->adcPeak();
    unsigned int adcSize = ((myTower.tower)->adc()).size();
    
    ATH_MSG_DEBUG("tslice: " << tslice << " adcSize: " << adcSize);

    // Number of triggered timeslice
    groupName = "groupTimeslice_";
    groupName.append(layerName);    

    auto adcPeak = Monitored::Scalar<unsigned int>("adcPeak", tslice);  
    fill(groupName, adcPeak);

    if (tslice < adcSize) {
      groupName = "groupADC_";
      groupName.append(layerName);
      const int ADC = ((myTower.tower)->adc())[tslice];
      if (ADC > m_TT_ADC_HitMap_Thresh) {
        // Fills both ppm_em_2d_etaPhi_tt_adc_HitMap (unweighted) and ppm_em_2d_etaPhi_tt_adc_ProfileHitMap (weighted) at the same time
        ATH_CHECK(fillPPMEtaVsPhi(myTower, groupName, "adcTT", ADC));
        ATH_MSG_DEBUG("ADC: " << ADC);
      }
    }
  
    // -------- Timing of FADC signal --------
    
    int max = myTower.maxADC;
    int maxADCPlus1 = max + 1;
    auto maxADC = Monitored::Scalar<unsigned int>("maxADC", max);
   
    groupName = "groupTimeslice_";
    groupName.append(layerName);
 
    if (max >= 0.) {
      fill(groupName, maxADC);
      ATH_CHECK(fillPPMEtaVsPhi(myTower, groupName, "maxADCPlus1", maxADCPlus1));
    }
   
    // -------- Bits of BCID logic word --------

    // ppm_2d_tt_adc_BcidBits
    groupName = "groupTimeslice";
    
    auto bcidBits = Monitored::Scalar<int>("bcidBits", 0);
    auto adcBCID  = Monitored::Scalar<int>("adcBCID", 0);
 
    short unsigned int peak = (myTower.tower)->peak();

    if (cpET > 0 && tslice < adcSize) { // Defined above in ADC hitmaps per timeslice block
     
      adcBCID = ((myTower.tower)->adc())[tslice]; 
      uint8_t bcidWord = (myTower.tower)->bcidVec()[peak];

      if (bcidWord == char(0) || bcidWord == char(1)) { // None (40 MHz)
        bcidBits = 0;
        fill(groupName, bcidBits, adcBCID);
      }
      else if (bcidWord == char(2) || bcidWord == char(3)) { // satBC only
        bcidBits = 1;
        fill(groupName, bcidBits, adcBCID);
      }
      else if (bcidWord == char(4) || bcidWord == char(5)) { // PF only
        bcidBits = 2;
        fill(groupName, bcidBits, adcBCID);
      }
      else if (bcidWord == char(6) || bcidWord == char(7)) { // satBC & PF
        bcidBits = 3;
        fill(groupName, bcidBits, adcBCID);
      }
      if (bcidWord == char(5) || bcidWord == char(7)) { // sat80BC & PF
        bcidBits = 4;
        fill(groupName, bcidBits, adcBCID);
      }
      if (bcidWord == char(3) || bcidWord == char(7)) { // sat80BC & sat40BC 
        bcidBits = 5;
        fill(groupName, bcidBits, adcBCID);
      }
      if (bcidWord == char(1)) { // sat80BC only
        bcidBits = 6;
        fill(groupName, bcidBits, adcBCID);
      }
    }    
   

    // --------  High/low threshold pass cases (Sat80) --------
    // ppm_1d_tt_adc_HLCase

    auto sat80Word = Monitored::Scalar<int>("sat80Word", (myTower.tower)->sat80Vec()[peak]);

    if (cpET > 0 && tslice < adcSize && peak < (myTower.tower)->sat80Vec().size()) {
      for(unsigned int i = 0; i < 8; i++) {
        if (sat80Word == char(i)) fill(groupName, sat80Word);
      }
    } 
      
   
    // -------- Signal shape profile --------
    // ppm_1d_tt_adc_SignalProfile

    const std::vector<short unsigned int> &vADC((myTower.tower)->adc());
    
    if (cpET > 0) {

      const std::string part = getPartition(layer, myTower.tower->eta());
      std::vector<short unsigned int>::const_iterator it = vADC.begin();
      std::vector<short unsigned int>::const_iterator itE = vADC.end();
     
      for (int s = 0; it!= itE && s < m_SliceNo; ++it, ++s) {
        auto slice = Monitored::Scalar<int>("slice", s);
        auto wADC  = Monitored::Scalar<int>("wADC", *it); 
        if (isEM) fill(m_tools[m_groupTimeslice_EM.at(part)], slice, wADC);
        else fill(m_tools[m_groupTimeslice_HAD.at(part)], slice, wADC);  
      }
    } // End if cpeT > 0 (signal shape block) 
 

   
    // -------- Pedestal correction over-/underflow --------

    // ppm_em_1d_pedOverflow_Eta, ppm_had_1d_pedOverflow_Eta
    // ppm_em_1d_pedUnderflow_Eta, ppm_had_1d_pedUnderflow_Eta

    groupName = "groupErrors_";
    groupName.append(layerName);

    bool isPedCorrOverflow = false; 
    bool isPedCorrUnderflow = false;

    for (auto pedCorr : (myTower.tower)->correction()) {
      if ( pedCorr >= 511 ) isPedCorrOverflow = true;
      else if ( pedCorr <= -512 ) isPedCorrUnderflow = true;
    }
    
    auto mask_PedCorrOverflow = Monitored::Scalar<bool>("mask_PedCorrOverflow", isPedCorrOverflow);
    auto mask_PedCorrUnderflow = Monitored::Scalar<bool>("mask_PedCorrUnderflow", isPedCorrUnderflow); 
    
    fill(groupName, eta_TT, mask_PedCorrOverflow, mask_PedCorrUnderflow);

  } // End loop over tower objects 

  
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
  const double phi = tt->phi();
  double phiMod = phi * m_phiScaleTT;
  
  // LUT JEP
  int jepET = 0;
  const std::vector<uint_least8_t> jepETvec = tt->lut_jep();
  if (jepETvec.size() > 0) jepET = tt->jepET();
  
  // ADC timeslice
  const std::vector<short unsigned int> &ADC( tt->adc() );
  double max = recTime(ADC, m_EMFADCCut); 
 
  // Fill TT quantities
  MonitorTT monTT;
  monTT.tower = tt;
  monTT.phiScaled = phiMod;
  monTT.jepET = jepET;
  monTT.maxADC = max;
  vecMonTT.push_back(monTT);
  if (layer == 0) vecMonTT_EM.push_back(monTT);
  if (layer == 1) vecMonTT_HAD.push_back(monTT);
  ATH_MSG_DEBUG("layer: " << layer << " eta: " << eta << " phi: " << phi << " scaled phi: " << monTT.phiScaled << " max: " << max);
   
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


std::string PprMonitorAlgorithm::getPartition(int layer, double eta) const {

  std::string part = "";
    if (layer == 0) {       // EM layer
      if (eta < -3.2)
        part = "LArFCAL1C";
      else if (eta < -1.5)
        part = "LArEMECC";
      else if (eta < -1.4)
        part = "LArOverlapC";
      else if (eta < 0.0)
        part = "LArEMBC";
      else if (eta < 1.4)
        part = "LArEMBA";
      else if (eta < 1.5)
        part = "LArOverlapA";
      else if (eta < 3.2)
        part = "LArEMECA";
      else
        part = "LArFCAL1A";
    } else {                // HAD layer 
      if (eta < -3.2)
        part = "LArFCAL23C";
      else if (eta < -1.5)
        part = "LArHECC";
      else if (eta < -0.9)
        part = "TileEBC";
      else if (eta < 0.0)
        part = "TileLBC";
      else if (eta < 0.9)
        part = "TileLBA";
      else if (eta < 1.5)
        part = "TileEBA";
      else if (eta < 3.2)
        part = "LArHECA";
      else
        part = "LArFCAL23A";
    }
  return part;
}


StatusCode PprMonitorAlgorithm::fillPPMEtaVsPhi( MonitorTT &monTT, 
                                           const std::string& groupName, 
                                           const std::string& weightName,
                                           double weight) const {
  
  // Number of bins filled in phi depends on eta due to electronics coverage
 
  // KW to do: fill in shrinkEtaBins part
  double phiMod = monTT.phiScaled;  // Integer binning for 2D plots 
  double etaMod = monTT.tower->eta();
  const double absEta = std::abs(etaMod);
  
  const std::vector<double> offset32 = {1.5, 0.5, -0.5, -1.5};                                                                             
  const std::vector<double> offset25 = {0.5, -0.5}; 
  std::vector<double> offset = {}; 
    
  if (absEta > 3.2) {
    // Fill four bins in phi
    phiMod = std::floor(phiMod/4)*4. + 2.;
    offset = offset32;
  } 
  else if (absEta > 2.5) {
    // Fill two bins in phi
    phiMod = std::floor(phiMod/2)*2. + 1.;
    offset = offset25;
  }     
  else {
    offset = {0.};
  }

  ATH_MSG_DEBUG("absEta: " << absEta << "offset.size(): " << offset.size());
 
  // Fill the histograms 
  for (auto phiOffset : offset)  {

    auto etaTT_2D = Monitored::Scalar<double>("etaTT_2D", etaMod);
    auto phiTT_2D = Monitored::Scalar<double>("phiTT_2D", phiMod + phiOffset);
    auto weight_2D = Monitored::Scalar<double>(weightName, weight); // Weight for filling 2D profile histograms; name must be included in python histogram definition
   
    ATH_MSG_DEBUG("etaTT_2D: " << etaTT_2D << " phiTT_2D: " << phiTT_2D << " weight_2D: " << weight_2D);    

    fill(groupName, etaTT_2D, phiTT_2D, weight_2D);
    
  }      

  return StatusCode::SUCCESS;
} 

StatusCode PprMonitorAlgorithm::fillPPMPhi( MonitorTT &monTT,
                                            const std::string& groupName) const {

  // Number of bins filled in phi depends on eta due to electronics coverage 

  const double phi = monTT.tower->phi();  // Using the actual phi value for 1D plots 
  const double absEta = std::abs(monTT.tower->eta());

  const std::vector<double> offset32 = {1.5, 0.5, -0.5, -1.5};
  const std::vector<double> offset25 = {0.5, -0.5};
  std::vector<double> offset = {};

  double phi1d = phi;

  if (absEta > 3.2) {
    // Fill four bins in phi
    offset = offset32;
  }
  else if (absEta > 2.5) {
    // Fill two bins in phi 
    offset = offset25;
  }
  else {
    // Fill one phi bin 
    offset = {0.};
  }

  // Fill the histogram
  for (auto phiOffset : offset) { 
    phi1d = phi + phiOffset/m_phiScaleTT;
    auto phiTT_1D = Monitored::Scalar<double>("phiTT_1D", phi1d);
    fill(groupName, phiTT_1D);
  }

  return StatusCode::SUCCESS;

}


