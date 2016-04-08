/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "iLumiCalc.h"
#include <typeinfo>
#include <TKey.h>
#include "CollectionBase/CollectionService.h"
#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionMetadata.h"
#include "DataModelRoot/RootType.h"

#include "TTree.h"
#include "TList.h"
#include "TObjString.h"
#include "TString.h"

#include <list>

// ToDo: make this whole code in
//       object oriented way...

//______________________________________________________________________________
int main(int argc, char * argv[]){

  if (argc == 1)print_usage();

  // enable cintex for proper stl functioning in combo with ROOT
  RootType::EnableCintex();

  // schedule for time info 
  TStopwatch timer;
  timer.Start();

  // Parse command-line options
  // ==========================

  gengetopt_args_info args_info;
  cmdline_parser (argc, argv, &args_info);

  // Output Level
  bool m_verbose = args_info.verbose_flag;
  bool m_quiet = args_info.quiet_flag;
  if (m_verbose && m_quiet) {
    m_logger << Root::kWARNING << "Can't specify verbose and quiet, using verbose" << Root::GEndl;
    m_quiet = false;
  }

  if (m_verbose) Root::TMsgLogger::SetMinLevel(Root::kDEBUG);
  if (m_quiet) Root::TMsgLogger::SetMinLevel(Root::kWARNING);

  // Other flags
  bool m_makeplots = args_info.plots_flag;
  bool m_collisionlists = args_info.xml_collisionlist_flag;

  // parse for any output xml file name
  m_xmloutfile = "";
  if(args_info.xml_out_given){
    m_xmloutfile = args_info.xml_out_arg;
  }

  //
  // Parameters to control luminosty calculation
  // ===========================================

  float m_scalel1trigrate = float(args_info.scale_lumi_arg); // = 1 by default
  if (m_scalel1trigrate != 1.) {
    m_logger << Root::kWARNING << "Luminoisty scaled by factor: " << m_scalel1trigrate << Root::GEndl;
  }

  // Use online folder
  bool m_online = args_info.online_flag;
  if (m_online) {
    m_logger << Root::kINFO << "Luminosity read from online folders" << Root::GEndl;
  }

  // Luminosity tag
  std::string m_lumitag = args_info.lumitag_arg;
  if (m_online) {
    m_logger << Root::kWARNING << "Lumitag: " << m_lumitag << " ignored due to --online " << Root::GEndl;
  } else {
    m_logger << Root::kINFO << "Lumitag: " << m_lumitag << Root::GEndl;
  }

  // LAr noise bursts
  bool m_lar = args_info.lar_flag;
  std::string m_lartag = args_info.lartag_arg;
  if (m_lar) {
    m_logger << Root::kINFO << "LAr noise burst inefficiency will be calculated from " << m_lartag << Root::GEndl;
  }

  // Online Beamspot validity
  bool m_beamspot = args_info.beamspot_flag;
  std::string m_beamspottag = args_info.beamspottag_arg;
  if (m_beamspot) {
    m_logger << Root::kINFO << "Livetime will include online beamspot validity requirement from " << m_beamspottag << Root::GEndl;
  }

  // Luminosity channel
  // ==================

  std::string m_lumimethod;
  int m_lumichannel = -1;

  // Check for online exceptions
  if (m_online && args_info.lumimethod_given){
    m_logger << Root::kERROR << "Sorry, the online database doesn't have luminosity method names, use --lumichannel instead!" << Root::GEndl;
    exit(-1);
  }

  if (args_info.lumimethod_given && args_info.lumichannel_given == 0) {
    m_lumimethod  =     args_info.lumimethod_arg;
    m_logger << Root::kINFO << "Lumimethod: " << m_lumimethod << Root::GEndl;
  } else if (args_info.lumimethod_given == 0 && args_info.lumichannel_given) {
    m_lumichannel = args_info.lumichannel_arg;
    m_logger << Root::kINFO << "Lumichannel: " << m_lumichannel << Root::GEndl;
  } else if (args_info.lumimethod_given && args_info.lumichannel_given) {
    m_lumichannel = args_info.lumichannel_arg;
    m_logger << Root::kINFO << "Both lumimethod and lumichannel is given, defaulting to Lumichannel: " << m_lumichannel << Root::GEndl;
  } else if (args_info.lumimethod_given == 0 && args_info.lumichannel_given == 0) {
    if (m_online) {
      m_lumichannel = args_info.lumichannel_arg;
      m_logger << Root::kINFO << "No lumimethod or lumichannel is given, defaulting to Lumichannel: " << m_lumichannel << Root::GEndl;
    } else {
      m_lumimethod = args_info.lumimethod_arg;
      m_logger << Root::kINFO << "No lumimethod or lumichannel is given, defaulting to Lumimethod: " << m_lumimethod << Root::GEndl;
    }
  }


  // Handle triggers
  // ===============
  if(args_info.trigger_given == 0) {
    m_logger << Root::kINFO << "No trigger specified, proceeding with --trigger=None" << Root::GEndl;
    m_triggerchain.push_back("None");
  } else {
    if (args_info.trigger_given > 1) {
      m_logger << Root::kINFO << "Processing Triggers: ";
    } else {
      m_logger << Root::kINFO << "Processing Trigger: ";
    }

    for(unsigned int i = 0; i < args_info.trigger_given; ++i){
      m_triggerchain.push_back(args_info.trigger_arg[i]);
      m_logger << Root::kINFO << args_info.trigger_arg[i] << ", ";
    }
    m_logger << Root::GEndl;
  }

  // Livetime triggers
  // =================
  bool m_uselivetrigger = false;
  m_livetrigger = args_info.livetrigger_arg;   // Default if not specified on command line

  if (args_info.livetrigger_given || !args_info.trigger_given) {
    // Either livetime trigger specified, or no prescale triggers specified
    m_uselivetrigger = true;
    m_logger << Root::kINFO << "Trigger used for livetime: " << m_livetrigger << Root::GEndl;
  } else {
    // Prescale not specified AND trigger list given 
    m_logger << Root::kINFO << "Prescale trigger chains will be used for livetime " << Root::GEndl;
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
	m_logger << Root::kWARNING << "Problem: file ["<< (*it)  <<"] may not exist.  Will try anyways..." << Root::GEndl;
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
	m_logger << Root::kWARNING << "Problem: file ["<< (*it)  <<"] may not exist.  Will try anyways..." << Root::GEndl;
      }
    }
  //std::cout << "RunType: " << runtype << std::endl;
  }

  // ROOT File -- ttree attached mode
  //============================================
  if((args_info.root_given >0 || args_info.tree_given > 0) && args_info.d3pd_dir_given == 0 && args_info.xml_given == 0 && args_info.tag_given == 0){
    if((args_info.root_given == 0 && args_info.tree_given > 0) || (args_info.root_given > 0 && args_info.tree_given == 0 )){
      m_logger << Root::kERROR << "Please provide BOTH --root=\"myfile.root\" AND --tree=\"mytreename\" OR --d3pd_dir=\"mydirname\" options " << Root::GEndl;
      exit(-1);
    }
    
    runtype = 4;
    for(unsigned int i = 0; i < args_info.root_given; ++i){
      rootfile.push_back(args_info.root_arg[i]);
    }
    if(args_info.tree_given){
      m_treename = args_info.tree_arg;
    }else{
      m_logger << Root::kERROR << "In Root file mode Tree name (--d3p_dir=\"mytreename\") must also be provided" << Root::GEndl;
    }
    for(std::vector<std::string>::iterator it = rootfile.begin(); it != rootfile.end(); ++it){
      if(!FileExists((*it))){
	m_logger << Root::kWARNING << "Problem: file ["<< (*it)  <<"] may not exist.  Will try anyways..." << Root::GEndl;
      }
    }
  }

  // ROOT File -- d3pd TDirectory mode
  //============================================
  if((args_info.root_given >0 || args_info.d3pd_dir_given > 0) && args_info.tree_given == 0 && args_info.xml_given == 0 && args_info.tag_given == 0){
    if((args_info.root_given == 0 && args_info.d3pd_dir_given > 0) || (args_info.root_given > 0 && args_info.d3pd_dir_given == 0 )){
      m_logger << Root::kERROR << "Please provide BOTH --root=\"myfile.root\" AND --d3pd_dir=\"myd3pddirname\" options" << Root::GEndl;
      exit(-1);
    }

    runtype = 5;
    for(unsigned int i = 0; i < args_info.root_given; ++i){
      rootfile.push_back(args_info.root_arg[i]);
    }
    if(args_info.d3pd_dir_given){
      m_d3pddirname = args_info.d3pd_dir_arg;
    }else{
      m_logger << Root::kWARNING << "In D3PD Root file mode Directory name (--d3pd_dir=\"mylumidir\") must also be provided" << Root::GEndl;
      m_logger << Root::kWARNING << "Ommitted, using default name \"Lumi\"" << Root::GEndl;
      m_d3pddirname= "Lumi";
    }

    for(std::vector<std::string>::iterator it = rootfile.begin(); it != rootfile.end(); ++it){
      if(!FileExists((*it))){
	m_logger << Root::kWARNING << "Problem: file ["<< (*it)  <<"] may not exist.  Will try anyways..." << Root::GEndl;
      }
    }
  }

  // Full command line mode
  //============================================
  if (runtype == 0) {

    // Nothing else specified, try run numbers
    if (!args_info.runnumber_given) {
      m_logger << Root::kERROR << "No input data specified!" << Root::GEndl;
      exit(-1);
    }

    std::list<std::pair<unsigned int, unsigned int> >::iterator itr = runList.begin();

    // Decode run IOV pairs
    for (; itr != runList.end(); itr++) {
      // Dont allow open-ended IOVs
      if ((itr->first == minrunnum) || (itr->second == maxrunnum)) {
	m_logger << Root::kERROR << "Can't use open-ended run ranges to specify sample!" << Root::GEndl;
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
  std::vector< std::vector<std::string> > m_triggerchainVec;
  Root::TGRLCollection m_grlcollection;
  LumiBlockRangeContainerConverter m_converter;
  TString m_version("30"); // [0-10): ATLRunQuery, [10-20): ntuple production, [20-30): xml merging, [30-40): LumiCalc

  //==========================================================================
  // User defined IOVRange in command line
  if (runtype == 0) {

    m_logger << Root::kINFO << "Proceeding with command-line run list" << Root::GEndl;
    if (lbstart.size() != lbend.size()) {
      m_logger << Root::kERROR << "number of lbstart and lbend values must match!" << Root::GEndl;
      exit(-1); 
    }

    if (runnumber.size() > 1 && (runnumber.size() != lbstart.size())) {
      m_logger << Root::kERROR << "number of lbstart and lbend values must match number of runs with multiple runs specified!" << Root::GEndl;
      exit(-1);
    }

    /*
    std::cout << "runlist length: " << runnumber.size() << std::endl;
    for (unsigned int i=0; i<runnumber.size(); i++) {
      std::cout << runnumber[i] << std::endl;
    }
    */

    uint32_t _lbstart;
    uint32_t _lbend;

    xAOD::LumiBlockRangeContainer* iovc = new xAOD::LumiBlockRangeContainer();
    xAOD::LumiBlockRangeAuxContainer* iovcAux = new xAOD::LumiBlockRangeAuxContainer();
    iovc->setStore( iovcAux );

    std::vector<uint32_t>::iterator itstart;
    std::vector<uint32_t>::iterator itend;
    std::vector<uint32_t>::iterator itrun;

    if (runnumber.size() == 1) {

	m_logger << Root::kINFO << "Runnumber [" << runnumber[0] <<  "]" << Root::GEndl;
	for(itstart = lbstart.begin(), itend = lbend.begin(); 
	    itstart != lbstart.end() && itend != lbend.end(); ++itstart,++itend) {
	  _lbstart = (*itstart);
	  _lbend = (*itend);
	  m_logger << Root::kINFO << "lbstart-lbend [" << _lbstart << "-" << _lbend << "]" << Root::GEndl; 
	  if (_lbstart > _lbend) {
	    m_logger << Root::kERROR << "lbstart > lbend! Should be: lbstart < = lbend" << Root::GEndl;
	    exit(-1);
	  } else {
	    xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange();
	    iovr->setStartRunNumber(runnumber[0]);
	    iovr->setStartLumiBlockNumber(_lbstart);
	    iovr->setStopRunNumber(runnumber[0]);
	    iovr->setStopLumiBlockNumber(_lbend);
	    iovc->push_back(iovr);
	  }
	}

    } else if (runnumber.size() > 1) {

      for(itrun = runnumber.begin(), itstart = lbstart.begin(), itend = lbend.begin(); 
	  itrun != runnumber.end() && itstart != lbstart.end() && itend != lbend.end();
	  ++itrun, ++itstart, ++itend) {
	m_logger << Root::kINFO << "Runnumbers [" << *itrun <<  "]" << Root::GEndl;
	m_logger << Root::kINFO << "lbstart-lbend [" << *itstart << "-" << *itend << "]" << Root::GEndl; 
	    xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange();
	    iovr->setStartRunNumber(*itrun);
	    iovr->setStartLumiBlockNumber(*itstart);
	    iovr->setStopRunNumber(*itrun);
	    iovr->setStopLumiBlockNumber(*itend);
	    iovc->push_back(iovr);
      }
    }

    iovcVec.push_back(iovc); // take over iovc for usage below
    m_triggerchainVec.push_back(m_triggerchain); // cmd-line triggerchain
    std::map<TString,TString> m_metadata;
    for (unsigned int j=0; j<m_triggerchain.size(); ++j)
      m_metadata[Form("TriggerName%d",j)] = TString(m_triggerchain[j]);
    m_grlcollection.push_back( *m_converter.GetGRLObject(*iovc,m_metadata,m_version) );
  }
  

  //==========================================================================
  // Fetch up xAOD::LumiBlockRangeContainer from input TAG file
  if (runtype == 1) {
    // open TAG files to build xAOD::LumiBlockRangeContainer
    m_logger << Root::kINFO << "Being in TAG file mode..." << Root::GEndl;

    Root::TGoodRunsListReader m_reader;
    std::string connection ="";
    std::string type = "RootCollection";
    bool readOnly(true);
    for(std::vector<std::string>::iterator it = tagfile.begin(); it != tagfile.end(); ++it){
      m_logger << Root::kINFO << "Processing file: <" << (*it) << ">" <<  Root::GEndl;
      int n = (*it).find(".root");
      std::string tagfilename =  (*it).substr(0,n);

      // get Value for a Key
      pool::CollectionService collectionService;
      pool::ICollection* collection = collectionService.handle(tagfilename, type, connection, readOnly);
      if(collection == NULL) {
         m_logger << Root::kERROR << "ICollection is NULL, exiting... " << Root::GEndl;
         exit(-1);
      }

      // MB : Reading incomplete LBs with tag. Request from Tulay
      const char* value = collection->metadata().getValueForKey("OutputLumirange");
      if(value == NULL) {         
         m_logger << Root::kERROR << "The collection has no such key of OutputLumirange in metadata, try with OutputIncompleteLumirange key" << Root::GEndl;
         value = collection->metadata().getValueForKey("OutputIncompleteLumirange");
         if (value != NULL) m_logger << Root::kINFO << "OutputIncompleteLumirange key is OK, reading the value..."  << Root::GEndl;
         else exit(-1);
      }

      if(m_verbose == true) m_logger << Root::kINFO << "Value :  " << value << Root::GEndl;
      // add xml string to TGoodRunsListReader. Sort out strings below
      m_reader.AddXMLString(value);
    }
    // do sorting of all grl objects
    m_reader.Interpret();
    m_grlcollection = m_reader.GetMergedGRLCollection();
    
    for (unsigned int j=0; j<m_grlcollection.size(); ++j) {
      iovcVec.push_back( m_converter.GetLumiBlockRangeContainer(m_grlcollection[j]) );
      // default: trigger names taken from xml metadata. Overwrite any existing cmd-line triggers.
      if ( m_grlcollection[j].HasTriggerInfo() ) {
        m_triggerchainVec.push_back(m_grlcollection[j].GetTriggerList()); // use existing trigger names
        if (!m_triggerchain.empty())
          m_logger << Root::kWARNING << "Input goodruns-list(s) <" << m_grlcollection[j].GetName() 
                   << "> already contain trigger names. Cmd-line triggers are ignored!" << Root::GEndl;
      } else { // use cmdline trigger names
        m_triggerchainVec.push_back(m_triggerchain) ;
        for (unsigned int k=0; k<m_triggerchain.size(); ++k)
          m_grlcollection[j].AddMetaData( Form("TriggerName%d",k),TString(m_triggerchain[k]) );
      }
    }
  }

  //==========================================================================
  // Fetch up xAOD::LumiBlockRangeContainer from input XML file
  if(runtype == 3){
    // open XML files to build xAOD::LumiBlockRangeContainer
    m_logger << Root::kINFO << "Being in XML file mode..." << Root::GEndl;
    Root::TGoodRunsListReader m_reader;
    // looping over XML files
    for(std::vector<std::string>::iterator it = xmlfile.begin(); it != xmlfile.end(); ++it){
      m_logger << Root::kINFO << "Processing file: <" << (*it) << ">" << Root::GEndl;
      m_reader.AddXMLFile(*it);
    }
    m_reader.Interpret();
    m_grlcollection = m_reader.GetMergedGRLCollection();

    for (unsigned int j=0; j<m_grlcollection.size(); ++j) {
      iovcVec.push_back( m_converter.GetLumiBlockRangeContainer(m_grlcollection[j]) );
      // default: trigger names taken from xml metadata. Overwrite any existing cmd-line triggers.
      if ( m_grlcollection[j].HasTriggerInfo() ) {
        m_triggerchainVec.push_back(m_grlcollection[j].GetTriggerList()); // use existing trigger names
        if (!m_triggerchain.empty())
          m_logger << Root::kWARNING << "Input goodruns-list(s) <" << m_grlcollection[j].GetName() 
                   << "> already contain trigger names. Cmd-line triggers are ignored!" << Root::GEndl;
      } else { // use cmdline trigger names
        m_triggerchainVec.push_back(m_triggerchain) ;
        for (unsigned int k=0; k<m_triggerchain.size(); ++k)
          m_grlcollection[j].AddMetaData( Form("TriggerName%d",k),TString(m_triggerchain[k]) );
      }
    }
  }


  //==========================================================================
  // Fetch up xAOD::LumiBlockRangeContainer from input ROOT files - Tree mode
  if(runtype == 4){
    // open ntuples to fetch xmlstrings
    m_logger << Root::kINFO << "Being in ROOT ntuple file mode..." << Root::GEndl;
    
    Root::TGoodRunsListReader m_reader;
    
    for(std::vector<std::string>::iterator it = rootfile.begin(); it != rootfile.end(); ++it){
      m_logger << Root::kINFO << "Processing root file: <" << (*it) << ">" <<  Root::GEndl;
      std::string filename = (*it);
      TFile* file = TFile::Open(filename.c_str());
      TTree * tree = NULL;
      TList * list = NULL;
      tree = dynamic_cast<TTree*>(file->Get(m_treename.c_str()));
      if(tree == 0){
	m_logger << Root::kERROR << "Tree: " << m_treename << " doesn't exist in file " << filename << Root::GEndl;
	exit(-1);
      }else{
	list = tree->GetUserInfo() ;
	
	// add xml string to TGoodRunsListReader. Sort out strings below
	for(int j=0; j<list->GetEntries();++j) {
	  TObjString* objstr = dynamic_cast<TObjString*>(list->At(j));
	  if (objstr==0) continue;
	  if ( objstr->GetString().BeginsWith("<?xml version=\"1.0\"?") &&
	       objstr->GetString().Contains("DOCTYPE LumiRangeCollection") ) // xml identifier
	    m_reader.AddXMLString(objstr->GetString());
	}
      }
      file->Close();
    }

    // do sorting of all grl objects
    m_reader.Interpret();
    m_grlcollection = m_reader.GetMergedGRLCollection();
    
    for (unsigned int j=0; j<m_grlcollection.size(); ++j) {
      iovcVec.push_back( m_converter.GetLumiBlockRangeContainer(m_grlcollection[j]) );
      // default: trigger names taken from xml metadata. Overwrite any existing cmd-line triggers.
      if ( m_grlcollection[j].HasTriggerInfo() ) {
	m_triggerchainVec.push_back(m_grlcollection[j].GetTriggerList()); // use existing trigger names
	if (!m_triggerchain.empty())
	  m_logger << Root::kWARNING << "Input goodruns-list(s) <" << m_grlcollection[j].GetName()
		   << "> already contain trigger names. Cmd-line triggers are ignored!" << Root::GEndl;
      } else { // use cmdline trigger names
	m_triggerchainVec.push_back(m_triggerchain) ;
	for (unsigned int k=0; k<m_triggerchain.size(); ++k)
	  m_grlcollection[j].AddMetaData( Form("TriggerName%d",k),TString(m_triggerchain[k]) );
      }
    }
  }
  
  //==========================================================================
  // Fetch up xAOD::LumiBlockRangeContainer from input ROOT files - D3PD mode
  if(runtype == 5){
    // open ntuples to fetch xmlstrings
    m_logger << Root::kINFO << "Being in ROOT D3PD ntuple file mode..." << Root::GEndl;

    Root::TGoodRunsListReader m_reader;

    for(std::vector<std::string>::iterator it = rootfile.begin(); it != rootfile.end(); ++it){
      m_logger << Root::kINFO << "Processing root file: <" << (*it) << ">" <<  Root::GEndl;
      std::string filename = (*it);
      TList* list = NULL;
      TFile* file = TFile::Open(filename.c_str()); 
      TDirectoryFile * dir = NULL;
      m_logger << Root::kINFO << "Using Directory name: " << m_d3pddirname.c_str() << Root::GEndl;
      dir = dynamic_cast<TDirectoryFile*>(file->GetDirectory(m_d3pddirname.c_str()));
      if(!dir){
	m_logger << Root::kERROR << "Directory [" << m_d3pddirname << "] doesn't exist in file " << filename << Root::GEndl;
	exit(-1);
      }else{
        TObjString* objstr = 0;
        std::map<TString,int> keymap;
	list = dir->GetListOfKeys(); 
	// add xml string to TGoodRunsListReader. Sort out strings below
	for(int j=0; j<list->GetEntries();j++) {
          if ( keymap.find(list->At(j)->GetName())==keymap.end() ) { keymap[list->At(j)->GetName()] = 1; } 
          else { keymap[list->At(j)->GetName()] = keymap[list->At(j)->GetName()]+1; }
          if(m_verbose)m_logger << Root::kINFO << "Found obj key: \"" << Form("%s;%d",list->At(j)->GetName(),keymap[list->At(j)->GetName()]) << "\"" << Root::GEndl;
	  objstr = dynamic_cast<TObjString*>(dir->Get( Form("%s;%d",list->At(j)->GetName(),keymap[list->At(j)->GetName()]) )); 
	  if (objstr!=0){
	    if(m_verbose)m_logger << Root::kINFO << "with obj: " << objstr->GetString() << Root::GEndl;
	    if ( objstr->GetString().BeginsWith("<?xml version=\"1.0\"?") &&
		 objstr->GetString().Contains("DOCTYPE LumiRangeCollection") ){ // xml identifier
	      m_reader.AddXMLString(objstr->GetString()); 
	    }else{
	      m_logger << Root::kERROR << "XML string is not in expected format: " << objstr->GetString() << ". Skipped." << Root::GEndl;
	      //exit(-1);
	    }
	  }else{
	    m_logger << Root::kERROR << "No obj found with key \"" << list->At(j)->GetName() << "\"" << Root::GEndl;
	    exit(-1);
	  }
	}// end for cycle
      }
      file->Close();
    }
    // do sorting of all grl objects

    m_reader.Interpret();
    m_grlcollection = m_reader.GetMergedGRLCollection();
    for (unsigned int j=0; j<m_grlcollection.size(); ++j) {
      iovcVec.push_back( m_converter.GetLumiBlockRangeContainer(m_grlcollection[j]) );
      // default: trigger names taken from xml metadata. Overwrite any existing cmd-line triggers.
      if ( m_grlcollection[j].HasTriggerInfo() ) {
        m_triggerchainVec.push_back(m_grlcollection[j].GetTriggerList()); // use existing trigger names
        if (!m_triggerchain.empty())
          m_logger << Root::kWARNING << "Input goodruns-list(s) <" << m_grlcollection[j].GetName() 
                   << "> already contain trigger names. Cmd-line triggers are ignored!" << Root::GEndl;
      } else { // use cmdline trigger names
        m_triggerchainVec.push_back(m_triggerchain) ;
        for (unsigned int k=0; k<m_triggerchain.size(); ++k)
          m_grlcollection[j].AddMetaData( Form("TriggerName%d",k),TString(m_triggerchain[k]) );
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
      // std::vector<std::vector<std::string> >::iterator trigIt = m_triggerchainVec.begin();

      for (;iovIt != iovcVec.end(); iovIt++) {

	xAOD::LumiBlockRangeContainer::iterator it = (*iovIt)->begin();
	while (it != (*iovIt)->end()) {

	  unsigned int runnum = (*it)->startRunNumber() ;

	  bool found = false;
	  std::list<std::pair<unsigned int, unsigned int> >::iterator runIt = runList.begin();
	  for (; runIt != runList.end(); runIt++) {
	    if (runnum < runIt->first) continue;
	    if (runnum > runIt->second) continue;
	    found = true;
	    break;
	  }

	  if (!found) {
	    m_logger << Root::kDEBUG << "Skipping run " << (*it)->startRunNumber() << " LB [" << (*it)->startLumiBlockNumber() << "-" << (*it)->stopLumiBlockNumber() << "] due to command-line run range" << Root::GEndl;
	    (*iovIt)->erase(it);

	    it = (*iovIt)->begin();

	  } else {
	    // m_logger << Root::kDEBUG << "Keeping run  " << runnum << " due to command-line run range" << Root::GEndl;
	    it++;
	  }
	}

      }

    }


    LumiCalculator m_lumicalc;
    for (unsigned int j=0; j<iovcVec.size(); ++j) {
      xAOD::LumiBlockRangeContainer* iovc = iovcVec[j];
      m_triggerchain = m_triggerchainVec[j];
      
      for(std::vector<std::string>::iterator it = m_triggerchain.begin(); it != m_triggerchain.end(); ++it){
	if(!iovc->empty()){
	  m_logger << Root::kINFO << "--------------------------------------------" << Root::GEndl;
	  TTree tree("LumiMetaData","LumiMetaData");
	  m_lumicalc.setTree(&tree);
	  m_lumicalc.UseLumiTag(m_lumitag);
	  if(m_lumimethod != "")m_lumicalc.UseLumiMethod(m_lumimethod);
	  if(m_lumichannel != -1)m_lumicalc.UseLumiChannel(m_lumichannel);
	  m_lumicalc.UseMC(false);
	  m_lumicalc.UseOnlineLumi(m_online);
	  m_lumicalc.Verbose(m_verbose);
	  m_lumicalc.MakePlots(m_makeplots);
	  m_lumicalc.MakeCollList(m_collisionlists);
          m_lumicalc.ScaleL1TrigRate(m_scalel1trigrate);
	  m_lumicalc.UseLiveTrigger(m_uselivetrigger, m_livetrigger);
	  m_lumicalc.UseLArNoiseDB(m_lar, m_lartag);
	  m_lumicalc.UseBeamspot(m_beamspot, m_beamspottag);
	  m_lumicalc.IntegrateLumi(iovc, (*it));
	  m_logger << Root::kINFO << "--------------------------------------------" << Root::GEndl;

	  // Write out some summary information for 'quiet' mode
	  if (m_quiet) {
	    m_lumicalc.printSummary(std::cout);
	  }
	}
      }
    }

    //==========================================================================
    // write out complete xml file for all lb collections together
    m_grlcollection.SetVersion(m_version);
    if(!m_grlcollection.empty()){
      TString xmlfile = "ilumicalc_merged_";
      if (m_xmloutfile.empty()) {
        if (m_grlcollection.size()==1) { xmlfile += m_grlcollection[0].GetSuggestedName() + ".xml"; }
        else { xmlfile += "grls.xml"; }
      } else { xmlfile = m_xmloutfile; }
      m_converter.CreateXMLFile(m_grlcollection,xmlfile);
    }
  }

  //==========================================================================
  // Print timing info 
  timer.Stop();
  m_logger << Root::kINFO << "Real time: " << std::setw(5) << timer.RealTime() << " s" << Root::GEndl;
  m_logger << Root::kINFO << "CPU time:  " << std::setw(5) << timer.CpuTime() << " s" << Root::GEndl;

  return 0;

}   
