/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <memory>

#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TSystem.h"

#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TActiveStore.h"

// bits for I/O performance monitoring
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/PerfStats.h"
#include "xAODCore/tools/IOStats.h"

// CutBookKeeping
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

#include "TopCorrections/ScaleFactorCalculator.h"
#include "TopCorrections/PDFScaleFactorCalculator.h"

#include "TopFakes/TopFakesMMWeightCalculator.h"

#include "TopSystematicObjectMaker/ObjectCollectionMaker.h"

#include "TopConfiguration/AodMetaDataAccess.h"
#include "TopConfiguration/ConfigurationSettings.h"
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/SelectionConfigurationData.h"

#include "TopAnalysis/EventSelectionManager.h"
#include "TopAnalysis/Tools.h"
#include "TopCPTools/TopToolStore.h"
#include "TopAnalysis/ObjectLoaderBase.h"
#include "TopAnalysis/EventSaverBase.h"
#include "TopAnalysis/MetadataTree.h"

#include "TopEvent/EventTools.h"
#include "TopEvent/TopEventMaker.h"
#include "TopEvent/SystematicEvent.h"

#include "TopObjectSelectionTools/TopObjectSelection.h"
#include "TopObjectSelectionTools/EventCleaningSelection.h"

#include "TopPartons/CalcTtbarPartonHistory.h"
#include "TopPartons/CalcTbbarPartonHistory.h"
#include "TopPartons/CalcTopPartonHistory.h"

#include "TopParticleLevel/ParticleLevelLoader.h"

#include "TopDataPreparation/SampleXsection.h"

#include "TopHLUpgrade/UpgradeObjectLoader.h"

// to disable the sending of file access statistics
#include "xAODRootAccess/tools/TFileAccessTracer.h"

// Path resolver for TopDataPreparation
#include "PathResolver/PathResolver.h"

/// Function prototypes used to refactor 

/// Check the command line inputs
int CheckArgs(int, char**);
/// Initialise the XAOD objects
xAOD::TStore* InitialiseXAOD();
/// Find a valid input file
std::string CheckForValidInput(std::vector<std::string>);
/// Fix the TopConfig object
void FixConfiguration(std::shared_ptr<top::TopConfig> );

// We have a lot of tools which need to be configured and managed (using shared_ptr to be able to pass to the struct below)
std::shared_ptr<top::TopConfig>                  CreateTopConfig(std::string, std::string, std::string);
void                                             SetMetadata(bool, std::string, std::string, std::shared_ptr<top::TopConfig>);
std::shared_ptr<TFile>                           CreateOutputFile();
std::shared_ptr<top::EventSelectionManager>      CreateEventSelectionManager(std::shared_ptr<TFile>, std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::EventSaverBase>             CreateEventSaver(std::shared_ptr<TFile>, std::shared_ptr<top::EventSelectionManager>, std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::EventCleaningSelection>     CreateEventCleaningSelection(std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::ObjectCollectionMaker>      CreateObjectCollectionMaker(std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::TopObjectSelection>         CreateTopObjectSelection(std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::CalcTopPartonHistory>       CreateTopPartonHistory(std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::PDFScaleFactorCalculator>   CreatePDFScaleFactorCalculator(std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::TopEventMaker>              CreateTopEventMaker(std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::ScaleFactorCalculator>      CreateScaleFactorCalculator(std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::TopFakesMMWeightCalculator> CreateTopFakesMMWeightCalculator(std::shared_ptr<top::EventSelectionManager>, std::shared_ptr<top::TopConfig>);
std::shared_ptr<MetadataTree>                    CreateMetadataTree(std::shared_ptr<TFile>, std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::TopToolStore>               CreateTopToolStore(std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::ParticleLevelLoader>        CreateParticleLevelLoader(std::shared_ptr<top::TopConfig>);
std::shared_ptr<top::UpgradeObjectLoader>        CreateUpgradeObjectLoader(std::shared_ptr<top::TopConfig>);

/// Use a struct to hold all shared_ptr references and pass around functions (eases functionality extension)
struct AnalysisTopTools{
  std::shared_ptr<top::TopConfig> topConfig;
  std::shared_ptr<TFile> outputFile;
  std::shared_ptr<top::EventSelectionManager> topEventSelectionManager;
  std::shared_ptr<top::EventSaverBase> topEventSaver;
  std::shared_ptr<top::EventCleaningSelection> topEventCleaningSelection;
  std::shared_ptr<top::ObjectCollectionMaker> topObjectCollectionMaker;
  std::shared_ptr<top::TopObjectSelection> topObjectSelection;
  std::shared_ptr<top::CalcTopPartonHistory> topCalcTopPartonHistory;
  std::shared_ptr<top::PDFScaleFactorCalculator> topPDFScaleFactorCalculator;
  std::shared_ptr<top::TopEventMaker> topEventMaker;
  std::shared_ptr<top::ScaleFactorCalculator> topScaleFactorCalculator;
  std::shared_ptr<top::TopFakesMMWeightCalculator> topFakesMMWeightCalculator;
  std::shared_ptr<MetadataTree> metadataTree;
  std::shared_ptr<top::TopToolStore> topToolStore;
  std::shared_ptr<top::ParticleLevelLoader> topParticleLevelLoader;
  std::shared_ptr<top::UpgradeObjectLoader> topUpgradeObjectLoader;
  xAOD::TEvent* xaodEvent;
  xAOD::TStore* store;
};

/// Run the event loop
void RunEventLoop(std::vector<std::string>, AnalysisTopTools);
/// Manage the bookkeeper objects
void ProcessBookkeeper(AnalysisTopTools);
/// Process a truth event selection
void ProcessTruthEvent(AnalysisTopTools, unsigned int&);
/// Process an upgrad event selection
void ProcessUpgradeEvent(AnalysisTopTools);
/// Process a particle level event selection
void ProcessParticleEvent(AnalysisTopTools, unsigned int&);
/// Initialise function for LHAPDF reweighting
void InitialiseLHAPDFWeights(AnalysisTopTools, bool);
/// Reprocessing function for LHAPDF reweighting
void ReprocessLHE3Weights(AnalysisTopTools, bool);
/// Preselection check
bool PassPreselection(AnalysisTopTools);
/// Function for checking and saving tight reco event
void ProcessRecoEventTight(AnalysisTopTools, unsigned int&);
/// Function for checking and saving loose reco event
void ProcessRecoEventLoose(AnalysisTopTools, unsigned int&);
/// Function to process the LHAPDF reweighted weight for an event
void ProcessLHAPDFWeights(AnalysisTopTools);
/// Small debugging function for TopEvent object
void DebugTopEvent(top::Event);
/// Small debugging function for AnalysisTopTools object
void DebugAnalysisTopTools(AnalysisTopTools);

/**
* @file The main executable.
*
* @brief Entry point code to perform object and event selection and write-out a
* few histograms and / or a tree in xAOD format.
*
* @return 0 If everything is okay.
*/

int main(int argc, char** argv) {

  // Check we have the correct number of command line inputs
  if ( CheckArgs(argc, argv) == 1 ) return 1;

  // Initialise the xAOD
  xAOD::TStore* store = InitialiseXAOD();

  // Read the inputs
  std::string settingsFilename       = std::string(argv[1]);
  std::vector<std::string> filenames = top::fileList(std::string(argv[2]));

  // Check if we have a useable file
  std::string validInputFileName = CheckForValidInput(filenames);

  // Create the TopConfig which is used EVERYWHERE
  std::shared_ptr<top::TopConfig> topConfig = CreateTopConfig(settingsFilename, std::string(argv[2]), validInputFileName);

  // Create the output file
  std::shared_ptr<TFile> outputFile = CreateOutputFile();

  // Create the metadata trees in output
  std::shared_ptr<MetadataTree> metadataTree = CreateMetadataTree(outputFile, topConfig);

  // Create the TEvent object before initialising ASG tools
  // In rel 19 we had a function to guess Class or Branch Access.
  // In rel20 just use branch (quicker)
  // In rel 20.7, need to go back to class access
  xAOD::TEvent* xaodEvent = new xAOD::TEvent(xAOD::TEvent::kClassAccess);

  // Create top tool store for ASG tools
  std::shared_ptr<top::TopToolStore> topToolStore = CreateTopToolStore(topConfig);

  // Create the event cleaning selection
  std::shared_ptr<top::EventCleaningSelection> topEventCleaningSelection = CreateEventCleaningSelection(topConfig);

  // Create the object collection maker
  std::shared_ptr<top::ObjectCollectionMaker> topObjectCollectionMaker = CreateObjectCollectionMaker(topConfig);

  // Create the top object selection
  std::shared_ptr<top::TopObjectSelection> topObjectSelection = CreateTopObjectSelection(topConfig);

  // Create the event selection manager
  std::shared_ptr<top::EventSelectionManager> topEventSelectionManager = CreateEventSelectionManager(outputFile, topConfig);

  // Create particle level loader
  std::shared_ptr<top::ParticleLevelLoader> topParticleLevelLoader = CreateParticleLevelLoader(topConfig);

  // Create upgrade object loader
  std::shared_ptr<top::UpgradeObjectLoader> topUpgradeObjectLoader = CreateUpgradeObjectLoader(topConfig);

  // Now fix the config before using it to configure the run
  FixConfiguration(topConfig);

  // Create top parton history
  std::shared_ptr<top::CalcTopPartonHistory> topCalcTopPartonHistory = CreateTopPartonHistory(topConfig);

  // Create top event maker
  std::shared_ptr<top::TopEventMaker> topEventMaker = CreateTopEventMaker(topConfig);

  // Create top scale factor calculator
  std::shared_ptr<top::ScaleFactorCalculator> topScaleFactorCalculator = CreateScaleFactorCalculator(topConfig);

  // Create top pdf scale factor calculator
  std::shared_ptr<top::PDFScaleFactorCalculator> topPDFScaleFactorCalculator = CreatePDFScaleFactorCalculator(topConfig);

  // Create top fakes MM weight calculator
  std::shared_ptr<top::TopFakesMMWeightCalculator> topFakesMMWeightCalculator = CreateTopFakesMMWeightCalculator(topEventSelectionManager, topConfig);

  // Create the event saver
  std::shared_ptr<top::EventSaverBase> topEventSaver = CreateEventSaver(outputFile, topEventSelectionManager, topConfig);


  // At this point, we have configured a lot of tools... let's store them somewhere safe
  AnalysisTopTools analysisTopTools;
  analysisTopTools.topConfig                   = topConfig;
  analysisTopTools.outputFile                  = outputFile;
  analysisTopTools.topEventSelectionManager    = topEventSelectionManager;
  analysisTopTools.topEventSaver               = topEventSaver;
  analysisTopTools.topEventCleaningSelection   = topEventCleaningSelection;
  analysisTopTools.topObjectCollectionMaker    = topObjectCollectionMaker;
  analysisTopTools.topObjectSelection          = topObjectSelection;
  analysisTopTools.topCalcTopPartonHistory     = topCalcTopPartonHistory;
  analysisTopTools.topPDFScaleFactorCalculator = topPDFScaleFactorCalculator;
  analysisTopTools.topEventMaker               = topEventMaker;
  analysisTopTools.topScaleFactorCalculator    = topScaleFactorCalculator;
  analysisTopTools.topFakesMMWeightCalculator  = topFakesMMWeightCalculator;
  analysisTopTools.metadataTree                = metadataTree;
  analysisTopTools.topToolStore                = topToolStore;
  analysisTopTools.topParticleLevelLoader      = topParticleLevelLoader;
  analysisTopTools.topUpgradeObjectLoader      = topUpgradeObjectLoader;
  analysisTopTools.xaodEvent                   = xaodEvent;
  analysisTopTools.store                       = store;


  // Run file and event loop
  RunEventLoop(filenames, analysisTopTools);

  // Get the settings object now as needed for finishing up
  auto* const settings = top::ConfigurationSettings::get();

  // Saving the events/trees/file
  analysisTopTools.outputFile->cd();
  analysisTopTools.topEventSelectionManager->finalise();
  analysisTopTools.topEventSaver->finalize();
  analysisTopTools.outputFile->Close();
  bool outputFileGood = !analysisTopTools.outputFile->TestBit(TFile::kWriteError);
  if (outputFileGood) {
    if (gSystem->Rename(analysisTopTools.outputFile->GetName(), settings->value("OutputFilename").c_str()) != 0)
      outputFileGood = false;
  }

  // Very end of top-xaod
  if (!outputFileGood) {
    std::cout << "ERROR: an I/O error occured while attempting to save the output file." << std::endl;
    return 1;
  }
  return 0;

}

int CheckArgs(int argc, char** argv){
  if (argc != 3) {
    std::cout << "Code to perform object and event selection and write-out\n";
    std::cout << "a few histograms and / or a tree in xAOD format.\n";
    std::cout << "Use like:\n";
    std::cout << "    " << argv[0] << " cuts.txt input.txt\n";
    std::cout << "    cuts.txt   - file containing cuts\n";
    std::cout << "    input.txt  - file containing list of input files\n";
    std::cout << "\n";
    std::cout << "For example\n";
    std::cout << "    " << argv[0] << " $ROOTCOREBIN/data/TopAnalysis/nocuts.txt $ROOTCOREBIN/data/TopAnalysis/input-13TeV-fondueworld.txt\n";
    return 1;
  }
  return 0;
}

xAOD::TStore* InitialiseXAOD(){

  // to disable the sending of file access statistics
  xAOD::TFileAccessTracer::enableDataSubmission( false );

  //magic xAOD stuff
  //true = fail at even the slightest xaod issue
  top::xAODInit(true);
  StatusCode::enableFailure();

  // Perhaps we return this object
  xAOD::TStore* store = new xAOD::TStore();

  return store;
}

std::string CheckForValidInput(std::vector<std::string> filenames){

  std::string usethisfile = "";
  bool atLeastOneFileIsValid(false);
  for (const auto& filename : filenames) {
    std::shared_ptr<TFile> checkingYieldFile(TFile::Open(filename.c_str()));
    //collection tree means > 0 events
    const TTree* const collectionTree = dynamic_cast<TTree* > (checkingYieldFile->Get("CollectionTree"));
    if (collectionTree) {
      usethisfile = filename;
      atLeastOneFileIsValid = true;
      break;
    }
  }

  return usethisfile;
}

std::shared_ptr<top::TopConfig> CreateTopConfig(std::string settingsFilename, std::string inputFileList, std::string validInputFileName){

  // Create the object
  std::shared_ptr<top::TopConfig> topConfig(new top::TopConfig());
  std::cout << "Configuration Files:\n";
  std::cout << "    " << settingsFilename << "\n";
  std::cout << "    " << inputFileList << "\n\n";

  //load the settings from the input file
  auto* const settings = top::ConfigurationSettings::get();
  settings->loadFromFile(settingsFilename);
  std::cout << "Configuration:\n";
  std::cout << *settings << "\n";

  // Pass the settings file to the TopConfig
  topConfig->setConfigSettings(settings);

  const std::string libraryNames = settings->value("LibraryNames");
  top::loadLibraries(libraryNames);

  // Load any meta-data-esque information in its own place
  bool useAodMetaData = false;
  settings->retrieve("UseAodMetaData", useAodMetaData);
  SetMetadata(useAodMetaData, validInputFileName, inputFileList, topConfig);

  // We still need to initialise tools before we fix the config
  return topConfig;
}

void SetMetadata(bool useAodMetaData, std::string usethisfile, std::string inputFileList, std::shared_ptr<top::TopConfig> topConfig){

  std::shared_ptr<TFile> testFile(TFile::Open(usethisfile.c_str()));

  // This function reads directly the Metadata object in xAOD file (we don't use it)
  if(! top::readMetaData(testFile.get(), topConfig)){
    std::cerr << "Unable to access metadata object in this file : " << usethisfile << std::endl;
    std::cerr << "Please report this message" << std::endl;
  }

  // This function uses the python interface to get metadata information (we do use it)
  std::cout << "Loading meta-data from input files ... " << std::flush;
  topConfig->setReleaseSeries();
  topConfig->aodMetaData().loadWithFilesFrom(inputFileList);

  // Work out if the file is MC or data (either from metadata, or otherwise)
  bool const isMC = (useAodMetaData ? topConfig->aodMetaData().isSimulation() : top::isFileSimulation(testFile.get(), topConfig->sgKeyEventInfo()));
  // Set in config
  topConfig->setIsMC(isMC);

  // Work out if the file is a primary xAOD
  const bool isPrimaryxAOD = top::isFilePrimaryxAOD(testFile.get());
  // Set in config
  topConfig->setIsPrimaryxAOD(isPrimaryxAOD);

  // Work out the derivation type (needed for trigger matching objects)
  const std::string derivationStream = top::getDerivationStream(testFile.get());
  // Tell the user
  std::cout << "Derivation stream is -> " << derivationStream << std::endl;
  // Set in config
  topConfig->setDerivationStream(derivationStream);

  if(isMC){
    ///-- Are we using a truth derivation (no reco information)? --///
    ///-- Let's find out in the first event, this could be done better --///
    // To-Do : Change to use derivation stream with a string comparison
    bool isTruthDxAOD = top::isTruthDxAOD(testFile.get());
    topConfig->setIsTruthDxAOD(isTruthDxAOD);

    // TopDataPreparation - This can be used for any derivation really (not only non-Truth)
    unsigned int DSID = top::getDSID(testFile.get(), topConfig->sgKeyEventInfo());
    topConfig -> setDSID(DSID);
    // now need to get and set the parton shower generator from TopDataPrep
    SampleXsection tdp;
    
    // List of environment variables from which to construct a searchable list of paths (order by priority)
    std::vector<const char*> vec_envvar = {"CRAP","WorkDir_DIR","AnalysisTop_DIR","ROOTCOREDIR"};
    std::string datapath;
    // Construct string list                                                                               
    for (auto var : vec_envvar){
      const char* envvar = getenv(var);
      if (envvar != NULL) datapath += ( std::string(envvar) + "/data:" );
    }
    // Package/filename - XS file we want to use                                                           
    std::string tdp_filename = "TopDataPreparation/XSection-MC15-13TeV.data";
    // Use the path resolver to find the first file in the list of possible paths                          
    std::string fullpath = PathResolver::find_file_from_list (tdp_filename, datapath);
    if (!tdp.readFromFile(fullpath.c_str())) {
      std::cout << "ERROR::TopDataPreparation - could not read file \n";
      std::cout << tdp_filename << "\n";
      exit(1);
    }
    std::cout << "SampleXsection::Found " << fullpath << std::endl;

    // Identify the shower used in the sample for b-tagging weights later
    int ShowerIndex = tdp.getShoweringIndex(DSID);
    std::cout << "DSID: " << DSID << "\t" << "ShowerIndex: " << ShowerIndex << std::endl;
    topConfig->setMapIndex(ShowerIndex);
  }
  return;
}

std::shared_ptr<TFile> CreateOutputFile(){

  // Get global object
  auto* const settings = top::ConfigurationSettings::get();
  // Create the output file
  std::shared_ptr<TFile> outputFile (TFile::Open((settings->value("OutputFilename") + ".tmp").c_str(), "RECREATE"));

  // return
  return outputFile;

}

std::shared_ptr<top::EventSelectionManager> CreateEventSelectionManager(std::shared_ptr<TFile> outputFile, std::shared_ptr<top::TopConfig> topConfig){

  // Get global object
  auto* const settings = top::ConfigurationSettings::get();
  const std::string libraryNames = settings->value("LibraryNames");

  std::cout << libraryNames << std::endl;

  // Setup event-level cuts
  std::shared_ptr<top::EventSelectionManager> eventSelectionManager(new top::EventSelectionManager(settings->selections(), outputFile.get(), libraryNames, topConfig));

  return eventSelectionManager;
}

std::shared_ptr<top::EventSaverBase> CreateEventSaver(std::shared_ptr<TFile> outputFile, std::shared_ptr<top::EventSelectionManager> eventSelectionManager, std::shared_ptr<top::TopConfig> topConfig){

  //A list of extra branches that we create in this code and want to save
  std::vector<std::string> extraBranches;
  eventSelectionManager->addExtraBranches(extraBranches);

  //output file format (and any user-modified code)
  std::shared_ptr<top::EventSaverBase> eventSaver(top::loadEventSaver(topConfig));
  eventSaver->initialize(topConfig, outputFile.get(), extraBranches);

  return eventSaver;
}

std::shared_ptr<top::EventCleaningSelection> CreateEventCleaningSelection(std::shared_ptr<top::TopConfig> topConfig){

  // Retrieve the configuration settings
  auto* const settings = top::ConfigurationSettings::get();

  // EventCleaningSelection
  // Decorates EventInfo with GRL decision (always true for MC)
  // Evaluates all triggers requested from all selections
  // Trigger matching
  // Check for good Tile and LAr calorimeters
  // Let's check this passes before running expensive calibrations
  // If ANY selection does not request a trigger, we won't veto any events
  std::shared_ptr<top::EventCleaningSelection> eventCleaning( new top::EventCleaningSelection("top::EventCleaningSelection") );
  top::check(eventCleaning->setProperty("config", topConfig), "Failed to setProperty of triggerGRLSelection" );
  top::check(eventCleaning->initialize(), "Failed to initialize triggerGRLSelection");
  eventCleaning->setEventSelections(settings->selections());

  return eventCleaning;
}

std::shared_ptr<top::ObjectCollectionMaker> CreateObjectCollectionMaker(std::shared_ptr<top::TopConfig> topConfig){

  // Systematic object collection making
  std::shared_ptr<top::ObjectCollectionMaker> systObjMaker( new top::ObjectCollectionMaker( "top::ObjectCollectionMaker" ) );
  top::check( systObjMaker->setProperty( "config" , topConfig ) , "Failed to setProperty of systObjMaker" );
  // Initialise if not truth DxAOD
  if (!topConfig->isTruthDxAOD()) {
    top::check( systObjMaker->initialize() , "Failed to initialize systObjMaker" );
  }
  return systObjMaker;
}

std::shared_ptr<top::TopObjectSelection> CreateTopObjectSelection(std::shared_ptr<top::TopConfig> topConfig){

  // Setup object definitions
  std::shared_ptr<top::TopObjectSelection> objectSelection(top::loadObjectSelection(topConfig));
  objectSelection->print(std::cout);
  return objectSelection;
}

std::shared_ptr<top::CalcTopPartonHistory> CreateTopPartonHistory(std::shared_ptr<top::TopConfig> topConfig){

  // Retrieve the configuration settings
  auto* const settings = top::ConfigurationSettings::get();

  // Top parton history for MC events
  // This is quite ugly and simple, it will be harmonized with in the future
  // along with all other factory methods (it's not a factory method right now)
  std::shared_ptr<top::CalcTopPartonHistory> topPartonHistory(nullptr);
  if(settings->value("TopPartonHistory") == "ttbar"){
    topPartonHistory = std::shared_ptr<top::CalcTopPartonHistory> ( new top::CalcTtbarPartonHistory( "top::CalcTtbarPartonHistory" ) );
    top::check(topPartonHistory->setProperty( "config" , topConfig ) , "Failed to setProperty of top::CalcTtbarPartonHistory");
  }
  else if(settings->value("TopPartonHistory") == "tb"){
    topPartonHistory = std::shared_ptr<top::CalcTopPartonHistory> ( new top::CalcTbbarPartonHistory( "top::CalcTbbarPartonHistory" ) );
    top::check(topPartonHistory->setProperty( "config" , topConfig ) , "Failed to setProperty of top::CalcTbbarPartonHistory");
  }

  return topPartonHistory;
}

std::shared_ptr<top::PDFScaleFactorCalculator> CreatePDFScaleFactorCalculator(std::shared_ptr<top::TopConfig> topConfig){

  //LHAPDF SF calculation
  std::shared_ptr<top::PDFScaleFactorCalculator> PDF_SF(nullptr);
  PDF_SF = std::shared_ptr<top::PDFScaleFactorCalculator> (new top::PDFScaleFactorCalculator( "top::PDFScaleFactorCalculator" ) );
  if ( topConfig->doLHAPDF() ){
    top::check( PDF_SF->setProperty( "config", topConfig ), "Failed to set config for PDF SF Calculator" );
    top::check( PDF_SF->initialize(), "Failed to initialize PDF SF calculator" );
  }
  return PDF_SF;
}

std::shared_ptr<top::TopEventMaker> CreateTopEventMaker(std::shared_ptr<top::TopConfig> topConfig){

  // make top::Event objects
  std::shared_ptr<top::TopEventMaker> topEventMaker( new top::TopEventMaker( "top::TopEventMaker" ) );
  top::check(topEventMaker->setProperty( "config" , topConfig ) , "Failed to setProperty of top::TopEventMaker");
  // Debug messages?
  // topEventMaker.msg().setLevel(MSG::DEBUG);
  return topEventMaker;
}

std::shared_ptr<top::ScaleFactorCalculator> CreateScaleFactorCalculator(std::shared_ptr<top::TopConfig> topConfig){

  std::shared_ptr<top::ScaleFactorCalculator> topScaleFactors( new top::ScaleFactorCalculator("top::ScaleFactorCalculator") );
  top::check(topScaleFactors->setProperty( "config" , topConfig ) , "Failed to setProperty of top::ScaleFactorCalculator");
  top::check(topScaleFactors->initialize(),"Failed to initialize  top::ScaleFactorCalculator");
  return topScaleFactors;
}

std::shared_ptr<top::TopFakesMMWeightCalculator> CreateTopFakesMMWeightCalculator(std::shared_ptr<top::EventSelectionManager> eventSelectionManager, std::shared_ptr<top::TopConfig> topConfig){

  // Retrieve the configuration settings
  auto* const settings = top::ConfigurationSettings::get();

  ///-- weights for matrix-method fakes estimate --///
  std::shared_ptr<top::TopFakesMMWeightCalculator> topfakesMMWeights(nullptr);
  if (!topConfig->isMC() && topConfig->doLooseEvents() && topConfig->doFakesMMWeights()) {
    topfakesMMWeights = std::shared_ptr<top::TopFakesMMWeightCalculator>( new top::TopFakesMMWeightCalculator() );
    top::check(topfakesMMWeights->setProperty( "config" , topConfig ) , "Failed to setProperty of top::TopFakesMMWeightCalculator");
    top::check(topfakesMMWeights->initialize(),"Failed to initialize  top::TopFakesMMWeightCalculator");
    for (auto sel : settings->selections()) {
      top::check(topfakesMMWeights->setSelectionConfigs(sel.m_name, eventSelectionManager->GetFakesMMConfigs(sel.m_name)),"Failed to set the selection FakesMMConfigs for selection"+sel.m_name);
    }
  }

  return topfakesMMWeights;
}

std::shared_ptr<top::TopToolStore> CreateTopToolStore(std::shared_ptr<top::TopConfig> topConfig){

  // Setup all asg::AsgTools
  std::shared_ptr<top::TopToolStore> topTools( new top::TopToolStore("top::TopToolStore") );
  top::check(topTools->setProperty("config", topConfig), "Failed to setProperty of topTools");
  top::check(topTools->initialize(), "Failed to initialize topTools");

  return topTools;

}

std::shared_ptr<top::ParticleLevelLoader> CreateParticleLevelLoader(std::shared_ptr<top::TopConfig> topConfig){

  // The loader tool for top::ParticleLevelEvent objects
  std::shared_ptr<top::ParticleLevelLoader> particleLevelLoader ( new top::ParticleLevelLoader(topConfig) );

  return particleLevelLoader;

}

std::shared_ptr<top::UpgradeObjectLoader> CreateUpgradeObjectLoader(std::shared_ptr<top::TopConfig> topConfig){

  // The loader tool for Upgrade objects
  std::shared_ptr<top::UpgradeObjectLoader>  upgradeLoader ( new top::UpgradeObjectLoader(topConfig) );

  return upgradeLoader;
}

std::shared_ptr<MetadataTree> CreateMetadataTree(std::shared_ptr<TFile> outputFile, std::shared_ptr<top::TopConfig> topConfig){

  // This new class will maintain the metadata tree and information
  std::shared_ptr<MetadataTree> metadataTree (new MetadataTree(outputFile));
  metadataTree->AssignVariables(topConfig);
  return metadataTree;
}

void FixConfiguration(std::shared_ptr<top::TopConfig> topConfig){

  // Fixing the config to stop any future changes to this object
  topConfig->fixConfiguration();

  // OK let's printout the TopConfig
  std::cout << *topConfig << "\n";
}

void RunEventLoop(std::vector<std::string> filenames, AnalysisTopTools analysisTopTools){

  // Get global object
  auto* const settings = top::ConfigurationSettings::get();
  // Get TopConfig as used a lot
  std::shared_ptr<top::TopConfig> topConfig = analysisTopTools.topConfig;

  // Here we will run our event loop
  // Open the files (to check they exist) and count how many events we have
  unsigned int totalYield = top::checkFiles(filenames);

  // The analysis loop
  std::cout << "Starting event loop\n";

  // I/O Performance stats
  // Summary gives a summary
  // Full gives detailed info on each collection in the file
  unsigned int doPerfStats(0);
  if (settings->value("PerfStats") == "Summary"){
    doPerfStats = 1;
    xAOD::PerfStats::instance().start();  // start Perfstats timer
  }
  if (settings->value("PerfStats") == "Full"){
    doPerfStats = 2;
    xAOD::PerfStats::instance().start();  // start Perfstats timer
  }

  // Event counters
  unsigned int totalYieldSoFar    = 0;
  unsigned int skippedEventsSoFar = 0;
  unsigned int eventSavedReco(0),eventSavedRecoLoose(0),eventSavedTruth(0),eventSavedParticle(0);

  // Loop on the input files
  for (const auto& filename : filenames) {
    if ( topConfig->numberOfEventsToRun() != 0 && totalYieldSoFar >= topConfig->numberOfEventsToRun() ) break;
    std::cout << "Opening " << filename << std::endl;
    std::unique_ptr<TFile> inputFile(TFile::Open(filename.c_str()));

    // Now we store the name of the current file in the metadata tree
    analysisTopTools.metadataTree->filename = std::string(inputFile->GetName());

    // The derivation framework is making files with an empty collection tree which TEvent can now handle
    // However, at least for data, this is causing problems with the trigger menu
    // skip the file, after the meta data access above
    const TTree* const collectionTree = dynamic_cast<TTree* > (inputFile->Get("CollectionTree"));
    if (!collectionTree && !topConfig->isMC()) {
      std::cout << "No CollectionTree found, skipping file\n";
      continue;
    }

    // Check the file is readable
    top::check(analysisTopTools.xaodEvent->readFrom(inputFile.get()), "xAOD::TEvent readFrom failed");
    // Get information from bookkeeper/for metadata for _each_ input file
    ProcessBookkeeper(analysisTopTools);
    // Work out if we are skipping events
    const unsigned int entries = analysisTopTools.xaodEvent->getEntries();
    // Store the number of entries in this file
    analysisTopTools.metadataTree->totalEventsInFiles = entries;
    // Check the information loaded in our new object
    analysisTopTools.metadataTree->Print();

    unsigned int firstEvent = 0;
    if(topConfig->numberOfEventsToSkip() > skippedEventsSoFar){
      firstEvent = topConfig->numberOfEventsToSkip()-skippedEventsSoFar;
      skippedEventsSoFar += entries < firstEvent ? entries : firstEvent;
    }

    // Start processing events
    for (unsigned int entry = firstEvent; entry < entries; ++entry, ++totalYieldSoFar) {     

      // Work out if we stop processing
      if (topConfig->numberOfEventsToRun() != 0 && totalYieldSoFar >= topConfig->numberOfEventsToRun() ) break;
      // Print out a heartbeat message
      if (entry % 100 == 0){
        std::cout << "    Processing event " << totalYieldSoFar << " / " << totalYield << " (current file: " << entry << " / " << entries << ")" << std::endl;
      }
      // Clear the TStore - get rid of the last events objects
      xAOD::TActiveStore::store()->clear();
      // Get the event
      analysisTopTools.xaodEvent->getEntry(entry);

      // Need to run pileup calculation first as required inside truth event
      if (topConfig->doPileupReweighting() && !topConfig->isTruthDxAOD()) {
        top::check(analysisTopTools.topScaleFactorCalculator->executePileup(), "Failed to execute pileup reweighting");
      }

      // Process truth
      ProcessTruthEvent(analysisTopTools, eventSavedTruth);
      // Process upgrade
      ProcessUpgradeEvent(analysisTopTools);
      // Process particle level event
      ProcessParticleEvent(analysisTopTools, eventSavedParticle);
      // Initialise LHAPDF branches (only occurs on first event)
      InitialiseLHAPDFWeights(analysisTopTools, (totalYieldSoFar == 0));
      // Recalculate LHE3 weights OTF (if required, only on first event)
      ReprocessLHE3Weights(analysisTopTools, (totalYieldSoFar == 0));
      // Get next event if we are running on TruthDxAOD
      if ( topConfig->isTruthDxAOD() )
        continue;
      // Run preselection and cutflows - if failed preselection, get next event
      if( !PassPreselection(analysisTopTools) )
        continue;
      // Process reconstruction level event which passed preselection - Tight selection
      if (topConfig->doTightEvents()) {
        ProcessRecoEventTight(analysisTopTools, eventSavedReco);
      }
      // Process reconstruction level event which passed preselection - Loose selection
      if (topConfig->doLooseEvents()) {
        ProcessRecoEventLoose(analysisTopTools, eventSavedRecoLoose);
      }
      // Needed for xAOD output, all systematics go into the same TTree
      analysisTopTools.topEventSaver->saveEventToxAOD();
    } // End events in file
    analysisTopTools.metadataTree->Update();
    // Handle the LHAPDF weights after processing current file
    ProcessLHAPDFWeights(analysisTopTools);
    // Fill our new metadata tree object at the end of the event loop (in case we recalculate LHE3)
    analysisTopTools.metadataTree->Fill();

  } // End list of files
  return;
}

void ProcessBookkeeper(AnalysisTopTools analysisTopTools){

  // Get the TEvent
  //xAOD::TEvent xaodEvent = analysisTopTools.xaodEvent;
  //xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);

  // Get TopConfig
  std::shared_ptr<top::TopConfig> topConfig = analysisTopTools.topConfig;

  // We need to access the bookkeeper once per input file and fill the metadata tree object with the info.
  // Then we have it filled once per file and we can use dummy values if the file is empty.
  // But in this case, do we still have the object and dsid?

  // I copy just the info from normal code here but will need to look at what is needed
  //Get some of event weights before derivation
  double initialSumOfWeightsInThisFile = 0;
  ULong64_t initialEvents              = 0;

  // vector of LH3 weights and teir names
  std::vector<float>       initialSumOfWeights_mc_generator_weightsInThisFile;
  std::vector<std::string> initialSumOfWeights_mc_generator_namesInThisFile;

  // vector of PDF weights and their names
  std::vector<float>       initialSumOfWeights_pdf_weightsInThisFile;
  std::vector<std::string> initialSumOfWeights_pdf_namesInThisFile;

  // See https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AnalysisMetadata#Event_Bookkeepers
  const xAOD::CutBookkeeperContainer* cutBookKeepers = 0;
  // Try to get the cut bookkeepers.
  // <tom.neep@cern.ch> (4/4/16): Not there in DAOD_TRUTH1?
  // If we can't get them and we are running on TRUTH then carry on,
  // but anything else is bad!
  if (!analysisTopTools.xaodEvent->retrieveMetaInput(cutBookKeepers, "CutBookkeepers")) {
    top::check(topConfig->isTruthDxAOD(), "Failed to retrieve cut book keepers in TRUTH DxAOD");
  }
  else {
    int maxCycle = -1;
    std::vector<int> maxCycle_LH3;// for LHE3 weights
    // for the sum of number of events and of mc event weights
    for ( auto cbk : *cutBookKeepers ) {
      // skip RDO and ESD numbers, which are nonsense; and
      // skip the derivation number, which is the one after skimming
      // we want the primary xAOD numbers
      if (cbk->inputStream() == "StreamAOD" && cbk->name() == "AllExecutedEvents") {
        double sumOfEventWeights = cbk->sumOfEventWeights();
        ULong64_t nEvents        = cbk->nAcceptedEvents();
        if (cbk->cycle() > maxCycle) {
          initialSumOfWeightsInThisFile = sumOfEventWeights;
          initialEvents                 = nEvents;
          maxCycle                      = cbk->cycle();
        }
      }
      else if (cbk->inputStream() == "StreamAOD" && cbk->name().find("LHE3Weight_") != std::string::npos && topConfig->doMCGeneratorWeights()) {
        double sumOfEventWeights = cbk->sumOfEventWeights();
        std::string name         = cbk->name();
        name.erase(0,11); // remove the "LHE3Weight_"
        // is it a new name? If yes append it to the vector of names
        // if not no need, but we must check the corresponding entry for the sum of weights exist
        auto pos_name = std::find(initialSumOfWeights_mc_generator_namesInThisFile.begin(), initialSumOfWeights_mc_generator_namesInThisFile.end(), name);
        // If it is a new name
        if (pos_name == initialSumOfWeights_mc_generator_namesInThisFile.end()) {
          initialSumOfWeights_mc_generator_namesInThisFile.push_back(name);
          initialSumOfWeights_mc_generator_weightsInThisFile.push_back(sumOfEventWeights);
          maxCycle_LH3.push_back(cbk->cycle());
        }
        // Already existing name
        else if (cbk->cycle() > maxCycle_LH3.at(pos_name-initialSumOfWeights_mc_generator_namesInThisFile.begin())) {
          initialSumOfWeights_mc_generator_weightsInThisFile.at(pos_name-initialSumOfWeights_mc_generator_namesInThisFile.begin()) = sumOfEventWeights;
          maxCycle_LH3.at(pos_name-initialSumOfWeights_mc_generator_namesInThisFile.begin())                                       = cbk->cycle();
        }
      }
      else continue;
    }
  }
  // We now want to fill the metadataTree once per file, not at the end of the processing
  // Will allow for better bookkeeping (if we also store the filename)
  analysisTopTools.metadataTree->totalEventsWeighted = initialSumOfWeightsInThisFile;
  analysisTopTools.metadataTree->totalEvents         = initialEvents;

  // now we must fill two vectors in sync
  if (topConfig->doMCGeneratorWeights()) {
    if (analysisTopTools.metadataTree->totalEventsWeighted_LHE3.size() != 0 ) {
      if (analysisTopTools.metadataTree->totalEventsWeighted_LHE3.size() != initialSumOfWeights_mc_generator_weightsInThisFile.size()
          || analysisTopTools.metadataTree->names_LHE3.size() != initialSumOfWeights_mc_generator_namesInThisFile.size()
          || analysisTopTools.metadataTree->names_LHE3.size() != analysisTopTools.metadataTree->totalEventsWeighted_LHE3.size() ) {
        std::cout << "Ouch: strange inconsistency of vector sizes in sum of LHE3 weights calculation. There is an issue somewhere." << std::endl;
        std::cout << "Exiting...." << std::endl;
        std::exit(1);
      }
      for (unsigned int i_genweights = 0; i_genweights < initialSumOfWeights_mc_generator_namesInThisFile.size(); i_genweights++) {
        if (analysisTopTools.metadataTree->names_LHE3.at(i_genweights) != initialSumOfWeights_mc_generator_namesInThisFile.at(i_genweights)) {
          std::cout << "Ouch: strange inconsistency in the vector of weight names in sum of LHE3 weights calculation. There is an issue somewhere." << std::endl;
          std::cout << "Exiting...." << std::endl;
          std::exit(1);
        }
        else {
          analysisTopTools.metadataTree->totalEventsWeighted_LHE3.at(i_genweights) = initialSumOfWeights_mc_generator_weightsInThisFile.at(i_genweights);
        }
      }
    }
    else {
      for (unsigned int i_genweights = 0; i_genweights < initialSumOfWeights_mc_generator_namesInThisFile.size(); i_genweights++) {
        analysisTopTools.metadataTree->names_LHE3.push_back(initialSumOfWeights_mc_generator_namesInThisFile.at(i_genweights));
        analysisTopTools.metadataTree->totalEventsWeighted_LHE3.push_back(initialSumOfWeights_mc_generator_weightsInThisFile.at(i_genweights));
      }
    }
    if (!analysisTopTools.metadataTree->names_LHE3.empty()) {
      std::cout << "The sum of weights for the following LHE3 weights were retrieved from the input file:"<<std::endl;
      for (std::string s : analysisTopTools.metadataTree->names_LHE3) std::cout<<s<<" ";
      std::cout<< std::endl;
      analysisTopTools.metadataTree->recalc_LHE3 = false;
    }
    else {
      std::cout << "No sum of LHE3 weights could be found in meta-data. Will try to recompute these sums."<<std::endl;
      std::cout << "It will only work on un-skimmed derivations, and it will be impossible to know the name of these weights."<<std::endl;
      analysisTopTools.metadataTree->recalc_LHE3 = true;
    }
  }

  // Get the DSID for the MetadataTree
  if (analysisTopTools.topConfig->isMC()) {
    const xAOD::EventInfo *ei(nullptr);
    top::check(analysisTopTools.xaodEvent->retrieve(ei, analysisTopTools.topConfig->sgKeyEventInfo()), "xAOD::TEvent retrieve failed to get EventInfo");
    //mcChannelNumber only available in MC simulation
    if (ei->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      analysisTopTools.metadataTree->dsid    = ei->mcChannelNumber();
      analysisTopTools.metadataTree->dsidPdf = ei->mcChannelNumber();
    }
  }


  // We could fill this here but maybe best at the end of truth loop because if LHE3 needs to be recalculated OTF, we cannot avoid it
  return;

}

void ProcessTruthEvent(AnalysisTopTools analysisTopTools, unsigned int &eventSavedTruth){

  // Get the config object
  std::shared_ptr<top::TopConfig> topConfig = analysisTopTools.topConfig;

  // Return straight away if this is data
  if(!topConfig->isMC()) return;

  // Save, if requested, MC truth block, PDFInfo, TopPartons
  // This will be saved for every event

  // Run topPartonHistory
  if ( topConfig->doTopPartonHistory() )
    top::check( analysisTopTools.topCalcTopPartonHistory->execute() , "Failed to execute topPartonHistory" );

  // calculate PDF weights
  if( topConfig->doLHAPDF() )
    top::check( analysisTopTools.topPDFScaleFactorCalculator->execute(), "Failed to execute PDF SF");

  // Save truth event and count
  analysisTopTools.topEventSaver->saveTruthEvent();
  ++eventSavedTruth;

  return;
}

void ProcessUpgradeEvent(AnalysisTopTools analysisTopTools){

  // Get the config object
  std::shared_ptr<top::TopConfig> topConfig = analysisTopTools.topConfig;

  // Running upgrade events on truth derivation and with appropriate HLLHC flag
  if (topConfig->isTruthDxAOD() && topConfig->HLLHC()) {
    top::ParticleLevelEvent upgradeEvent = analysisTopTools.topUpgradeObjectLoader->load();

    // Determine if we keep this event
    const bool saveEventInOutputFile = analysisTopTools.topEventSelectionManager->applyUpgradeLevel( upgradeEvent );

    // Save the event
    if ( saveEventInOutputFile ) {
      analysisTopTools.topEventSaver->saveUpgradeEvent( upgradeEvent );
    }
  }
  return;
}

void ProcessParticleEvent(AnalysisTopTools analysisTopTools, unsigned int &eventSavedParticle){

  // Get the config object
  std::shared_ptr<top::TopConfig> topConfig = analysisTopTools.topConfig;

  // Return straight away if this is data
  if(!topConfig->isMC()) return;

  // Particle level analysis, saved only for truth events passing fiducial selection

  // If the truth loader is active, perform truth loading.
  if ( analysisTopTools.topParticleLevelLoader->active() && !topConfig->HLLHC() ){

    // Get the top::TruthEvent for the current event
    top::ParticleLevelEvent particleLevelEvent = analysisTopTools.topParticleLevelLoader->load();

    // Determine if we keep this event
    const bool saveEventInOutputFile = analysisTopTools.topEventSelectionManager->applyParticleLevel( particleLevelEvent );

    // Save the event
    if ( saveEventInOutputFile ) {
      analysisTopTools.topEventSaver->saveParticleLevelEvent( particleLevelEvent );
      ++eventSavedParticle;
    }
  }
  return;
}

void InitialiseLHAPDFWeights(AnalysisTopTools analysisTopTools, bool firstEvent){

  // Only do this on first event (but needs to have event loaded so inside loop)
  if( !firstEvent ) return;

  // Get the config object
  std::shared_ptr<top::TopConfig> topConfig = analysisTopTools.topConfig;

  // On the first event, set the vectors up if the info is available
  if (topConfig->isMC() && topConfig->doLHAPDF()) {
    const xAOD::TruthEventContainer* truthEvent(nullptr);
    top::check( analysisTopTools.xaodEvent->retrieve(truthEvent, topConfig->sgKeyTruthEvent()) , "Failed to retrieve truth PDF info" );
    for( auto tePtr : *truthEvent ) {
      for (auto &pdf : analysisTopTools.metadataTree->totalEventsPdfWeighted) {
        if( tePtr->isAvailable< std::vector<float> >("AnalysisTop_"+pdf.first+"_Weights") ) {
          pdf.second->resize(tePtr->auxdata< std::vector<float> >("AnalysisTop_"+pdf.first+"_Weights").size());
        }
      }
    }
  }
  return;
}

void ReprocessLHE3Weights(AnalysisTopTools analysisTopTools, bool firstEvent){

  // This needs to be run each event if we are recalculating the weights OTF

  // Get the config object
  std::shared_ptr<top::TopConfig> topConfig = analysisTopTools.topConfig;

  // On the first event, set the size of the vector of sum of LHE3 weights in case it needs to be calculated on the fly
  if (topConfig->isMC() && topConfig->doMCGeneratorWeights()) {
    const xAOD::TruthEventContainer* truthEvent(nullptr);
    top::check( analysisTopTools.xaodEvent->retrieve(truthEvent, topConfig->sgKeyTruthEvent()) , "Failed to retrieve LHE3 weights from truth event" );
    unsigned int weightsSize = truthEvent->at(0)->weights().size();
    if ( analysisTopTools.metadataTree->recalc_LHE3 ) {
      if ( firstEvent ) {
        analysisTopTools.metadataTree->totalEventsWeighted_LHE3_temp.resize(weightsSize);
        for (unsigned int i_LHE3 = 0; i_LHE3<weightsSize; i_LHE3++) {
          analysisTopTools.metadataTree->totalEventsWeighted_LHE3_temp.at(i_LHE3) = truthEvent->at(0)->weights().at(i_LHE3);
        }
        analysisTopTools.metadataTree->names_LHE3.resize(weightsSize);
        std::fill(analysisTopTools.metadataTree->names_LHE3.begin(),analysisTopTools.metadataTree->names_LHE3.end(),"?");
      }
      else {
        for (unsigned int i_LHE3 = 0; i_LHE3<weightsSize; i_LHE3++) {
          analysisTopTools.metadataTree->totalEventsWeighted_LHE3_temp.at(i_LHE3) += truthEvent->at(0)->weights().at(i_LHE3);
        }
      }
    }
    // Otherwise we just make a quick check
    else if (weightsSize != analysisTopTools.metadataTree->names_LHE3.size()) {
      // don't recalc sum of weights, but cross-check the size of the vectors
      std::cout << "Ouch: strange inconsistency in the sum of LHE3 weights vectors from the meta-data and the vector of LHE3 weights in the TruthEventContainer." << std::endl;
      std::cout << "It should be the same; since it's not, it's pointless to continue.";
      std::cout << "Exiting...." << std::endl;
      std::exit(1);
    }
  }
  return;
}

bool PassPreselection(AnalysisTopTools analysisTopTools){

  // Get the config object
  std::shared_ptr<top::TopConfig> topConfig = analysisTopTools.topConfig;

  // Weights required to normalise the cut flows (Pile up and MC event weight)
  float mcEventWeight(1.), pileupWeight(1.), zvtxWeight(1.);
  if (topConfig->isMC()){
    mcEventWeight = analysisTopTools.topScaleFactorCalculator->mcEventWeight();
  }

  if (topConfig->doPileupReweighting() && !topConfig->isTruthDxAOD()) {
    pileupWeight = analysisTopTools.topScaleFactorCalculator->pileupWeight();
  }

  ///-- We veto events that do not pass the GRL, trigger or have bad calorimeters --///
  ///-- No point calibrating and cutting on events that fail these cuts --///
  ///-- We only apply a veto if ALL selectors request a cut --///
  ///-- And if the user selects "OutputEvents SelectedEvents" --///

  ///-- Count initial events --///
  analysisTopTools.topEventSelectionManager->countInitial(mcEventWeight,pileupWeight,zvtxWeight);

  ///-- Does event pass the GRL? (always true for MC) --///
  ///-- Only veto events when ALL user selectors request GRL --///
  bool passGRLVeto = analysisTopTools.topEventCleaningSelection->applyGRL();
  if (!passGRLVeto)
    return false;
  analysisTopTools.topEventSelectionManager->countGRL(mcEventWeight,pileupWeight,zvtxWeight);

  ///-- Are the Tile and LAr calorimeters in a good state? (always true for MC) --///
  ///-- Only veto events when ALL user selectors request GOODCALO --///
  bool passGoodCalo = analysisTopTools.topEventCleaningSelection->applyGoodCalo();
  if (!passGoodCalo)
    return false;
  analysisTopTools.topEventSelectionManager->countGoodCalo(mcEventWeight,pileupWeight,zvtxWeight);

  ///-- Do we have a Primary Vertex? --///
  ///-- Only veto events when ALL user selectors request PRIVTX -- ///
  bool passPriVtx = analysisTopTools.topEventCleaningSelection->applyPrimaryVertex();
  if (!passPriVtx)
    return false;
  analysisTopTools.topEventSelectionManager->countPrimaryVertex(mcEventWeight,pileupWeight,zvtxWeight);

  ///-- Wondering which triggers are available ??? --///
  ///-- Uncomment this line and get ready for a LOT of output --///
  // eventCleaning->printTriggers();

  ///-- Does event pass any trigger ? --///
  ///-- Only veto events when ALL user selectors request TRIGDEC --///
  ///-- And all trigger fail --///
  bool passAnyTriggerVeto = analysisTopTools.topEventCleaningSelection->applyTrigger();
  if (!passAnyTriggerVeto)
    return false;

  ///-- Calibrate objects and make all required systematic copies --///
  top::check( analysisTopTools.topObjectCollectionMaker->execute() , "Failed to execute systObjMaker" );

  ///-- Object selection (e.g. good electrons, muons, jets etc.). Event selection cuts comes later --///
  top::check( analysisTopTools.topObjectSelection->execute() , "Failed to execute objectSelection" );

  ///-- Recalculate MissingET based on object selection --///
  top::check( analysisTopTools.topObjectCollectionMaker->recalculateMET() , "Failed to recalculateMET with systObjMaker" );

  ///-- Scale Factor calculation --///
  if (topConfig->isMC())
    top::check( analysisTopTools.topScaleFactorCalculator->execute() , "Failed to calculate scale factors" );

  return true;
}

void ProcessRecoEventTight(AnalysisTopTools analysisTopTools, unsigned int &eventSavedReco){

  // Get the config object
  std::shared_ptr<top::TopConfig> topConfig = analysisTopTools.topConfig;
  // Get the event saver
  top::EventSaverBase* topEventSaver = analysisTopTools.topEventSaver.get();

  ///-- Loop over all systematics and make a "top::Event" for each --///
  const xAOD::SystematicEventContainer* allSystematics = analysisTopTools.topEventMaker->systematicEvents( topConfig->sgKeyTopSystematicEvents() );
  for (auto currentSystematic : *allSystematics) {
    ///-- Make a top::Event --///
    top::Event topEvent = analysisTopTools.topEventMaker->makeTopEvent( *currentSystematic );
    ///-- Apply event selection --///
    const bool passAnyEventSelection = analysisTopTools.topEventSelectionManager->apply( topEvent,*currentSystematic );
    currentSystematic->auxdecor<char>(topConfig->passEventSelectionDecoration()) = passAnyEventSelection ? 1 : 0;
    topEvent.m_saveEvent = passAnyEventSelection;
    ///-- Debug --///
    //DebugTopEvent(topEvent);
    ///-- Save event - we defer to eventSaver the decision to write or not --///
    //analysisTopTools.topEventSaver->saveEvent(topEvent);
    topEventSaver->saveEvent(topEvent);
    
    ///-- For printout at the end of job, only count nominal events --///
    if (passAnyEventSelection && currentSystematic->hashValue() == topConfig->nominalHashValue())
    ++eventSavedReco;

  }
  ///-- End loop over all tight calibration systematics --///
  return;

}

void ProcessRecoEventLoose(AnalysisTopTools analysisTopTools, unsigned int &eventSavedRecoLoose){

  // Get the config object
  std::shared_ptr<top::TopConfig> topConfig = analysisTopTools.topConfig;

  ///-- Loop over all Loose systematics and make a "top::Event" for each --///
  const xAOD::SystematicEventContainer* allSystematicsLoose = analysisTopTools.topEventMaker->systematicEvents( topConfig->sgKeyTopSystematicEventsLoose() );
  for (auto currentSystematic : *allSystematicsLoose) {
    ///-- Make a top::Event --///
    top::Event topEvent = analysisTopTools.topEventMaker->makeTopEvent( *currentSystematic );
    ///-- Apply event selection --///
    const bool passAnyEventSelection = analysisTopTools.topEventSelectionManager->apply(topEvent,*currentSystematic );
    currentSystematic->auxdecor<char>(topConfig->passEventSelectionDecoration()) = passAnyEventSelection ? 1 : 0;
    topEvent.m_saveEvent = passAnyEventSelection;
    ///-- weights for matrix-method fakes estimate, only for nominal --///
    if (!topConfig->isMC() && topConfig->doFakesMMWeights() && currentSystematic->hashValue() == topConfig->nominalHashValue()) {
      top::check( analysisTopTools.topFakesMMWeightCalculator->execute(&topEvent) , "Failed to execute fakes mmweight calculation");
    }
    ///-- Save event - we defer to eventSaver the decision to write or not --///
    analysisTopTools.topEventSaver->saveEvent(topEvent);

    ///-- For printout at the end of job, only count nominal events --///
    if (passAnyEventSelection && currentSystematic->hashValue() == topConfig->nominalHashValue())
    ++eventSavedRecoLoose;
  }
  ///-- End loop over all loose calibration systematics --///
  return;

}

void ProcessLHAPDFWeights(AnalysisTopTools analysisTopTools){

  // Get the config object
  std::shared_ptr<top::TopConfig> topConfig = analysisTopTools.topConfig;

  // Get a bookkeeper object
  const xAOD::CutBookkeeperContainer* cutBookKeepers = 0;

  // Do it at the end, so we can get the DS id from the first event
  // Notice this is different from the normal sum of weights: all entries matter, not only the highest per file
  if (topConfig->doLHAPDF()) {
    bool pdfInCBK = false;
    if (analysisTopTools.xaodEvent->retrieveMetaInput( cutBookKeepers, "CutBookkeepers" )) {
      // try to get the first entry
      std::string pdf_set = analysisTopTools.metadataTree->totalEventsPdfWeighted.begin()->first;
      std::string p = pdf_set+"_0";
      for ( auto cbk : *cutBookKeepers ) {
        std::string pdfName = cbk->name();
        if (p != pdfName) continue;
        analysisTopTools.metadataTree->pdfMetadataExists = true;
        pdfInCBK = true;
        // Reload to set iterator back to original state
        if (!analysisTopTools.xaodEvent->retrieveMetaInput(cutBookKeepers, "CutBookkeepers")) {}
        break;
      }
    }
    // if the pdf info was in the CBK the PDFSumOfWeights retrieve never happens,
    // and cutBookKeepers, will keep its reference to the main CBK
    if (!pdfInCBK) { // only try this if the info is not in the CBK -- this is legacy stuff
      if (!analysisTopTools.xaodEvent->retrieveMetaInput( cutBookKeepers, "PDFSumOfWeights" )) {
        analysisTopTools.metadataTree->pdfMetadataExists = false;
      }
    }
    if (analysisTopTools.metadataTree->pdfMetadataExists && topConfig->saveLHAPDFEvent() && topConfig->baseLHAPDF().empty()) {
      // just make a copy of it, but not all derivations have it, so just ignore it if it is not there
      for (auto &pdfentry : analysisTopTools.metadataTree->totalEventsPdfWeighted) {
        std::string pdf_set = pdfentry.first;
        for (size_t n = 0; n < analysisTopTools.metadataTree->totalEventsPdfWeighted[pdf_set]->size(); ++n) {
          std::string p = pdf_set+"_"+std::to_string(n);
          bool foundPdf = false;
          for ( auto cbk : *cutBookKeepers ) {
            std::string pdfName = cbk->name();
            if (p != pdfName)
            continue;
            analysisTopTools.metadataTree->totalEventsPdfWeighted[pdf_set]->at(n) = cbk->sumOfEventWeights();
            foundPdf = true;
            break;
          }
          if (!foundPdf) { // asked for PDF weighting, the PDF metadata exists, but this particular PDF is missing
            // crash hard
            std::cout << "The force is not strong with us, young Padawan ..." << std::endl;
            std::cout << "You want to save weights to do PDF reweighting using '" << pdf_set << "', which I figure has " << analysisTopTools.metadataTree->totalEventsPdfWeighted[pdf_set]->size() << " sets on it." << std::endl;
            std::cout << "There is metadata information for the sum of MC*PDF weights in PDFSumOfWeights, but none seem to refer to '" << p << "' therefore I do not know how to estimate the sum of weights before acceptance for this configuration." << std::endl;
            std::cout << "May the force be with you in your next attempt." << std::endl;
            std::exit(1);
          }
        }
      }
      //analysisTopTools.metadataTree->sumPdfWeights->Fill();
    }
    // Now things relating to the PDF tool
    // Finalise tool (need to initalise it at the start of processing file)
    top::check( analysisTopTools.topPDFScaleFactorCalculator->finalize() , "Failed to finalize PDF SF" );
    // Save sum of weights of PDF variations
    // Only do this if the metadata is not available
    if (analysisTopTools.metadataTree->totalEventsInFiles != analysisTopTools.metadataTree->totalEvents && !analysisTopTools.metadataTree->pdfMetadataExists) {
      std::cout << "These are not the droids you are looking for." << std::endl;
      std::cout << "There are " << analysisTopTools.metadataTree->totalEventsInFiles << " events in the input files, but the metadata tells me there were " << analysisTopTools.metadataTree->totalEvents << "before skimming." << std::endl;
      std::cout << "This means you are running on skimmed derivations. You also want me to do PDF reweighting, but no PDF metadata exists!" << std::endl;
      std::cout << "I don't know how to tell you the sum of MC*PDF weights before skimming. I will explode now in 3, 2, 1 ..." << std::endl;
      std::exit(1);
    }
    // save recomputed sum weights
    if( (!topConfig->baseLHAPDF().empty() || !analysisTopTools.metadataTree->pdfMetadataExists) ) {
      for( auto& pdf_result : topConfig->getLHAPDFResults() )
      *(analysisTopTools.metadataTree->totalEventsPdfWeighted[pdf_result.first]) = pdf_result.second;
    }
    // Reinitialise the PDF SF tool before next file is processed [CHECK]
    top::check( analysisTopTools.topPDFScaleFactorCalculator->initialize(), "Failed to reinitialize PDF SF calculator" );
  } //doLHAPDF
  return;
}

void DebugTopEvent(top::Event topEvent){
  std::cout << "TopEvent :: Debug" << std::endl;
  std::cout << "Hash        " << topEvent.m_hashValue   << std::endl;
  std::cout << "Index       " << topEvent.m_ttreeIndex  << std::endl;
  std::cout << "Loose       " << topEvent.m_isLoose     << std::endl;
  std::cout << "Save        " << topEvent.m_saveEvent   << std::endl;
  // Need to check actual xAOD objects which are meant to be linked

}

void DebugAnalysisTopTools(AnalysisTopTools analysisTopTools){
  std::cout << " - DebugAnalysisTopTools - Reference Counter -" << std::endl;
  std::cout << " - topConfig                 " << analysisTopTools.topConfig.use_count() << std::endl;
  std::cout << " - topEventSelectionManager  " << analysisTopTools.topEventSelectionManager.use_count() << std::endl;
  std::cout << " - topEventSaver             " << analysisTopTools.topEventSaver.use_count() << std::endl;
  std::cout << " - topEventCleaningSelection " << analysisTopTools.topEventCleaningSelection.use_count() << std::endl;
  std::cout << " - topObjectCollectionMaker  " << analysisTopTools.topObjectCollectionMaker.use_count() << std::endl;
  std::cout << " - topEventMaker             " << analysisTopTools.topEventMaker.use_count() << std::endl;
  std::cout << " - topScaleFactorCalculator  " << analysisTopTools.topScaleFactorCalculator.use_count() << std::endl;
  std::cout << " - outputFile                " << analysisTopTools.outputFile.use_count() << std::endl;
  
  return;
}
