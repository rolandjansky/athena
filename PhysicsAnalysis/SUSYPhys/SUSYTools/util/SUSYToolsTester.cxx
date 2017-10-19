/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SUSYToolsTester_opt.cxx 696994 2015-09-26 20:40:26Z khoo $

// System include(s):
#include <memory>
#include <cstdlib>
#include <string>
#include <iostream>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TStopwatch.h>
#include <TSystem.h>
#include "TObjArray.h"
#include "TObjString.h"

// Infrastructure include(s):
// #ifdef ROOTCORE
// #   include "xAODRootAccess/Init.h"
// #   include "xAODRootAccess/TEvent.h"
// #endif // ROOTCORE

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TStore.h"
#ifdef ROOTCORE
#include "xAODRootAccess/TEvent.h"
#else
#include "POOLRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTruth/xAODTruthHelpers.h"
#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"

// Local include(s):
#include "SUSYTools/SUSYObjDef_xAOD.h"
#include "SUSYTools/SUSYCrossSection.h"

// Other includes
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PathResolver/PathResolver.h"
#include "AsgTools/AnaToolHandle.h"

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

#include "TrigDecisionTool/ChainGroup.h"

const size_t Ncuts = 10;
const char *cut_name[] =
{ "All",
  "GRL+Cleaning",
  "TileTrip",
  "Trigger",
  "Cosmic veto",
  "==1 baseline lepton",
  "==1 signal lepton",
  "lepton pT>20 GeV",
  "trig. matched lepton",
  "Njet(pT>50 GeV)>=2"
};

const unsigned int nSel=8;
enum sel {
  signallep,
  baseline,
  bad,
  cosmic,
  goodpt,
  btagged,
  trgmatch,
  passOR
};

std::vector<std::string> getTokens(TString line, TString delim){
  std::vector<std::string> vtokens;
  TObjArray* tokens = TString(line).Tokenize(delim); //delimiters                                                                                                                             
  if(tokens->GetEntriesFast()) {
    TIter iString(tokens);
    TObjString* os=0;
    while ((os=(TObjString*)iString())) {
      vtokens.push_back( os->GetString().Data() );
    }
  }
  delete tokens;

  return vtokens;
}

int main( int argc, char* argv[] ) {

  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int);

  //StatusCode::enableFailure();
  CP::SystematicCode::enableFailure();
  CP::CorrectionCode::enableFailure();

  // The application's name:
  const char* APP_NAME = argv[ 0 ];

  // Check if we received a file name:
  if ( argc < 2 ) {
    Error( APP_NAME, "No file name received!" );
    Error( APP_NAME, "  Usage: %s [xAOD file name] [maxEvents] [isData=0/1 isAtlfast=0/1] [NoSyst] [Debug] [ConfigFile=<cfile.conf>] [PRWFile=<prwfile.root>] [doRTT] [SUSYx]", APP_NAME );
    return 1;
  }


  /// READ CONFIG ------------

  int isData = -1;
  int isAtlfast = -1;
  int NoSyst = 1;
  int debug = 1;
  int JESNPset = -99;

  int doRTT=0; //if running on RTT DxAODs
  int SUSYx=1; //SUSY DxAOD flavour
  Long64_t entries=-1;

  std::string config_file = "SUSYTools/SUSYTools_Default.conf";
  std::string prw_file = "DUMMY";
  std::string ilumicalc_file = "DUMMY";

  for (int i = 1 ; i < argc ; i++) {
    const char* key = strtok(argv[i], "=") ;
    const char* val = strtok(0, " ") ;

    Info( APP_NAME,  "processing key %s  with value %s", key, val );

    if (strcmp(key, "isData") == 0) isData = atoi(val);
    if (strcmp(key, "isAtlfast") == 0) isAtlfast = atoi(val);
    if (strcmp(key, "NoSyst") == 0) NoSyst = atoi(val);
    if (strcmp(key, "Debug") == 0) debug = atoi(val);
    if (strcmp(key, "ConfigFile") == 0) config_file = std::string(val);
    if (strcmp(key, "PRWFile") == 0) prw_file = std::string(val);
    if (strcmp(key, "ilumicalcFile") == 0) ilumicalc_file = std::string(val);
    if (strcmp(key, "JESNPset") == 0) JESNPset = atoi(val);
    if (strcmp(key, "doRTT") == 0) doRTT = atoi(val);
    if (strcmp(key, "SUSYx") == 0) SUSYx = atoi(val);
    if (strcmp(key, "maxEvents") == 0) entries = atoi(val);

  }

  if (isData < 0 || isAtlfast < 0) {
    Error( APP_NAME, "One of the flags isData or isAtlfast was not set! Must provide isData or isAtlfast." );
    Error( APP_NAME, "  Usage: %s [xAOD file name] [maxEvents] [isData=0/1 isAtlfast=0/1] [NoSyst] [Debug]", APP_NAME );
    return 10;
  }
  ST::ISUSYObjDef_xAODTool::DataSource datasource = (isData ? ST::ISUSYObjDef_xAODTool::Data : (isAtlfast ? ST::ISUSYObjDef_xAODTool::AtlfastII : ST::ISUSYObjDef_xAODTool::FullSim));
 
  ///
  static SG::AuxElement::Accessor<int> acc_susyid("SUSY_procID");

  // Initialise the application:
  //ANA_CHECK( xAOD::Init( APP_NAME ) );  //NOT WORKING? //MT,WB

  // Open the input file:
  TString fileName;
  if(!doRTT){
    fileName = argv[1];
  }
  else{
    //get release
    TString relS = std::getenv("AtlasVersion");
    if(relS.IsNull()){
      Error( APP_NAME, "Not able to find a release config! Note that RTT mode is only available in CMT at the moment!");
      return 10;
    }
    relS.ReplaceAll("rel_","");
    int relN = atoi(relS)-1;
    if(relN<0) relN=6;
    //get fileName from RTT output
    fileName = Form("/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_%d/20.7.X.Y-VAL/build/x86_64-slc6-gcc49-opt/AtlasDerivation/DerivationFrameworkRTT/%s15SUSY%dMerge/t\
est.pool.root",relN,(isData?"Data":"MC"),SUSYx);
  }
  Info( APP_NAME, "Opening file: %s", fileName.Data() );
  std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  ANA_CHECK( ifile.get() );

  // Create a TEvent object:


#ifdef ROOTCORE
  xAOD::TEvent event( xAOD::TEvent::kAthenaAccess );
#else
  POOL::TEvent event( POOL::TEvent::kAthenaAccess );
#endif

  //xAOD::TEvent event( xAOD::TEvent::kBranchAccess );
  //xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  ANA_CHECK( event.readFrom( ifile.get() ) );
  Info( APP_NAME, "Number of events in the file: %i",
        static_cast< int >( event.getEntries() ) );

  TString xStream=""; //identify SUSY DxAOD flavour   (inside entry loop now /MT)

  xAOD::TStore store;

  // Open up an output file.  Not needed for this example.  If you wish to enable
  // this, please don't forget to add the call to fill towards the end of the code!
  //std::auto_ptr< TFile > ofile( TFile::Open( "out.root", "RECREATE" ) );
  //ANA_CHECK( event.writeTo( ofile.get() ) );

  // If we haven't set the number of events, then run over the whole tree
  if (entries<0){
    entries = event.getEntries();
  }

  // GRL tool
  asg::AnaToolHandle<IGoodRunsListSelectionTool> m_grl;
  if (isData) {
    m_grl.setTypeAndName("GoodRunsListSelectionTool/grl");
    m_grl.isUserConfigured();
    std::vector<std::string> myGRLs;
    myGRLs.push_back(PathResolverFindCalibFile("GoodRunsLists/data15_13TeV/20160720/physics_25ns_20.7.xml"));
    myGRLs.push_back(PathResolverFindCalibFile("GoodRunsLists/data16_13TeV/20161101/physics_25ns_20.7.xml"));

    ANA_CHECK( m_grl.setProperty("GoodRunsListVec", myGRLs) );
    ANA_CHECK( m_grl.setProperty("PassThrough", false) );
    ANA_CHECK( m_grl.retrieve() );

    Info( APP_NAME, "GRL tool retrieve & initialized... " );
  }

  //xsec DB
  SUSY::CrossSectionDB *my_XsecDB(0);
  if (!isData) {
    //my_XsecDB = new SUSY::CrossSectionDB(gSystem->ExpandPathName("$ROOTCOREBIN/data/SUSYTools/mc15_13TeV/"));
    my_XsecDB = new SUSY::CrossSectionDB(PathResolverFindCalibDirectory("SUSYTools/mc15_13TeV/"));
    //my_XsecDB = new SUSY::CrossSectionDB("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/mc15_13TeV/");
    
    Info(APP_NAME, "xsec DB initialized" );
        
    // In case you want to overwrite the default with your local values 
    // my_XsecDB->loadFile( <PATH_TO_YOUR_XSECTION_FILE> );
  }

  // Create the tool(s) to test:
  ST::SUSYObjDef_xAOD objTool("SUSYObjDef_xAOD");

  std::cout << " ABOUT TO INITIALIZE SUSYTOOLS " << std::endl;

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Configure the SUSYObjDef instance
  ANA_CHECK( objTool.setProperty("DataSource", datasource) ) ;
  if(!config_file.empty())
    ANA_CHECK( objTool.setProperty("ConfigFile", config_file) );

  ANA_CHECK( objTool.setBoolProperty("METDoTrkSyst", true) );
  ANA_CHECK( objTool.setBoolProperty("METDoCaloSyst", false) );

  ///////////////////////////////////////////////////////////////////////////////////////////
  ////                                            
  ////       ****     THESE FILES ARE MEANT FOR EXAMPLES OF USAGE ONLY        ****       ////
  ////       ****        AND SHOULD NOT BE USED FOR SERIOUS ANALYSIS          ****       ////
  ////                                                                                   ////
  ///////////////////////////////////////////////////////////////////////////////////////////
  std::vector<std::string> prw_conf;
  if (prw_file == "DUMMY") {
    prw_conf.push_back("dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root");
    prw_conf.push_back("dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root");
  }
  else {
    prw_conf = getTokens(prw_file,",");
    //    prw_conf.push_back(prw_file);
  }
  ANA_CHECK( objTool.setProperty("PRWConfigFiles", prw_conf) );

  std::vector<std::string> prw_lumicalc;
  if (ilumicalc_file == "DUMMY") {
    prw_lumicalc.push_back(PathResolverFindCalibFile("GoodRunsLists/data15_13TeV/20160720/physics_25ns_20.7.lumicalc.OflLumi-13TeV-005.root"));
    prw_lumicalc.push_back(PathResolverFindCalibFile("GoodRunsLists/data16_13TeV/20160720/physics_25ns_20.7.lumicalc.OflLumi-13TeV-005.root"));
  }
  else {
    prw_lumicalc = getTokens(ilumicalc_file,",");
  }
  ANA_CHECK( objTool.setProperty("PRWLumiCalcFiles", prw_lumicalc) );
  ///////////////////////////////////////////////////////////////////////////////////////////

  ANA_CHECK(objTool.setProperty("JESNuisanceParameterSet", JESNPset) );

  //ANA_CHECK(objTool.setBoolProperty("UseBtagging", false) );

  //Guess shower type for btagging MC/MC SFs 
  if(!isData){
    //unsigned int ishower = objTool.getMCShowerType("mc15_13TeV.Sherpa_blabla"); //
    int ishower = objTool.getMCShowerType("mc15_13TeV.PowhegPythiaEvtGen_blabla"); //get your sample name here!
    ANA_CHECK( objTool.setProperty("ShowerType", (int)ishower) );
  }

  //// Don't touch these unless you know what you are after!!! FOR EFF TESTS ONLY
  // ANA_CHECK( objTool.setBoolProperty("EleForceNoId", false) ); 
  // ANA_CHECK( objTool.setBoolProperty("MuForceNoId", false) );
  ////

  if (debug) objTool.msg().setLevel( MSG::VERBOSE );
  ANA_CHECK(objTool.setBoolProperty("DebugMode", (bool)debug) );

  if ( objTool.initialize() != StatusCode::SUCCESS) {
    Error( APP_NAME, "Cannot initialize SUSYObjDef_xAOD..." );
    Error( APP_NAME, "Exiting... " );
    exit(-1);
  } else {
    Info( APP_NAME, "SUSYObjDef_xAOD initialized... " );
  }

  std::cout << " INITIALIZED SUSYTOOLS " << std::endl;


  /// SETUP TRIGGERS TO BE CHECKED
  std::vector<std::string> el_triggers {"HLT_e24_lhmedium_L1EM18VH","HLT_e60_lhmedium","HLT_e120_lhloose"};
  //std::vector<std::string> el_triggers {"HLT_e24_lhmedium_L1EM20VH","HLT_e60_lhmedium","HLT_e120_lhloose"};
  std::vector<std::string> mu_triggers {"HLT_mu20_iloose_L1MU15","HLT_mu50","HLT_mu18","HLT_mu8noL1","HLT_mu18_mu8noL1"};
  std::vector<std::string> ph_triggers {"HLT_g120_loose"};
  std::vector<std::string> tau_triggers {"HLT_tau25_medium1_tracktwo", "HLT_tau35_medium1_tracktwo"};


  // Counter for cuts:
  std::vector<ST::SystInfo> systInfoList;
  if (NoSyst) {
    ST::SystInfo infodef;
    infodef.affectsKinematics = false;
    infodef.affectsWeights = false;
    infodef.affectsType = ST::Unknown;
    systInfoList.push_back(infodef);
  } else {
    systInfoList = objTool.getSystInfoList();
  }

  size_t isys = 0;
  const size_t Nsyst = systInfoList.size();
  std::vector<std::vector<int> > elcuts;
  std::vector<std::vector<int> > mucuts;
  for (isys = 0; isys < Nsyst; ++isys) {
    std::vector<int> elcutsCurrentSyst;
    std::vector<int> mucutsCurrentSyst;
    for (size_t icut = 0; icut < Ncuts; ++icut) {
      elcutsCurrentSyst.push_back(0);
      mucutsCurrentSyst.push_back(0);
    }
    elcuts.push_back(elcutsCurrentSyst);
    mucuts.push_back(elcutsCurrentSyst);
  }

  if (objTool.resetSystematics() != CP::SystematicCode::Ok) {
    Error(APP_NAME, "Cannot reset SUSYTools systematics" );
    exit(-2);
  }

  TStopwatch m_clock0;
  m_clock0.Start();
  TStopwatch m_clock1;
  m_clock1.Start();
  TStopwatch m_clock2; 

  int period = debug ? 1 : 100;

  // Loop over the events:
  for ( Long64_t entry = 0; entry < entries; ++entry ) {

    if (entry == 1) {
      m_clock1.Stop();
      m_clock2.Start();
      // CALLGRIND_TOGGLE_COLLECT;
      // ProfilerStart("gperftools.profile.out");
    }
  

    // Tell the object which entry to look at:
    event.getEntry( entry );

    if(entry == 0){
      //** Moved inside here to please POOL:: access in CMT  //MT   (need to read the first event for the metadata to be exposed (at the moment))

      // Read the CutBookkeeper container
      const xAOD::CutBookkeeperContainer* completeCBC = 0;
      if (!event.retrieveMetaInput(completeCBC, "CutBookkeepers").isSuccess()) {
        Error( APP_NAME, "Failed to retrieve CutBookkeepers from MetaData! Exiting.");
      }
      
      // Let's find the right CBK (latest with StreamAOD input before derivations)
      const xAOD::CutBookkeeper* allEventsCBK = 0;
      int maxcycle=-1;
      for ( auto cbk : *completeCBC ) {
        std::cout << cbk->nameIdentifier() << " : " << cbk->name() << " : desc = " << cbk->description() 
                  << " : inputStream = " << cbk->inputStream()  << " : outputStreams = " << (cbk->outputStreams().size() ? cbk->outputStreams()[0] : "") 
                  << " : cycle = " << cbk->cycle() << " :  allEvents = " << cbk->nAcceptedEvents() 
                  << std::endl;
        
        if ( cbk->name() == "AllExecutedEvents" && TString(cbk->inputStream()).Contains("StreamDAOD")){ //guess DxAOD flavour
          xStream = TString(cbk->inputStream()).ReplaceAll("Stream","");
          std::cout << "xStream = " << xStream << "  (i.e. indentified DxAOD flavour)" << std::endl;
        }
        if ( cbk->name() == "AllExecutedEvents" && cbk->inputStream() == "StreamAOD" && cbk->cycle() > maxcycle){
          maxcycle = cbk->cycle();
          allEventsCBK = cbk;
        }
      }
      
      if (allEventsCBK) {
        uint64_t nEventsProcessed  = allEventsCBK->nAcceptedEvents();
        double sumOfWeights        = allEventsCBK->sumOfEventWeights();
        double sumOfWeightsSquared = allEventsCBK->sumOfEventWeightsSquared();

#ifdef __APPLE__
        Info( APP_NAME, "CutBookkeepers Accepted %llu SumWei %f sumWei2 %f ", nEventsProcessed, sumOfWeights, sumOfWeightsSquared);
#else
        Info( APP_NAME, "CutBookkeepers Accepted %lu SumWei %f sumWei2 %f ", nEventsProcessed, sumOfWeights, sumOfWeightsSquared);
#endif

      } else { Info( APP_NAME, "No relevent CutBookKeepers found" ); }
      
    }

   
    ANA_CHECK( objTool.ApplyPRWTool());

    //if(debug) Info( APP_NAME, "PRW Weight = %f", objTool.GetPileupWeight());

    // Print some event information for fun:
    const xAOD::EventInfo* ei = 0;
    ANA_CHECK( event.retrieve( ei, "EventInfo" ) );


    if (entry % period == 0) {
      Info( APP_NAME,
            "===>>>  start processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( ei->eventNumber() ),
            static_cast< int >( ei->runNumber() ),
            static_cast< int >( entry ) );
    }

    // if (!isData && entry % period == 0) {
    //   float xsectTimesEff =  my_XsecDB->xsectTimesEff(ei->mcChannelNumber());
    //   Info( APP_NAME, "===>>>  process cross section %f", xsectTimesEff );
    // }

    bool eventPassesGRL(true);
    bool eventPassesCleaning(true);
    bool eventPassesTileTrip(true); // move to xAOD tool!
    bool eventPassesTrigger(true);  // coming soon!

    //Info( APP_NAME, "Passed MET Trigger = %i", (int)objTool.IsMETTrigPassed() );

    if (isData) {
      eventPassesGRL = m_grl->passRunLB(ei->runNumber(), ei->lumiBlock());

      eventPassesCleaning = !((ei->errorState(xAOD::EventInfo::LAr)  == xAOD::EventInfo::Error ) ||
                              (ei->errorState(xAOD::EventInfo::Tile) == xAOD::EventInfo::Error ) ||
                              (ei->errorState(xAOD::EventInfo::SCT)  == xAOD::EventInfo::Error ) ||
                              (ei->isEventFlagBitSet(xAOD::EventInfo::Core, 18) ));

    }
    else {
      // Check SUSY Proc. ID for signal MC (only for first event for now!)
      if(entry<5){
        UInt_t  procID = 0;
        int pdgid1 = 0;
        int pdgid2 = 0;
        
        if( objTool.FindSusyHP(pdgid1, pdgid2) != StatusCode::SUCCESS ){
          Error(APP_NAME, "--- SOMETHING IS WRONG WITH THE SUSY PROC FINDING... ---");
          return StatusCode::FAILURE;
        }
        
        if( pdgid1!=0 && pdgid2!=0){ //(just to avoid warnings)
          procID = SUSY::finalState(pdgid1, pdgid2); // get prospino proc ID
          Info(APP_NAME , "--- SIGNAL ID1     : %d", pdgid1);
          Info(APP_NAME , "    SIGNAL ID2     : %d", pdgid2);
          Info(APP_NAME , "    SIGNAL PROC ID : %d", procID);

	  if ( acc_susyid.isAvailable(*ei)  ) 
	    Info(APP_NAME , "    SIGNAL PROC ID (DECO) : %d", acc_susyid(*ei) );

          Info(APP_NAME , "--- XSECTION DETAILS");
          Info(APP_NAME , "    Xsec (high order)    : %f", my_XsecDB->xsectTimesEff(ei->mcChannelNumber(),procID));
          Info(APP_NAME , "    kfactor (high order) : %f", my_XsecDB->kfactor(ei->mcChannelNumber(),procID));
          Info(APP_NAME , "    filter efficiency    : %f", my_XsecDB->efficiency(ei->mcChannelNumber(),procID));

        }
      }
    }
    
    //Check PV in the event
    if (objTool.GetPrimVtx() == nullptr) {
      Warning(APP_NAME , "No PV found for this event! Skipping...");
      store.clear();
      continue;
    }

    // Get the nominal object containers from the event
    // Electrons
    xAOD::ElectronContainer* electrons_nominal(0);
    xAOD::ShallowAuxContainer* electrons_nominal_aux(0);
    //if( !xStream.Contains("SUSY8") ) //SMP derivation, no electrons, no photons // Martin : TBC
    ANA_CHECK( objTool.GetElectrons(electrons_nominal, electrons_nominal_aux) );

    for (const auto& electron : *electrons_nominal){
      if (debug && entry<10){
        bool accepted=false;
        bool idok = electron->passSelection(accepted, "passBaseID");
        if(idok)
          std::cout << "Electron Baseline ID Decision : " << accepted << std::endl;
        else
          std::cout << "Electron Baseline ID Decision not available " << std::endl;
      }
    }

    // // Photons
    xAOD::PhotonContainer* photons_nominal(0);
    xAOD::ShallowAuxContainer* photons_nominal_aux(0);
    if( !xStream.Contains("SUSY12") )//&& !xStream.Contains("SUSY8") ) // Martin : TBC
      ANA_CHECK( objTool.GetPhotons(photons_nominal,photons_nominal_aux) );

    // Muons
    xAOD::MuonContainer* muons_nominal(0);
    xAOD::ShallowAuxContainer* muons_nominal_aux(0);
    ANA_CHECK( objTool.GetMuons(muons_nominal, muons_nominal_aux) );

    // HighPt muons (if required)
    for (const auto& muon : *muons_nominal){
      if (debug && entry<10){
              std::cout << "--------------------------------------" << std::endl;
        std::cout << "Muon pt = " << muon->pt()*0.001 << " , " 
                  << "baseline = " << (int)muon->auxdata<char>("baseline") << " ,"
                  << "bad = " << (int)muon->auxdata<char>("bad") << " ,"
                  << "IsHighPt(deco) = " << (int)muon->auxdata<char>("passedHighPtCuts") << " , "
                  << "IsHighPt(only) = " << (int)objTool.IsHighPtMuon(*muon) << std::endl;
      }
    }
    

    // Jets
    xAOD::JetContainer* jets_nominal(0);
    xAOD::ShallowAuxContainer* jets_nominal_aux(0);
    ANA_CHECK( objTool.GetJets(jets_nominal, jets_nominal_aux) );
    
    // FatJets
    const xAOD::JetContainer* FJC(0);
    xAOD::JetContainer* fatjets_nominal(0);
    xAOD::ShallowAuxContainer* fatjets_nominal_aux(0);
    if(xStream.Contains("SUSY10")){
      if( event.retrieve(FJC, "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets").isSuccess() ){
        
        ANA_CHECK( objTool.GetFatJets(fatjets_nominal, fatjets_nominal_aux) );
        if (debug && entry < 10) {
          for (const auto& fatjet : *fatjets_nominal) {
            Info( APP_NAME, " FatJet pt = %f, Wtag = %d, Ztag = %d", fatjet->pt()*0.001, fatjet->auxdata<int>("Wtag"), fatjet->auxdata<int>("Ztag") );
          }
        }
      }
      else {
        Error( APP_NAME, " LargeR jet collection AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets not available in input file. CHECK!");
        //        std::cout << "Warning :: LargeR jet collection AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets not available in input file. Skipping this part of the test!" << std::endl;
      }
    }
    
    //Taus
    xAOD::TauJetContainer* taus_nominal(0);
    xAOD::ShallowAuxContainer* taus_nominal_aux(0);
    if(xStream.Contains("SUSY3")){
      ANA_CHECK( objTool.GetTaus(taus_nominal,taus_nominal_aux) );
    }


    // MET
    xAOD::MissingETContainer* metcst_nominal = new xAOD::MissingETContainer;
    xAOD::MissingETAuxContainer* metcst_nominal_aux = new xAOD::MissingETAuxContainer;
    metcst_nominal->setStore(metcst_nominal_aux);
    metcst_nominal->reserve(10);

    xAOD::MissingETContainer* mettst_nominal = new xAOD::MissingETContainer;
    xAOD::MissingETAuxContainer* mettst_nominal_aux = new xAOD::MissingETAuxContainer;
    mettst_nominal->setStore(mettst_nominal_aux);
    mettst_nominal->reserve(10);

    // Set up the event weights
    // Base should include all weights that do not depend on individual objects
    double base_event_weight(1.);
    if (!isData){
      base_event_weight *= ei->mcEventWeight();
      //Check for the new Sherpa 2.2 Reweighting function
      if(debug && entry < 5){
        float RW_sh22 = objTool.getSherpaVjetsNjetsWeight("AntiKt4TruthJets");
        Info(APP_NAME , "--- SHERPA2.2 REWEIGHTING : %f", RW_sh22); 
      }
    }

    // Additionally define a nominal weight for each object type
    double elecSF_nominal(1.);
    double muonSF_nominal(1.);
    //    double tauSF_nominal(1.);
    double btagSF_nominal(1.);

    bool isNominal(true);
    isys = 0;
    // Now loop over all the systematic variations
    for (const auto& sysInfo : systInfoList) {
      const CP::SystematicSet& sys = sysInfo.systset;
      // std::cout << ">>>> Working on variation: \"" <<(sys.name()).c_str() << "\" <<<<<<" << std::endl;

      size_t icut = 0;
      // log all events
      elcuts[isys][icut] += 1;
      mucuts[isys][icut] += 1;
      ++icut;

      // does the event pass the GRL + Cleaning?
      if (!(eventPassesGRL && eventPassesCleaning)) {++isys; continue;}
      elcuts[isys][icut] += 1;
      mucuts[isys][icut] += 1;
      ++icut;

      // Apply TileTrip
      if (!eventPassesTileTrip) {++isys; continue;}
      elcuts[isys][icut] += 1;
      mucuts[isys][icut] += 1;
      ++icut;

      if (debug && entry==0) {
        // Testing trigger
        std::string trigItem[6] = {"HLT_e26_lhtight_iloose", "HLT_e60_lhmedium",
                                   "HLT_mu26_imedium", "HLT_mu50",
                                   "HLT_xe100", "HLT_noalg_.*"
                                  };
        for (int it = 0; it < 6; it++) {
          bool passed = objTool.IsTrigPassed(trigItem[it]);
          float prescale = objTool.GetTrigPrescale(trigItem[it]);
          Info( APP_NAME, "passing %s trigger? %d, prescale %f", trigItem[it].c_str(), (int)passed, prescale );
          // example of more sophisticated trigger access
          const Trig::ChainGroup* cg = objTool.GetTrigChainGroup(trigItem[it]);
          bool cg_passed = cg->isPassed();
          float cg_prescale = cg->getPrescale();
          Info( APP_NAME, "ChainGroup %s: passing trigger? %d, prescale %f", trigItem[it].c_str(), (int)cg_passed, cg_prescale );
          for (const auto& cg_trig : cg->getListOfTriggers()) {
            Info( APP_NAME, "               includes trigger %s", cg_trig.c_str() );
          }
        }
      }

      // Trigger (coming soon...)
      if (!eventPassesTrigger)  {++isys; continue;}
      elcuts[isys][icut] += 1;
      mucuts[isys][icut] += 1;
      ++icut;

      // Generic pointers for either nominal or systematics copy
      xAOD::ElectronContainer* electrons(electrons_nominal);
      xAOD::PhotonContainer* photons(photons_nominal);
      xAOD::MuonContainer* muons(muons_nominal);
      xAOD::JetContainer* jets(jets_nominal);
      xAOD::TauJetContainer* taus(taus_nominal);
      xAOD::MissingETContainer* metcst(metcst_nominal);
      xAOD::MissingETContainer* mettst(mettst_nominal);
      // Aux containers too
      xAOD::MissingETAuxContainer* metcst_aux(metcst_nominal_aux);
      xAOD::MissingETAuxContainer* mettst_aux(mettst_nominal_aux);

      xAOD::JetContainer* goodJets = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
      //ANA_CHECK( store.record(goodJets, "MySelJets" + sys.name()) ); //NOT WORKING? //MT,WB
      
      // Tell the SUSYObjDef_xAOD which variation to apply
      if (objTool.applySystematicVariation(sys) != CP::SystematicCode::Ok) {
        Error(APP_NAME, "Cannot configure SUSYTools for systematic var. %s", (sys.name()).c_str() );
      } else {
        if (debug) Info(APP_NAME, "Variation \"%s\" configured...", (sys.name()).c_str() );
      }
      if (sysInfo.affectsKinematics || sysInfo.affectsWeights) isNominal = false;

      // If nominal, compute the nominal weight, otherwise recompute the weight
      double event_weight = base_event_weight;


      // If necessary (kinematics affected), make a shallow copy with the variation applied
      bool syst_affectsElectrons = ST::testAffectsObject(xAOD::Type::Electron, sysInfo.affectsType);
      bool syst_affectsMuons = ST::testAffectsObject(xAOD::Type::Muon, sysInfo.affectsType);
      bool syst_affectsTaus = ST::testAffectsObject(xAOD::Type::Tau, sysInfo.affectsType);
      bool syst_affectsPhotons = ST::testAffectsObject(xAOD::Type::Photon, sysInfo.affectsType);
      bool syst_affectsJets = ST::testAffectsObject(xAOD::Type::Jet, sysInfo.affectsType);
      bool syst_affectsBTag = ST::testAffectsObject(xAOD::Type::BTag, sysInfo.affectsType);
      //      bool syst_affectsMET = ST::testAffectsObject(xAOD::Type::MissingET, sysInfo.affectsType);

      if (sysInfo.affectsKinematics) {
        if (syst_affectsElectrons) {
          xAOD::ElectronContainer* electrons_syst(0);
          xAOD::ShallowAuxContainer* electrons_syst_aux(0);
          ANA_CHECK( objTool.GetElectrons(electrons_syst, electrons_syst_aux) );
          electrons = electrons_syst;
        }

        if (syst_affectsMuons) {
          xAOD::MuonContainer* muons_syst(0);
          xAOD::ShallowAuxContainer* muons_syst_aux(0);
          ANA_CHECK( objTool.GetMuons(muons_syst, muons_syst_aux) );
          muons = muons_syst;
        }

	if(syst_affectsTaus) {
	  xAOD::TauJetContainer* taus_syst(0);
	  xAOD::ShallowAuxContainer* taus_syst_aux(0);
	  if(xStream.Contains("SUSY3")){
	    ANA_CHECK( objTool.GetTaus(taus_syst,taus_syst_aux) );
	  }
	  taus = taus_syst;
	}

        if(syst_affectsPhotons) {
          xAOD::PhotonContainer* photons_syst(0);
          xAOD::ShallowAuxContainer* photons_syst_aux(0);
          ANA_CHECK( objTool.GetPhotons(photons_syst,photons_syst_aux) );
          photons = photons_syst;
        }

        if (syst_affectsJets) {
          xAOD::JetContainer* jets_syst(0);
          xAOD::ShallowAuxContainer* jets_syst_aux(0);
          ANA_CHECK( objTool.GetJetsSyst(*jets_nominal, jets_syst, jets_syst_aux) );
	  jets = jets_syst;
        }

        xAOD::MissingETContainer* metcst_syst = new xAOD::MissingETContainer;
        xAOD::MissingETAuxContainer* metcst_syst_aux = new xAOD::MissingETAuxContainer;
        xAOD::MissingETContainer* mettst_syst = new xAOD::MissingETContainer;
        xAOD::MissingETAuxContainer* mettst_syst_aux = new xAOD::MissingETAuxContainer;
        metcst_syst->setStore(metcst_syst_aux);
        mettst_syst->setStore(mettst_syst_aux);
        metcst_nominal->reserve(10);
        metcst_nominal->reserve(10);

        metcst = metcst_syst;
        mettst = mettst_syst;
        metcst_aux = metcst_syst_aux;
        mettst_aux = mettst_syst_aux;
      }

      // ***************    ************************    *****************
      // *************** Now start processing the event *****************
      // ***************    ************************    *****************

      if (isNominal || (sysInfo.affectsKinematics && syst_affectsElectrons) ) {
        for (const auto& el : *electrons) {
          //objTool.IsSignalElectron( *el ) ; // TODO: check that this should be removed /CO

          if (!isData) {
            const xAOD::TruthParticle* truthEle = xAOD::TruthHelpers::getTruthParticle(*el);
            if (truthEle) {
              if (debug) {
                Info( APP_NAME, " Truth Electron pt %f eta %f", truthEle->pt(), truthEle->eta() );
                Info( APP_NAME, " IsPrompt() = %d", objTool.isPrompt(el) );
              }
            }
            else {if (debug) Info( APP_NAME, " Truth Electron not found" );}
          }
        }
      }

      if (isNominal || (sysInfo.affectsKinematics && syst_affectsMuons) ) {
        for (const auto& mu : *muons) {
          // TODO: check that these should be removed too /CO
          //objTool.IsSignalMuon( *mu ) ;
          //objTool.IsCosmicMuon( *mu ) ;

          if (!isData) {
            // Example to access MC type/origin
            int muonTruthType = 0;
            int muonTruthOrigin = 0;
            const xAOD::TrackParticle* trackParticle = mu->primaryTrackParticle();
            if (trackParticle) {
              static SG::AuxElement::Accessor<int> acc_truthType("truthType");
              static SG::AuxElement::Accessor<int> acc_truthOrigin("truthOrigin");
              if (acc_truthType.isAvailable(*trackParticle)  ) muonTruthType   = acc_truthType(*trackParticle);
              if (acc_truthOrigin.isAvailable(*trackParticle)) muonTruthOrigin = acc_truthOrigin(*trackParticle);
              const xAOD::TruthParticle* truthMu = xAOD::TruthHelpers::getTruthParticle(*trackParticle);
              if (truthMu)
              { if (debug) Info( APP_NAME, " Truth Muon pt %f eta %f, type %d, origin %d",
                                   truthMu->pt(), truthMu->eta(), muonTruthType, muonTruthOrigin );
              }
              else
              {if (debug) Info( APP_NAME, " Truth Muon not found");}
            }
          }
        }
      }

      if (isNominal || (sysInfo.affectsKinematics && syst_affectsJets)) {
        for (const auto& jet : *jets) {
          objTool.IsBJet( *jet) ;
        }
      }

      // if (debug) Info(APP_NAME, "Tau truth");
      // if (isNominal) {
      //  for(const auto& tau : *taus){
      //    if (!isData){
      //      const xAOD::TruthParticle* truthTau = T2MT.applyTruthMatch(*tau) ;
      //      if (tau->auxdata<char>("IsTruthMatched") || !truthTau){
      //        if (debug) Info( APP_NAME,
      //            "Tau was matched to a truth tau, which has %i prongs and a charge of %i",
      //            int(tau->auxdata<size_t>("TruthProng")),
      //            tau->auxdata<int>("TruthCharge"));
      //      } else {
      //        if (debug) Info( APP_NAME, "Tau was not matched to truth" );
      //      }
      //    }
      //  }
      // }

      if (debug) Info(APP_NAME, "Overlap removal");

      // do overlap removal
      if (isNominal || (sysInfo.affectsKinematics && (syst_affectsElectrons || syst_affectsMuons || syst_affectsJets))) {
        if(xStream.Contains("SUSY3")){
          ANA_CHECK( objTool.OverlapRemoval(electrons, muons, jets, 0, taus) );
        }
        else if(xStream.Contains("SUSY10")){
          ANA_CHECK( objTool.OverlapRemoval(electrons, muons, jets, 0, 0, fatjets_nominal) );
        }
        else{
          ANA_CHECK( objTool.OverlapRemoval(electrons, muons, jets, photons) );
        }
      }

      if (debug) Info(APP_NAME, "GoodJets?");
      for (const auto& jet : *jets) {
        if (jet->auxdata<char>("baseline") == 1  &&
            jet->auxdata<char>("passOR") == 1  &&
	    jet->auxdata<char>("signal") == 1  &&
            jet->pt() > 20000.  && ( fabs( jet->eta()) < 2.5) ) {
          goodJets->push_back(jet);
        }
      }
      
      if (isNominal || sysInfo.affectsKinematics) {
        if(xStream.Contains("SUSY3")){
	  if (debug) Info(APP_NAME, "METCST?");
          ANA_CHECK( objTool.GetMET(*metcst,
                                    jets,
                                    electrons,
                                    muons,
                                    photons,
                                    taus,
                                    false, // CST
				    false) ); // No JVT if you use CST

	  if (debug) Info(APP_NAME, "METTST?");
	  ANA_CHECK( objTool.GetMET(*mettst,
				    jets,
				    electrons,
				    muons,
				    photons,
				    taus,
				    true) );
	}
	else{
	  if (debug) Info(APP_NAME, "METCST?");
	  ANA_CHECK( objTool.GetMET(*metcst,
	  			    jets,
	  			    electrons,
	  			    muons,
	  			    photons,
	  			    0, // taus
	  			    false, // CST
	  			    false) ); // No JVT if you use CST

          if (debug) Info(APP_NAME, "METTST?");
          ANA_CHECK( objTool.GetMET(*mettst,
                                    jets,
                                    electrons,
                                    muons,
                                    photons,
                                    0, // taus,
                                    true) );

        }
        if (debug) Info(APP_NAME, "MET done");
      }
      
      float elecSF = 1.0;
      int el_idx[nSel] = {0};
      for (const auto& el : *electrons) {
        if ( el->auxdata<char>("passOR") == 0  ) {
          el_idx[passOR]++;
          continue;
        }
        if ( el->auxdata<char>("baseline") == 1  )
          el_idx[baseline]++;
        if ( el->auxdata<char>("signal") == 1  ) {
          el_idx[signallep]++;
          if ( el->pt() > 20000. ) {
            el_idx[goodpt]++;
            
            bool passTM=false;
            for(const auto& t : el_triggers){
              passTM |= objTool.IsTrigMatched(el, t);
              //passTM |= objTool.IsTrigMatchedDeco(el, t);
            }              
            if(passTM)
              el_idx[trgmatch]++; 


	    //check ChID BDT
	    //Info(APP_NAME, "electron passChID : %d ,  BDT : %.3f", el->auxdata<char>("passChID") , el->auxdata<double>("ecisBDT"));            
          }
        }
      }
      if (isNominal || syst_affectsElectrons) {
        if(!isData) elecSF = objTool.GetTotalElectronSF(*electrons);

	// double trigElSF=1.;
	// if(!isData) trigElSF = objTool.GetTotalElectronSF(*electrons,false,false,true,false,objTool.TrigSingleLep());
	// std::cout << "DEBUG : " << trigElSF << "  Ntrigmatch = " << el_idx[trgmatch] << "    year = " << objTool.treatAsYear() << std::endl;
      }
      if (isNominal) {elecSF_nominal = elecSF;}
      else if (!syst_affectsElectrons) {elecSF = elecSF_nominal;}
      event_weight *= elecSF;

      float muonSF = 1.0;
      int mu_idx[nSel] = {0};
      bool passTMtest = false;
      
      TString muTrig2015 = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50"; //"HLT_mu18_mu8noL1"; //"HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
      TString muTrig2016 = "HLT_mu24_imedium"; //"HLT_mu20_mu8noL1";  //HLT_mu20_iloose_L1MU15_OR_HLT_mu50
      
      for (const auto& mu : *muons) {
        if ( mu->auxdata<char>("passOR") == 0  ) {
          mu_idx[passOR]++;
          continue;
        }
        if ( mu->auxdata<char>("baseline") == 1  ) {
          mu_idx[baseline]++;
          if ( mu->auxdata<char>("cosmic") == 1  ) {
            mu_idx[cosmic]++;
          }
        }
        if ( mu->auxdata<char>("signal") == 1  ) {
          mu_idx[signallep]++;
          if ( mu->pt() > 20000. )  {
            mu_idx[goodpt]++;
            
            bool passTM=false;
            for(const auto& t : mu_triggers){
              //std::cout << "Pass " << t << " : " << (int)objTool.IsTrigMatched(mu, t) << std::endl;
              passTM |= objTool.IsTrigMatched(mu, t);
            }
            if(passTM)
              mu_idx[trgmatch]++;              
            
          }

          if(!isData){
            if(objTool.treatAsYear()==2015)
              passTMtest |= objTool.IsTrigMatched(mu, muTrig2015.Copy().ReplaceAll("_OR_","").Data());
            else
              passTMtest |= objTool.IsTrigMatched(mu, muTrig2016.Copy().ReplaceAll("_OR_","").Data());
          }
          else{
            passTMtest |= objTool.IsTrigMatched(mu, muTrig2016.Copy().ReplaceAll("_OR_","").Data());
          }
        }
      }


      ///CHECK FOR ATLSUSYSW-147
      // if( mu_idx[goodpt] >= 2 ){
      //         std::cout << "TRIGMUTEST ----------------------" << std::endl;
      //         std::cout << "TRIGMUTEST " << objTool.IsTrigPassed("HLT_2mu14") << std::endl;
      //         auto imudbg=0;
      //         for (const auto& mu : *muons) {
      //           std::cout << "TRIGMUTEST Muon "<< imudbg << " pass 2mu14 = " << objTool.IsTrigMatched(mu, "HLT_2mu14") << std::endl;
      //           imudbg++;
      //         }      
      //         if( mu_idx[goodpt] == 2 ){
      //           std::cout << "TRIGMUTEST MuonPair pass 2mu14 = " << objTool.IsTrigMatched(muons->at(0), muons->at(1), "HLT_2mu14") << std::endl;
      //         }
      // }
      ////


      if ((!isData && isNominal) || syst_affectsMuons) {
        if(passTMtest || 1){ //objTool.IsTrigPassed(muTrig.ReplaceAll("_OR_",",").Data())){
          //std::cout << "MUON BEFORE SF = " << muonSF << "   " << objTool.treatAsYear() << "   "  << objTool.GetRandomRunNumber() << "    " <<  objTool.GetPileupWeight() << std::endl;
          if(objTool.treatAsYear()==2015)
            muonSF = objTool.GetTotalMuonSF(*muons, true, true, muTrig2015.Data());
          else 
            muonSF = objTool.GetTotalMuonSF(*muons, true, true, muTrig2016.Data());

          //std::cout << "MUON AFTER SF = " << muonSF << "   " << objTool.treatAsYear() << "   "  << objTool.GetRandomRunNumber() << "    " <<  objTool.GetPileupWeight() << std::endl;
        }
      }

      if (isNominal) {muonSF_nominal = muonSF;}
      else if (!syst_affectsMuons) {muonSF = muonSF_nominal;}
      event_weight *= muonSF;

      //taus
      if(isNominal && !isData && xStream.Contains("SUSY3")){
        if (entry<10){
          // CP::SystematicSet testSet("TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2016__1up");
          // testSet.insert( CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2016", 1) );

          // CP::SystematicSet testSet("TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2015__1up");
          // testSet.insert( CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2015", 1) );
          // if(objTool.applySystematicVariation(testSet) != CP::SystematicCode::Ok){
          //   Error( APP_NAME, "Problems with tau trig eff systematic settings!");
          // }
          
          for (const auto& tau : *taus){
                   Info( APP_NAME, " Tau pt = %.5f, idSF = %.5f, trigSF = %.5F, totSF = %.5f",
                         tau->pt()*1e-3,
                         objTool.GetSignalTauSF(*tau,true,false,"tau25_medium1_tracktwo"),
                         objTool.GetSignalTauSF(*tau,false,true,"tau25_medium1_tracktwo"),
                         objTool.GetSignalTauSF(*tau,true,true,"tau25_medium1_tracktwo"));
                 }
          Info( APP_NAME, " Total Event Tau SF = %.4f", objTool.GetTotalTauSF(*taus, true, true, "tau25_medium1_tracktwo"));
          // if(objTool.resetSystematics() != CP::SystematicCode::Ok){
          //   Error( APP_NAME, "Problems going back to default systematic set!");
          // }
        }
      }

      if (debug) Info(APP_NAME, "Lepton SF done");

      int jet_idx[nSel] = {0};
      for (const auto& jet : *goodJets) {
        if ( jet->auxdata<char>("bad") == 1  )
          jet_idx[bad]++;
        if ( jet->auxdata<char>("passOR") == 0  ) {
          jet_idx[passOR]++;
          continue;
        }
        if ( jet->auxdata<char>("baseline") == 1  ) {
          jet_idx[baseline]++;
          if ( jet->pt() > 50000. )
            jet_idx[goodpt]++;
        }
        if ( jet->auxdata<char>("bjet") == 1  )
          jet_idx[btagged]++;
      }

      // compute b-tagging SF
      float btagSF(1.);
      if (!isData) {
        if (isNominal) {btagSF = btagSF_nominal = objTool.BtagSF(jets);}
        else if (syst_affectsBTag || (sysInfo.affectsKinematics && syst_affectsJets)) {btagSF = objTool.BtagSF(jets);}
        else {btagSF = btagSF_nominal;}
      }
      event_weight *= btagSF;

      if (debug) Info(APP_NAME, "Jet SF done");

      // Cosmics
      if (mu_idx[cosmic] == 0 ) {
        elcuts[isys][icut] += 1;
        mucuts[isys][icut] += 1;
        ++icut;

        bool passlep = (el_idx[baseline] + mu_idx[baseline]) == 1;
        if (passlep) {
          bool passel = el_idx[baseline] == 1;
          bool passmu = mu_idx[baseline] == 1;
          if (passel) elcuts[isys][icut] += 1;
          if (passmu) mucuts[isys][icut] += 1;
          ++icut;

          passel = el_idx[signallep] == 1;
          passmu = mu_idx[signallep] == 1;
          if (passel) elcuts[isys][icut] += 1;
          if (passmu) mucuts[isys][icut] += 1;
          ++icut;

          passel = el_idx[goodpt] == 1;
          passmu = mu_idx[goodpt] == 1;
          if (passel) elcuts[isys][icut] += 1;
          if (passmu) mucuts[isys][icut] += 1;
          ++icut;

          passel = el_idx[trgmatch] == 1;
          passmu = mu_idx[trgmatch] == 1;
          if (passel) elcuts[isys][icut] += 1;
          if (passmu) mucuts[isys][icut] += 1;
          ++icut;

          if (jet_idx[goodpt] >= 2) {
            if (passel) elcuts[isys][icut] += 1;
            if (passmu) mucuts[isys][icut] += 1;
            ++icut;
          } // good jets
        } // passlep
      } // cosmics

      if (debug) Info(APP_NAME, "Lepton SF done");

      // Clean up the systematics copies
      if (sysInfo.affectsKinematics) {
        delete metcst; 
        delete metcst_aux;
        delete mettst;
        delete mettst_aux;
      }

      isNominal = false;
      if(debug)
	std::cout << ">>>> Finished with variation: \"" <<(sys.name()).c_str() << "\" <<<<<<" << std::endl;

      ++isys;
    }


    //Reset systematics settings for next event
    ANA_CHECK( objTool.resetSystematics());


    // Only fill the event if you want an output file.  We don't need one, so no need to fill.
    //  Unfortunately, this manipulation is needed in order for the METMaker to find all of its
    //  objects.  See also ATLASG-801
    //event.fill();


    // The containers created by the shallow copy are owned by you. Remember to delete them.
    // In our case, all of these were put into the store

    // store.print();
    store.clear();

    // Close with a message:
    if (entry % period == 0) {
      Info( APP_NAME,
            "===>>>  done processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( ei->eventNumber() ),
            static_cast< int >( ei->runNumber() ),
            static_cast< int >( entry + 1 ) );
    }
  }

  m_clock0.Stop();
  m_clock2.Stop();

  // CALLGRIND_TOGGLE_COLLECT;
  // CALLGRIND_DUMP_STATS;
  // ProfilerStop();

  Info( APP_NAME,
        "Time per event:          (CPU / WALL)\n"
        "  Entire loop           - %f / %f ms\n"
        "  First event           - %f / %f ms\n"
        "  Excluding first event - %f / %f ms",
        m_clock0.CpuTime() * 1e3 / entries, m_clock0.RealTime() * 1e3 / entries,
        m_clock1.CpuTime() * 1e3, m_clock1.RealTime() * 1e3,
        m_clock2.CpuTime() * 1e3 / (entries - 1), m_clock2.RealTime() * 1e3 / (entries - 1));


  // Dump the cutflow:
  isys = 0;
  for (const auto& sysInfo : systInfoList) {
    const CP::SystematicSet& sys = sysInfo.systset;
    Info( APP_NAME, "-- Dummy Cutflow -- >>>> Variation \"%s\" <<<<", sys.name().c_str() );
    for (size_t icut = 0; icut < Ncuts; ++icut) {
      Info( APP_NAME, "Cut %3lu (%20s)-> el: %d   mu: %d", icut, cut_name[icut], elcuts[isys][icut], mucuts[isys][icut] );
    }
    ++isys;
  }

  // Return gracefully:
  return 0;
}
