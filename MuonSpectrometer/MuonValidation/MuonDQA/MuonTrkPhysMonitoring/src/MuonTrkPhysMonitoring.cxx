/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// /* ================================================================================
//  MuonTrkPhysMonitoring.cxx
//  ------------------------------
//  AUTHORS:     Nektarios Chr.Benekos, Austin Basye
//  created:     September 2011
//  modifed:     September 2013 , Christina G. Skarpathiotaki (UoP)
//  description: Implementation code for the MuonTrkPhysMonitoring
//  ============================================================================== */

// #include "MuonTrkPhysMonitoring/MuonTrkPhysMonitoring.h"

// using namespace MuonTrkPhysMonUtils;

// MuonTrkPhysMonitoring::MuonTrkPhysMonitoring(const std::string & type, 
//                                              const std::string & name, 
//                                              const IInterface* parent):
//     ManagedMonitorToolBase(type, name, parent),
//     m_hNEvent(0),
//     m_log(msgSvc(), name),
//     m_lumiblock(0),
//     m_eventCounter(0),
//     m_oZSignalPlots(0),
//     m_oJpsiSignalPlots(0),
//     m_ZmumuResonanceSelectionTool("MuonResonanceSelectionTool/ZmumuResonanceSelectionTool"),
//     m_ZmumuResonancePairingTool("MuonResonancePairingTool/ZmumuResonancePairingTool"),
//     m_JpsimumuResonanceSelectionTool("MuonResonanceSelectionTool/JpsimumuResonanceSelectionTool"),
//     m_JpsimumuResonancePairingTool("MuonResonancePairingTool/JpsimumuResonancePairingTool"),
//     m_isMC(false)
// {

//     m_pullCalculator = ToolHandle<Trk::IResidualPullCalculator>("Trk::ResidualPullCalculator/ResidualPullCalculator");

//     // inital components
//     declareProperty("Muons",                 m_MuonCollectionName);
//     declareProperty("MuonTriggerChainName",  m_MuonTriggerChainName);
//     declareProperty("TriggerDecisionTool",   m_trigDecTool);
//     declareProperty("Muon_Trigger_Items",    m_muon_triggers);
//     declareProperty("UseTriggerVector",      m_useTrigger);

//     // global cuts
//     declareProperty("GlobalPtCut_pp",        m_GlobalPtCut_pp       = 15.0 /*GeV*/);
//     declareProperty("GlobalPtCut_cosmic",    m_GlobalPtCut_cosmic   = 00.0 /*GeV*/);
//     declareProperty("GlobalPtCut_heavyIon",  m_GlobalPtCut_heavyIon = 05.0 /*GeV*/);
//     declareProperty("GlobalResidualError",   m_GlobalResidualError  = 01.0 /*mm*/ );

//     // tag and probe properties
//     declareProperty("diMuonMassWindow",      m_cut_diMuonMass = 0);
//     declareProperty("InDetTrackParticles",   m_InnerTrackContainer);
//     declareProperty("doZTrigMatch",          m_doZTrigMatch = true);
//     declareProperty("tagMuonTrigger",        m_tagMuonTrigger = std::string("EF_mu18_medium"));

//     // MCP
//     declareProperty("ZmumuResonanceSelectionTool",    m_ZmumuResonanceSelectionTool);    
//     declareProperty("ZmumuResonancePairingTool",      m_ZmumuResonancePairingTool);  
//     declareProperty("JpsimumuResonanceSelectionTool", m_JpsimumuResonanceSelectionTool);    
//     declareProperty("JpsimumuResonancePairingTool",   m_JpsimumuResonancePairingTool);  

//     // others
//     declareProperty("MuonHistSumTool",       m_muonHitSummaryTool = std::string("Muon::MuonHitSummaryTool/MuonHitSummaryTool"));
// }

// //---------------------------------------------------------------------------------------
// MuonTrkPhysMonitoring::~MuonTrkPhysMonitoring()
// {
//     ATH_MSG_INFO("Deleting MuonTrkPhysMonitoring");
// }

// //---------------------------------------------------------------------------------------
// StatusCode MuonTrkPhysMonitoring::initialize()
// {
//     ATH_MSG_DEBUG("MuonTrkPhysMonitoring::initialize() to setup tools/services");
//     if(ManagedMonitorToolBase::initialize().isFailure()) return StatusCode::FAILURE;
//     if(setupTools().isFailure())                         return StatusCode::FAILURE;
//     return StatusCode::SUCCESS;
// }

// //---------------------------------------------------------------------------------------
// StatusCode MuonTrkPhysMonitoring::bookHistograms()
// {
  
//   ATH_MSG_DEBUG("In bookHistograms()");

//   if(m_environment == AthenaMonManager::tier0    || 
//      m_environment == AthenaMonManager::tier0ESD || 
//      m_environment == AthenaMonManager::online   ||
//      m_environment == AthenaMonManager::AOD){

//     MgmtAttr_t attr = ATTRIB_MANAGED;

//     std::string muonMon_path = "Muon/MuonTrkPhysMonitoring";
    
//     if(newEventsBlock){}
//     if(newLumiBlock){}
//     if(newRun){

//         m_hNEvent = new TH1F(pathToHistName(muonMon_path+"_nEvent").c_str(), "Number of Events; LumiBlock; Nevents", 2000, -0.5, 1999.5);
        
//         MonGroup mongroup_overview(this, muonMon_path, run, attr);    
//         bookInMongroup(m_hNEvent, mongroup_overview).ignore(); 

//         m_oZSignalPlots.push_back(new MuMuSignalPlots(0, muonMon_path, "Z"));
//         MonGroup mongroup_Zsignal(this, muonMon_path, run, attr);
//         bookInMongroup(*m_oZSignalPlots[0], mongroup_Zsignal).ignore();

//         m_oJpsiSignalPlots.push_back(new MuMuSignalPlots(0, muonMon_path, "Jpsi"));
//         MonGroup mongroup_Jpsisignal(this, muonMon_path, run, attr);
//         bookInMongroup(*m_oJpsiSignalPlots[0], mongroup_Jpsisignal).ignore();

//     }
//   }
  
//   return StatusCode::SUCCESS;  
// }

// StatusCode MuonTrkPhysMonitoring::bookInMongroup(TH1* hist, MonGroup& mongroup)
// {
//     ATH_MSG_DEBUG ("Initializing " << hist << " " << hist->GetName() << "...");
//     return (mongroup.regHist(hist));
// }

// StatusCode MuonTrkPhysMonitoring::bookInMongroup(HistData& hist, MonGroup& mongroup)
// {
//     ATH_MSG_DEBUG ("Initializing " << hist.first << " " << hist.first->GetName() << " " << hist.second << "...");
//     ATH_CHECK(mongroup.regHist(hist.first));

//     return StatusCode::SUCCESS;
// }

// StatusCode MuonTrkPhysMonitoring::bookInMongroup(PlotBase& valPlots, MonGroup& mongroup)
// {
//     valPlots.initialize();
//     std::vector<HistData> hists = valPlots.retrieveBookedHistograms(); // HistData -> std::pair<TH1*, std::string> 
//     for (auto hist: hists){ 
//         bookInMongroup(hist, mongroup).ignore();
//     }

//     return StatusCode::SUCCESS;
// }

// //---------------------------------------------------------------------------------------
// StatusCode MuonTrkPhysMonitoring::fillHistograms()
// {
//     ATH_MSG_DEBUG("Inside fillHistograms()");
//     if(!(m_environment == AthenaMonManager::tier0    || 
//          m_environment == AthenaMonManager::tier0ESD || 
//          m_environment == AthenaMonManager::online   || 
//          m_environment == AthenaMonManager::AOD)) return StatusCode::SUCCESS;

//     const xAOD::EventInfo* eventInfo;
//     if (evtStore()->retrieve(eventInfo).isFailure()){
//         ATH_MSG_ERROR("Cannot access to event info");
//         return StatusCode::SUCCESS;
//     }

//     m_lumiblock = retrieveLumiBlock();
//     handleMuonContainer();
//     ++m_eventCounter;
//     m_hNEvent->Fill(eventInfo->lumiBlock(), 1);

//     return StatusCode::SUCCESS;
// }

// //---------------------------------------------------------------------------------------
// StatusCode MuonTrkPhysMonitoring::procHistograms()
// {
//     if(m_environment == AthenaMonManager::tier0    || 
//        m_environment == AthenaMonManager::tier0ESD || 
//        m_environment == AthenaMonManager::online   || 
//        m_environment == AthenaMonManager::AOD){

//         ATH_MSG_DEBUG("MuonTrkPhysMonitoring finalize()");

//         if(endOfLumiBlock){}

//         if(endOfRun){

//             m_oZSignalPlots[0]->finalize();
//             m_oJpsiSignalPlots[0]->finalize();

//             if(m_environment != AthenaMonManager::online){
//                 ATH_MSG_DEBUG("Processing Histograms for procHistograms Z Signal");
//                 for (auto plots : m_oZSignalPlots){ delete plots; }
//                 ATH_MSG_DEBUG("Processing Histograms for procHistograms Jpsi Signal");
//                 for (auto plots : m_oJpsiSignalPlots){ delete plots; }
//             }
//         }
//     }
//     return StatusCode::SUCCESS;

// }

// //---------------------------------------------------------------------------------------
// bool MuonTrkPhysMonitoring::handleMuonContainer(){

//     const xAOD::MuonContainer* MuContainer = 0;

//     if (evtStore()->retrieve(MuContainer, m_MuonCollectionName).isFailure()){
//         ATH_MSG_WARNING("Could not retrieve muon container: retrieval isFailure()");
//         return false;
//     }

//     if (MuContainer == 0){
//         ATH_MSG_WARNING("Could not retrieve muon container: MuContainer == 0");
//         return false;
//     }

//     ATH_MSG_DEBUG("Number of muons before cuts " << MuContainer->size() << ". Container:  " << m_MuonCollectionName);

//     if (MuContainer->empty()){
//         return true;
//     }


//     auto goodMuons   = m_ZmumuResonanceSelectionTool->selectMuons(MuContainer, m_isMC, CP::SystematicSet());
//     auto Zresonances = m_ZmumuResonancePairingTool->buildPairs(goodMuons);

//     auto goodMuons_Jpsi   = m_JpsimumuResonanceSelectionTool->selectMuons(MuContainer, m_isMC, CP::SystematicSet());
//     auto Jpsiresonances = m_JpsimumuResonancePairingTool->buildPairs(goodMuons_Jpsi);

//     fillHistograms_MuMuSignal(Zresonances, m_oZSignalPlots);
//     fillHistograms_MuMuSignal(Jpsiresonances, m_oJpsiSignalPlots);

//     return true;
// }

// //---------------------------------------------------------------------------------------
// StatusCode MuonTrkPhysMonitoring::finalize()
// {
//     ATH_CHECK(ManagedMonitorToolBase::finalize());

//     return StatusCode::SUCCESS;
// }

// //---------------------------------------------------------------------------------------
// StatusCode MuonTrkPhysMonitoring::setupTools()
// {
//     ATH_CHECK(    m_pullCalculator.retrieve());
//     ATH_CHECK(m_muonHitSummaryTool.retrieve());

//     ATH_CHECK(m_ZmumuResonanceSelectionTool.retrieve());
//     ATH_CHECK(m_ZmumuResonancePairingTool.retrieve());

//     ATH_CHECK(m_JpsimumuResonanceSelectionTool.retrieve());
//     ATH_CHECK(m_JpsimumuResonancePairingTool.retrieve());

//     ATH_CHECK_FATAL(  m_helperTool.retrieve());
//     ATH_CHECK_FATAL(m_idHelperTool.retrieve());

//     return StatusCode::SUCCESS;
// }

// //---------------------------------------------------------------------------------------
// int MuonTrkPhysMonitoring::retrieveLumiBlock(){

//   const xAOD::EventInfo* eventInfo = evtStore()->retrieve<const xAOD::EventInfo>("EventInfo");

//   if (!eventInfo){
//       ATH_MSG_ERROR("Could not retrieve EventInfo. LumiBlock set to -1");
//       return -1;
//   } 

//   return eventInfo->lumiBlock();
// }

// //---------------------------------------------------------------------------------------
// void MuonTrkPhysMonitoring::fillHistograms_MuMuSignal(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > mumucandidates, std::vector<MuMuSignalPlots*> plots){
    
//   for (auto mumu: mumucandidates){

//       // NB: ResonancePairingTool returns a pair of (plus, minus)-charged muons.
//       fillTagAndProbe(mumu.first, mumu.second, plots);
//       fillQuality(mumu.first, mumu.second, plots);
//       fillAuthor(mumu.first, mumu.second, plots);
      
//   }
// }

// //---------------------------------------------------------------------------------------
// void MuonTrkPhysMonitoring::fillTagAndProbe(const xAOD::Muon* mu_plus, const xAOD::Muon* mu_minus, std::vector<MuMuSignalPlots*> plots){

//     const float invariant_mass = (mu_plus->p4() + mu_minus->p4()).M();

//     plots[0]->fill(mu_plus->eta(), mu_minus->eta(), invariant_mass);
                             
// }

// //---------------------------------------------------------------------------------------
// void MuonTrkPhysMonitoring::fillQuality(const xAOD::Muon* mu_plus, const xAOD::Muon* mu_minus, std::vector<MuMuSignalPlots*> plots){

//     xAOD::Muon::Quality qual_plus  = mu_plus->quality();
//     xAOD::Muon::Quality qual_minus = mu_minus->quality();

//     plots[0]->fill(qual_plus, qual_minus);
                             
// }

// //---------------------------------------------------------------------------------------
// void MuonTrkPhysMonitoring::fillAuthor(const xAOD::Muon* mu_plus, const xAOD::Muon* mu_minus, std::vector<MuMuSignalPlots*> plots){

//     xAOD::Muon::Author auth_plus  = mu_plus->author();
//     xAOD::Muon::Author auth_minus = mu_minus->author();

//     plots[0]->fill(auth_plus, auth_minus);

// }
