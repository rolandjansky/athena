/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "iLumiCalc.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "CollectionBase/CollectionService.h"
#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionMetadata.h"
#include "DataModelRoot/RootType.h"

#include "TTree.h"
#include "TList.h"
#include "TObjString.h"
#include "TString.h"
#include <TKey.h>

#include <typeinfo>
#include <list>

// ToDo: make this whole code in
//       object oriented way...


//______________________________________________________________
void print_usage(){

  std::cout << std::endl;
  std::cout << " --->>> i L U M I C A L C <<<---"<< std::endl;
  std::cout << std::endl;
  std::cout << "iLumiCalc: this c++ program calculates integrated luminosity" << std::endl ;
  std::cout << "by looping over a list of lumiblocks, given a set of input options." << std::endl;
  std::cout << "Input file can be either an xml file, or a TAG file." << std::endl;
  std::cout << std::endl;
  std::cout << "Type: iLumiCalc --help for the complete list of options" << std::endl ;
  std::cout << std::endl;
  std::cout << "Further help: https://twiki.cern.ch/twiki/bin/view/Atlas/CoolLumiCalc"<< std::endl;
  std::cout << std::endl;
  exit(-1);

}

//______________________________________________________________________________
int main(int argc, char * argv[]){

  if (argc == 1)print_usage();

  // schedule for time info 
  TStopwatch timer;
  timer.Start();

  // Parse command-line options
  // ==========================

  gengetopt_args_info args_info;
  cmdline_parser (argc, argv, &args_info);

  // Output Level
  bool verbose = args_info.verbose_flag;
  bool quiet = args_info.quiet_flag;
  if (verbose && quiet) {
    logger << Root::kWARNING << "Can't specify verbose and quiet, using verbose" << Root::GEndl;
    quiet = false;
  }

  if (verbose) Root::TMsgLogger::SetMinLevel(Root::kDEBUG);
  if (quiet) Root::TMsgLogger::SetMinLevel(Root::kWARNING);

  // Other flags
  bool makeplots = args_info.plots_flag;
  bool collisionlists = args_info.xml_collisionlist_flag;

  // parse for any output xml file name
  xmloutfile = "";
  if(args_info.xml_out_given){
    xmloutfile = args_info.xml_out_arg;
  }

  //
  // Parameters to control luminosty calculation
  // ===========================================

  float scalel1trigrate = float(args_info.scale_lumi_arg); // = 1 by default
  if (scalel1trigrate != 1.) {
    logger << Root::kWARNING << "Luminoisty scaled by factor: " << scalel1trigrate << Root::GEndl;
  }

  // Use online folder
  bool online = args_info.online_flag;
  if (online) {
    logger << Root::kINFO << "Luminosity read from online folders" << Root::GEndl;
  }

  // Luminosity tag
  std::string lumitag = args_info.lumitag_arg;
  if (online) {
    logger << Root::kWARNING << "Lumitag: " << lumitag << " ignored due to --online " << Root::GEndl;
  } else {
    logger << Root::kINFO << "Lumitag: " << lumitag << Root::GEndl;
  }

  // LAr noise bursts
  bool uselar = args_info.lar_flag;
  std::string lartag = args_info.lartag_arg;
  if (uselar) {
    logger << Root::kINFO << "LAr noise burst inefficiency will be calculated from " << lartag << Root::GEndl;
  }

  // Online Beamspot validity
  bool usebeamspot = args_info.beamspot_flag;
  std::string beamspottag = args_info.beamspottag_arg;
  if (usebeamspot) {
    logger << Root::kINFO << "Livetime will include online beamspot validity requirement from " << beamspottag << Root::GEndl;
  }

  // Luminosity channel
  // ==================

  std::string lumimethod;
  int lumichannel = -1;

  // Check for online exceptions
  if (online && args_info.lumimethod_given){
    logger << Root::kERROR << "Sorry, the online database doesn't have luminosity method names, use --lumichannel instead!" << Root::GEndl;
    exit(-1);
  }

  if (args_info.lumimethod_given && args_info.lumichannel_given == 0) {
    lumimethod  =     args_info.lumimethod_arg;
    logger << Root::kINFO << "Lumimethod: " << lumimethod << Root::GEndl;
  } else if (args_info.lumimethod_given == 0 && args_info.lumichannel_given) {
    lumichannel = args_info.lumichannel_arg;
    logger << Root::kINFO << "Lumichannel: " << lumichannel << Root::GEndl;
  } else if (args_info.lumimethod_given && args_info.lumichannel_given) {
    lumichannel = args_info.lumichannel_arg;
    logger << Root::kINFO << "Both lumimethod and lumichannel is given, defaulting to Lumichannel: " << lumichannel << Root::GEndl;
  } else if (args_info.lumimethod_given == 0 && args_info.lumichannel_given == 0) {
    if (online) {
      lumichannel = args_info.lumichannel_arg;
      logger << Root::kINFO << "No lumimethod or lumichannel is given, defaulting to Lumichannel: " << lumichannel << Root::GEndl;
    } else {
      lumimethod = args_info.lumimethod_arg;
      logger << Root::kINFO << "No lumimethod or lumichannel is given, defaulting to Lumimethod: " << lumimethod << Root::GEndl;
    }
  }


  // Handle triggers
  // ===============
  if(args_info.trigger_given == 0) {
    logger << Root::kINFO << "No trigger specified, proceeding with --trigger=None" << Root::GEndl;
    triggerchain.push_back("None");
  } else {
    if (args_info.trigger_given > 1) {
      logger << Root::kINFO << "Processing Triggers: ";
    } else {
      logger << Root::kINFO << "Processing Trigger: ";
    }

    for(unsigned int i = 0; i < args_info.trigger_given; ++i){
      triggerchain.push_back(args_info.trigger_arg[i]);
      logger << Root::kINFO << args_info.trigger_arg[i] << ", ";
    }
    logger << Root::GEndl;
  }

  // Livetime triggers
  // =================
  bool uselivetrigger = false;
  livetrigger = args_info.livetrigger_arg;   // Default if not specified on command line

  if (args_info.livetrigger_given || !args_info.trigger_given) {
    // Either livetime trigger specified, or no prescale triggers specified
    uselivetrigger = true;
    logger << Root::kINFO << "Trigger used for livetime: " << livetrigger << Root::GEndl;
  } else {
    // Prescale not specified AND trigger list given 
    logger << Root::kINFO << "Prescale trigger chains will be used for livetime " << Root::GEndl;
  }


  // Parse the run number list
  // =========================

  // Run list has pair of first, last allowed run inclusive
  std::list<std::pair<unsigned int, unsigned int> > runList;
  runList.clear();

  // Comma-separated lists already get split by options parser.  Just need to look for ranges here.
  for (unsigned int i=0; i<args_info.runnumber_given; i++) {

    // Split by hyphens to find run range
    std::string full(args_info.runnumber_arg[i]);

    size_t found = full.find('-');

    unsigned int val1, val2;
    if (found != std::string::npos) {
      val1 = atoi(full.substr(0, found).c_str());
      val2 = atoi(full.substr(found+1, std::string::npos).c_str());
    } else {
      val1 = atoi(full.c_str());
      val2 = val1;
    }
    if (val1 == 0) val1 = minrunnum; // Set to min run number
    if (val2 == 0) val2 = maxrunnum; // Set to max run number

    // std::cout << "Parsed [" << full << "] into (" << val1 << ", " << val2 << ")" << std::endl;
    runList.push_back( std::pair<unsigned int, unsigned int>(val1, val2) );
  }

  // std::cout << "Parsed Run List:" << std::endl;
  // for (std::list<std::pair<unsigned int, unsigned int> >::iterator it = runList.begin(); it != runList.end(); it++)
  // std::cout << it->first << " " << it->second << std::endl;

  // Now determine the input data source
  // ===================================

  // what shall be the resource for LumiBlockCollection
  unsigned int runtype = 0; // 0 - no input file --> COOL, 1 - TAG input file, 2 - AOD input file, 3 - XML input file, 4 - ROOT input file

  // TAG File
  //============================================
  if(args_info.tag_given > 0 && args_info.xml_given == 0 && args_info.root_given == 0){
    runtype = 1;
    std::cout << args_info.tag_given << " TAG file(s) is given..." << std::endl;
    for(unsigned int i = 0; i < args_info.tag_given; ++i){
      tagfile.push_back(args_info.tag_arg[i]);
    }
    // Try to see if file(s) exist
    for(std::vector<std::string>::iterator it = tagfile.begin(); it != tagfile.end(); ++it){
      if(!FileExists((*it))){
	logger << Root::kWARNING << "Problem: file ["<< (*it)  <<"] may not exist.  Will try anyways..." << Root::GEndl;
      }
    }
    //std::cout << "RunType: " << runtype << std::endl; 

  }

  // XML File
  //============================================
  if(args_info.tag_given == 0 && args_info.xml_given > 0 && args_info.root_given == 0){
    runtype = 3;
    for(unsigned int i = 0; i < args_info.xml_given; ++i){
      xmlfile.push_back(args_info.xml_arg[i]);
    }
    // Try to see if file(s) exist
    for(std::vector<std::string>::iterator it = xmlfile.begin(); it != xmlfile.end(); ++it){
      if(! FileExists((*it))){
	logger << Root::kWARNING << "Problem: file ["<< (*it)  <<"] may not exist.  Will try anyways..." << Root::GEndl;
      }
    }
  //std::cout << "RunType: " << runtype << std::endl;
  }

  // ROOT File -- ttree attached mode
  //============================================
  if((args_info.root_given >0 || args_info.tree_given > 0) && args_info.d3pd_dir_given == 0 && args_info.xml_given == 0 && args_info.tag_given == 0){
    if((args_info.root_given == 0 && args_info.tree_given > 0) || (args_info.root_given > 0 && args_info.tree_given == 0 )){
      logger << Root::kERROR << "Please provide BOTH --root=\"myfile.root\" AND --tree=\"mytreename\" OR --d3pd_dir=\"mydirname\" options " << Root::GEndl;
      exit(-1);
    }
    
    runtype = 4;
    for(unsigned int i = 0; i < args_info.root_given; ++i){
      rootfile.push_back(args_info.root_arg[i]);
    }
    if(args_info.tree_given){
      treename = args_info.tree_arg;
    }else{
      logger << Root::kERROR << "In Root file mode Tree name (--d3p_dir=\"mytreename\") must also be provided" << Root::GEndl;
    }
    for(std::vector<std::string>::iterator it = rootfile.begin(); it != rootfile.end(); ++it){
      if(!FileExists((*it))){
	logger << Root::kWARNING << "Problem: file ["<< (*it)  <<"] may not exist.  Will try anyways..." << Root::GEndl;
      }
    }
  }

  // ROOT File -- d3pd TDirectory mode
  //============================================
  if((args_info.root_given >0 || args_info.d3pd_dir_given > 0) && args_info.tree_given == 0 && args_info.xml_given == 0 && args_info.tag_given == 0){
    if((args_info.root_given == 0 && args_info.d3pd_dir_given > 0) || (args_info.root_given > 0 && args_info.d3pd_dir_given == 0 )){
      logger << Root::kERROR << "Please provide BOTH --root=\"myfile.root\" AND --d3pd_dir=\"myd3pddirname\" options" << Root::GEndl;
      exit(-1);
    }

    runtype = 5;
    for(unsigned int i = 0; i < args_info.root_given; ++i){
      rootfile.push_back(args_info.root_arg[i]);
    }
    if(args_info.d3pd_dir_given){
      d3pddirname = args_info.d3pd_dir_arg;
    }else{
      logger << Root::kWARNING << "In D3PD Root file mode Directory name (--d3pd_dir=\"mylumidir\") must also be provided" << Root::GEndl;
      logger << Root::kWARNING << "Ommitted, using default name \"Lumi\"" << Root::GEndl;
      d3pddirname= "Lumi";
    }

    for(std::vector<std::string>::iterator it = rootfile.begin(); it != rootfile.end(); ++it){
      if(!FileExists((*it))){
	logger << Root::kWARNING << "Problem: file ["<< (*it)  <<"] may not exist.  Will try anyways..." << Root::GEndl;
      }
    }
  }

  // Full command line mode
  //============================================
  if (runtype == 0) {

    // Nothing else specified, try run numbers
    if (!args_info.runnumber_given) {
      logger << Root::kERROR << "No input data specified!" << Root::GEndl;
      exit(-1);
    }

    std::list<std::pair<unsigned int, unsigned int> >::iterator itr = runList.begin();

    // Decode run IOV pairs
    for (; itr != runList.end(); ++itr) {
      // Dont allow open-ended IOVs
      if ((itr->first == minrunnum) || (itr->second == maxrunnum)) {
	logger << Root::kERROR << "Can't use open-ended run ranges to specify sample!" << Root::GEndl;
	exit(-1);
      }

      for (unsigned int runnum = itr->first; runnum <= itr->second; runnum++) 
	runnumber.push_back(runnum);

    }

    // Also add in lumi block ranges
    if (args_info.lbstart_given ==0) {
      for (unsigned int i=0; i<runnumber.size(); i++) {
	lbstart.push_back(minlbstart);
      }
    } else {
      for (unsigned int i=0; i<args_info.lbstart_given; i++) {
	lbstart.push_back(args_info.lbstart_arg[i]);
      }
    }

    if (args_info.lbend_given ==0) {
      for (unsigned int i=0; i<runnumber.size(); i++) {
	lbend.push_back(maxlbend);
      }
    } else {
      for (unsigned int i=0; i<args_info.lbend_given; i++) {
	lbend.push_back(args_info.lbend_arg[i]);
      }
    }

  }


  // Finish parsing
  //============================================
  cmdline_parser_free (&args_info); /* release allocated memory */


  //==========================================================================
  // Set up LumiBlockCollection for the different scenarios
  std::vector< xAOD::LumiBlockRangeContainer* > iovcVec;
  std::vector< std::vector<std::string> > triggerchainVec;
  Root::TGRLCollection grlcollection;
  LumiBlockRangeContainerConverter converter;
  TString version("30"); // [0-10): ATLRunQuery, [10-20): ntuple production, [20-30): xml merging, [30-40): LumiCalc

  //==========================================================================
  // User defined IOVRange in command line
  if (runtype == 0) {

    logger << Root::kINFO << "Proceeding with command-line run list" << Root::GEndl;
    if (lbstart.size() != lbend.size()) {
      logger << Root::kERROR << "number of lbstart and lbend values must match!" << Root::GEndl;
      exit(-1); 
    }

    if (runnumber.size() > 1 && (runnumber.size() != lbstart.size())) {
      logger << Root::kERROR << "number of lbstart and lbend values must match number of runs with multiple runs specified!" << Root::GEndl;
      exit(-1);
    }

    /*
    std::cout << "runlist length: " << runnumber.size() << std::endl;
    for (unsigned int i=0; i<runnumber.size(); i++) {
      std::cout << runnumber[i] << std::endl;
    }
    */

    uint32_t lbstart_val;
    uint32_t lbend_val;

    xAOD::LumiBlockRangeContainer* iovc = new xAOD::LumiBlockRangeContainer();
    xAOD::LumiBlockRangeAuxContainer* iovcAux = new xAOD::LumiBlockRangeAuxContainer();
    iovc->setStore( iovcAux );

    std::vector<uint32_t>::iterator itstart;
    std::vector<uint32_t>::iterator itend;
    std::vector<uint32_t>::iterator itrun;

    if (runnumber.size() == 1) {

	logger << Root::kINFO << "Runnumber [" << runnumber[0] <<  "]" << Root::GEndl;
	for(itstart = lbstart.begin(), itend = lbend.begin(); 
	    itstart != lbstart.end() && itend != lbend.end(); ++itstart,++itend) {
	  lbstart_val = (*itstart);
	  lbend_val = (*itend);
	  logger << Root::kINFO << "lbstart-lbend [" << lbstart_val << "-" << lbend_val << "]" << Root::GEndl; 
	  if (lbstart_val > lbend_val) {
	    logger << Root::kERROR << "lbstart > lbend! Should be: lbstart < = lbend" << Root::GEndl;
	    exit(-1);
	  } else {
	    xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange();
	    iovc->push_back(iovr);  // Must put this in container first
	    iovr->setStartRunNumber(runnumber[0]);
	    iovr->setStartLumiBlockNumber(lbstart_val);
	    iovr->setStopRunNumber(runnumber[0]);
	    iovr->setStopLumiBlockNumber(lbend_val);
	  }
	}

    } else if (runnumber.size() > 1) {

      for(itrun = runnumber.begin(), itstart = lbstart.begin(), itend = lbend.begin(); 
	  itrun != runnumber.end() && itstart != lbstart.end() && itend != lbend.end();
	  ++itrun, ++itstart, ++itend) {
	logger << Root::kINFO << "Runnumbers [" << *itrun <<  "]" << Root::GEndl;
	logger << Root::kINFO << "lbstart-lbend [" << *itstart << "-" << *itend << "]" << Root::GEndl; 
	    xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange();
	    iovc->push_back(iovr);  // Must put this in container first
	    iovr->setStartRunNumber(*itrun);
	    iovr->setStartLumiBlockNumber(*itstart);
	    iovr->setStopRunNumber(*itrun);
	    iovr->setStopLumiBlockNumber(*itend);
      }
    }

    iovcVec.push_back(iovc); // take over iovc for usage below
    triggerchainVec.push_back(triggerchain); // cmd-line triggerchain
    std::map<TString,TString> metadata;
    for (unsigned int j=0; j<triggerchain.size(); ++j)
      metadata[Form("TriggerName%d",j)] = TString(triggerchain[j]);
    grlcollection.push_back( *converter.GetGRLObject(*iovc,metadata,version) );
  }
  

  //==========================================================================
  // Fetch up xAOD::LumiBlockRangeContainer from input TAG file
  if (runtype == 1) {
    // open TAG files to build xAOD::LumiBlockRangeContainer
    logger << Root::kINFO << "Being in TAG file mode..." << Root::GEndl;

    Root::TGoodRunsListReader reader;
    std::string connection ="";
    std::string type = "RootCollection";
    bool readOnly(true);
    for(std::vector<std::string>::iterator it = tagfile.begin(); it != tagfile.end(); ++it){
      logger << Root::kINFO << "Processing file: <" << (*it) << ">" <<  Root::GEndl;
      int n = (*it).find(".root");
      std::string tagfilename =  (*it).substr(0,n);

      // get Value for a Key
      pool::CollectionService collectionService;
      pool::ICollection* collection = collectionService.handle(tagfilename, type, connection, readOnly);
      if(collection == NULL) {
         logger << Root::kERROR << "ICollection is NULL, exiting... " << Root::GEndl;
         exit(-1);
      }

      // MB : Reading incomplete LBs with tag. Request from Tulay
      const char* value = collection->metadata().getValueForKey("OutputLumirange");
      if(value == NULL) {         
         logger << Root::kERROR << "The collection has no such key of OutputLumirange in metadata, try with OutputIncompleteLumirange key" << Root::GEndl;
         value = collection->metadata().getValueForKey("OutputIncompleteLumirange");
         if (value != NULL) logger << Root::kINFO << "OutputIncompleteLumirange key is OK, reading the value..."  << Root::GEndl;
         else exit(-1);
      }

      if(verbose == true) logger << Root::kINFO << "Value :  " << value << Root::GEndl;
      // add xml string to TGoodRunsListReader. Sort out strings below
      reader.AddXMLString(value);
    }
    // do sorting of all grl objects
    reader.Interpret();
    grlcollection = reader.GetMergedGRLCollection();
    
    for (unsigned int j=0; j<grlcollection.size(); ++j) {
      iovcVec.push_back( converter.GetLumiBlockRangeContainer(grlcollection[j]) );
      // default: trigger names taken from xml metadata. Overwrite any existing cmd-line triggers.
      if ( grlcollection[j].HasTriggerInfo() ) {
        triggerchainVec.push_back(grlcollection[j].GetTriggerList()); // use existing trigger names
        if (!triggerchain.empty())
          logger << Root::kWARNING << "Input goodruns-list(s) <" << grlcollection[j].GetName() 
                   << "> already contain trigger names. Cmd-line triggers are ignored!" << Root::GEndl;
      } else { // use cmdline trigger names
        triggerchainVec.push_back(triggerchain) ;
        for (unsigned int k=0; k<triggerchain.size(); ++k)
          grlcollection[j].AddMetaData( Form("TriggerName%d",k),TString(triggerchain[k]) );
      }
    }
  }

  //==========================================================================
  // Fetch up xAOD::LumiBlockRangeContainer from input XML file
  if(runtype == 3){
    // open XML files to build xAOD::LumiBlockRangeContainer
    logger << Root::kINFO << "Being in XML file mode..." << Root::GEndl;
    Root::TGoodRunsListReader reader;
    // looping over XML files
    for(std::vector<std::string>::iterator it = xmlfile.begin(); it != xmlfile.end(); ++it){
      logger << Root::kINFO << "Processing file: <" << (*it) << ">" << Root::GEndl;
      reader.AddXMLFile(*it);
    }
    reader.Interpret();
    grlcollection = reader.GetMergedGRLCollection();

    for (unsigned int j=0; j<grlcollection.size(); ++j) {
      iovcVec.push_back( converter.GetLumiBlockRangeContainer(grlcollection[j]) );
      // default: trigger names taken from xml metadata. Overwrite any existing cmd-line triggers.
      if ( grlcollection[j].HasTriggerInfo() ) {
        triggerchainVec.push_back(grlcollection[j].GetTriggerList()); // use existing trigger names
        if (!triggerchain.empty())
          logger << Root::kWARNING << "Input goodruns-list(s) <" << grlcollection[j].GetName() 
                   << "> already contain trigger names. Cmd-line triggers are ignored!" << Root::GEndl;
      } else { // use cmdline trigger names
        triggerchainVec.push_back(triggerchain) ;
        for (unsigned int k=0; k<triggerchain.size(); ++k)
          grlcollection[j].AddMetaData( Form("TriggerName%d",k),TString(triggerchain[k]) );
      }
    }
  }


  //==========================================================================
  // Fetch up xAOD::LumiBlockRangeContainer from input ROOT files - Tree mode
  if(runtype == 4){
    // open ntuples to fetch xmlstrings
    logger << Root::kINFO << "Being in ROOT ntuple file mode..." << Root::GEndl;
    
    Root::TGoodRunsListReader reader;
    
    for(std::vector<std::string>::iterator it = rootfile.begin(); it != rootfile.end(); ++it){
      logger << Root::kINFO << "Processing root file: <" << (*it) << ">" <<  Root::GEndl;
      std::string filename = (*it);
      TFile* file = TFile::Open(filename.c_str());
      TTree * tree = NULL;
      TList * list = NULL;
      tree = dynamic_cast<TTree*>(file->Get(treename.c_str()));
      if(tree == 0){
	logger << Root::kERROR << "Tree: " << treename << " doesn't exist in file " << filename << Root::GEndl;
	exit(-1);
      }else{
	list = tree->GetUserInfo() ;
	
	// add xml string to TGoodRunsListReader. Sort out strings below
	for(int j=0; j<list->GetEntries();++j) {
	  TObjString* objstr = dynamic_cast<TObjString*>(list->At(j));
	  if (objstr==0) continue;
	  if ( objstr->GetString().BeginsWith("<?xml version=\"1.0\"?") &&
	       objstr->GetString().Contains("DOCTYPE LumiRangeCollection") ) // xml identifier
	    reader.AddXMLString(objstr->GetString());
	}
      }
      file->Close();
    }

    // do sorting of all grl objects
    reader.Interpret();
    grlcollection = reader.GetMergedGRLCollection();
    
    for (unsigned int j=0; j<grlcollection.size(); ++j) {
      iovcVec.push_back( converter.GetLumiBlockRangeContainer(grlcollection[j]) );
      // default: trigger names taken from xml metadata. Overwrite any existing cmd-line triggers.
      if ( grlcollection[j].HasTriggerInfo() ) {
	triggerchainVec.push_back(grlcollection[j].GetTriggerList()); // use existing trigger names
	if (!triggerchain.empty())
	  logger << Root::kWARNING << "Input goodruns-list(s) <" << grlcollection[j].GetName()
		   << "> already contain trigger names. Cmd-line triggers are ignored!" << Root::GEndl;
      } else { // use cmdline trigger names
	triggerchainVec.push_back(triggerchain) ;
	for (unsigned int k=0; k<triggerchain.size(); ++k)
	  grlcollection[j].AddMetaData( Form("TriggerName%d",k),TString(triggerchain[k]) );
      }
    }
  }
  
  //==========================================================================
  // Fetch up xAOD::LumiBlockRangeContainer from input ROOT files - D3PD mode
  if(runtype == 5){
    // open ntuples to fetch xmlstrings
    logger << Root::kINFO << "Being in ROOT D3PD ntuple file mode..." << Root::GEndl;

    Root::TGoodRunsListReader reader;

    for(std::vector<std::string>::iterator it = rootfile.begin(); it != rootfile.end(); ++it){
      logger << Root::kINFO << "Processing root file: <" << (*it) << ">" <<  Root::GEndl;
      std::string filename = (*it);
      TList* list = NULL;
      TFile* file = TFile::Open(filename.c_str()); 
      TDirectoryFile * dir = NULL;
      logger << Root::kINFO << "Using Directory name: " << d3pddirname.c_str() << Root::GEndl;
      dir = dynamic_cast<TDirectoryFile*>(file->GetDirectory(d3pddirname.c_str()));
      if(!dir){
	logger << Root::kERROR << "Directory [" << d3pddirname << "] doesn't exist in file " << filename << Root::GEndl;
	exit(-1);
      }else{
        TObjString* objstr = 0;
        std::map<TString,int> keymap;
	list = dir->GetListOfKeys(); 
	// add xml string to TGoodRunsListReader. Sort out strings below
	for(int j=0; j<list->GetEntries();j++) {
          if ( keymap.find(list->At(j)->GetName())==keymap.end() ) { keymap[list->At(j)->GetName()] = 1; } 
          else { keymap[list->At(j)->GetName()] = keymap[list->At(j)->GetName()]+1; }
          if(verbose)logger << Root::kINFO << "Found obj key: \"" << Form("%s;%d",list->At(j)->GetName(),keymap[list->At(j)->GetName()]) << "\"" << Root::GEndl;
	  objstr = dynamic_cast<TObjString*>(dir->Get( Form("%s;%d",list->At(j)->GetName(),keymap[list->At(j)->GetName()]) )); 
	  if (objstr!=0){
	    if(verbose)logger << Root::kINFO << "with obj: " << objstr->GetString() << Root::GEndl;
	    if ( objstr->GetString().BeginsWith("<?xml version=\"1.0\"?") &&
		 objstr->GetString().Contains("DOCTYPE LumiRangeCollection") ){ // xml identifier
	      reader.AddXMLString(objstr->GetString()); 
	    }else{
	      logger << Root::kERROR << "XML string is not in expected format: " << objstr->GetString() << ". Skipped." << Root::GEndl;
	      //exit(-1);
	    }
	  }else{
	    logger << Root::kERROR << "No obj found with key \"" << list->At(j)->GetName() << "\"" << Root::GEndl;
	    exit(-1);
	  }
	}// end for cycle
      }
      file->Close();
    }
    // do sorting of all grl objects

    reader.Interpret();
    grlcollection = reader.GetMergedGRLCollection();
    for (unsigned int j=0; j<grlcollection.size(); ++j) {
      iovcVec.push_back( converter.GetLumiBlockRangeContainer(grlcollection[j]) );
      // default: trigger names taken from xml metadata. Overwrite any existing cmd-line triggers.
      if ( grlcollection[j].HasTriggerInfo() ) {
        triggerchainVec.push_back(grlcollection[j].GetTriggerList()); // use existing trigger names
        if (!triggerchain.empty())
          logger << Root::kWARNING << "Input goodruns-list(s) <" << grlcollection[j].GetName() 
                   << "> already contain trigger names. Cmd-line triggers are ignored!" << Root::GEndl;
      } else { // use cmdline trigger names
        triggerchainVec.push_back(triggerchain) ;
        for (unsigned int k=0; k<triggerchain.size(); ++k)
          grlcollection[j].AddMetaData( Form("TriggerName%d",k),TString(triggerchain[k]) );
      }
    }
  }

  //==========================================================================
  // Run Lumi Calculation is all is well - i.e. runtype != 999
  if(runtype != 999){

    // 
    // If runtype != 0 and a run range has been specified, use that to filter runs
    // ===========================================================================
    if (runtype != 0 && runList.size() > 0) {
      std::vector<xAOD::LumiBlockRangeContainer*>::iterator iovIt = iovcVec.begin();
      for (;iovIt != iovcVec.end(); ++iovIt) {

	xAOD::LumiBlockRangeContainer::iterator it = (*iovIt)->begin();
	while (it != (*iovIt)->end()) {

	  unsigned int runnum = (*it)->startRunNumber() ;

	  bool found = false;
	  std::list<std::pair<unsigned int, unsigned int> >::iterator runIt = runList.begin();
	  for (; runIt != runList.end(); ++runIt) {
	    if (runnum < runIt->first) continue;
	    if (runnum > runIt->second) continue;
	    found = true;
	    break;
	  }

	  if (!found) {
	    logger << Root::kDEBUG << "Skipping run " << (*it)->startRunNumber() << " LB [" << (*it)->startLumiBlockNumber() << "-" << (*it)->stopLumiBlockNumber() << "] due to command-line run range" << Root::GEndl;
	    (*iovIt)->erase(it);

	    it = (*iovIt)->begin();

	  } else {
	    // logger << Root::kDEBUG << "Keeping run  " << runnum << " due to command-line run range" << Root::GEndl;
	    ++it;
	  }
	}

      }

    }


    LumiCalculator lumicalc;
    for (unsigned int j=0; j<iovcVec.size(); ++j) {
      xAOD::LumiBlockRangeContainer* iovc = iovcVec[j];
      triggerchain = triggerchainVec[j];
      
      for(std::vector<std::string>::iterator it = triggerchain.begin(); it != triggerchain.end(); ++it){
	if(!iovc->empty()){
	  logger << Root::kINFO << "--------------------------------------------" << Root::GEndl;
	  TTree tree("LumiMetaData","LumiMetaData");
	  lumicalc.setTree(&tree);
	  lumicalc.UseLumiTag(lumitag);
	  if(lumimethod != "")lumicalc.UseLumiMethod(lumimethod);
	  if(lumichannel != -1)lumicalc.UseLumiChannel(lumichannel);
	  lumicalc.UseMC(false);
	  lumicalc.UseOnlineLumi(online);
	  lumicalc.Verbose(verbose);
	  lumicalc.MakePlots(makeplots);
	  lumicalc.MakeCollList(collisionlists);
          lumicalc.ScaleL1TrigRate(scalel1trigrate);
	  lumicalc.UseLiveTrigger(uselivetrigger, livetrigger);
	  lumicalc.UseLArNoiseDB(uselar, lartag);
	  lumicalc.UseBeamspot(usebeamspot, beamspottag);
	  lumicalc.IntegrateLumi(iovc, (*it));
	  logger << Root::kINFO << "--------------------------------------------" << Root::GEndl;

	  // Write out some summary information for 'quiet' mode
	  if (quiet) {
	    lumicalc.printSummary(std::cout);
	  }
	}
      }
    }

    //==========================================================================
    // write out complete xml file for all lb collections together
    grlcollection.SetVersion(version);
    if(!grlcollection.empty()){
      TString xmlfile = "ilumicalc_merged_";
      if (xmloutfile.empty()) {
        if (grlcollection.size()==1) { xmlfile += grlcollection[0].GetSuggestedName() + ".xml"; }
        else { xmlfile += "grls.xml"; }
      } else { xmlfile = xmloutfile; }
      converter.CreateXMLFile(grlcollection,xmlfile);
    }
  }

  //==========================================================================
  // Print timing info 
  timer.Stop();
  logger << Root::kINFO << "Real time: " << std::setw(5) << timer.RealTime() << " s" << Root::GEndl;
  logger << Root::kINFO << "CPU time:  " << std::setw(5) << timer.CpuTime() << " s" << Root::GEndl;

  return 0;

}   
