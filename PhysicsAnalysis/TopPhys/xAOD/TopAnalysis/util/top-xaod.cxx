/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "TopAnalysis/AnalysisTrackingHelper.h"
#include "TopAnalysis/EventSelectionManager.h"
#include "TopAnalysis/Tools.h"
#include "TopCPTools/TopToolStore.h"
#include "TopAnalysis/ObjectLoaderBase.h"
#include "TopAnalysis/EventSaverBase.h"

#include "TopEvent/EventTools.h"
#include "TopEvent/TopEventMaker.h"
#include "TopEvent/SystematicEvent.h"

#include "TopObjectSelectionTools/TopObjectSelection.h"
#include "TopObjectSelectionTools/EventCleaningSelection.h"

#include "TopPartons/CalcTtbarPartonHistory.h"
#include "TopPartons/CalcTtbarLightPartonHistory.h"
#include "TopPartons/CalcTbbarPartonHistory.h"
#include "TopPartons/CalcWtbPartonHistory.h"
#include "TopPartons/CalcTTZPartonHistory.h"
#include "TopPartons/CalcTopPartonHistory.h"
#include "TopPartons/CalcTtbarGammaPartonHistory.h"

#include "TopParticleLevel/ParticleLevelLoader.h"

#include "TopDataPreparation/SampleXsection.h"

#include "TopHLUpgrade/UpgradeObjectLoader.h"

// to disable the sending of file access statistics
#include "xAODRootAccess/tools/TFileAccessTracer.h"

// Path resolver
#include "PathResolver/PathResolver.h"

/**
 * @file The main executable.
 *
 * @brief Entry point code to perform object and event selection and write-out a
 * few histograms and / or a tree in xAOD format.
 *
 * @return 0 If everything is okay.
 */
int main(int argc, char** argv) {
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

    // to disable the sending of file access statistics
    xAOD::TFileAccessTracer::enableDataSubmission( false );

    //magic xAOD stuff
    //true = fail at even the slightest xaod issue
    top::xAODInit(true);
    StatusCode::enableFailure();

    xAOD::TStore store;

    std::cout << "Configuration Files:\n";
    std::string settingsFilename = std::string(argv[1]);
    std::cout << "    " << settingsFilename << "\n";
    std::cout << "    " << std::string(argv[2]) << "\n\n";

    //load the settings from the input file
    auto* const settings = top::ConfigurationSettings::get();
    settings->loadFromFile(settingsFilename);
    std::cout << "Configuration:\n";
    std::cout << *settings << "\n";

    const std::string libraryNames = settings->value("LibraryNames");
    top::loadLibraries(libraryNames);

    // Analysis tracking
    std::unique_ptr<top::AnalysisTrackingHelper> tracker;
    {
        bool useTracking = true;
        settings->retrieve("WriteTrackingData", useTracking);
        if (useTracking) {
            tracker.reset(new top::AnalysisTrackingHelper());
        }
    }

    // I/O Performance stats?
    // Summary gives a summary
    // Full gives detailed info on each collection in the file
    unsigned int doPerfStats(0);
    if (settings->value("PerfStats") == "Summary")
        doPerfStats = 1;

    if (settings->value("PerfStats") == "Full")
        doPerfStats = 2;

    //load the event selection from the input file
    std::string cutsFilename = std::string(settingsFilename);

    //open the files (to check they exist) and count how many events we have
    std::vector<std::string> filenames = top::fileList(std::string(argv[2]));
    unsigned int totalYield = top::checkFiles(filenames);

    //open output file
    std::unique_ptr<TFile> outputFile(TFile::Open((settings->value("OutputFilename") + ".tmp").c_str(), "RECREATE"));

    // A configuration that John can easily understand
    // This is not the same as a good configuration
    std::shared_ptr<top::TopConfig> topConfig(new top::TopConfig());

    // load AOD meta-data from all files
    bool useAodMetaData = false;
    settings->retrieve("UseAodMetaData", useAodMetaData);
    if (useAodMetaData) {
        std::cout << "Loading meta-data from input files ... " << std::flush;
        topConfig->aodMetaData().loadWithFilesFrom(argv[2]);
        std::cout << "OK." << std::endl;
    }

    //picking the first file was a bad idea because in the derivations it often
    //has no events (no CollectionTree).  Be sure to pick a file with events in
    //it...
    // The isMC flag needs to be taken from the xAOD metadata
    // We also need something for AFII
    {
        std::string usethisfile = filenames[0];
        bool atLeastOneFileIsValid(false);
        for (const auto& filename : filenames) {
            std::unique_ptr<TFile> checkingYieldFile(TFile::Open(filename.c_str()));

            //collection tree means > 0 events
            const TTree* const collectionTree = dynamic_cast<TTree* > (checkingYieldFile->Get("CollectionTree"));
            if (collectionTree) {
                usethisfile = filename;
                atLeastOneFileIsValid = true;
                break;
            }
        }

        // If there are no valid files, let's exit cleanly
        if (!(atLeastOneFileIsValid || useAodMetaData)) {
          std::cout << "No input file contains a CollectionTree -- exiting.\n"
                    << "Unfortunately, we don't have enough information to prepare an output file for\n"
                    << "you. If you had turned on 'UseAodMetaData' in your configuration, we could\n"
                    << "overcome this limitation.\n" << std::endl;
          return 1;
        }

        std::unique_ptr<TFile> testFile(TFile::Open(usethisfile.c_str()));
	
	if(! top::readMetaData(testFile.get(), topConfig) ){
	  std::cerr << "Unable to access metadata object in this file : " << usethisfile << std::endl;
	  std::cerr << "Please report this message" << std::endl;
	}


        bool const isMC = (useAodMetaData ?
              topConfig->aodMetaData().isSimulation() :
              top::isFileSimulation(testFile.get(), topConfig->sgKeyEventInfo())
           );
        topConfig->setIsMC(isMC);

        const bool isPrimaryxAOD = top::isFilePrimaryxAOD(testFile.get());
        topConfig->setIsPrimaryxAOD(isPrimaryxAOD);

	const std::string derivationStream = top::getDerivationStream(testFile.get());
	std::cout << "Derivation stream is -> " << derivationStream << std::endl;
	topConfig->setDerivationStream(derivationStream);

	if(isMC){
          ///-- Are we using a truth derivation (no reco information)? --///
          ///-- Let's find out in the first event, this could be done better --///
          bool isTruthDxAOD = top::isTruthDxAOD(testFile.get());
          topConfig->setIsTruthDxAOD(isTruthDxAOD);

          if (!isTruthDxAOD && atLeastOneFileIsValid) {
            unsigned int DSID = top::getDSID(testFile.get(), topConfig->sgKeyEventInfo());
            topConfig -> setDSID(DSID);

            // now need to get and set the parton shower generator from TopDataPrep
            SampleXsection tdp;	   
	    // Package/filename - XS file we want to use (can now be configured via cutfile)                                                    
	    const std::string tdp_filename = settings->value("TDPPath");
	    // Use the path resolver to find the first file in the list of possible paths ($CALIBPATH)
	    std::string fullpath = PathResolverFindCalibFile(tdp_filename);
	    if (!tdp.readFromFile(fullpath.c_str())) {
	      std::cout << "ERROR::TopDataPreparation - could not read file \n";
	      std::cout << tdp_filename << "\n";
	      exit(1);
	    }
	    std::cout << "SampleXsection::Found " << fullpath << std::endl;


            int ShowerIndex = tdp.getShoweringIndex(DSID);
            std::cout << "DSID: " << DSID << "\t" << "ShowerIndex: " << ShowerIndex << std::endl;
            topConfig->setMapIndex(ShowerIndex);
          }
	}
    } //close and delete the ptr to testFile

    // Pass the settings file to the TopConfig
    topConfig->setConfigSettings(settings);

    //In rel 19 we had a function to guess Class or Branch Access.
    //In rel20 just use branch (quicker)
    // In rel 20.7, need to go back to class access
    xAOD::TEvent xaodEvent(xAOD::TEvent::kClassAccess);

    // Setup all asg::AsgTools
    top::TopToolStore topTools("top::TopToolStore");
    top::check(topTools.setProperty("config", topConfig),
               "Failed to setProperty of topTools");
    top::check(topTools.initialize() , "Failed to initialize topTools");
    // EventCleaningSelection
    // Decorates EventInfo with GRL decision (always true for MC)
    // Evaluates all triggers requested from all selections
    // Trigger matching
    // Check for good Tile and LAr calorimeters
    // Let's check this passes before running expensive calibrations
    // If ANY selection does not request a trigger, we won't veto any events

    std::unique_ptr<top::EventCleaningSelection> eventCleaning( new top::EventCleaningSelection("top::EventCleaningSelection") );
    top::check(eventCleaning->setProperty("config", topConfig),
               "Failed to setProperty of triggerGRLSelection" );
    top::check(eventCleaning->initialize(),
               "Failed to initialize triggerGRLSelection");
    eventCleaning->setEventSelections(settings->selections());
    

    // Systematic object collection making
    std::unique_ptr<top::ObjectCollectionMaker> systObjMaker( new top::ObjectCollectionMaker( "top::ObjectCollectionMaker" ) );
    top::check( systObjMaker->setProperty( "config" , topConfig ) , "Failed to setProperty of systObjMaker" );
    if (!topConfig->isTruthDxAOD())
      top::check( systObjMaker->initialize() , "Failed to initialize systObjMaker" );

    //setup object definitions - not used in HLUpgrade tools
    std::unique_ptr<top::TopObjectSelection> objectSelection;
    if(!topConfig->HLLHC()) {
      objectSelection.reset(top::loadObjectSelection(topConfig));
      objectSelection->print(std::cout);
    }

    //setup event-level cuts
    top::EventSelectionManager eventSelectionManager(settings->selections(), outputFile.get(), libraryNames, topConfig);

    //The loader tool for top::ParticleLevelEvent objects
    top::ParticleLevelLoader particleLevelLoader( topConfig );

    // The loader tool for Upgrade objects
    top::UpgradeObjectLoader upgradeLoader( topConfig );

    // Fix the configuration - it now knows about:
    //     * all objects collections to work with
    //     * all systematic variations
    topConfig->fixConfiguration();


    // OK let's printout the TopConfig
    std::cout << *topConfig << "\n";
    if (tracker) tracker->setTopConfig(topConfig);

    //Top parton history for MC events
    // This is quite ugly and simple, it will be harmonized with in the future
    // along with all other factory methods (it's not a factory method right now)
    std::unique_ptr<top::CalcTopPartonHistory> topPartonHistory(nullptr);
    if(settings->value("TopPartonHistory") == "ttbar"){
      topPartonHistory = std::unique_ptr<top::CalcTopPartonHistory> ( new top::CalcTtbarPartonHistory( "top::CalcTtbarPartonHistory" ) );
      top::check(topPartonHistory->setProperty( "config" , topConfig ) , "Failed to setProperty of top::CalcTtbarPartonHistory");
    }
    else if(settings->value("TopPartonHistory") == "ttbarlight"){
      topPartonHistory = std::unique_ptr<top::CalcTopPartonHistory> ( new top::CalcTtbarLightPartonHistory( "top::CalcTtbarLightPartonHistory" ) );
      top::check(topPartonHistory->setProperty( "config" , topConfig ) , "Failed to setProperty of top::CalcTtbarLightPartonHistory");
    }
    else if(settings->value("TopPartonHistory") == "tb"){
      topPartonHistory = std::unique_ptr<top::CalcTopPartonHistory> ( new top::CalcTbbarPartonHistory( "top::CalcTbbarPartonHistory" ) );
      top::check(topPartonHistory->setProperty( "config" , topConfig ) , "Failed to setProperty of top::CalcTbbarPartonHistory");
    }
    else if(settings->value("TopPartonHistory") == "Wtb"){
      topPartonHistory = std::unique_ptr<top::CalcTopPartonHistory> ( new top::CalcWtbPartonHistory( "top::CalcWtbPartonHistory" ) );
      top::check(topPartonHistory->setProperty( "config" , topConfig ) , "Failed to setProperty of top::CalcWtbPartonHistory");
    }
    else if(settings->value("TopPartonHistory") == "ttz"){
      topPartonHistory = std::unique_ptr<top::CalcTopPartonHistory> ( new top::CalcTTZPartonHistory( "top::CalcTTZPartonHistory" ) );
      top::check(topPartonHistory->setProperty( "config" , topConfig ) , "Failed to setProperty of top::CalcTTZPartonHistory");
    }
    else if(settings->value("TopPartonHistory") == "ttgamma"){
      topPartonHistory = std::unique_ptr<top::CalcTopPartonHistory> ( new top::CalcTtbarGammaPartonHistory( "top::CalcTtbarGammaPartonHistory" ) );
      top::check(topPartonHistory->setProperty( "config" , topConfig ) , "Failed to setProperty of top::CalcTtbarGammaPartonHistory");
    }


    //LHAPDF SF calculation
    std::unique_ptr<top::PDFScaleFactorCalculator> PDF_SF(nullptr);
    PDF_SF = std::unique_ptr<top::PDFScaleFactorCalculator> (new top::PDFScaleFactorCalculator( "top::PDFScaleFactorCalculator" ) );
    if ( topConfig->doLHAPDF() ){
      top::check( PDF_SF->setProperty( "config", topConfig ), "Failed to set config for PDF SF Calculator" );
      top::check( PDF_SF->initialize(), "Failed to initialize PDF SF calculator" );
    }


    // make top::Event objects
    std::unique_ptr<top::TopEventMaker> topEventMaker( new top::TopEventMaker( "top::TopEventMaker" ) );
    top::check(topEventMaker->setProperty( "config" , topConfig ) , "Failed to setProperty of top::TopEventMaker");
    top::check(topEventMaker->initialize(),"Failed to initialize top::TopEventMaker");
    // Debug messages?
    // topEventMaker.msg().setLevel(MSG::DEBUG);

    std::unique_ptr<top::ScaleFactorCalculator> topScaleFactors( new top::ScaleFactorCalculator("top::ScaleFactorCalculator") );
    top::check(topScaleFactors->setProperty( "config" , topConfig ) , "Failed to setProperty of top::ScaleFactorCalculator");
    top::check(topScaleFactors->initialize(),"Failed to initialize  top::ScaleFactorCalculator");

    ///-- weights for matrix-method fakes estimate --///
    std::unique_ptr<top::TopFakesMMWeightCalculator> topfakesMMWeights(nullptr);
    if (!topConfig->isMC() && topConfig->doLooseEvents() && topConfig->doFakesMMWeights()) {
      topfakesMMWeights = std::unique_ptr<top::TopFakesMMWeightCalculator>( new top::TopFakesMMWeightCalculator() );
      top::check(topfakesMMWeights->setProperty( "config" , topConfig ) , "Failed to setProperty of top::TopFakesMMWeightCalculator");
      top::check(topfakesMMWeights->initialize(),"Failed to initialize  top::TopFakesMMWeightCalculator");
      for (auto sel : settings->selections()) {
	top::check(topfakesMMWeights->setSelectionConfigs(sel.m_name, eventSelectionManager.GetFakesMMConfigs(sel.m_name)),"Failed to set the selection FakesMMConfigs for selection"+sel.m_name);
      }
    }

    //A list of extra branches that we create in this code and want to save
    std::vector<std::string> extraBranches;
    eventSelectionManager.addExtraBranches(extraBranches);

    //output file format (and any user-modified code)
    std::unique_ptr<top::EventSaverBase> eventSaver(top::loadEventSaver(topConfig));
    eventSaver->initialize(topConfig, outputFile.get(), extraBranches);

    // save sum of weights before derivations
    outputFile->cd();
    TTree* sumWeights = new TTree("sumWeights", "");
    float totalEventsWeighted = 0;
    std::vector<float> totalEventsWeighted_LHE3;
    std::vector<double> totalEventsWeighted_LHE3_temp;// having doubles is necessary in case of re-calculation of the sum of weights on the fly
    std::vector<std::string> names_LHE3;
    bool recalc_LHE3 = false;
    int dsid = 0;
    int isAFII = topConfig->isAFII();
    std::string generators = topConfig->getGenerators();
    std::string AMITag = topConfig->getAMITag();
    ULong64_t totalEvents = 0;
    ULong64_t totalEventsInFiles = 0;
    sumWeights->Branch("dsid", &dsid);
    sumWeights->Branch("isAFII", &isAFII);
    sumWeights->Branch("generators", &generators);
    sumWeights->Branch("AMITag", &AMITag);
    sumWeights->Branch("totalEventsWeighted", &totalEventsWeighted);
    if (topConfig->doMCGeneratorWeights()) {// the main problem is that we don't have the list of names a priori
      sumWeights->Branch("totalEventsWeighted_mc_generator_weights", &totalEventsWeighted_LHE3);
      sumWeights->Branch("names_mc_generator_weights", &names_LHE3);
    }
    sumWeights->Branch("totalEvents", &totalEvents, "totalEvents/l");

    TTree* sumPdfWeights = 0;
    std::unordered_map<std::string, std::vector<float> *> totalEventsPdfWeighted;
    int dsidPdf = 0;
    bool pdfMetadataExists = false;
    if (topConfig->doLHAPDF()) {
      sumPdfWeights = new TTree("PDFsumWeights", "");
      sumPdfWeights->Branch("dsid", &dsidPdf);
      for (const auto &pdf_set : topConfig->LHAPDFSets()) {
        totalEventsPdfWeighted[pdf_set] = new std::vector<float>();
        sumPdfWeights->Branch(pdf_set.c_str(), totalEventsPdfWeighted[pdf_set]);
      }
    }

    //the analysis loop
    std::cout << "Starting event loop\n";

    // Performance stats
    if (doPerfStats > 0)
        xAOD::PerfStats::instance().start();  // start Perfstats timer

    unsigned int totalYieldSoFar = 0;
    unsigned int skippedEventsSoFar = 0;
    unsigned int eventSavedReco(0),eventSavedRecoLoose(0),eventSavedTruth(0),eventSavedParticle(0),eventSavedUpgrade(0);
    for (const auto& filename : filenames) {
        if (topConfig->numberOfEventsToRun() != 0 && totalYieldSoFar >= topConfig->numberOfEventsToRun() ) break;
        std::cout << "Opening " << filename << std::endl;
        std::unique_ptr<TFile> inputFile(TFile::Open(filename.c_str()));

        // the derivation framework is making files with an empty collection tree
        // which TEvent can now handle
        // However, at least for data, this is causing problems with the trigger menu
	// skip the file, after the meta data access above
        const TTree* const collectionTree = dynamic_cast<TTree* > (inputFile->Get("CollectionTree"));
        if (!collectionTree && !topConfig->isMC()) {
          if (top::ConfigurationSettings::get()->feature("SkipInputFilesWithoutCollectionTree")) {
            std::cout << "No CollectionTree found, skipping file\n";
            continue;
          }
        }

	top::check(xaodEvent.readFrom(inputFile.get()), "xAOD::TEvent readFrom failed");

	//Get some of event weights before derivation
	double initialSumOfWeightsInThisFile = 0;
        ULong64_t initialEvents = 0;

        // vector of LH3 weights and teir names
        std::vector<float> initialSumOfWeights_mc_generator_weightsInThisFile;
        std::vector<std::string> initialSumOfWeights_mc_generator_namesInThisFile;

        // vector of PDF weights and their names
        std::vector<float> initialSumOfWeights_pdf_weightsInThisFile;
        std::vector<std::string> initialSumOfWeights_pdf_namesInThisFile;

	// See https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AnalysisMetadata#Event_Bookkeepers
	const xAOD::CutBookkeeperContainer* cutBookKeepers = 0;
        // Try to get the cut bookkeepers.
        // <tom.neep@cern.ch> (4/4/16): Not there in DAOD_TRUTH1?
        // If we can't get them and we are running on TRUTH then carry on,
        // but anything else is bad!
        if (!xaodEvent.retrieveMetaInput(cutBookKeepers, "CutBookkeepers")) {
          top::check(topConfig->isTruthDxAOD(),
                     "Failed to retrieve cut book keepers");
        } else {
	  int maxCycle = -1;
          std::vector<int> maxCycle_LH3;// for LHE3 weights
          // for the sum of number of events and of mc event weights
          for ( auto cbk : *cutBookKeepers ) {
            // skip RDO and ESD numbers, which are nonsense; and
            // skip the derivation number, which is the one after skimming
            // we want the primary xAOD numbers
            if ((cbk->inputStream() == "StreamAOD" || (topConfig->HLLHC() && cbk->inputStream() == "StreamDAOD_TRUTH1")) && cbk->name() == "AllExecutedEvents") {
              double sumOfEventWeights = cbk->sumOfEventWeights();
              ULong64_t nEvents = cbk->nAcceptedEvents();
              if (cbk->cycle() > maxCycle) {
                initialSumOfWeightsInThisFile = sumOfEventWeights;
                initialEvents = nEvents;
                maxCycle = cbk->cycle();
              }
            }
            else if ((cbk->inputStream() == "StreamAOD" || (topConfig->HLLHC() && cbk->inputStream() == "StreamDAOD_TRUTH1")) && cbk->name().find("LHE3Weight_") != std::string::npos
              && topConfig->doMCGeneratorWeights()) {
              double sumOfEventWeights = cbk->sumOfEventWeights();
              std::string name=cbk->name();
              name.erase(0,11); // remove the "LHE3Weight_"
              // is it a new name? If yes append it to the vector of names
              // if not no need, but we must check the corresponding entry for the sum of weights exist
              auto pos_name = std::find(initialSumOfWeights_mc_generator_namesInThisFile.begin(),
                        initialSumOfWeights_mc_generator_namesInThisFile.end(),name);
              if (pos_name == initialSumOfWeights_mc_generator_namesInThisFile.end()) {
                initialSumOfWeights_mc_generator_namesInThisFile.push_back(name);
                initialSumOfWeights_mc_generator_weightsInThisFile.push_back(sumOfEventWeights);
                maxCycle_LH3.push_back(cbk->cycle());
              }
              else if (cbk->cycle() > maxCycle_LH3.at(pos_name-initialSumOfWeights_mc_generator_namesInThisFile.begin())) {
                initialSumOfWeights_mc_generator_weightsInThisFile.at(pos_name-initialSumOfWeights_mc_generator_namesInThisFile.begin()) = sumOfEventWeights;
                maxCycle_LH3.at(pos_name-initialSumOfWeights_mc_generator_namesInThisFile.begin()) = cbk->cycle();
              }
            }
            else continue;
          }
        }
	totalEventsWeighted += initialSumOfWeightsInThisFile;
	totalEvents += initialEvents;
        
        // now we must fill two vectors in sync
        if (topConfig->doMCGeneratorWeights()) {
          if (totalEventsWeighted_LHE3.size() != 0 ) {
            if (totalEventsWeighted_LHE3.size() != initialSumOfWeights_mc_generator_weightsInThisFile.size()
              || names_LHE3.size() != initialSumOfWeights_mc_generator_namesInThisFile.size()
              || names_LHE3.size() != totalEventsWeighted_LHE3.size() ) {
                std::cout << "Ouch: strange inconsistency of vector sizes in sum of LHE3 weights calculation. There is an issue somewhere." << std::endl;
                std::cout << "Exiting...." << std::endl;
                std::exit(1);
            }
            for (unsigned int i_genweights = 0; i_genweights < initialSumOfWeights_mc_generator_namesInThisFile.size(); i_genweights++) {
              if (names_LHE3.at(i_genweights) != initialSumOfWeights_mc_generator_namesInThisFile.at(i_genweights)) {
                std::cout << "Ouch: strange inconsistency in the vector of weight names in sum of LHE3 weights calculation. There is an issue somewhere." << std::endl;
                std::cout << "Exiting...." << std::endl;
                std::exit(1);
              }
              else {
                totalEventsWeighted_LHE3.at(i_genweights)
                  =  totalEventsWeighted_LHE3.at(i_genweights)
                  + initialSumOfWeights_mc_generator_weightsInThisFile.at(i_genweights);
              }
            }
          }
          else {
            for (unsigned int i_genweights = 0; i_genweights < initialSumOfWeights_mc_generator_namesInThisFile.size(); i_genweights++) {
              names_LHE3.push_back(initialSumOfWeights_mc_generator_namesInThisFile.at(i_genweights));
              totalEventsWeighted_LHE3.push_back(initialSumOfWeights_mc_generator_weightsInThisFile.at(i_genweights));
            }
          }
          if (!names_LHE3.empty()) {
            std::cout << "The sum of weights for the following LHE3 weights were retrieved from the input file:"<<std::endl;
            for (std::string s : names_LHE3) std::cout<<s<<" ";
            std::cout<< std::endl;
          }
          else {
            std::cout << "No sum of LHE3 weights could be found in meta-data. Will try to recompute these sums."<<std::endl;
            std::cout << "It will only work on un-skimmed derivations, and it will be impossible to know the name of these weights."<<std::endl;
            recalc_LHE3 = true;
          }
        }

        const unsigned int entries = xaodEvent.getEntries();
	totalEventsInFiles += entries;
	unsigned int firstEvent = 0;
	if(topConfig->numberOfEventsToSkip() > skippedEventsSoFar){
	  firstEvent = topConfig->numberOfEventsToSkip()-skippedEventsSoFar;
	  skippedEventsSoFar += entries < firstEvent ? entries : firstEvent;
	}
        unsigned int entry;
        for (entry = firstEvent; entry < entries; ++entry, ++totalYieldSoFar) {

   	    if (topConfig->numberOfEventsToRun() != 0 && totalYieldSoFar >= topConfig->numberOfEventsToRun() ) break;

            if (entry % 100 == 0)
                std::cout << "    Processing event " << totalYieldSoFar << " / " << totalYield << " (current file: " << entry << " / " << entries << ")" << std::endl;

            // clear the TStore - get rid of the last events objects
            xAOD::TActiveStore::store()->clear();
            // increment the event
            xaodEvent.getEntry(entry);

            // Pile up and MC event weight - used to normalize the cut flows
            float mcEventWeight(1.),pileupWeight(1.),zvtxWeight(1.);
            if (topConfig->isMC())
              mcEventWeight = topScaleFactors->mcEventWeight();

	    if (topConfig->doPileupReweighting() && !topConfig->isTruthDxAOD()) {
	      top::check(topScaleFactors->executePileup(), "Failed to execute pileup reweighting");
	      pileupWeight = topScaleFactors->pileupWeight();
	    }


            // get mc channel number
            // to save the sum of weights
            if (totalYieldSoFar == 0 && topConfig->isMC()) {
                const xAOD::EventInfo *ei(nullptr);
                top::check(xaodEvent.retrieve(ei, topConfig->sgKeyEventInfo()),
                           "xAOD::TEvent retrieve failed to get EventInfo");

                //mcChannelNumber only available in MC simulation
                if (ei->eventType(xAOD::EventInfo::IS_SIMULATION)) {
                    dsid = ei->mcChannelNumber();
		    dsidPdf = ei->mcChannelNumber();
		}
            }

            ///-- Truth events --///
            if (topConfig->isMC()) {

              // Save, if requested, MC truth block, PDFInfo, TopPartons
              // This will be saved for every event

              // Run topPartonHistory
              if (topConfig->doTopPartonHistory())
		top::check( topPartonHistory->execute() , "Failed to execute topPartonHistory" );

	      // calculate PDF weights
	      if( topConfig->doLHAPDF() )
		top::check( PDF_SF->execute(),
			    "Failed to execute PDF SF");

              eventSaver->saveTruthEvent();
              ++eventSavedTruth;

              // Upgrade analysis - only for truth DAODs when asking to do upgrade studies
              if (topConfig->isTruthDxAOD() && topConfig->HLLHC()) {
                top::ParticleLevelEvent upgradeEvent = upgradeLoader.load();

                  //event selection
                  const bool saveEventInOutputFile = eventSelectionManager.applyUpgradeLevel( upgradeEvent );

                  if ( saveEventInOutputFile ) {
                  eventSaver->saveUpgradeEvent( upgradeEvent );
                  ++eventSavedUpgrade;
                }
              }

              // Particle level analysis, saved only for truth events passing fiducial selection

              // --------------------------------------------------
              // If the truth loader is active, perform truth loading.
              if (particleLevelLoader.active()){
                // --------------------------------------------------
                // Get the top::TruthEvent for the current event
                top::ParticleLevelEvent particleLevelEvent = particleLevelLoader.load();

                //event selection
                const bool saveEventInOutputFile = eventSelectionManager.applyParticleLevel( particleLevelEvent );

                if ( saveEventInOutputFile ) {
                  eventSaver->saveParticleLevelEvent( particleLevelEvent );
                  ++eventSavedParticle;
                }
              }

            }
            if (totalYieldSoFar == 0 && topConfig->isMC() && topConfig->doLHAPDF()) {
                const xAOD::TruthEventContainer* truthEvent(nullptr);
                top::check( xaodEvent.retrieve(truthEvent, topConfig->sgKeyTruthEvent()) , "Failed to retrieve truth PDF info" );
                for( auto tePtr : *truthEvent ) {
                  for (auto &pdf : totalEventsPdfWeighted) {
                    if( tePtr->isAvailable< std::vector<float> >("AnalysisTop_"+pdf.first+"_Weights") ) {
                      pdf.second->resize(tePtr->auxdata< std::vector<float> >("AnalysisTop_"+pdf.first+"_Weights").size());
                    }
                  }
                }
            }
            // on the first event, set the size of the vector of sum of LHE3 weights in case it needs to be calculated on the fly
            if (topConfig->isMC() && topConfig->doMCGeneratorWeights()) {
                  const xAOD::TruthEventContainer* truthEvent(nullptr);
                  top::check( xaodEvent.retrieve(truthEvent, topConfig->sgKeyTruthEvent()) , "Failed to retrieve LHE3 weights from truth event" );
                  unsigned int weightsSize = truthEvent->at(0)->weights().size();
                  if (recalc_LHE3) {
                      if (totalYieldSoFar == 0) {
                        totalEventsWeighted_LHE3_temp.resize(weightsSize);
                        for (unsigned int i_LHE3 = 0; i_LHE3<weightsSize; i_LHE3++) {
                            totalEventsWeighted_LHE3_temp.at(i_LHE3) = truthEvent->at(0)->weights().at(i_LHE3);
                        }
                        names_LHE3.resize(weightsSize);
                        std::fill(names_LHE3.begin(),names_LHE3.end(),"?");
                    }
                    else {
                        for (unsigned int i_LHE3 = 0; i_LHE3<weightsSize; i_LHE3++) {
                            totalEventsWeighted_LHE3_temp.at(i_LHE3) = totalEventsWeighted_LHE3_temp.at(i_LHE3) + truthEvent->at(0)->weights().at(i_LHE3);
                        }
                    }
                }
                else if (weightsSize != names_LHE3.size()) {// don't recalc sum of weights, but cross-check the size of the vectors
                    std::cout << "Ouch: strange inconsistency in the sum of LHE3 weights vectors from the meta-data and the vector of LHE3 weights in the TruthEventContainer." << std::endl;
                    std::cout << "It should be the same; since it's not, it's pointless to continue.";
                    std::cout << "Exiting...." << std::endl;
                    std::exit(1);
                }
            }
            ///-- End of Truth events -- start of reconstruction level events --///
            if (topConfig->isTruthDxAOD())
              continue;


            ///-- We veto events that do not pass the GRL, trigger or have bad calorimeters --///
            ///-- No point calibrating and cutting on events that fail these cuts --///
            ///-- We only apply a veto if ALL selectors request a cut --///
            ///-- And if the user selects "OutputEvents SelectedEvents" --///

            ///-- Count initial events --///
            eventSelectionManager.countInitial(mcEventWeight,pileupWeight,zvtxWeight);

            ///-- Does event pass the GRL? (always true for MC) --///
            ///-- Only veto events when ALL user selectors request GRL --///
            bool passGRLVeto = eventCleaning->applyGRL();
            if (!passGRLVeto)
                continue;
            eventSelectionManager.countGRL(mcEventWeight,pileupWeight,zvtxWeight);

            ///-- Are the Tile and LAr calorimeters in a good state? (always true for MC) --///
            ///-- Only veto events when ALL user selectors request GOODCALO --///
            bool passGoodCalo = eventCleaning->applyGoodCalo();
            if (!passGoodCalo)
                continue;
            eventSelectionManager.countGoodCalo(mcEventWeight,pileupWeight,zvtxWeight);

            ///-- Do we have a Primary Vertex? --///
            ///-- Only veto events when ALL user selectors request PRIVTX -- ///
            bool passPriVtx = eventCleaning->applyPrimaryVertex();
            if (!passPriVtx)
                continue;
            eventSelectionManager.countPrimaryVertex(mcEventWeight,pileupWeight,zvtxWeight);

            ///-- Wondering which triggers are available ??? --///
            ///-- Uncomment this line and get ready for a LOT of output --///
            // eventCleaning->printTriggers();

            ///-- Does event pass any trigger ? --///
            ///-- Only veto events when ALL user selectors request TRIGDEC --///
            ///-- And all trigger fail --///
            bool passAnyTriggerVeto = eventCleaning->applyTrigger();
            if (!passAnyTriggerVeto)
                continue;

	    ///-- Nominal objects -- ///
            ///-- Calibrate objects and make all required systematic copies --///
            top::check( systObjMaker->execute(true) , "Failed to execute systObjMaker" );

            ///-- Object selection (e.g. good electrons, muons, jets etc.). Event selection cuts comes later --///
            top::check( objectSelection->execute(true) , "Failed to execute objectSelection" );

            ///-- Recalculate MissingET based on object selection --///
            top::check( systObjMaker->recalculateMET(true) , "Failed to recalculateMET with systObjMaker" );

	    ///-- Systematic objects -- ///
	    ///-- Calibrate objects and make all required systematic copies --///                                  
	    top::check( systObjMaker->execute(false) , "Failed to execute systObjMaker" );

	    ///-- Object selection (e.g. good electrons, muons, jets etc.). Event selection cuts comes later --/// 
	    top::check( objectSelection->execute(false) , "Failed to execute objectSelection" );

	    ///-- Recalculate MissingET based on object selection --///                                            
	    top::check( systObjMaker->recalculateMET(false) , "Failed to recalculateMET with systObjMaker" );


            ///-- Scale Factor calculation --///
            if (topConfig->isMC())
                top::check( topScaleFactors->execute() , "Failed to calculate scale factors" );

            if (topConfig->doTightEvents()) {
              ///-- Loop over all systematics and make a "top::Event" for each --///
              const xAOD::SystematicEventContainer* allSystematics = topEventMaker->systematicEvents( topConfig->sgKeyTopSystematicEvents() );
              for (auto currentSystematic : *allSystematics) {
		if (!(currentSystematic->hashValue() == topConfig->nominalHashValue() || topConfig->doTightSysts())) continue;
                ///-- Make a top::Event --///
                top::Event topEvent = topEventMaker->makeTopEvent( currentSystematic );
                ///-- Apply event selection --///
                const bool passAnyEventSelection = eventSelectionManager.apply(topEvent,*currentSystematic );
                currentSystematic->auxdecor<char>(topConfig->passEventSelectionDecoration()) = passAnyEventSelection ? 1 : 0;
                topEvent.m_saveEvent = passAnyEventSelection;
                ///-- Save event - we defer to eventSaver the decision to write or not --///
                eventSaver->saveEvent(topEvent);

                ///-- For printout at the end of job, only count nominal events --///
                if (passAnyEventSelection && currentSystematic->hashValue() == topConfig->nominalHashValue())
                    ++eventSavedReco;
              }
              ///-- End loop over all calibration systematics --///
            }
            ///-- End of tight events --///

            if (topConfig->doLooseEvents()) {
              ///-- Loop over all Loose systematics and make a "top::Event" for each --///
              const xAOD::SystematicEventContainer* allSystematicsLoose = topEventMaker->systematicEvents( topConfig->sgKeyTopSystematicEventsLoose() );
              for (auto currentSystematic : *allSystematicsLoose) {
		if (!(currentSystematic->hashValue() == topConfig->nominalHashValue() || topConfig->doLooseSysts())) continue;
                ///-- Make a top::Event --///
                top::Event topEvent = topEventMaker->makeTopEvent( currentSystematic );
                ///-- Apply event selection --///
                const bool passAnyEventSelection = eventSelectionManager.apply(topEvent,*currentSystematic );
                currentSystematic->auxdecor<char>(topConfig->passEventSelectionDecoration()) = passAnyEventSelection ? 1 : 0;
                topEvent.m_saveEvent = passAnyEventSelection;
		///-- weights for matrix-method fakes estimate, only for nominal --///
		if (!topConfig->isMC() && topConfig->doFakesMMWeights() && currentSystematic->hashValue() == topConfig->nominalHashValue()) {
		  top::check( topfakesMMWeights->execute(&topEvent) , "Failed to execute fakes mmweight calculation");
		}
                ///-- Save event - we defer to eventSaver the decision to write or not --///
                eventSaver->saveEvent(topEvent);

                ///-- For printout at the end of job, only count nominal events --///
                if (passAnyEventSelection && currentSystematic->hashValue() == topConfig->nominalHashValue())
                    ++eventSavedRecoLoose;
              }
              ///-- End loop over all loose calibration systematics --///
            }
            ///-- End of loose events --///

            ///-- Needed for xAOD output, all systematics go into the same TTree --///
            eventSaver->saveEventToxAOD();

        } //loop over events in current file
        if (tracker) tracker->addInputFile(inputFile->GetName(), entry-firstEvent);

        // do it at the end, so we can get the DS id from the first event
        // notice this is different from the normal sum of weights: all entries matter, not only the highest per file
        if (topConfig->doLHAPDF()) {
          bool pdfInCBK = false;
          if (xaodEvent.retrieveMetaInput( cutBookKeepers, "CutBookkeepers" )) {
            // try to get the first entry
            std::string pdf_set = totalEventsPdfWeighted.begin()->first;
            std::string p = pdf_set+"_0";
            for ( auto cbk : *cutBookKeepers ) {
              std::string pdfName = cbk->name();
              if (p != pdfName) continue;
              pdfMetadataExists = true;
              pdfInCBK = true;
	      // reload to set iterator back to original state
	      if (!xaodEvent.retrieveMetaInput(cutBookKeepers, "CutBookkeepers")) {}
              break;
            }
          }
	  // if the pdf info was in the CBK the PDFSumOfWeights retrieve never happens,
	  // and cutBookKeepers, will keep its reference to the main CBK
	  if (!pdfInCBK) { // only try this if the info is not in the CBK -- this is legacy stuff
	    if (!xaodEvent.retrieveMetaInput( cutBookKeepers, "PDFSumOfWeights" )) {
	      pdfMetadataExists = false;
	    }
	  }
	  if (pdfMetadataExists && topConfig->saveLHAPDFEvent() && topConfig->baseLHAPDF().empty()) {
            // just make a copy of it, but not all derivations have it, so just ignore it if it is not there
	    for (auto &pdfentry : totalEventsPdfWeighted) {
              std::string pdf_set = pdfentry.first;
              for (size_t n = 0; n < totalEventsPdfWeighted[pdf_set]->size(); ++n) {
                std::string p = pdf_set+"_"+std::to_string(n);
                bool foundPdf = false;
	        for ( auto cbk : *cutBookKeepers ) {
                  std::string pdfName = cbk->name();
                  if (p != pdfName)
                    continue;
                  totalEventsPdfWeighted[pdf_set]->at(n) = cbk->sumOfEventWeights();
                  foundPdf = true;
                  break;
                }
                if (!foundPdf) { // asked for PDF weighting, the PDF metadata exists, but this particular PDF is missing
                  // crash hard
                  std::cout << "The force is not strong with us, young Padawan ..." << std::endl;
                  std::cout << "You want to save weights to do PDF reweighting using '" << pdf_set << "', which I figure has " << totalEventsPdfWeighted[pdf_set]->size() << " sets on it." << std::endl;
                  std::cout << "There is metadata information for the sum of MC*PDF weights in PDFSumOfWeights, but none seem to refer to '" << p << "' therefore I do not know how to estimate the sum of weights before acceptance for this configuration." << std::endl;
                  std::cout << "May the force be with you in your next attempt." << std::endl;
                  std::exit(1);
                }
              }
            }
            sumPdfWeights->Fill();
	  }
        } //doLHAPDF

    } //loop over files

    //finish
    std::cout << "\n";

    if (doPerfStats > 0)
        xAOD::PerfStats::instance().stop(); // Stop the PerfStats timer

    if( topConfig->doLHAPDF() ) top::check( PDF_SF->finalize() , "Failed to finalize PDF SF" );

    // this fills the sum of weights
    // if the list of input files has more than
    // one type of dataset (ie: the first file is ttbar and the second one
    // is W+jets), then the Fill statement below needs to come in the end of
    // each file's processing instead ... but why would you do that?!
    if (recalc_LHE3) {// in case the sum of LHE3 weight has been re-calculated with double (but we need floats in the end)
      for (double d:totalEventsWeighted_LHE3_temp) totalEventsWeighted_LHE3.push_back(d);
    }
    sumWeights->Fill();
    outputFile->cd();

    if (topConfig->doLHAPDF()) {
    // Save sum of weights of PDF variations
    // Only do this if the metadata is not available
    if (totalEventsInFiles != totalEvents && !pdfMetadataExists) {
      std::cout << "These are not the droids you are looking for." << std::endl;
      std::cout << "There are " << totalEventsInFiles << " events in the input files, but the metadata tells me there were " << totalEvents << "before skimming." << std::endl;
      std::cout << "This means you are running on skimmed derivations. You also want me to do PDF reweighting, but no PDF metadata exists!" << std::endl;
      std::cout << "I don't know how to tell you the sum of MC*PDF weights before skimming. I will explode now in 3, 2, 1 ..." << std::endl;
      std::exit(1);
    }
    // save recomputed sum weights
    if( (!topConfig->baseLHAPDF().empty() || !pdfMetadataExists) ) {
      for( auto& pdf_result : topConfig->getLHAPDFResults() )
         *(totalEventsPdfWeighted[pdf_result.first]) = pdf_result.second;
      sumPdfWeights->Fill();
    }
    } //doLHAPDF

    outputFile->cd();
    eventSelectionManager.finalise();
    eventSaver->finalize();
    outputFile->cd();
    if (tracker) tracker->writeTree("AnalysisTracking");
    outputFile->Close();
    bool outputFileGood = !outputFile->TestBit(TFile::kWriteError);
    if (outputFileGood) {
      if (gSystem->Rename(outputFile->GetName(), settings->value("OutputFilename").c_str()) != 0)
        outputFileGood = false;
    }

    if (!topConfig->isTruthDxAOD()) {
      if (topConfig->doTightEvents())
        std::cout << "\nEvents saved to output file nominal reconstruction tree: " << eventSavedReco << "\n";
      if (topConfig->doLooseEvents())
        std::cout << "Events saved to output file nominal Loose reconstruction tree: " << eventSavedRecoLoose << "\n";
    }
    if (topConfig->isMC()) {
      std::cout << "Events saved to output file truth tree : "<<eventSavedTruth << "\n";
      if (particleLevelLoader.active()) {
        std::cout << "Events saved to output file particle level tree : "<<eventSavedParticle << "\n";
      }
      if (upgradeLoader.active()) {
        std::cout << "Events saved to output file upgrade tree : "<<eventSavedUpgrade << "\n";
      }
    }
    std::cout << "Total sum-of-weights (for normalization) : " 
	      << totalEventsWeighted
	      << std::endl;

    //print some xAOD performance info
    if (doPerfStats == 1)
        xAOD::IOStats::instance().stats().Print("Summary");
    if (doPerfStats == 2)
        xAOD::IOStats::instance().stats().Print();

    if (!outputFileGood) {
      std::cout << "ERROR: an I/O error occured while attempting to save the output file." << std::endl;
      return 1;
    }

    return 0;
}
