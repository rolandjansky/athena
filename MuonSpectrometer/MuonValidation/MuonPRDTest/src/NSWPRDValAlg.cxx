/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>

// NSWValAlg inlcudes
#include "NSWPRDValAlg.h"

#include "MMDigitVariables.h"
#include "MMSimHitVariables.h"
#include "MMSDOVariables.h"
#include "MMRDOVariables.h"
#include "MMPRDVariables.h"

#include "sTGCDigitVariables.h"
#include "sTGCSimHitVariables.h"
#include "sTGCRDOVariables.h"
#include "sTGCSDOVariables.h"
#include "sTGCPRDVariables.h"

#include "CSCDigitVariables.h"

#include "MuEntryVariables.h"
#include "TruthVariables.h"

#include "EDM_object.h"

// Other NSW includes
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"

// ROOT includes
#include "TTree.h"

// Misc includes
#include "GaudiKernel/ITHistSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


NSWPRDValAlg::NSWPRDValAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_TruthVar(nullptr),
    m_MuEntryVar(nullptr),
    m_sTgcSimHitVar(nullptr),
    m_sTgcRdoVar(nullptr),
    m_sTgcSdoVar(nullptr),
    m_sTgcFastSdoVar(nullptr),
    m_sTgcDigitVar(nullptr),
    m_sTgcPrdVar(nullptr),
    m_MmSimHitVar(nullptr),
    m_MmSdoVar(nullptr),
    m_MmFastSdoVar(nullptr),
    m_MmDigitVar(nullptr),
    m_MmRdoVar(nullptr),
    m_MmPrdVar(nullptr),
    m_CscDigitVar(nullptr),
    m_thistSvc(nullptr),
    m_tree(nullptr),
    m_detManager(nullptr),
    m_MmIdHelper(nullptr),
    m_sTgcIdHelper(nullptr),
    m_CscIdHelper(nullptr),
    m_runNumber(0),
    m_eventNumber(0)
{
  // Input properties: Container names
  declareProperty("Truth_ContainerName",            m_Truth_ContainerName="TruthEvent");
  declareProperty("MuonEntryLayer_ContainerName",   m_MuEntry_ContainerName="MuonEntryLayer");
  declareProperty("NSWsTGC_ContainerName",          m_NSWsTGC_ContainerName="sTGCSensitiveDetector");
  declareProperty("NSWsTGC_DigitContainerName",     m_NSWsTGC_DigitContainerName="sTGC_DIGITS");
  declareProperty("NSWsTGC_SDOContainerName",       m_NSWsTGC_SDOContainerName="sTGC_SDO");
  declareProperty("NSWsTGC_RDOContainerName",       m_NSWsTGC_RDOContainerName="sTGCRDO");
  declareProperty("NSWsTGC_PRDContainerName",       m_NSWsTGC_PRDContainerName="STGC_Measurements");
  declareProperty("NSWMM_ContainerName",            m_NSWMM_ContainerName="MicromegasSensitiveDetector");
  declareProperty("NSWMM_DigitContainerName",       m_NSWMM_DigitContainerName="MM_DIGITS");
  declareProperty("NSWMM_SDOContainerName",         m_NSWMM_SDOContainerName="MM_SDO");
  declareProperty("NSWMM_RDOContainerName",         m_NSWMM_RDOContainerName="MMRDO");
  declareProperty("NSWMM_PRDContainerName",         m_NSWMM_PRDContainerName="MM_Measurements");
  declareProperty("CSC_DigitContainerName",         m_CSC_DigitContainerName="CSC_DIGITS");

  // Input properties: do EDM objects
  declareProperty("doTruth",         m_doTruth=false);
  declareProperty("doMuEntry",       m_doMuEntry=false);
  declareProperty("doSTGCHit",       m_doSTGCHit=false);
  declareProperty("doSTGCDigit",     m_doSTGCDigit=false);
  declareProperty("doSTGCFastDigit", m_doSTGCFastDigit=false);
  declareProperty("doSTGCRDO",       m_doSTGCRDO=false);
  declareProperty("doSTGCPRD",       m_doSTGCPRD=false);
  declareProperty("doMMHit",         m_doMMHit=false);
  declareProperty("doMMDigit",       m_doMMDigit=false);
  declareProperty("doMMFastDigit",   m_doMMFastDigit=false);
  declareProperty("doMMRDO",         m_doMMRDO=false);
  declareProperty("doMMPRD",         m_doMMPRD=false);
  declareProperty("doCSCDigit",      m_doCSCDigit=false);

  // Input properties: NSW Maching algorithm
  declareProperty("doNSWMatchingAlg",   m_doNSWMatching=true);
  declareProperty("doNSWMatchingMuonOnly",  m_doNSWMatchingMuon=false);
  declareProperty("setMaxStripDistance",  m_maxStripDiff=3);
}

StatusCode NSWPRDValAlg::initialize() {

  ATH_MSG_INFO("initialize()");

  ATH_CHECK( service("THistSvc", m_thistSvc) );

  m_tree = new TTree("NSWValTree", "Ntuple for NSW EDM Validation");
  m_tree->Branch("runNumber", &m_runNumber, "runNumber/i");
  m_tree->Branch("eventNumber", &m_eventNumber, "eventNumber/i");

  ATH_CHECK( m_thistSvc->regTree("/NSWPRDValAlg/NSWValTree", m_tree) );

  ATH_CHECK( detStore()->retrieve( m_detManager ) );

  ATH_CHECK( detStore()->retrieve( m_MmIdHelper ) );

  ATH_CHECK( detStore()->retrieve( m_sTgcIdHelper ) );

  ATH_CHECK( detStore()->retrieve( m_CscIdHelper ) );

  if (m_doTruth){
     m_TruthVar = new TruthVariables(&(*(evtStore())), m_detManager,
                                                m_tree, m_Truth_ContainerName, msgLevel());
     ATH_CHECK( m_TruthVar->initializeVariables() );
  }

  if (m_doMuEntry){
     m_MuEntryVar = new MuEntryVariables(&(*(evtStore())), m_detManager,
                                                m_tree, m_MuEntry_ContainerName, msgLevel());
     ATH_CHECK( m_MuEntryVar->initializeVariables() );
  }

  if (m_doSTGCHit){
     m_sTgcSimHitVar = new sTGCSimHitVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_ContainerName, msgLevel());
     ATH_CHECK( m_sTgcSimHitVar->initializeVariables() );
  }
  
  if (m_doSTGCDigit){
     m_sTgcDigitVar = new sTGCDigitVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_DigitContainerName, msgLevel());
     ATH_CHECK( m_sTgcDigitVar->initializeVariables() );

  	  // Take SDO conainer
     m_sTgcSdoVar = new sTGCSDOVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_SDOContainerName, msgLevel());
     ATH_CHECK( m_sTgcSdoVar->initializeVariables() );
  }

  if (m_doSTGCFastDigit){
  	  // Take the "fast_SDO" instead of the SDOs from full sim
     m_sTgcFastSdoVar = new sTGCSDOVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, "sTGCfast_SDO", msgLevel());
     ATH_CHECK( m_sTgcFastSdoVar->initializeVariables() );

     // Fast digits = PRD
     m_doSTGCPRD = true;
  }

  if (m_doSTGCRDO){
     m_sTgcRdoVar = new sTGCRDOVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_RDOContainerName, msgLevel());
     ATH_CHECK( m_sTgcRdoVar->initializeVariables() );
  }

  if (m_doSTGCPRD){
     m_sTgcPrdVar = new sTGCPRDVariables(&(*(evtStore())), m_detManager,
                                                m_sTgcIdHelper, m_tree, m_NSWsTGC_PRDContainerName, msgLevel());
     ATH_CHECK( m_sTgcPrdVar->initializeVariables() );
  }

  if (m_doMMHit) {
     m_MmSimHitVar = new MMSimHitVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, m_NSWMM_ContainerName, msgLevel());
     ATH_CHECK( m_MmSimHitVar->initializeVariables() );
  }

  if (m_doMMDigit) {
     m_MmDigitVar = new MMDigitVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, m_NSWMM_DigitContainerName, msgLevel());
     ATH_CHECK( m_MmDigitVar->initializeVariables() );

     // Take SDO conainer
     m_MmSdoVar = new MMSDOVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, m_NSWMM_SDOContainerName, msgLevel());
     ATH_CHECK( m_MmSdoVar->initializeVariables() );
  }

  if (m_doMMFastDigit){
  	  // Take the "fast_SDO" instead of the SDOs from full sim
     m_MmFastSdoVar = new MMSDOVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, "MMfast_SDO", msgLevel());
     ATH_CHECK( m_MmFastSdoVar->initializeVariables() );

     // Fast digits = PRD
     m_doMMPRD = true;
  }

  if (m_doMMRDO) {

    m_MmRdoVar = new MMRDOVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, m_NSWMM_RDOContainerName, msgLevel());
    ATH_CHECK( m_MmRdoVar->initializeVariables() );
  }

 if (m_doMMPRD){
     m_MmPrdVar = new MMPRDVariables(&(*(evtStore())), m_detManager,
                                                m_MmIdHelper, m_tree, m_NSWMM_PRDContainerName, msgLevel());
     ATH_CHECK( m_MmPrdVar->initializeVariables() );
  }

  if (m_doCSCDigit){
     m_CscDigitVar = new CSCDigitVariables(&(*(evtStore())), m_detManager,
                                                m_CscIdHelper, m_tree, m_CSC_DigitContainerName, msgLevel());
     ATH_CHECK( m_CscDigitVar->initializeVariables() );
  }

  return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::finalize()
{
  ATH_MSG_DEBUG("NSWPRDValAlg:: Finalize + Matching");
  if (m_doNSWMatching) { ATH_CHECK ( NSWMatchingAlg() );} 

  if (m_TruthVar) { delete m_TruthVar; m_TruthVar=0;}
  if (m_doMuEntry) { delete m_MuEntryVar; m_MuEntryVar=0;}
  if (m_sTgcSimHitVar) { delete m_sTgcSimHitVar; m_sTgcSimHitVar=0;}
  if (m_sTgcRdoVar) { delete m_sTgcRdoVar; m_sTgcRdoVar=0;}
  if (m_sTgcSdoVar) { delete m_sTgcSdoVar; m_sTgcSdoVar=0;}
  if (m_sTgcFastSdoVar) { delete m_sTgcSdoVar; m_sTgcSdoVar=0;}
  if (m_sTgcDigitVar) { delete m_sTgcDigitVar; m_sTgcDigitVar=0;}
  if (m_sTgcPrdVar) { delete m_sTgcPrdVar; m_sTgcPrdVar=0;}
  if (m_MmSimHitVar) { delete m_MmSimHitVar; m_MmSimHitVar=0;}
  if (m_MmSdoVar) { delete m_MmSdoVar; m_MmSdoVar=0;}
  if (m_MmFastSdoVar) { delete m_MmSdoVar; m_MmSdoVar=0;}
  if (m_MmDigitVar) { delete m_MmDigitVar; m_MmDigitVar=0;}
  if (m_MmRdoVar) { delete m_MmRdoVar; m_MmRdoVar=0;}
  if (m_MmPrdVar) { delete m_MmPrdVar; m_MmPrdVar=0;}
  if (m_CscDigitVar) { delete m_CscDigitVar; m_CscDigitVar=0;}

  return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::execute()
{
  ATH_MSG_DEBUG("execute()");

  // Event information
  const EventInfo* pevt(0);
  if( evtStore()->retrieve(pevt).isSuccess() ) {
    m_runNumber = pevt->event_ID()->run_number();
    m_eventNumber = pevt->event_ID()->event_number();
    ATH_MSG_DEBUG("Now processing event number:" << m_eventNumber << ", run number:" << m_runNumber);
  } else {
    ATH_MSG_WARNING("Could not retrieve event info!");
    m_runNumber = -1;
    m_eventNumber = -1;
  }

  if (m_doTruth) ATH_CHECK( m_TruthVar->fillVariables() );

  if (m_doMuEntry) ATH_CHECK( m_MuEntryVar->fillVariables() );

  if (m_doSTGCHit) ATH_CHECK( m_sTgcSimHitVar->fillVariables() );

  if (m_doSTGCDigit) { ATH_CHECK( m_sTgcDigitVar->fillVariables() ); ATH_CHECK( m_sTgcSdoVar->fillVariables() ); }

  if (m_doSTGCFastDigit) ATH_CHECK( m_sTgcFastSdoVar->fillVariables() );

  if (m_doSTGCRDO) ATH_CHECK( m_sTgcRdoVar->fillVariables() );

  if (m_doSTGCPRD) ATH_CHECK( m_sTgcPrdVar->fillVariables() );

  if (m_doMMHit) ATH_CHECK( m_MmSimHitVar->fillVariables() );

  if (m_doMMDigit) { ATH_CHECK( m_MmDigitVar->fillVariables() ); ATH_CHECK( m_MmSdoVar->fillVariables() ); }

  if (m_doMMFastDigit) ATH_CHECK( m_MmFastSdoVar->fillVariables() );

  if (m_doMMRDO) ATH_CHECK( m_MmRdoVar->fillVariables() );

  if (m_doMMPRD) ATH_CHECK( m_MmPrdVar->fillVariables() );

  if (m_doCSCDigit) ATH_CHECK( m_CscDigitVar->fillVariables() );

  m_tree->Fill();

  return StatusCode::SUCCESS;
}

/*****************************************************************************************************************************************
The rest of this file is the NSW matching algorithm. This can be (de)activated with the input variable doNSWMatchingAlg. 
The matching algorithm will check each conversion in the Hit->PRD digitization chain.
For each conversion, the algorithm will try to match the two objects. 
The input variable setMaxStripDistance (default value = 3) give the maximum strip distance two objects can have, while still be considered a match
The input variable doNSWMatchingMuonOnly can be set to true to only use events which contain exclusively muons 
******************************************************************************************************************************************/

// First set up which object should be matched, given the input used to fill the NSW Ntuple
StatusCode NSWPRDValAlg::NSWMatchingAlg () {

  ATH_MSG_DEBUG("NSWMatchingAlg: building Data objects");
  
  // Use the EDM object, as defined in EDM_object.h, to build the hits, digits, SDO, RDO and PRD for both sTGC and MM
  EDM_object Hits_sTGC, Digits_sTGC, SDO_sTGC, RDO_sTGC, PRD_sTGC;
  EDM_object Hits_MM, Digits_MM, SDO_MM, RDO_MM, PRD_MM;

  Hits_sTGC.setName("Hits", "sTGC");
  Digits_sTGC.setName("Digits", "sTGC");
  SDO_sTGC.setName("SDO", "sTGC");
  RDO_sTGC.setName("RDO", "sTGC");
  PRD_sTGC.setName("PRD", "sTGC");

  Hits_MM.setName("Hits", "MM");
  Digits_MM.setName("Digits", "MM");
  SDO_MM.setName("SDO", "MM");
  RDO_MM.setName("RDO", "MM");
  PRD_MM.setName("PRD", "MM");

  // Match the EDM objects with the variables in the NSW Validation Ntuple
  TString branch_name;
  TObjArray* brlst = m_tree->GetListOfBranches();
  for (TBranch *branch = (TBranch*) brlst->First(); branch; branch = (TBranch*) brlst->After(branch)) {
    branch_name = branch->GetName(); 
    ATH_MSG_VERBOSE("About to check branch: " << branch_name);

    ATH_CHECK( setDataAdress(Hits_sTGC, branch_name) ); 
    ATH_CHECK( setDataAdress(Digits_sTGC, branch_name) ); 
    ATH_CHECK( setDataAdress(SDO_sTGC, branch_name) ); 
    ATH_CHECK( setDataAdress(RDO_sTGC, branch_name) ); 
    ATH_CHECK( setDataAdress(PRD_sTGC, branch_name) ); 

    ATH_CHECK( setDataAdress(Hits_MM, branch_name) ); 
    ATH_CHECK( setDataAdress(Digits_MM, branch_name) ); 
    ATH_CHECK( setDataAdress(SDO_MM, branch_name) ); 
    ATH_CHECK( setDataAdress(RDO_MM, branch_name) ); 
    ATH_CHECK( setDataAdress(PRD_MM, branch_name) ); 
  }

  // Prepare the output file
  std::ofstream efficiencies;
  efficiencies.open("NSWMatchingAlg_efficiencies.txt");
  efficiencies << "NSW Matching algorithm, efficiencies of conversion from and to various EDM objects" << std::endl;
  efficiencies << "Settings:\n" << m_doNSWMatchingMuon << std::endl;
  efficiencies << " 'Maximum Strip Distance':" << m_maxStripDiff << std::endl;
  efficiencies.close();

  // sTGC matching
  if (m_doSTGCHit && m_doSTGCDigit) { 
    ATH_CHECK( NSWMatchingAlg(Hits_sTGC, Digits_sTGC) ); 
    ATH_CHECK( NSWMatchingAlg(Hits_sTGC, SDO_sTGC) );
  }
  if (m_doSTGCDigit && m_doSTGCRDO) { 
    ATH_CHECK( NSWMatchingAlg(Digits_sTGC, RDO_sTGC) );
    ATH_CHECK( NSWMatchingAlg(SDO_sTGC, RDO_sTGC) );
  }
  if (m_doSTGCRDO && m_doSTGCPRD) { ATH_CHECK( NSWMatchingAlg(RDO_sTGC, PRD_sTGC) ); }

  // sTGC fast digitization
  if (m_doSTGCHit && m_doSTGCFastDigit) { 
    ATH_CHECK( NSWMatchingAlg(Hits_sTGC, PRD_sTGC) );
    ATH_CHECK( NSWMatchingAlg(Hits_sTGC, SDO_sTGC) );
    ATH_CHECK( NSWMatchingAlg(SDO_sTGC, PRD_sTGC) );
  }

  // MM matching
  if (m_doMMHit && m_doMMDigit) { 
    ATH_CHECK( NSWMatchingAlg(Hits_MM, Digits_MM) );
    ATH_CHECK( NSWMatchingAlg(Hits_MM, SDO_MM) );
  }
  if (m_doMMDigit && m_doMMRDO) { 
    ATH_CHECK( NSWMatchingAlg(Digits_MM, RDO_MM) );
    ATH_CHECK( NSWMatchingAlg(SDO_MM, RDO_MM) );
  }
  if (m_doMMRDO && m_doMMPRD) { ATH_CHECK( NSWMatchingAlg(RDO_MM, PRD_MM) ); }

  //  MM fast digitization
  if (m_doMMHit && m_doMMFastDigit) { 
    ATH_CHECK( NSWMatchingAlg(Hits_MM, PRD_MM) );
    ATH_CHECK( NSWMatchingAlg(Hits_MM, SDO_MM) );
    ATH_CHECK( NSWMatchingAlg(SDO_MM, PRD_MM) );
  }

  return StatusCode::SUCCESS;
}

// This part of the matching algortihm does the actual comparison given two EDM obects
StatusCode NSWPRDValAlg::NSWMatchingAlg (EDM_object data0, EDM_object data1) {
  if (data0.getDetector() != data1.getDetector()) { ATH_MSG_ERROR ("Matching " << data0.getDetector() << " data with " << data1.getDetector() << " data. This is not implemented in this algorithm"); }
  
  ATH_MSG_INFO("NSWMatchingAlg: Start matching " << data0.getName() << " and " << data1.getName() << " for " << data0.getDetector());
  data0.setMatchedwith(data1.getName());
  data1.setMatchedwith(data0.getName());

  // Prepare Muon only check
  std::vector<int>* TruthParticle_Pdg;
  if ( m_doNSWMatchingMuon ) { m_tree->SetBranchAddress("TruthParticle_Pdg", &TruthParticle_Pdg); }

  Long64_t nEntries = m_tree->GetEntriesFast();
  for (Long64_t i_entry = 0; i_entry < nEntries; ++i_entry) {
    // event numbering starts at 1
    ATH_MSG_DEBUG("Now checking event number " << i_entry + 1);
    m_tree->GetEntry(i_entry);
    
    if (data0.empty()) { ATH_MSG_WARNING("No " << data0.getDetector() << data0.getName() << " found in event " << i_entry + 1); continue; }
    if (data1.empty()) { ATH_MSG_WARNING("No " << data1.getDetector() << data1.getName() << " found in event " << i_entry + 1); continue; }

    ATH_MSG_DEBUG("Number of " << data0.getDetector() << data0.getName() << ": " << data0.size() << 
                  ", number of " << data1.getDetector() << data1.getName() << ": " << data1.size());

  //only muon events:
  if (m_doNSWMatchingMuon) {
    bool allMu = true;
    int mu_pdg = 13;
    for ( int pdg : *TruthParticle_Pdg ) { allMu &= (abs(pdg) == mu_pdg); }
    if ( !allMu ) { ATH_MSG_VERBOSE("Skipping event, because doNSWMatchingMuonOnly is true and there non muon particles"); continue; }
  }

  // Reset variables to have a clean sheet
    data0.init_matching();
    data1.init_matching();

    // Actual Matching
    for (uint i = 0; i < data0.size(); ++i)
     {
      int nMatch = 0;
        for (uint j = 0; j < data1.size(); ++j)
        {
           if ( data0.identifierMatch(data0, data1, i, j) )
           {
              nMatch++;
              data0.update_match(i, data1.m_channel->at(j));
              data1.update_match(j, data0.m_channel->at(i));
           }
        }
        ATH_MSG_VERBOSE("Total Number of matches found: " << nMatch << " " << data1.getName() << " for a single " << data0.getName() );
        if (nMatch == 0) {
          ATH_MSG_WARNING("No match found!");
        }
     }
    if (msgLevel() <= MSG::DEBUG) { 
      ATH_MSG_DEBUG("Full info for " << data0.getName() << data0.getDetector());
      data0.printInfo(); 
      ATH_MSG_DEBUG("Full info for " << data1.getName() << data1.getDetector());
      data1.printInfo(); 
    }
    
    data0.update_efficiency(m_maxStripDiff);
    data1.update_efficiency(m_maxStripDiff);

  // The data0 vs data1 matching will now be overwritten
  data0.clearVars();
  data1.clearVars();
  }

  // Write result to file
  std::ofstream efficiencies; 
  efficiencies.open("NSWMatchingAlg_efficiencies.txt", std::ofstream::app);
  data0.printEfficiency(efficiencies);
  data1.printEfficiency(efficiencies);
  efficiencies.close();

  return StatusCode::SUCCESS;

}

// This function couples the branch of the NSW validation Ntuple with the EDM object. 
// In case the brach does not exist, or is completely empty, the object will remain empty and the code will skip the matching entirly, giving a ATH_WARNING in the process
StatusCode NSWPRDValAlg::setDataAdress (EDM_object &oData, TString branch_name) {
  bool setBranch = false;
  if (!(branch_name.Contains(oData.getName()) && branch_name.Contains(oData.getDetector()))) { return StatusCode::SUCCESS; }
  // For sim hits select the offline identifiers, rather than the sim identifiers
  if (branch_name.Contains("_sim_")) { return StatusCode::SUCCESS; }
  if (branch_name.EndsWith("stationName")) { m_tree->SetBranchAddress(branch_name, &oData.m_stationName); setBranch = true; }
  if (branch_name.EndsWith("stationEta")) { m_tree->SetBranchAddress(branch_name, &oData.m_stationEta); setBranch = true; }
  if (branch_name.EndsWith("stationPhi")) { m_tree->SetBranchAddress(branch_name, &oData.m_stationPhi); setBranch = true; }
  if (branch_name.EndsWith("multiplet")) { m_tree->SetBranchAddress(branch_name, &oData.m_multiplet); setBranch = true; }
  if (branch_name.EndsWith("gas_gap")) { m_tree->SetBranchAddress(branch_name, &oData.m_gas_gap); setBranch = true; }
  if (branch_name.EndsWith("channel")) { m_tree->SetBranchAddress(branch_name, &oData.m_channel); setBranch = true; }
  if (branch_name.EndsWith("channel_type")) { m_tree->SetBranchAddress(branch_name, &oData.m_channel_type); setBranch = true; }
  if (setBranch) { ATH_MSG_DEBUG("Set data adress of branch " << branch_name); }
  
  return StatusCode::SUCCESS;
}
