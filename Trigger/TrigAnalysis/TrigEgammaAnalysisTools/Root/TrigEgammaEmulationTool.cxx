/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************
 * AsgTool: TrigEgammaEmulationTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Date: Feb 2015
 * Description:
 *      Derived class for studying performance triggers (e.g e0_perf)
 *      Can be used to emulate trigger selection with triggered objects.
 *      Or to study performance of new calibrations, selections, etc.
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaEmulationTool.h"
#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
//**********************************************************************
// ******* For track isolation *****************************************
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "xAODPrimitives/IsolationCorrection.h"
#include "xAODPrimitives/tools/getIsolationAccessor.h"
#include "xAODPrimitives/IsolationType.h"
#include "RecoToolInterfaces/IsolationCommon.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
//**********************************************************************

#ifndef XAOD_ANALYSIS
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#endif


TrigEgammaEmulationTool::
TrigEgammaEmulationTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname),
  m_inDetTrackSelectionTool("InDet::InDetTrackSelectionTool/TrackSelectionTool", this),
  m_trackIsolationTool("xAOD::TrackIsolationTool/TrackIsolationTool", this)
{
  declareProperty("DirectoryPath",m_dir="EmulationTool");
  declareProperty("ElectronOnlPPSelector", m_electronOnlIsEMTool);
  declareProperty("PhotonOnlPPSelector", m_photonOnlIsEMTool);
  declareProperty("ElectronOnlLHSelector", m_electronOnlLHTool);
  declareProperty("ElectronCaloPPSelector", m_electronCaloIsEMTool);
  declareProperty("PhotonCaloPPSelector", m_photonCaloIsEMTool);
  declareProperty("ElectronCaloLHSelector", m_electronCaloLHTool);
  declareProperty("TriggerList",m_trigInputList);
  m_offElectrons=nullptr;
  m_onlElectrons=nullptr;
  m_onlPhotons=nullptr;
  m_trigElectrons=nullptr;
  m_caloClusters=nullptr;
  m_trigEMClusters=nullptr;
  m_emTauRoI=nullptr;
  m_eventCounter=0;
  // ******* For track isolation ********************************************
  declareProperty("TrackSelectionTool", m_inDetTrackSelectionTool);
  declareProperty("TrackIsolationTool", m_trackIsolationTool);
  // ************************************************************************
}

//**********************************************************************
StatusCode TrigEgammaEmulationTool::childInitialize() {
  m_eventCounter=0;
  std::cout << "child Initialize" << std::endl;
  if (m_electronOnlIsEMTool.retrieve().isFailure() ) {
    std::cout << "retrieve failed for tools" << std::endl;
  }
  if (m_photonOnlIsEMTool.retrieve().isFailure() ) {
    std::cout << "retrieve failed for tools" << std::endl;
  }
  if (m_electronOnlLHTool.retrieve().isFailure() ) {
    std::cout << "retrieve failed for tools" << std::endl;
  }
  if (m_electronCaloIsEMTool.retrieve().isFailure() ) {
    std::cout << "retrieve failed for tools" << std::endl;
  }
  if (m_photonCaloIsEMTool.retrieve().isFailure() ) {
    std::cout << "retrieve failed for tools" << std::endl;
  }
  if (m_electronCaloLHTool.retrieve().isFailure() ) {
    std::cout << "retrieve failed for tools" << std::endl;
  }
  if(m_trackIsolationTool.retrieve().isFailure()){
    ATH_MSG_ERROR("Unable to retrieve " << m_trackIsolationTool);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaEmulationTool::childBook() {
  InitializeTriggerList(0);
  InitializeMaps();
  // Book histograms
  const int nTriggerL1 = m_trigL1.size();
  const int nTriggerHLTe = m_trigListElectrons.size();
  const int nTriggerHLTg = m_trigListPhotons.size();
  // m_dir="Emulation";
  m_dir_tdcounts = m_dir+"/TDCounts";
  addDirectory(m_dir_tdcounts);
  addHistogram(new TH1F("TDCountsBitsL1","Trigger Counts;Trigger;Counts",nTriggerL1,1,nTriggerL1));
  addHistogram(new TH1F("TDCountsEmulationL1","Trigger Counts;Trigger;Counts",nTriggerL1,1,nTriggerL1));
  addHistogram(new TH1F("TDCountsBitsElectronChains","Trigger Counts;Trigger;Counts",nTriggerHLTe,1,nTriggerHLTe));
  addHistogram(new TH1F("TDCountsEmulationElectronChains","Trigger Counts;Trigger;Counts",nTriggerHLTe,1,nTriggerHLTe));
  addHistogram(new TH1F("TDCountsEmulationElectronChains_Perf","Trigger Counts;Trigger;Counts",nTriggerHLTe,1,nTriggerHLTe));
  addHistogram(new TH1F("TDCountsBitsPhotonChains","Trigger Counts;Trigger;Counts",nTriggerHLTg,1,nTriggerHLTg));
  addHistogram(new TH1F("TDCountsEmulationPhotonChains","Trigger Counts;Trigger;Counts",nTriggerHLTg,1,nTriggerHLTg));
  //
  addHistogram(new TH1F("TDCountsEmulationL1Errors","Trigger Counts;Trigger;Counts",nTriggerL1,1,nTriggerL1));
  addHistogram(new TH1F("TDCountsEmulationL1ErrorsTD","Trigger Counts;Trigger;Counts",nTriggerL1,1,nTriggerL1));
  addHistogram(new TH1F("TDCountsEmulationL1ErrorsEL1","Trigger Counts;Trigger;Counts",nTriggerL1,1,nTriggerL1));
  //
  addHistogram(new TH1F("TDCountsEmulationElectronChainsErrors","TriggerCounts;Trigger;Counts",nTriggerHLTe,1,nTriggerHLTe));
  addHistogram(new TH1F("TDCountsEmulationElectronChainsErrorsTD","TriggerCounts;Trigger;Counts",nTriggerHLTe,1,nTriggerHLTe));
  addHistogram(new TH1F("TDCountsEmulationElectronChainsErrorsEEF","TriggerCounts;Trigger;Counts",nTriggerHLTe,1,nTriggerHLTe));
  //
  addHistogram(new TH1F("TDCountsEmulationPhotonChainsErrors","TriggerCounts;Trigger;Counts",nTriggerHLTg,1,nTriggerHLTg));
  addHistogram(new TH1F("TDCountsEmulationPhotonChainsErrorsTD","TriggerCounts;Trigger;Counts",nTriggerHLTg,1,nTriggerHLTg));
  addHistogram(new TH1F("TDCountsEmulationPhotonChainsErrorsEEF","TriggerCounts;Trigger;Counts",nTriggerHLTg,1,nTriggerHLTg));
  // Histogram labels
  for (int i=0; i<nTriggerL1; ++i) {
    hist1("TDCountsBitsL1")->GetXaxis()->SetBinLabel(i+1,m_trigL1.at(i).c_str());
    hist1("TDCountsEmulationL1")->GetXaxis()->SetBinLabel(i+1,m_trigL1.at(i).c_str());
    hist1("TDCountsEmulationL1Errors")->GetXaxis()->SetBinLabel(i+1,m_trigL1.at(i).c_str());
    hist1("TDCountsEmulationL1ErrorsTD")->GetXaxis()->SetBinLabel(i+1,m_trigL1.at(i).c_str());
    hist1("TDCountsEmulationL1ErrorsEL1")->GetXaxis()->SetBinLabel(i+1,m_trigL1.at(i).c_str());
  }
  for (int i=0; i<nTriggerHLTe; ++i) {
    hist1("TDCountsBitsElectronChains")->GetXaxis()->SetBinLabel(i+1,m_trigListElectrons.at(i).c_str());
    hist1("TDCountsEmulationElectronChains")->GetXaxis()->SetBinLabel(i+1,m_trigListElectrons.at(i).c_str());
    hist1("TDCountsEmulationElectronChains_Perf")->GetXaxis()->SetBinLabel(i+1,m_trigListElectrons.at(i).c_str());
    hist1("TDCountsEmulationElectronChainsErrors")->GetXaxis()->SetBinLabel(i+1,m_trigListElectrons.at(i).c_str());
    hist1("TDCountsEmulationElectronChainsErrorsTD")->GetXaxis()->SetBinLabel(i+1,m_trigListElectrons.at(i).c_str());
    hist1("TDCountsEmulationElectronChainsErrorsEEF")->GetXaxis()->SetBinLabel(i+1,m_trigListElectrons.at(i).c_str());
  }
  for (int i=0; i<nTriggerHLTg; ++i) {
    hist1("TDCountsBitsPhotonChains")->GetXaxis()->SetBinLabel(i+1,m_trigListPhotons.at(i).c_str());
    hist1("TDCountsEmulationPhotonChains")->GetXaxis()->SetBinLabel(i+1,m_trigListPhotons.at(i).c_str());
    hist1("TDCountsEmulationPhotonChainsErrors")->GetXaxis()->SetBinLabel(i+1,m_trigListPhotons.at(i).c_str());
    hist1("TDCountsEmulationPhotonChainsErrorsTD")->GetXaxis()->SetBinLabel(i+1,m_trigListPhotons.at(i).c_str());
    hist1("TDCountsEmulationPhotonChainsErrorsEEF")->GetXaxis()->SetBinLabel(i+1,m_trigListPhotons.at(i).c_str());
  }
  // Create maps with the correct route to each trigger plot
  for (const auto &trigger : m_trigL1) {
    mapL1Directory[trigger] = m_dir+"/L1/"+trigger;
  }
  for (const auto &trigger : m_trigListElectrons) {
    mapHLTDirectory[trigger] = m_dir+"/HLT/"+trigger;
    setTrigInfo(trigger);
  }
  for (const auto &trigger : m_trigListPhotons) {
    mapHLTDirectory[trigger] = m_dir+"/HLT/"+trigger;
    setTrigInfo(trigger);
  }
  // Binning as defined in TP framework
  // double coarse_eta_bins[9] ={-2.47,-1.52,-1.37,-0.60,0.00,0.60,1.37,1.52,2.47};
  // double coarse_et_bins[7] = {4.,20.,30.,40.,50.,60.,150.};
  // double default_eta_bins[21] = {-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.80,-0.60,-0.10,0.00,
  //                               0.10, 0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47};
  // double default_et_bins[14] = {4.,7.,10.,15.,20.,25.,30.,35.,40.,45.,50.,60.,80.,150.};  
  // L1
  for (const auto &trigger : m_trigL1) {
    addDirectory(mapL1Directory[trigger]);
    // Only passed TDT l1 is accesible
    addHistogram(new TH2D("tdt_1_emE_eta",    "TDT passed;#eta;E_{T} GeV;Count",
                              51, -2.55, 2.55,
                              100, 0, 100));
    addHistogram(new TH2D("tdt_1_emE_emIso",  "TDT passed;emIso GeV;E_{T} GeV;Count",
                              20, -0.1, 9.9,
                              100, 0, 100));
    addHistogram(new TH2D("tdt_1_emE_hadCore","TDT passed;hadCore GeV;E_{T} GeV;Count",
                              10, -0.1, 4.9,
                              100, 0, 100));
    // For emulation we can have full level one
    addHistogram(new TH2D("emu_0_emE_eta",    "Emulation not passed;#eta;E_{T} GeV;Count",
                              51, -2.55, 2.55,
                              100, 0, 100));
    addHistogram(new TH2D("emu_0_emE_emIso",  "Emulation not passed;emIso GeV;E_{T} GeV;Count",
                              20, -0.1, 9.9,
                              100, 0, 100));
    addHistogram(new TH2D("emu_0_emE_hadCore","Emulation not passed;hadCore GeV;E_{T} GeV;Count",
                              10, -0.1, 4.9,
                              100, 0, 100));
    addHistogram(new TH2D("emu_1_emE_eta",    "Emulation passed;#eta;E_{T} GeV;Count",
                              51, -2.55, 2.55,
                              100, 0, 100));
    addHistogram(new TH2D("emu_1_emE_emIso",  "Emulation passed;emIso GeV;E_{T} GeV;Count",
                              20, -0.1, 9.9,
                              100, 0, 100));
    addHistogram(new TH2D("emu_1_emE_hadCore","Emulation passed;hadCore GeV;E_{T} GeV;Count",
                              10, -0.1, 4.9,
                              100, 0, 100));
  }
  for (const auto &trigger : m_trigListElectrons) {
    addDirectory(mapHLTDirectory[trigger]);
    addHistogram(new TH2D("pt20","correlation;ptcone20_new/pt;ptcone20_old/pt;count",50, 0.0, 0.2, 50, 0.0, 0.2));
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaEmulationTool::childExecute() {
    ATH_MSG_DEBUG("Executing NavZeePerf");
    m_offElectrons=0;
    m_onlElectrons=0;
    m_trigElectrons=0;
    m_caloClusters=0;
    if ((m_storeGate->retrieve(m_offElectrons,m_offElContKey)).isFailure() ) {
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Offline Electron container size " << m_offElectrons->size());
    if ((m_storeGate->retrieve(m_onlElectrons,"HLT_xAOD__ElectronContainer_egamma_Electrons")).isFailure() ) {
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Online Electron container size " << m_onlElectrons->size());
    if ((m_storeGate->retrieve(m_onlPhotons,"HLT_xAOD__PhotonContainer_egamma_Photons")).isFailure() ) {
        ATH_MSG_ERROR("Failed to retrieve offline Photons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Online Photon container size " << m_onlPhotons->size());
    if ((m_storeGate->retrieve(m_trigElectrons,"HLT_xAOD__TrigElectronContainer_L2ElectronFex")).isFailure() ) {
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Trig Electron container size " << m_trigElectrons->size());
    if ((m_storeGate->retrieve(m_emTauRoI,"LVL1EmTauRoIs")).isFailure() ) {
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("L1 EM container size " << m_emTauRoI->size());
    if ((m_storeGate->retrieve(m_caloClusters,"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex")).isFailure() ) {
        ATH_MSG_ERROR("Failed to retrieve CaloClusters");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("CaloCluster size " << m_caloClusters->size());
    if ((m_storeGate->retrieve(m_trigEMClusters,"HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma")).isFailure() ) {
        ATH_MSG_DEBUG("Failed to retrieve TrigEMClusters");
    }
    else ATH_MSG_DEBUG("TrigEMCluser size " << m_trigEMClusters->size());

    StatusCode sc = TDCounts();
    if (sc != StatusCode::SUCCESS) ATH_MSG_ERROR("TDCounts Fails");

    // StatusCode ti = TrackIsolation();
    // if (sc != StatusCode::SUCCESS) ATH_MSG_ERROR("TrackIsolation Fails");

    m_eventCounter++;

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaEmulationTool::childFinalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "..." << "Total counts " << m_eventCounter);
  std::ofstream countsFile("TriggerCounts.txt");
  std::ofstream countsFileL1 ("TriggerCountsL1.txt");
  float ratioL1;
  float ratioHLT;
  float ratioHLTPerf;
  if (countsFile.is_open()) {
    countsFile << std::setw(50) << "Trigger Chain" 
               << std::setw(20) << "TDT" 
               << std::setw(20) << "Emulation" 
               << std::setw(20) << "Emulation w/Perf" 
               << std::setw(20) << "Emulation/TDT" 
               << std::setw(20) << "(Emulation w/Perf)/TDT" 
               <<  "\n";
    for (auto iter = m_counterBits.begin(); iter != m_counterBits.end(); iter++) {
      ATH_MSG_INFO(iter->first << " == " << iter->second << " emulated " << m_counterEmulationHLT[iter->first]);
      ratioHLT = 100-100*(double)m_counterEmulationHLT[iter->first]/(double)iter->second;
      ratioHLTPerf = 100-100*(double)m_counterEmulationHLT_Perf[iter->first]/(double)iter->second;
      countsFile << std::setw(50) << iter->first 
                 << std::setw(20) << iter->second 
                 << std::setw(20) << m_counterEmulationHLT[iter->first] 
                 << std::setw(20) << m_counterEmulationHLT_Perf[iter->first] 
                 << std::setw(20) << ratioHLT
                 << std::setw(20) << ratioHLTPerf
                 << "%\n";
    }
  }
  if (countsFileL1.is_open()) {
    countsFileL1 << std::setw(50) << "Trigger Chain" 
                 << std::setw(10) << "TDT" 
                 << std::setw(10) << "Emulated" 
                 << std::setw(10) << "Emulated/TDT"
                 << "\n";
    for (auto iter = m_counterBitsL1.begin(); iter != m_counterBitsL1.end(); iter++) {
      ATH_MSG_INFO(iter->first << " == " << iter->second << " emulated " << m_counterEmulationL1[iter->first]);
      ratioL1 = 100-100*(double)m_counterEmulationL1[iter->first]/(double)iter->second;
      countsFileL1 << std::setw(50) << iter->first 
                   << std::setw(10) << iter->second 
                   << std::setw(10) << m_counterEmulationL1[iter->first] 
                   << std::setw(10) << ratioL1 
                   << "%\n";
    }
  }
  countsFile.close();
  countsFileL1.close();
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaEmulationTool::TDCounts() {
  ATH_MSG_DEBUG( "L1: "    << tdt()->isPassed( "L1_.*" )
              << ", L2: "  << tdt()->isPassed( "L2_.*" )
              << ", EF: "  << tdt()->isPassed( "EF_.*" )
              << ", HLT: " << tdt()->isPassed( "HLT_.*"));
  // L1 emulation
  bool decision;
  bool emu;
  bool emu_wperf;
  for (const auto &trig : m_trigL1) {
    decision = tdt()->isPassed(trig);
    emu = EmulationL1(trig);
    mapL1TDTDec[trig] = decision;
    if (decision) {
      ++m_counterBitsL1[trig];
      cd(m_dir_tdcounts); hist1("TDCountsBitsL1")->Fill(trig.c_str(),1);
    }
    if (emu) {
      ++m_counterEmulationL1[trig];
      cd(m_dir_tdcounts); hist1("TDCountsEmulationL1")->Fill(trig.c_str(),1);
    }
    ATH_MSG_DEBUG("Emulation Decision = " << emu);
    ATH_MSG_DEBUG("      TDT Decision = " << decision);
    if (decision != emu) {
      ATH_MSG_DEBUG("TDT and Emulation are not the same " << trig);
    }
  }
  // HLT Emulation Electrons
  ATH_MSG_DEBUG("HLT Emulation - ElectronEF");
  std::string l1item="";
  for (const auto &trig : m_trigListElectrons) {
    decision = tdt()->isPassed(trig);
    emu = EmulationEF(trig);
    emu_wperf = EmulationEF_Perf(trig);
    ATH_MSG_DEBUG("HLT Emulation of " << trig);
    if (decision) {
      ++m_counterBits[trig];
      cd(m_dir_tdcounts); hist1("TDCountsBitsElectronChains")->Fill(trig.c_str(),1);
    }
    if (boost::contains(trig,"eb")) continue;
    l1item = getThisL1Item(trig);
    ATH_MSG_DEBUG("HLT Emulation of " << trig << " with L1 item " << l1item);
    if (mapL1TDTDec[l1item]) {  // this is better
    // if (EmulationL1(l1item)) {
      if (EmulationEF(trig)) {
        ++m_counterEmulationHLT[trig];
        cd(m_dir_tdcounts); hist1("TDCountsEmulationElectronChains")->Fill(trig.c_str(),1);
      }
      if (emu_wperf) {
        ++m_counterEmulationHLT_Perf[trig];
        cd(m_dir_tdcounts); hist1("TDCountsEmulationElectronChains_Perf")->Fill(trig.c_str(),1);
      }
    }
    ATH_MSG_DEBUG("Emulation Decision = " << emu);
    ATH_MSG_DEBUG("Emulation Decision = " << emu_wperf << " w/Perf chain");
    ATH_MSG_DEBUG("      TDT Decision = " << decision);
    if (decision != emu || decision != emu_wperf) {
      ATH_MSG_DEBUG("TDT and Emulation are not the same " << trig);
    }
  }
  // HLT Emulation Photons
  ATH_MSG_DEBUG("HLT Emulation - PhotonEF");
  for (const auto &trig : m_trigListPhotons) {
    decision = tdt()->isPassed(trig);
    emu = EmulationPhotonEF(trig);
    ATH_MSG_DEBUG("HLT Emulation of " << trig);
    if (decision) {
      ++m_counterBits[trig];
      cd(m_dir_tdcounts); hist1("TDCountsBitsPhotonChains")->Fill(trig.c_str(),1);
    }
    if (boost::contains(trig,"eb")) continue;
    l1item = getThisL1Item(trig);
    ATH_MSG_DEBUG("HLT Emulation of " << trig << " with L1 item " << l1item);
    if (mapL1TDTDec[l1item]) {  
    // if (EmulationL1(l1item)) {
      if (emu) {
        ++m_counterEmulationHLT[trig];
        cd(m_dir_tdcounts); hist1("TDCountsEmulationPhotonChains")->Fill(trig.c_str(),1);
      }
    }
    ATH_MSG_DEBUG("Emulation Decision = " << emu);
    ATH_MSG_DEBUG("      TDT Decision = " << decision);
    if (decision != emu) {
      ATH_MSG_DEBUG("TDT and Emulation are not the same " << trig);
    }
  }
  // test track isolation
  TrackIsolation();
  return StatusCode::SUCCESS;
}

bool TrigEgammaEmulationTool::EmulationL1(const std::string trigger) {
  bool isPassed = false;
  std::string L1Item = trigger;
  L1Item.erase(0, 5); //Removes L1EM prefix from name
  std::string l1type = boost::trim_copy_if(L1Item, boost::is_digit());
  std::string l1cut = boost::trim_copy_if(L1Item, !boost::is_digit());
  float l1threshold = atof(l1cut.c_str()); // Is the L1 threshold +1 GeV
  ATH_MSG_DEBUG("L1 Emulation for " << trigger);
  ATH_MSG_DEBUG("L1 chain       = " << trigger);
  ATH_MSG_DEBUG("L1 item        = " << L1Item);
  ATH_MSG_DEBUG("L1 threshold   = " << l1threshold);
  ATH_MSG_DEBUG("L1 type        = " << l1type);
  ATH_MSG_DEBUG("L1 cut         = " << l1cut);
  // float hadCoreCut = 0.0;
  float hadCoreCutMin = 1.0; // This could be defined somewhere else
  float hadCoreCutOff = -0.2;
  float hadCoreSlope = 1/23.0;
  // float emIsolCut = 0.0;
  float emIsolCutMin = 2.0; // This could be defined somewhere else
  float emIsolCutOff = -1.8;
  float emIsolCutSlope = 1/8.0;
  // float emEClusVCut = 0.0;
  float emE = 0.0;
  float emIsol = 0.0;
  float hadCore = 0.0;
  float eta = 0.0; 
  std::vector<xAOD::EmTauRoI*> l1AcceptedObj;
  std::vector<xAOD::EmTauRoI*> l1RejectedObj;
  for (const auto &l1: *m_emTauRoI) {
    if (l1->roiType() != xAOD::EmTauRoI::EMRoIWord) continue;
    emE     = l1->emClus()/1.e3;   // Cluster energy
    eta     = l1->eta();           // eta
    hadCore = l1->hadCore()/1.e3;  // Hadronic core energy
    emIsol  = l1->emIsol()/1.e3;   // EM Isolation energy
    ATH_MSG_DEBUG("emE     = " << emE);
    ATH_MSG_DEBUG("eta     = " << eta);
    ATH_MSG_DEBUG("hadCore = " << hadCore);
    ATH_MSG_DEBUG("emIsol  = " << emIsol);
    // ATH_MSG_DEBUG("bitset  = " << tdec.all());
    // l1s.push_back(l1);
    if (boost::contains(l1type,"H")) {
      ATH_MSG_DEBUG("L1 (H) CUT");
      if (!IsolationL1(hadCoreCutMin,hadCoreCutOff,hadCoreSlope,hadCore,emE)) {
        ATH_MSG_DEBUG("rejected");
        l1RejectedObj.push_back(l1);
        continue;
      }
      ATH_MSG_DEBUG("accepted");
    }
    if (boost::contains(l1type,"I")) {
      ATH_MSG_DEBUG("L1 (I) CUT");
      if (!IsolationL1(emIsolCutMin,emIsolCutOff,emIsolCutSlope,emIsol,emE)) {
        ATH_MSG_DEBUG("rejected");
        l1RejectedObj.push_back(l1);
        continue;
      }
      ATH_MSG_DEBUG("accepted");
    }
    ATH_MSG_DEBUG("Apply L1 Et cut " << l1threshold << " cluster emE " << emE << " eta " << eta);
    if (boost::contains(l1type,"V")) {
      ATH_MSG_DEBUG("L1 (V) CUT");
      if (!VariableEtL1(L1Item,emE,eta)) {
        ATH_MSG_DEBUG("rejected");
        l1RejectedObj.push_back(l1);
        continue;
      }
      ATH_MSG_DEBUG("accepted");
    }
    // add new method for this also
    else if (emE <= l1threshold) { // this cut is confirmed to be <=
      l1RejectedObj.push_back(l1);
      continue;
    }
    l1AcceptedObj.push_back(l1);
    isPassed = true;
  }
  fillL1histograms(l1AcceptedObj,l1RejectedObj,trigger);
  ATH_MSG_DEBUG("Emulation L1 Finished " << trigger);
  return isPassed;
}

// (H) and (I) Hadronic core and electromagnetic isolation
bool TrigEgammaEmulationTool::IsolationL1(const float min, const float offset, const float slope, const float energy, const float emE) {
  float isol_max_cut = 50.0; // This could be defined somewhere else
  if (emE > isol_max_cut) {
    ATH_MSG_DEBUG("L1 Isolation skipped, ET > Maximum isolation");
    return true;
  }
  float isolation = offset + emE*slope;
  if (isolation < min) isolation = min;
  /* 
   from CaloDef.py
   isolation <= ET/m + offset
  */
  bool value = (energy > isolation) ? false : true;
  ATH_MSG_DEBUG("L1 Isolation ET = " << energy << " ISOLATION CUT " << isolation);
  return value;
}

// (V) Variable Et cut
bool TrigEgammaEmulationTool::VariableEtL1(const std::string L1item, const float l1energy, const float l1eta) {
  float cut = EmulationL1V(L1item,l1eta);
  float energy = l1energy;
  ATH_MSG_DEBUG("L1 V " << L1item << " cut = " << cut << " eta " << l1eta << " energy " << l1energy << " rounded to " << energy);
  // if (energy <= cut) return false;
  bool value = (energy <= cut) ? false : true;
  return value;
}

// Eta dependant cuts for (V)
float TrigEgammaEmulationTool::EmulationL1V(const std::string L1item, const float l1eta) {
 // Values updated from TriggerMenu-00-13-26
 // Now they all look symmetric in negative and positive eta
 // look that in general que can remove the first region since it is the defaul value
  float cut=0.;
  // float eta = fabs((int)l1eta*10);
  float eta = fabs(l1eta);
  if (L1item=="50V") {
    if      (eta >= 0.8 && eta < 1.2) cut = 51.0;
    else if (eta >= 1.2 && eta < 1.6) cut = 50.0;
    else if (eta >= 1.6 && eta < 2.0) cut = 51.0;
    else cut = 52;
  }
  else if (L1item=="8VH") {
    if      (eta > 0.8 && eta <= 1.1) cut = 7.0;
    else if (eta > 1.1 && eta <= 1.4) cut = 6.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 5.0;
    else if (eta > 1.5 && eta <= 1.8) cut = 7.0;
    else if (eta > 1.8 && eta <= 2.5) cut = 8.0;
    else cut = 9.0;
  }
  else if (L1item=="10VH") {
    if      (eta > 0.8 && eta <= 1.1) cut = 9.0;
    else if (eta > 1.1 && eta <= 1.4) cut = 8.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 7.0;
    else if (eta > 1.5 && eta <= 1.8) cut = 9.0;
    else if (eta > 1.8 && eta <= 2.5) cut = 10.0;
    else cut = 11.;
  }
  else if (L1item=="13VH") {
    if      (eta > 0.7 && eta <= 0.9) cut = 14.0;
    else if (eta > 0.9 && eta <= 1.2) cut = 13.0;
    else if (eta > 1.2 && eta <= 1.4) cut = 12.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 11.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 13.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 14.0;
    else cut = 15.0;
  }
  else if (L1item=="15VH") {
    if      (eta > 0.7 && eta <= 0.9) cut = 16.0;
    else if (eta > 0.9 && eta <= 1.2) cut = 15.0;
    else if (eta > 1.2 && eta <= 1.4) cut = 14.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 13.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 15.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 16.0;
    else cut = 17.0;
  }
  else if (L1item == "18VH") {
    if      (eta > 0.7 && eta <= 0.8) cut = 19.0;
    else if (eta > 0.8 && eta <= 1.1) cut = 18.0;
    else if (eta > 1.1 && eta <= 1.3) cut = 17.0;
    else if (eta > 1.3 && eta <= 1.4) cut = 16.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 15.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 17.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 19.0;
    else cut = 20.0;
  }
  else if (L1item == "20VH") {
    if      (eta > 0.7 && eta <= 0.8) cut = 21.0;
    else if (eta > 0.8 && eta <= 1.1) cut = 20.0;
    else if (eta > 1.1 && eta <= 1.3) cut = 19.0;
    else if (eta > 1.3 && eta <= 1.4) cut = 18.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 17.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 19.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 21.0;
    else cut = 22.0;
  }
  else if (L1item == "20VHI") { // Same as 20VH
    if      (eta > 0.7 && eta <= 0.8) cut = 21.0;
    else if (eta > 0.8 && eta <= 1.1) cut = 20.0;
    else if (eta > 1.1 && eta <= 1.3) cut = 19.0;
    else if (eta > 1.3 && eta <= 1.4) cut = 18.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 17.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 19.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 21.0;
    else cut = 22.0;
  }
  else if (L1item == "22VHI") {
    if      (eta > 0.7 && eta <= 0.8) cut = 23.0;
    else if (eta > 0.8 && eta <= 1.1) cut = 22.0;
    else if (eta > 1.1 && eta <= 1.3) cut = 21.0;
    else if (eta > 1.3 && eta <= 1.4) cut = 20.0;
    else if (eta > 1.4 && eta <= 1.5) cut = 19.0;
    else if (eta > 1.5 && eta <= 1.7) cut = 21.0;
    else if (eta > 1.7 && eta <= 2.5) cut = 23.0;
    else cut = 24.0;
  }
  return cut;
}

void TrigEgammaEmulationTool::fillL1histograms(const std::vector<xAOD::EmTauRoI*> l1AcceptedObj,const std::vector<xAOD::EmTauRoI*> l1RejectedObj,const std::string trigger) {
  
  cd (mapL1Directory[trigger]);
  ATH_MSG_DEBUG("Number of accepted l1 objects " << l1AcceptedObj.size());
  ATH_MSG_DEBUG("Number of rejected l1 objects " << l1RejectedObj.size());
  for (const auto &l1 : l1RejectedObj) {
    hist2("emu_0_emE_eta")->Fill(l1->eta(),l1->emClus()/1.e3);
    hist2("emu_0_emE_emIso")->Fill(l1->emIsol()/1.e3,l1->emClus()/1.e3);
    hist2("emu_0_emE_hadCore")->Fill(l1->hadCore()/1.e3,l1->emClus()/1.e3);
  }
  for (const auto &l1 : l1AcceptedObj) {
    hist2("emu_1_emE_eta")->Fill(l1->eta(),l1->emClus()/1.e3);
    hist2("emu_1_emE_emIso")->Fill(l1->emIsol()/1.e3,l1->emClus()/1.e3);
    hist2("emu_1_emE_hadCore")->Fill(l1->hadCore()/1.e3,l1->emClus()/1.e3);
  }
  
  // TDT
  auto fc = tdt()->features(trigger, TrigDefs::alsoDeactivateTEs);
  auto initRois = fc.get<TrigRoiDescriptor>("initialRoI");
  for(auto initRoi : initRois){
      //if(feat.te()==NULL) continue;
      auto itEmTau = tdt()->ancestor<xAOD::EmTauRoI>(initRoi);
      const xAOD::EmTauRoI *l1tdt = itEmTau.cptr();
      if(l1tdt==NULL) continue;
      ATH_MSG_DEBUG("Filling L1 from TDT");
      ATH_MSG_DEBUG("\t TDT emE     = " << l1tdt->emClus()/1.e3);
      ATH_MSG_DEBUG("\t TDT eta     = " << l1tdt->eta());
      ATH_MSG_DEBUG("\t TDT hadCore = " << l1tdt->hadCore()/1.e3);
      ATH_MSG_DEBUG("\t TDT emIsol  = " << l1tdt->emIsol()/1.e3);
      hist2("tdt_1_emE_eta")->Fill(l1tdt->eta(),l1tdt->emClus()/1.e3);
      hist2("tdt_1_emE_emIso")->Fill(l1tdt->emIsol()/1.e3,l1tdt->emClus()/1.e3);
      hist2("tdt_1_emE_hadCore")->Fill(l1tdt->hadCore()/1.e3,l1tdt->emClus()/1.e3);
  }
}

bool TrigEgammaEmulationTool::EmulationPhotonEF(const std::string trigger) {  
  std::string trigItem = trigger;
  trigItem.erase( 0, 4); //Removes HLT_ prefix from name
  bool isPassed=false;
  bool isL1=false;
  std::string type="";
  float etthr=0;
  float l1thr=0;
  std::string l1type="";
  std::string pidname="";
  bool perf=false;
  bool etcut=false;
  bool isolation = false;
  bool idperf = false;
  bool hltcalo = false;
  if (boost::contains(trigItem,"iloose")) isolation = true;
  if (boost::contains(trigItem,"idperf")) idperf = true;
  // if (boost::contains(trigItem,"HLTCalo")) hltcalo = true;
  parseTriggerName(trigItem,"Loose",isL1,type,etthr,l1thr,l1type,pidname,etcut,perf); // Determines probe PID from trigger
  PrintDebugInfo(trigger,trigItem,etthr,l1thr,l1type,pidname,isolation,etcut,idperf,perf,hltcalo);
  if (boost::contains(pidname,"1")) return false; // Not emulating Run1
  for (const auto& eg : *m_onlPhotons) {
    if (perf) {
      ATH_MSG_DEBUG(trigItem << " Etcut trigger cut " << etthr);
      if (eg->caloCluster()->et()/1.e3 < etthr) continue;
      ATH_MSG_DEBUG("Passed " << eg->caloCluster()->et());
      isPassed=true; //at least one electron
    }
    else if (etcut) {
      ATH_MSG_DEBUG(trigItem << " Etcut trigger cut " << etthr);
      if (eg->caloCluster()->et()/1.e3 < etthr) continue;
      ATH_MSG_DEBUG("Passed " << eg->caloCluster()->et());
      isPassed=true; //at least one electron
    }
    else {
      ATH_MSG_DEBUG("Apply Pid " << pidname);
      if (pidname == "Tight") {
        if (!m_photonOnlIsEMTool[0]->accept(eg)) continue;
      }
      else if (pidname == "Medium") {
        if (!m_photonOnlIsEMTool[1]->accept(eg)) continue;
      }
      else if (pidname == "Loose") {
        if (!m_photonOnlIsEMTool[2]->accept(eg)) continue;
      }
      else ATH_MSG_DEBUG("No Pid tool, continue without PID"); 
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << eg->caloCluster()->et());
      if (eg->caloCluster()->et()/1.e3 < etthr) continue;
      ATH_MSG_DEBUG("Passed");
      isPassed=true;
    }
  }
  return isPassed; 
}

bool TrigEgammaEmulationTool::EmulationEF(const std::string trigger) {    
  std::string trigItem = trigger;
  trigItem.erase(0, 4); //Removes HLT_ prefix from name
  bool isPassed = false;
  bool isL1 = false;
  bool perf = false;
  bool etcut = false;
  bool isolation = false;
  bool idperf = false;
  bool hltcalo = false;
  float etthr = 0;
  float l1thr = 0;
  std::string type = "";
  std::string l1type = "";
  std::string pidname = "";
  if (boost::contains(trigItem,"iloose")) isolation = true;
  if (boost::contains(trigItem,"idperf")) idperf = true;
  if (boost::contains(trigItem,"HTLcalo")) hltcalo = true;
  // Use TrigInfo here
  parseTriggerName(trigItem,"Loose",isL1,type,etthr,l1thr,l1type,pidname,etcut,perf); // Determines probe PID from trigger
  PrintDebugInfo(trigger,trigItem,etthr,l1thr,l1type,pidname,isolation,etcut,idperf,perf,hltcalo);
  // Not emulating Run1
  if (boost::contains(pidname,"1")) return false;
  if (perf && m_onlElectrons->size() !=0) return true;
  ATH_MSG_DEBUG("Emulating EF with " << m_onlElectrons->size() << " Electrons");
  for (const auto& eg : *m_onlElectrons) {
    // Check Run2 only tracks
    //if (!eg->trackParticle()->patternRecoInfo()[xAOD::FastTrackFinderSeed]) continue;
    if (idperf) {
      ATH_MSG_DEBUG("Doing idperf");
      if (!ApplyElectronCaloPid(eg,pidname)) continue;
      ATH_MSG_DEBUG("Apply Pid " << pidname);
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << eg->caloCluster()->et());
      if (eg->caloCluster()->et()/1.e3 < etthr) continue;
      ATH_MSG_DEBUG("Passed");
      isPassed=true;
    }
    else if (etcut) {
      ATH_MSG_DEBUG("Doing etcut");
      ATH_MSG_DEBUG("ET = " << eg->caloCluster()->et()/1.e3 << " ET threshold = " << etthr);
      if (eg->caloCluster()->et()/1.e3 < etthr) {
        ATH_MSG_DEBUG("rejected");
        continue;
      }
      ATH_MSG_DEBUG("accepted");
      ATH_MSG_DEBUG("Track pattern " << eg->trackParticle()->patternRecoInfo());
      isPassed=true; //at least one electron
    }
    else if (isolation || hltcalo) {
      ATH_MSG_DEBUG("Doing isolation");
      if (!ApplyElectronCaloPid(eg,pidname)) continue;
      if (!ApplyElectronPid(eg,pidname)) continue;
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << eg->caloCluster()->et());
      if (eg->caloCluster()->et()/1.e3 < etthr) continue;
      float val=0.;
      ATH_MSG_DEBUG("Isolation");
      eg->isolationValue(val,xAOD::Iso::ptcone20);
      if ((val/eg->trackParticle()->pt()) > 0.1) continue;
      ATH_MSG_DEBUG("Passed");
      isPassed=true;
    }
    else {
      if (!ApplyElectronCaloPid(eg,pidname)) continue;
      if (!ApplyElectronPid(eg,pidname)) continue;
      ATH_MSG_DEBUG("Apply Pid " << pidname);
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << eg->caloCluster()->et());
      if (eg->caloCluster()->et()/1.e3 < etthr) continue;
      ATH_MSG_DEBUG("Passed");
      isPassed=true;
    }
  }
  return isPassed; 
}

bool TrigEgammaEmulationTool::EmulationEF_Perf(const std::string trigger) {
  std::string trigItem = trigger;
  trigItem.erase( 0, 4); //Removes HLT_ prefix from name
  bool isPassed=false;
  bool isL1=false;
  std::string type ="";
  float etthr=0;
  float l1thr=0;
  std::string l1type="";
  std::string pidname="";
  bool perf=false;
  bool etcut=false;
  bool isolation=false;
  bool idperf=false;
  bool hltcalo = false;
  ATH_MSG_DEBUG("Emulating EF with " << m_onlElectrons->size() << " Electrons");
  if (boost::contains(trigItem,"iloose")) isolation = true;
  if (boost::contains(trigItem,"idperf")) idperf = true;
  parseTriggerName(trigItem,"Loose",isL1,type,etthr,l1thr,l1type,pidname,etcut,perf); // Determines probe PID from trigger
  PrintDebugInfo(trigger,trigItem,etthr,l1thr,l1type,pidname,isolation,etcut,idperf,perf,hltcalo);
  // Not emulating Run1
  if (boost::contains(pidname,"1")) 
    return false;
  if (perf && m_onlElectrons->size() !=0) 
    return true;
  //Get the features for a trigger, e0_perf_L1EM15
  std::string trig00 = "e0_perf_L1EM15";
  ATH_MSG_DEBUG("Emulating EF with perf chain " << trig00);
  Trig::FeatureContainer fc = tdt()->features("HLT_"+trig00);
  // Get the vector of containers, so all the RoIs
  const auto vec = fc.get<xAOD::ElectronContainer>("egamma_Electrons");
  for (const auto feat : vec) {
    //Retrieve the electron container for each RoI
    const xAOD::ElectronContainer *cont = feat.cptr();
    for (const auto &eg : *cont) {
      // Apply the electron selection
      // Find one electron in one RoI which passes the selection and accept the event
      // Call the same method as the event-level container
      if (idperf) {
        if (!ApplyElectronCaloPid(eg,pidname)) continue;
        ATH_MSG_DEBUG("Apply Pid "    << pidname);
        ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << eg->caloCluster()->et());
        if (eg->caloCluster()->et()/1.e3 < etthr) continue;
        ATH_MSG_DEBUG("Passed");
        isPassed=true;
      }
      else if (etcut) {
        ATH_MSG_DEBUG(trigItem << " Etcut trigger cut " << etthr);
        if (eg->caloCluster()->et()/1.e3 < etthr) continue;
        ATH_MSG_DEBUG("Passed " << eg->caloCluster()->et());
        ATH_MSG_DEBUG("Track pattern " << eg->trackParticle()->patternRecoInfo());
        isPassed=true; //at least one electron
      }
      else if (isolation) {
        if (!ApplyElectronCaloPid(eg,pidname)) continue;
        if (!ApplyElectronPid(eg,pidname)) continue;
        ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << eg->caloCluster()->et());
        if (eg->caloCluster()->et()/1.e3 < etthr) continue;
        float val=0.;
        ATH_MSG_DEBUG("Isolation");
        eg->isolationValue(val,xAOD::Iso::ptcone20);
        if ((val/eg->trackParticle()->pt()) > 0.1) continue;
        ATH_MSG_DEBUG("Passed");
        isPassed=true;
      }
      else {
        if (!ApplyElectronCaloPid(eg,pidname)) continue;
        if (!ApplyElectronPid(eg,pidname)) continue;
        ATH_MSG_DEBUG("Apply Pid " << pidname);
        ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << eg->caloCluster()->et());
        if (eg->caloCluster()->et()/1.e3 < etthr) continue;
        ATH_MSG_DEBUG("Passed");
        isPassed=true;
      }
    }
  }
  return isPassed; 
}

bool TrigEgammaEmulationTool::ApplyElectronCaloPid(const xAOD::Electron *eg, const std::string pidname) {
  ATH_MSG_DEBUG("Apply Pid " << pidname);
  bool passTool=false;
  bool passSel=false;
  eg->passSelection(passSel,pidname);
  if (pidname == "Tight") {
    passTool = m_electronCaloIsEMTool[0]->accept(eg);
  }
  else if (pidname == "Medium") {
    passTool = m_electronCaloIsEMTool[1]->accept(eg);
  }
  else if (pidname == "Loose") {
    passTool = m_electronCaloIsEMTool[2]->accept(eg);
  }
  else if (pidname == "VLoose") {
    passTool = m_electronCaloIsEMTool[3]->accept(eg);
  }
  else if (pidname == "LHTight") {
    passTool = m_electronCaloLHTool[0]->accept(eg);
  }
  else if (pidname == "LHMedium") {
    passTool = m_electronCaloLHTool[1]->accept(eg);
  }
  else if (pidname == "LHLoose") {
    passTool = m_electronCaloLHTool[2]->accept(eg);
  }
  else if (pidname == "LHVLoose") {
    passTool = m_electronCaloLHTool[3]->accept(eg);
  }
  else {
    ATH_MSG_DEBUG("No Pid tool, continue without PID");
  }
  // if (passTool != passSel) {
  //   ATH_MSG_DEBUG("Apply Pid ERROR in Tool and eg->passSelection is " << passSel << " and ApplyElectronCaloPid is " << passTool);
  // }
  return passTool;
}

bool TrigEgammaEmulationTool::ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname) {
  ATH_MSG_DEBUG("Apply Pid " << pidname);
  bool passTool = false;
  bool passSel = false;
  eg->passSelection(passSel,pidname);
  if (pidname == "Tight") {
    passTool = m_electronOnlIsEMTool[0]->accept(eg);
  }
  else if (pidname == "Medium") {
    passTool = m_electronOnlIsEMTool[1]->accept(eg);
  }
  else if (pidname == "Loose") {
    passTool = m_electronOnlIsEMTool[2]->accept(eg);
  }
  else if (pidname == "VLoose") {
    passTool = m_electronOnlIsEMTool[3]->accept(eg);
  }
  else if (pidname == "LHTight") {
    passTool = m_electronOnlLHTool[0]->accept(eg);
  }
  else if (pidname == "LHMedium") {
    passTool = m_electronOnlLHTool[1]->accept(eg);
  }
  else if (pidname == "LHLoose") {
    passTool = m_electronOnlLHTool[2]->accept(eg);
  }
  else if (pidname == "LHVLoose") {
    passTool = m_electronOnlLHTool[3]->accept(eg);
  }
  else {
    ATH_MSG_DEBUG("No Pid tool, continue without PID");
  }
  if (passTool != passSel) {
    ATH_MSG_DEBUG("Apply Pid ERROR in Tool and eg->passSelection is " << passSel << " and ApplyElectronPid is " << passTool << " for " << pidname);
  }
  return passTool;
}

// Track Isolation
void TrigEgammaEmulationTool::TrackIsolation() {
  const std::string trigger = "HLT_e24_lhmedium_L1EM18VH";
  float m_ptcone20_old;
  float m_ptcone20_new;
  // get electrons from RoI using performance chain
  std::string perf = "HLT_e0_perf_L1EM15";
  ATH_MSG_DEBUG("Fetching electrons with performance chain " << perf);
  Trig::FeatureContainer fc = tdt()->features(perf);
  const auto vec = fc.get<xAOD::ElectronContainer>("egamma_Electrons",TrigDefs::alsoDeactivateTEs);
  for(auto feat : vec){
    const xAOD::ElectronContainer *cont = feat.cptr();
    const HLT::TriggerElement *te = feat.te();
    const auto* trackParticles  = tdt()->ancestor<xAOD::TrackParticleContainer>(te).cptr();
    // loop over electron containers
    for (const auto &el : *cont) {
      // more code here to requiere *el to have passed trigger
      // define all the stuff
      std::vector<xAOD::Iso::IsolationType> isoTypes;
      isoTypes.push_back(xAOD::Iso::ptcone20);
      xAOD::TrackCorrection corrlist;
      corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
      xAOD::Vertex *vx = 0;
      const std::set<const xAOD::TrackParticle*> tracksToExclude = xAOD::EgammaHelpers::getTrackParticles(el, 0);
      // old isolation value from xAOD
      el->isolationValue(m_ptcone20_old, xAOD::Iso::ptcone20);
      xAOD::TrackIsolation trackIsolation;
      if (!m_trackIsolationTool->trackIsolation(trackIsolation, *el, isoTypes, corrlist, vx, &tracksToExclude, trackParticles)) {
        std::cout << "Track Isolation failed!" << std::endl;
      }
      m_ptcone20_new = trackIsolation.ptcones[0];
      ATH_MSG_DEBUG("Filling pt20 with " << m_ptcone20_new/el->pt() << " and " << m_ptcone20_old/el->pt());
      cd(mapHLTDirectory[trigger]); hist2("pt20")->Fill(m_ptcone20_new/el->pt(),m_ptcone20_old/el->pt());
    }
  }
}



void TrigEgammaEmulationTool::InitializeTriggerList(int show) {
  // Define trigger list
  m_trigL1.clear(); 
  m_trigListElectrons.clear(); 
  m_trigListPhotons.clear();
  std::vector<std::string> selectElectronChains  = tdt()->getListOfTriggers("HLT_e.*");
  std::vector<std::string> selectPhotonChains  = tdt()->getListOfTriggers("HLT_g.*");
  std::string trigname = "";
  for (int i = 0; i < (int) m_trigInputList.size(); i++) {
    trigname = "HLT_"+m_trigInputList[i];
    for (int j = 0; j < (int) selectElectronChains.size(); j++) {
      size_t found = trigname.find(selectElectronChains[j]);
      if (found != std::string::npos) {
        m_trigListElectrons.push_back("HLT_"+m_trigInputList[i]);
        break;
      }
    }
    for (int j = 0; j < (int) selectPhotonChains.size(); j++) {
      size_t found = trigname.find(selectPhotonChains[j]);
      if (found != std::string::npos) {
        m_trigListPhotons.push_back("HLT_"+m_trigInputList[i]);
        break;
      }
    }
  }
  m_trigL1.push_back("L1_EM3");
  m_trigL1.push_back("L1_EM7");
  m_trigL1.push_back("L1_EM10");
  m_trigL1.push_back("L1_EM12");
  m_trigL1.push_back("L1_EM8VH");
  m_trigL1.push_back("L1_EM8I");
  m_trigL1.push_back("L1_EM10VH");
  m_trigL1.push_back("L1_EM13VH");
  m_trigL1.push_back("L1_EM15");
  m_trigL1.push_back("L1_EM15I");
  m_trigL1.push_back("L1_EM15HI");
  m_trigL1.push_back("L1_EM15VH");
  m_trigL1.push_back("L1_EM18VH");
  m_trigL1.push_back("L1_EM20VH");
  m_trigL1.push_back("L1_EM20VHI");
  m_trigL1.push_back("L1_EM22VHI");
  // m_trigL1.push_back("L1_EM3");
  // m_trigL1.push_back("L1_EM7");
  // m_trigL1.push_back("L1_EM10");
  // m_trigL1.push_back("L1_EM12");
  // m_trigL1.push_back("L1_EM15");
  // m_trigL1.push_back("L1_EM8I");
  // m_trigL1.push_back("L1_EM15I");
  // m_trigL1.push_back("L1_EM15HI");
  // m_trigL1.push_back("L1_EM8VH");
  // m_trigL1.push_back("L1_EM10VH");
  // m_trigL1.push_back("L1_EM13VH");
  // m_trigL1.push_back("L1_EM15VH");
  // m_trigL1.push_back("L1_EM18VH");
  // m_trigL1.push_back("L1_EM20VH");
  // m_trigL1.push_back("L1_EM20VHI");
  // m_trigL1.push_back("L1_EM22VHI");
  if (show == 1) {
    for (int i=0; i<(int)m_trigInputList.size();++i) 
      ATH_MSG_INFO("Input list: " << m_trigInputList[i]);
    for (int i=0; i<(int)m_trigListElectrons.size();++i) 
      ATH_MSG_INFO("Electron list: " << m_trigListElectrons[i]);
    for (int i=0; i<(int)m_trigListPhotons.size();++i) 
      ATH_MSG_INFO("Photon list: " << m_trigListPhotons[i]);
    for (int i=0; i<(int)m_trigL1.size(); ++i) 
      ATH_MSG_INFO("L1 item list: " << m_trigL1[i]);
  }
}

void TrigEgammaEmulationTool::InitializeMaps() {
  // Initialize maps
  for (const auto &trig : m_trigL1) {
    m_counterBitsL1[trig] = 0;
    m_counterEmulationL1[trig] = 0;
  }
  for (const auto &trig : m_trigListElectrons) {
    m_counterBits[trig] = 0;
    m_counterEmulationHLT[trig] = 0;
    m_counterEmulationHLT_Perf[trig] = 0;
  }
  for (const auto &trig : m_trigListPhotons) {
    m_counterBits[trig] = 0;
    m_counterEmulationHLT[trig] = 0;
  }
}

std::string TrigEgammaEmulationTool::getThisL1Item(std::string trigger){
  static std::map<std::string,std::string> m_triggerMap; //no longer class member but static
  if(m_triggerMap.empty()) { //Only initialize once
      /*
      Last update from 
      https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TriggerCommon/TriggerMenu/branches/TriggerMenu-00-09-41-branch/python/menu/MC_pp_v5.py#L784
      */
      m_triggerMap["e24_medium_iloose_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_medium_iloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_tight_iloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_tight_iloose"]="L1_EM20VHI";
      m_triggerMap["e26_tight_iloose"]="L1_EM22VHI";
      m_triggerMap["e60_medium"]="L1_EM22VHI";
      m_triggerMap["e120_loose"]="L1_EM22VHI";
      m_triggerMap["e140_loose"]="L1_EM22VHI";
      m_triggerMap["e24_lhmedium_iloose_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhmedium_iloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhtight_iloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhtight_iloose"]="L1_EM20VHI";
      m_triggerMap["e26_lhtight_iloose"]="L1_EM22VHI";
      m_triggerMap["e60_lhmedium"]="L1_EM22VHI";
      m_triggerMap["e120_lhloose"]="L1_EM22VHI";
      m_triggerMap["e140_lhloose"]="L1_EM22VHI";
      m_triggerMap["e24_vloose_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_vloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e26_vloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_medium_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_tight_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhvloose_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhvloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e26_lhvloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhmedium_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhtight_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_medium_iloose_L2StarA_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_medium_L2Star_idperf_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhmedium_iloose_L2StarA_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhmedium_L2Star_idperf_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e5_loose"]="L1_EM3";
      m_triggerMap["e5_loose_L2StarA"]="L1_EM3";
      m_triggerMap["e5_loose_L2Star_idperf"]="L1_EM3";
      m_triggerMap["e5_loose_idperf"]="L1_EM3";
      m_triggerMap["e24_medium_idperf_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e5_lhloose"]="L1_EM3";
      m_triggerMap["e5_lhloose_L2StarA"]="L1_EM3";
      m_triggerMap["e5_lhloose_L2Star_idperf"]="L1_EM3";
      m_triggerMap["e5_lhloose_idperf"]="L1_EM3";
      m_triggerMap["e24_lhmedium_idperf_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e5_vloose"]="L1_EM3";
      m_triggerMap["e10_vloose_L1EM7"]="L1_EM7";
      m_triggerMap["e15_vloose_L1EM7"]="L1_EM7";
      m_triggerMap["e20_vloose_L1EM12"]="L1_EM12";
      m_triggerMap["e25_vloose_L1EM15"]="L1_EM15";
      m_triggerMap["e30_vloose_L1EM15"]="L1_EM15";
      m_triggerMap["e40_vloose_L1EM15"]="L1_EM15";
      m_triggerMap["e50_vloose_L1EM15"]="L1_EM15";
      m_triggerMap["e60_vloose"]="L1_EM22VHI";
      m_triggerMap["e70_vloose"]="L1_EM22VHI";
      m_triggerMap["e80_vloose"]="L1_EM22VHI";
      m_triggerMap["e100_vloose"]="L1_EM22VHI";
      m_triggerMap["e120_vloose"]="L1_EM22VHI";
      m_triggerMap["e12_vloose_L1EM10VH"]="L1_EM10VH";
      m_triggerMap["e15_vloose_L1EM13VH"]="L1_EM13VH";
      m_triggerMap["e17_vloose"]="L1_EM15VH";
      m_triggerMap["e20_vloose"]="L1_EM15VH";
      m_triggerMap["e5_lhvloose"]="L1_EM3";
      m_triggerMap["e10_lhvloose_L1EM7"]="L1_EM7";
      m_triggerMap["e15_lhvloose_L1EM7"]="L1_EM7";
      m_triggerMap["e20_lhvloose_L1EM12"]="L1_EM12";
      m_triggerMap["e25_lhvloose_L1EM15"]="L1_EM15";
      m_triggerMap["e30_lhvloose_L1EM15"]="L1_EM15";
      m_triggerMap["e40_lhvloose_L1EM15"]="L1_EM15";
      m_triggerMap["e50_lhvloose_L1EM15"]="L1_EM15";
      m_triggerMap["e60_lhvloose"]="L1_EM22VHI";
      m_triggerMap["e70_lhvloose"]="L1_EM22VHI";
      m_triggerMap["e80_lhvloose"]="L1_EM22VHI";
      m_triggerMap["e100_lhvloose"]="L1_EM22VHI";
      m_triggerMap["e120_lhvloose"]="L1_EM22VHI";
      m_triggerMap["e12_lhvloose_L1EM10VH"]="L1_EM10VH";
      m_triggerMap["e15_lhvloose_L1EM13VH"]="L1_EM13VH";
      m_triggerMap["e17_lhvloose"]="L1_EM15VH";
      m_triggerMap["e20_lhvloose"]="L1_EM15VH";
      m_triggerMap["e17_loose_L1EM15VHJJ1523ETA49"]="L1_EM15VH_JJ15.23ETA49";
      m_triggerMap["e17_lhloose_L1EM15VHJJ1523ETA49"]="L1_EM15VH_JJ15.23ETA49";
      m_triggerMap["e0_perf_L1EM15"]="L1_EM15";
      m_triggerMap["e0_L2Star_perf_L1EM15"]="L1_EM15";
      m_triggerMap["e5_etcut"]="L1_EM3";
      m_triggerMap["e10_etcut_L1EM7"]="L1_EM7";
      m_triggerMap["e15_etcut_L1EM7"]="L1_EM7";
      m_triggerMap["e20_etcut_L1EM12"]="L1_EM12";
      m_triggerMap["e25_etcut_L1EM15"]="L1_EM15";
      m_triggerMap["e30_etcut_L1EM15"]="L1_EM15";
      m_triggerMap["e40_etcut_L1EM15"]="L1_EM15";
      m_triggerMap["e50_etcut_L1EM15"]="L1_EM15";
      m_triggerMap["e60_etcut"]="L1_EM22VHI";
      m_triggerMap["e80_etcut"]="L1_EM22VHI";
      m_triggerMap["e100_etcut"]="L1_EM22VHI";
      m_triggerMap["e120_etcut"]="L1_EM22VHI";
      m_triggerMap["e4_etcut"]="L1_EM3";
      m_triggerMap["e9_etcut"]="L1_EM7";
      m_triggerMap["e14_etcut"]="L1_EM12";
      m_triggerMap["e9_medium"]="L1_EM7";
      m_triggerMap["e12_loose"]="L1_EM8VH";
      m_triggerMap["e12_loose_L1EM10VH"]="L1_EM10VH";
      m_triggerMap["e15_loose_L1EM13VH"]="L1_EM13VH";
      m_triggerMap["e17_loose_L1EM15"]="L1_EM15";
      m_triggerMap["e17_loose"]="L1_EM15VH";
      m_triggerMap["e17_medium"]="L1_EM15VH";
      m_triggerMap["e17_medium_iloose"]="L1_EM15VH";
      m_triggerMap["e20_medium"]="L1_EM15VH";
      m_triggerMap["e24_medium_L1EM15VH"]="L1_EM15VH";
      m_triggerMap["e60_loose"]="L1_EM22VHI";
      m_triggerMap["e70_loose"]="L1_EM22VHI";
      m_triggerMap["e5_tight"]="L1_EM3";
      m_triggerMap["e9_tight"]="L1_EM7";
      m_triggerMap["e14_tight"]="L1_EM12";
      m_triggerMap["e9_lhmedium"]="L1_EM7";
      m_triggerMap["e12_lhloose"]="L1_EM8VH";
      m_triggerMap["e12_lhloose_L1EM10VH"]="L1_EM10VH";
      m_triggerMap["e15_lhloose_L1EM13VH"]="L1_EM13VH";
      m_triggerMap["e17_lhloose_L1EM15"]="L1_EM15";
      m_triggerMap["e17_lhloose"]="L1_EM15VH";
      m_triggerMap["e17_lhmedium"]="L1_EM15VH";
      m_triggerMap["e17_lhmedium_iloose"]="L1_EM15VH";
      m_triggerMap["e20_lhmedium"]="L1_EM15VH";
      m_triggerMap["e24_lhmedium_L1EM15VH"]="L1_EM15VH";
      m_triggerMap["e60_lhloose"]="L1_EM22VHI";
      m_triggerMap["e70_lhloose"]="L1_EM22VHI";
      m_triggerMap["e5_lhtight"]="L1_EM3";
      m_triggerMap["e9_lhtight"]="L1_EM7";
      m_triggerMap["e14_lhtight"]="L1_EM12";
      m_triggerMap["e24_tight_iloose_L2EFCalo_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhtight_iloose_L2EFCalo_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_tight_iloose_HLTCalo_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhtight_iloose_HLTCalo_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_tight_iloose_etisem_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhmedium_iloose_HLTCalo_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhmedium_iloose_HLTCalo_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhtight_iloose_HLTCalo"]="L1_EM20VHI";
      m_triggerMap["e26_lhtight_iloose_HLTCalo"]="L1_EM22VHI";
      m_triggerMap["e60_lhmedium_HLTCalo"]="L1_EM22VHI";
      m_triggerMap["e120_lhloose_HLTCalo"]="L1_EM22VHI";
      m_triggerMap["e140_lhloose_HLTCalo"]="L1_EM22VHI";
      m_triggerMap["g120_loose_HLTCalo"]="L1_EM22VHI";
      m_triggerMap["g35_medium_HLTCalo_g25_medium_HLTCalo"]="L1_2EM15VH";
      m_triggerMap["2e12_lhloose_HLTCalo_L12EM10VH"]="L1_2EM10VH";
      m_triggerMap["2e15_lhloose_HLTCalo_L12EM13VH"]="L1_2EM13VH";
      m_triggerMap["2e17_lhloose_HLTCalo"]="L1_2EM15VH";
      m_triggerMap["e17_lhloose_cutd0dphideta_L1EM15"]="L1_EM15";
      m_triggerMap["e17_lhloose_nod0_L1EM15"]="L1_EM15";
      m_triggerMap["e17_lhloose_nodeta_L1EM15"]="L1_EM15";
      m_triggerMap["e17_lhloose_nodphires_L1EM15"]="L1_EM15";
      m_triggerMap["e24_lhmedium_nod0_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhmedium_nodeta_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhmedium_nodphires_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhmedium_cutd0dphideta_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhmedium_nod0_iloose_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhmedium_nodeta_iloose_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhmedium_nodphires_iloose_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhmedium_cutd0dphideta_iloose_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhmedium_cutd0dphideta_iloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhtight_cutd0dphideta_iloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhtight_cutd0dphideta_iloose"]="L1_EM20VHI";
      m_triggerMap["e26_lhtight_cutd0dphideta_iloose"]="L1_EM22VHI";
      m_triggerMap["e60_lhmedium_cutd0dphideta"]="L1_EM22VHI";
      m_triggerMap["2e12_lhloose_nod0_L12EM10VH"]="L1_2EM10VH";
      m_triggerMap["2e12_lhloose_nodeta_L12EM10VH"]="L1_2EM10VH";
      m_triggerMap["2e12_lhloose_nodphires_L12EM10VH"]="L1_2EM10VH";
      m_triggerMap["2e12_lhloose_cutd0dphideta_L12EM10VH"]="L1_2EM10VH";
      m_triggerMap["2e15_lhloose_cutd0dphideta_L12EM13VH"]="L1_2EM13VH";
      m_triggerMap["2e17_lhloose_cutd0dphideta"]="L1_2EM15VH";
      m_triggerMap["e24_lhmedium_nod0_iloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhtight_nod0_iloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhtight_nod0_iloose"]="L1_EM20VHI";
      m_triggerMap["e26_lhtight_nod0_iloose"]="L1_EM22VHI";
      m_triggerMap["e60_lhmedium_nod0"]="L1_EM22VHI";
      m_triggerMap["2e15_lhloose_nod0_L12EM13VH"]="L1_2EM13VH";
      m_triggerMap["2e17_lhloose_nod0"]="L1_2EM15VH";
      m_triggerMap["e120_lhloose_nod0"]="L1_EM22VHI";
      m_triggerMap["e140_lhloose_nod0"]="L1_EM22VHI";
      m_triggerMap["e120_loose1"]="L1_EM22VHI";
      m_triggerMap["e24_medium1_iloose_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_medium1_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_tight1_iloose_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_tight1_iloose"]="L1_EM20VHI";
      m_triggerMap["e26_tight1_iloose"]="L1_EM22VHI";
      m_triggerMap["e60_medium1"]="L1_EM22VHI";
      m_triggerMap["e26_lhtight_smooth_iloose"]="L1_EM22VHI";
      m_triggerMap["2e12_loose_L12EM10VH"]="L1_2EM10VH";
      m_triggerMap["2e15_loose_L12EM13VH"]="L1_2EM13VH";
      m_triggerMap["2e17_loose_L12EM15"]="L1_2EM15";
      m_triggerMap["2e17_loose"]="L1_2EM15VH";
      m_triggerMap["2e12_lhloose_L12EM10VH"]="L1_2EM10VH";
      m_triggerMap["2e15_lhloose_L12EM13VH"]="L1_2EM13VH";
      m_triggerMap["2e17_lhloose"]="L1_2EM15VH";
      m_triggerMap["2e17_lhloose_L12EM15"]="L1_2EM15";
      m_triggerMap["e17_medium_2e9_medium"]="L1_EM15VH_3EM7";
      m_triggerMap["e17_loose_2e9_loose"]="L1_EM15VH_3EM7";
      m_triggerMap["e17_lhloose_2e9_lhloose"]="L1_EM15VH_3EM7";
      m_triggerMap["e17_medium_iloose_2e9_medium"]="L1_EM15VH_3EM7";
      m_triggerMap["e17_lhmedium_2e9_lhmedium"]="L1_EM15VH_3EM7";
      m_triggerMap["e17_lhmedium_iloose_2e9_lhmedium"]="L1_EM15VH_3EM7";
      m_triggerMap["2e12_loose1_L12EM10VH"]="L1_2EM10VH";
      m_triggerMap["2e17_loose1"]="L1_2EM15VH";
      m_triggerMap["e5_tight_e4_etcut"]="L1_2EM3";
      m_triggerMap["e5_tight_e4_etcut_Jpsiee"]="L1_2EM3";
      m_triggerMap["e9_tight_e4_etcut_Jpsiee"]="L1_EM7_2EM3";
      m_triggerMap["e9_etcut_e5_tight_Jpsiee"]="L1_EM7_2EM3";
      m_triggerMap["e14_tight_e4_etcut_Jpsiee"]="L1_EM12_2EM3";
      m_triggerMap["e14_etcut_e5_tight_Jpsiee"]="L1_EM12_2EM3";
      m_triggerMap["e5_lhtight_e4_etcut"]="L1_2EM3";
      m_triggerMap["e5_lhtight_e4_etcut_Jpsiee"]="L1_2EM3";
      m_triggerMap["e9_lhtight_e4_etcut_Jpsiee"]="L1_EM7_2EM3";
      m_triggerMap["e9_etcut_e5_lhtight_Jpsiee"]="L1_EM7_2EM3";
      m_triggerMap["e14_lhtight_e4_etcut_Jpsiee"]="L1_EM12_2EM3";
      m_triggerMap["e14_etcut_e5_lhtight_Jpsiee"]="L1_EM12_2EM3";
      m_triggerMap["e5_tight1_e4_etcut"]="L1_2EM3";
      m_triggerMap["e5_tight1_e4_etcut_Jpsiee"]="L1_2EM3";
      m_triggerMap["e5_tight_e4_etcut_L1JPSI-1M5"]="L1_JPSI-1M5";
      m_triggerMap["e5_lhtight_e4_etcut_L1JPSI-1M5"]="L1_JPSI-1M5";
      m_triggerMap["e5_tight1_e4_etcut_L1JPSI-1M5"]="L1_JPSI-1M5";
      m_triggerMap["e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5"]="L1_JPSI-1M5";
      m_triggerMap["e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7"]="L1_JPSI-1M5-EM7";
      m_triggerMap["e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7"]="L1_JPSI-1M5-EM7";
      m_triggerMap["e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12"]="L1_JPSI-1M5-EM12";
      m_triggerMap["e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12"]="L1_JPSI-1M5-EM12";
      m_triggerMap["e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5"]="L1_JPSI-1M5";
      m_triggerMap["e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7"]="L1_JPSI-1M5-EM7";
      m_triggerMap["e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7"]="L1_JPSI-1M5-EM7";
      m_triggerMap["e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12"]="L1_JPSI-1M5-EM12";
      m_triggerMap["e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12"]="L1_JPSI-1M5-EM12";
      m_triggerMap["e5_tight1_e4_etcut_Jpsiee_L1JPSI-1M5"]="L1_JPSI-1M5";
      m_triggerMap["e13_etcut_trkcut"]="L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0";
      m_triggerMap["e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE"]="L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE";
      m_triggerMap["e18_etcut_trkcut"]="L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0";
      m_triggerMap["e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE"]="L1_EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE";
      m_triggerMap["e5_etcut_L1W-05DPHI-JXE-0"]="L1_W-05DPHI-JXE-0";
      m_triggerMap["e5_etcut_L1W-10DPHI-JXE-0"]="L1_W-10DPHI-JXE-0";
      m_triggerMap["e5_etcut_L1W-15DPHI-JXE-0"]="L1_W-15DPHI-JXE-0";
      m_triggerMap["e5_etcut_L1W-10DPHI-EMXE-0"]="L1_W-10DPHI-EMXE-0";
      m_triggerMap["e5_etcut_L1W-15DPHI-EMXE-0"]="L1_W-15DPHI-EMXE-0";
      m_triggerMap["e5_etcut_L1W-05DPHI-EMXE-1"]="L1_W-05DPHI-EMXE-1";
      m_triggerMap["e5_etcut_L1W-05RO-XEHT-0"]="L1_W-05RO-XEHT-0";
      m_triggerMap["e5_etcut_L1W-90RO2-XEHT-0"]="L1_W-90RO2-XEHT-0";
      m_triggerMap["e5_etcut_L1W-250RO2-XEHT-0"]="L1_W-250RO2-XEHT-0";
      m_triggerMap["e5_etcut_L1W-HT20-JJ15.ETA49"]="L1_W-HT20-JJ15.ETA49";
      m_triggerMap["e13_etcut_L1W-NOMATCH"]="L1_W-NOMATCH";
      m_triggerMap["e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT"]="L1_W-NOMATCH_W-05RO-XEEMHT";
      m_triggerMap["e13_etcut_L1EM10_W-MT25"]="L1_EM10_W-MT25";
      m_triggerMap["e13_etcut_L1EM10_W-MT30"]="L1_EM10_W-MT30";
      m_triggerMap["e18_etcut_L1EM15_W-MT35"]="L1_EM15_W-MT35";
      m_triggerMap["e24_lhtight_L1EM20VH_e15_etcut_Zee"]="L1_EM20VH";
      m_triggerMap["e24_tight_L1EM20VH_e15_etcut_Zee"]="L1_EM20VH";
      m_triggerMap["e26_lhtight_e15_etcut_Zee"]="L1_EM22VHI";
      m_triggerMap["e26_tight_e15_etcut_Zee"]="L1_EM22VHI";
      m_triggerMap["g120_loose"]="L1_EM22VHI";
      m_triggerMap["g120_loose1"]="L1_EM22VHI";
      m_triggerMap["g140_loose"]="L1_EM22VHI";
      m_triggerMap["g200_etcut"]="L1_EM22VHI";
      m_triggerMap["g0_perf_L1EM15"]="L1_EM15";
      m_triggerMap["g20_etcut_L1EM12"]="L1_EM12";
      m_triggerMap["g15_loose_L1EM7"]="L1_EM7";
      m_triggerMap["g20_loose_L1EM12"]="L1_EM12";
      m_triggerMap["g40_loose_L1EM15"]="L1_EM15";
      m_triggerMap["g45_loose_L1EM15"]="L1_EM15";
      m_triggerMap["g50_loose_L1EM15"]="L1_EM15";
      m_triggerMap["g80_loose"]="L1_EM22VHI";
      m_triggerMap["g100_loose"]="L1_EM22VHI";
      m_triggerMap["g10_loose"]="L1_EM7";
      m_triggerMap["g25_loose_L1EM15"]="L1_EM15";
      m_triggerMap["g35_loose_L1EM15"]="L1_EM15";
      m_triggerMap["g60_loose"]="L1_EM22VHI";
      m_triggerMap["g70_loose"]="L1_EM22VHI";
      m_triggerMap["g10_etcut"]="L1_EM7";
      m_triggerMap["g10_medium"]="L1_EM7";
      m_triggerMap["g15_loose_L1EM3"]="L1_EM3";
      m_triggerMap["g15_loose"]="L1_EM13VH";
      m_triggerMap["g20_loose_L1EM15"]="L1_EM15";
      m_triggerMap["g20_loose"]="L1_EM15VH";
      m_triggerMap["g20_tight"]="L1_EM15VH";
      m_triggerMap["g25_loose"]="L1_EM15VH";
      m_triggerMap["g25_medium"]="L1_EM15VH";
      m_triggerMap["g35_loose"]="L1_EM15VH";
      m_triggerMap["g35_medium"]="L1_EM15VH";
      m_triggerMap["g40_tight"]="L1_EM20VH";
      m_triggerMap["g45_tight"]="L1_EM22VHI";
      m_triggerMap["g50_loose"]="L1_EM15VH";
      m_triggerMap["g60_loose_L1EM15VH"]="L1_EM15VH";
      m_triggerMap["g35_loose_L1EM15_g25_loose_L1EM15"]="L1_2EM15";
      m_triggerMap["g35_loose_g25_loose"]="L1_2EM15VH";
      m_triggerMap["g35_loose1_g25_loose1"]="L1_2EM15VH";
      m_triggerMap["g35_medium_g25_medium"]="L1_2EM15VH";
      m_triggerMap["g35_medium1_g25_medium1"]="L1_2EM15VH";
      m_triggerMap["2g20_loose_L12EM15"]="L1_2EM15";
      m_triggerMap["2g20_tight"]="L1_2EM15VH";
      m_triggerMap["2g50_loose"]="L1_2EM15VH";
      m_triggerMap["2g60_loose_L12EM15VH"]="L1_2EM15VH";
      m_triggerMap["2g22_tight"]="L1_2EM15VH";
      m_triggerMap["2g25_tight"]="L1_2EM15VH";
      m_triggerMap["2g10_loose"]="L1_2EM7";
      m_triggerMap["3g15_loose"]="L1_2EM13VH";
      m_triggerMap["g20_loose_2g15_loose_L12EM13VH"]="L1_2EM13VH";
      m_triggerMap["2g20_loose_g15_loose"]="L1_2EM15VH";
      m_triggerMap["3g20_loose"]="L1_2EM15VH";
      m_triggerMap["g0_hiptrt_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["g0_hiptrt_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["g0_hiptrt_L1EM20VHI"]="L1_EM20VHI";
      m_triggerMap["g0_hiptrt_L1EM22VHI"]="L1_EM22VHI";
      m_triggerMap["e17_etcut_L1EM15"]="L1_EM15";
      m_triggerMap["e17_etcut_L2StarA_L1EM15"]="L1_EM15";
      m_triggerMap["e17_etcut_L2StarB_L1EM15"]="L1_EM15";
      m_triggerMap["e17_etcut_L2StarC_L1EM15"]="L1_EM15";
      m_triggerMap["e17_etcut_L2Star_idperf_L1EM15"]="L1_EM15";
      m_triggerMap["e17_etcut_idperf_L1EM15"]="L1_EM15";
      m_triggerMap["e24_medium1_L2Star_idperf_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e5_loose1_L2Star_idperf"]="L1_EM3";
      m_triggerMap["e24_medium1_idperf_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e5_loose1_idperf"]="L1_EM3";
      m_triggerMap["e24_medium1_iloose_L2StarA_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e5_tight_idperf"]="L1_EM3";
      m_triggerMap["e10_tight_idperf"]="L1_EM7";
      m_triggerMap["e5_lhtight_idperf"]="L1_EM3";
      m_triggerMap["e10_lhtight_idperf"]="L1_EM7";
      m_triggerMap["e24_lhvloose_nod0_L1EM18VH"]="L1_EM18VH";
      m_triggerMap["e24_lhvloose_nod0_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e26_lhvloose_nod0_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e24_lhtight_nod0_L1EM20VH"]="L1_EM20VH";
      m_triggerMap["e5_lhvloose_nod0"]="L1_EM3";
      m_triggerMap["e20_lhvloose_nod0"]="L1_EM15VH";
      m_triggerMap["e40_lhvloose_nod0"]="L1_EM20VH";
      m_triggerMap["e60_lhvloose_nod0"]="L1_EM22VHI";
      m_triggerMap["e12_lhvloose_nod0_L1EM10VH"]="L1_EM10VH";
      m_triggerMap["e15_lhvloose_nod0_L1EM13VH"]="L1_EM13VH";
      m_triggerMap["e17_lhvloose_nod0"]="L1_EM15VH";
      m_triggerMap["e7_lhmedium_nod0"]="L1_EM3";
      m_triggerMap["e9_lhloose_nod0"]="L1_EM7";
      m_triggerMap["e9_lhmedium_nod0"]="L1_EM7";
      m_triggerMap["e12_lhloose_nod0"]="L1_EM8VH";
      m_triggerMap["e12_lhmedium_nod0"]="L1_EM8VH";
      m_triggerMap["e12_lhloose_nod0_L1EM10VH"]="L1_EM10VH";
      m_triggerMap["e15_lhloose_nod0_L1EM13VH"]="L1_EM13VH";
      m_triggerMap["e17_lhloose_nod0"]="L1_EM15VH";
      m_triggerMap["e17_lhmedium_nod0"]="L1_EM15VH";
      m_triggerMap["e17_lhmedium_nod0_iloose"]="L1_EM15VH";
      m_triggerMap["e20_lhmedium_nod0"]="L1_EM15VH";
      m_triggerMap["e24_lhmedium_nod0_L1EM15VH"]="L1_EM15VH";
      m_triggerMap["e60_lhloose_nod0"]="L1_EM22VHI";
      m_triggerMap["e70_lhloose_nod0"]="L1_EM22VHI";
      m_triggerMap["e5_lhtight_nod0"]="L1_EM3";
      m_triggerMap["e9_lhtight_nod0"]="L1_EM7";
      m_triggerMap["e14_lhtight_nod0"]="L1_EM12";
      m_triggerMap["e24_lhmedium_nod0_L1EM20VHI"]="L1_EM20VHI";
      m_triggerMap["e26_lhmedium_nod0_L1EM22VHI"]="L1_EM22VHI";
      m_triggerMap["e17_lhloose_nod0_2e9_lhloose_nod0"]="L1_EM15VH_3EM7";
      m_triggerMap["e17_lhmedium_nod0_2e9_lhmedium_nod0"]="L1_EM15VH_3EM7";
      m_triggerMap["e17_lhmedium_nod0_iloose_2e9_lhmedium_nod0"]="L1_EM15VH_3EM7";
      m_triggerMap["e5_lhtight_nod0_e4_etcut"]="L1_2EM3";
      m_triggerMap["e5_lhtight_nod0_e4_etcut_Jpsiee"]="L1_2EM3";
      m_triggerMap["e9_lhtight_nod0_e4_etcut_Jpsiee"]="L1_EM7_2EM3";
      m_triggerMap["e9_etcut_e5_lhtight_nod0_Jpsiee"]="L1_EM7_2EM3";
      m_triggerMap["e14_lhtight_nod0_e4_etcut_Jpsiee"]="L1_EM12_2EM3";
      m_triggerMap["e14_etcut_e5_lhtight_nod0_Jpsiee"]="L1_EM12_2EM3";
      m_triggerMap["e5_lhtight_nod0_e4_etcut_L1JPSI-1M5"]="L1_JPSI-1M5";
      m_triggerMap["e5_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5"]="L1_JPSI-1M5";
      m_triggerMap["e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7"]="L1_JPSI-1M5-EM7";
      m_triggerMap["e9_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM7"]="L1_JPSI-1M5-EM7";
      m_triggerMap["e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12"]="L1_JPSI-1M5-EM12";
      m_triggerMap["e14_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM12"]="L1_JPSI-1M5-EM12";
      m_triggerMap["e40_lhvloose"]="L1_EM20VH";
      m_triggerMap["e7_lhmedium"]="L1_EM3";
      m_triggerMap["e9_lhloose"]="L1_EM7";
      m_triggerMap["e12_lhmedium"]="L1_EM8VH";
      m_triggerMap["e24_lhmedium_L1EM20VHI"]="L1_EM20VHI";
      m_triggerMap["e26_lhmedium_L1EM22VHI"]="L1_EM22VHI";
      m_triggerMap["e40_vloose"]="L1_EM20VH";
      m_triggerMap["e7_medium"]="L1_EM3";
      m_triggerMap["e9_loose"]="L1_EM7";
      m_triggerMap["e12_medium"]="L1_EM8VH";
      m_triggerMap["e24_medium_L1EM20VHI"]="L1_EM20VHI";
      m_triggerMap["e26_medium_L1EM22VHI"]="L1_EM22VHI";
  }
  if(boost::contains(trigger,"HLT")) trigger.erase(0,4);
  return m_triggerMap[trigger];
}

void TrigEgammaEmulationTool::PrintDebugInfo(
  const std::string trigger,
  const std::string trigItem,
  const float etthr,
  const float l1thr,
  const std::string l1type,
  const std::string pidname,
  const bool isolation,
  const bool etcut,
  const bool idperf,
  const bool perf,
  const bool hltcalo) {
  ATH_MSG_DEBUG("HLT Emulation for " << trigger);
  ATH_MSG_DEBUG("Trigger item      " << trigItem);
  ATH_MSG_DEBUG("ET cut  " << etthr);
  ATH_MSG_DEBUG("L1 cut  " << l1thr);
  ATH_MSG_DEBUG("L1 type " << l1type);
  ATH_MSG_DEBUG("pid     " << pidname);
  ATH_MSG_DEBUG("Trigger cut components: ");
  ATH_MSG_DEBUG("isolation " << isolation);
  ATH_MSG_DEBUG("etcut     " << etcut);
  ATH_MSG_DEBUG("idperf    " << idperf);
  ATH_MSG_DEBUG("perf      " << perf);
  ATH_MSG_DEBUG("hltcalo   " << hltcalo);
}
