/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for Athena-Rivet interface

#include "Rivet_i/Rivet_i.h"
#include "Rivet_i/LogLevels.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "GeneratorObjects/McEventCollection.h"
#include "GenInterfaces/IxAODtoHepMCTool.h"
#include "AthenaKernel/errorcheck.h"
#include "PathResolver/PathResolver.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ITHistSvc.h"

#include "Rivet/Rivet.hh"
#include "Rivet/Analysis.hh"
#include "Rivet/Tools/RivetYODA.hh"

//#include "TH1D.h"
//#include "TGraphAsymmErrors.h"

//#include "YODA/ROOTCnv.h"

#include <cstdlib>
#include <cstdio>
#include <memory>
#include <regex>

using namespace std;


/// Convert to inherit from GenBase, now we're developing Rivet 3 interfacing for R21+ only

Rivet_i::Rivet_i(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_xAODtoHepMCTool("xAODtoHepMCTool"),
  m_analysisHandler(0),
  m_init(false)
{
  // Options
  declareProperty("McEventKey", m_genEventKey="GEN_EVENT");
  declareProperty("Analyses", m_analysisNames);
  declareProperty("CrossSection", m_crossSection=0.0);
  declareProperty("CrossSectionUncertainty", m_crossSection_uncert=0.0);
  declareProperty("Stream", m_stream="/Rivet");
  declareProperty("RunName", m_runname="");
  declareProperty("HistoFile", m_file="Rivet.yoda.gz");
  declareProperty("HistoPreload", m_preload="");
  declareProperty("AnalysisPath", m_anapath="");
  declareProperty("IgnoreBeamCheck", m_ignorebeams=false);
  declareProperty("DoRootHistos", m_doRootHistos=false);
  declareProperty("SkipWeights", m_skipweights=false);
  declareProperty("MatchWeights", m_matchWeights="");
  declareProperty("UnmatchWeights", m_unmatchWeights="");
  declareProperty("WeightCap", m_weightcap=-1.0);
  declareProperty("AddMissingBeamParticles",m_patchBeams=false);
  // Service handles
  //declareProperty("THistSvc", m_histSvc);
}

string getenv_str(const string& key) {
  char const* val = getenv(key.c_str());
  return val == NULL ? string() : string(val);
}


StatusCode Rivet_i::initialize() {
  ATH_MSG_DEBUG("Rivet_i initializing");
  ATH_MSG_INFO("Using Rivet version " << Rivet::version());
  
  // Get tool for xAOD::Truth to HepMC::GenEvent conversion
  ATH_CHECK(m_xAODtoHepMCTool.retrieve());

  // Get histo service
  //if (m_doRootHistos && m_histSvc.retrieve().isFailure()) {
  // ATH_MSG_FATAL("Failed to retrieve service " << m_histSvc);
  // return StatusCode::FAILURE;
  //}

  // Set RIVET_ANALYSIS_PATH based on alg setup

  // First set (overwrite, if necessary) the RIVET_ANALYSIS_PATH variable
  string env_rap(getenv_str("RIVET_ANALYSIS_PATH"));
  if (m_anapath.size() > 0) {
    ATH_MSG_INFO("Setting Rivet plugin analyses loader env path: " << m_anapath);
    if (!env_rap.empty()) ATH_MSG_INFO("Overwriting environment's RIVET_ANALYSIS_PATH = " << env_rap << "!");
    setenv("RIVET_ANALYSIS_PATH", m_anapath.c_str(), 1);
  }

  // Now horrid runtime ATLAS env variable and CMT path mangling to work out the std analysis plugin search paths
  vector<string> anapaths;
  const string cmtpath = getenv_str("CMTPATH");
  if (cmtpath.empty()) {
    ATH_MSG_WARNING("$CMTPATH variable not set: finding the main analysis plugin directory will be difficult...");
  } 
  else {
    vector<string> cmtpaths;
    std::stringstream ss(cmtpath);
    std::string item;
    while (std::getline(ss, item, ':')) {
      cmtpaths.push_back(std::move(item));
    }
    const string cmtconfig = getenv_str("CMTCONFIG");
    if (cmtconfig.empty()) {
      ATH_MSG_WARNING("$CMTCONFIG variable not set: finding the main analysis plugin directory will be difficult...");
    } 
    else {
      const string libpath = "/InstallArea/" + cmtconfig + "/lib";
      for (const string& p : cmtpaths) {
        const string cmtlibpath = p + libpath;
        if (PathResolver::find_file_from_list("RivetMCAnalyses.so", cmtlibpath).empty()) continue;
        ATH_MSG_INFO("Appending " + cmtlibpath + " to default Rivet analysis search path");
        anapaths.push_back(cmtlibpath);
        break;
      }
    }
  }

  // Then re-grab RIVET_ANALYSIS_PATH and append all the discovered std plugin paths to it
  string anapathstr = getenv_str("RIVET_ANALYSIS_PATH");
  for (const string& ap : anapaths) {
    if (anapathstr.size() > 0) anapathstr += ":";
    anapathstr += ap;
  }
  setenv("RIVET_ANALYSIS_PATH", anapathstr.c_str(), 1);

  // Get the final form of RIVET_ANALYSIS_PATH and talk about it. Phew.
  env_rap = getenv_str("RIVET_ANALYSIS_PATH");
  if (!env_rap.empty()) ATH_MSG_DEBUG("Loading Rivet plugin analyses from env path: " << env_rap);

  // Set up analysis handler
  m_analysisHandler = new Rivet::AnalysisHandler(m_runname);
  assert(m_analysisHandler);
  m_analysisHandler->setIgnoreBeams(m_ignorebeams); //< Whether to do beam ID/energy consistency checks
  m_analysisHandler->skipMultiWeights(m_skipweights); //< Only run on the nominal weight
  //m_analysisHandler->selectMultiWeights(m_matchWeights); //< Only run on a subset of the multi-weights
  //m_analysisHandler->deselectMultiWeights(m_unmatchWeights); //< Veto a subset of the multi-weights
  if(m_weightcap>0) m_analysisHandler->setWeightCap(m_weightcap);

  // Set Rivet native log level to match Athena
  Rivet::Log::setLevel("Rivet", rivetLevel(msg().level()));

  // Get all available analysis names
  if (msgLvl(MSG::VERBOSE)) {
    vector<string> analysisNames = Rivet::AnalysisLoader::analysisNames();
    ATH_MSG_VERBOSE("List of available Rivet analyses:");
    for (const string& a : analysisNames)  ATH_MSG_VERBOSE(" " + a);
  }

  // Add analyses
  for (const string& a : m_analysisNames) {
    ATH_MSG_INFO("Loading Rivet analysis " << a);
    m_analysisHandler->addAnalysis(a);
    Rivet::Log::setLevel("Rivet.Analysis."+a, rivetLevel(msg().level()));
  }

  // Initialise Rivet
  // m_analysisHandler->init();

  //load a pre-existing yoda file to initialize histograms
  if (m_preload!= "") {
    m_analysisHandler->readData(m_preload);
  }


  return StatusCode::SUCCESS;
}


StatusCode Rivet_i::execute() {
  ATH_MSG_DEBUG("Rivet_i execute");

  m_needsConversion = !evtStore()->contains<McEventCollection>("GEN_EVENT");
  ATH_MSG_DEBUG("Rivet_i needs xAOD::Truth to HepMC::GenEvent conversion? " << m_needsConversion);

  // Get the event collection
  /// @todo Replace with new GenBase functionality
  const HepMC::GenEvent* checkedEvent;
  if (m_needsConversion) {
    const DataHandle<EventInfo> eventInfo;
    if (evtStore()->retrieve(eventInfo).isFailure()) {
      ATH_MSG_ERROR("Could not retrieve EventInfo, aborting.");
      return StatusCode::FAILURE;
    }
    unsigned long long eventNumber = eventInfo.cptr()->event_ID()->event_number();

    const xAOD::TruthEventContainer* truthCollection;
    if (evtStore()->retrieve(truthCollection, "TruthEvents").isFailure()) {
      ATH_MSG_ERROR("Could not retrieve TruthEvents collection, aborting.");
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG("Attempt xAOD::Truth to HepMC::GenEvent conversion.");
    const std::vector<HepMC::GenEvent>& hepmc_evts = m_xAODtoHepMCTool->getHepMCEvents(truthCollection, eventNumber);
    if (hepmc_evts.empty()) {
      ATH_MSG_ERROR("Conversion didn't yield HepMC::GenEvent, aborting.");
      return StatusCode::FAILURE;
    }
    checkedEvent = checkEvent(hepmc_evts[0]);
   } 
   else {
    const McEventCollection* eventCollection;
    if (evtStore()->retrieve(eventCollection, m_genEventKey).isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve event collection from StoreGate with key " << m_genEventKey);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("Retrieved event collection from StoreGate with key " << m_genEventKey);
    }
    // Get the first event in the event collection (the signal event; additional members would be e.g. PU event)
    /// @todo Replace with new GenBase const_event() functionality
    if (eventCollection->front() == NULL) {
      ATH_MSG_ERROR("Rivet_i received a null HepMC event");
      return StatusCode::FAILURE;
    }
    checkedEvent = checkEvent(*eventCollection->front());
  }
  
  if(!checkedEvent) {
    ATH_MSG_ERROR("Check on HepMC event failed!");
    return StatusCode::FAILURE;
  }

  // Initialize Rivet (on the first event only)
  if (!m_init) {
    m_analysisHandler->init(*checkedEvent);
    m_init = true;
  }

  // Analyse the event
  m_analysisHandler->analyze(*checkedEvent);

  delete checkedEvent;

  return StatusCode::SUCCESS;
}


StatusCode Rivet_i::finalize() {
  ATH_MSG_INFO("Rivet_i finalizing");

  // Setting cross-section in Rivet
  // If no user-specified cross-section available,
  // take whatever is in the GenEvent
  if (m_crossSection != 0)   m_analysisHandler->setCrossSection({m_crossSection, m_crossSection_uncert});
  
  m_analysisHandler->finalize();

  // Convert YODA-->ROOT
  //This does not work in rivet 3 so temporarily disabled.
  /*if (m_doRootHistos) {
    for (const Rivet::AnalysisObjectPtr ao : m_analysisHandler->getData()) {
      // Normalize path name to be usable by ROOT
      string path = string(ao->path());
      std::replace(path.begin(), path.end(), '-', '_');
      const string basename = ao->path().substr(ao->path().rfind("/")+1); // There should always be >= 1 slash

      // Convert YODA histos to heap-allocated ROOT objects and register
      /// @todo Convert Counter... as what?
      if (ao->type() == "Histo1D") {
        TH1* h = (TH1*) YODA::toTH1D(*dynamic_pointer_cast<YODA::Histo1D>(ao)).Clone(basename.c_str());
        CHECK(m_histSvc->regHist(m_stream + path, h));
        ATH_MSG_INFO("TH1D " + path + " created from YODA::Histo1D");
      } else if (ao->type() == "Histo2D") {
        TH2* h = (TH2*) YODA::toTH2D(*dynamic_pointer_cast<YODA::Histo2D>(ao)).Clone(basename.c_str());
        CHECK(m_histSvc->regHist(m_stream + path, h));
        ATH_MSG_INFO("TH2 " + path + " created from YODA::Histo2D");
      // } else if (ao->type() == "Profile2D") {
      //   TProfile2D* h = (TProfile2D*) YODA::toTProfile2D(*boost::dynamic_pointer_cast<YODA::Profile2D>(ao)).Clone(basename.c_str());
      //   CHECK(m_histSvc->regHist(m_stream + path, h));
      //   ATH_MSG_INFO("TProfile2D " + path + " created from YODA::Profile2D");
      } else if (ao->type() == "Profile1D") {
        TH1* h = (TH1*) YODA::toTProfile(*dynamic_pointer_cast<YODA::Profile1D>(ao)).Clone(basename.c_str());
        CHECK(m_histSvc->regHist(m_stream + path, h));
        ATH_MSG_INFO("TProfile " + path + " created from YODA::Profile1D");
      // } else if (ao->type() == "Scatter1D") {
      //   TGraph* g = (TGraph*) YODA::toTGraph(*boost::dynamic_pointer_cast<YODA::Scatter1D>(ao)).Clone(basename.c_str());
      //   CHECK(m_histSvc->regGraph(m_stream + path, g));
      //   ATH_MSG_INFO("TGraph " + path + " created from YODA::Scatter2D");
      } else if (ao->type() == "Scatter2D") {
        TGraph* g = (TGraph*) YODA::toTGraph(*dynamic_pointer_cast<YODA::Scatter2D>(ao)).Clone(basename.c_str());
        CHECK(m_histSvc->regGraph(m_stream + path, g));
        ATH_MSG_INFO("TGraph " + path + " created from YODA::Scatter2D");
      // } else if (ao->type() == "Scatter3D") {
      //   TGraph* g = (TGraph*) YODA::toTGraph(*boost::dynamic_pointer_cast<YODA::Scatter3D>(ao)).Clone(basename.c_str());
      //   CHECK(m_histSvc->regGraph(m_stream + path, g));
      //   ATH_MSG_INFO("TGraph " + path + " created from YODA::Scatter2D");
      } else {
        ATH_MSG_WARNING("Couldn't convert YODA histo " + path + " to ROOT: unsupported data type " + ao->type());
      }
    }
  }*/

  // Write out YODA file (add .yoda suffix if missing)
  if (m_file.find(".yoda") == string::npos) m_file += ".yoda";
  auto pos = m_file.find(".gz.");
  if (pos == string::npos) {
    m_analysisHandler->writeData(m_file);
  }
  else {//filename is e.g. something.yoda.gz.1
      //write to output file name without the ".1"
      m_analysisHandler->writeData(m_file.substr(0, pos)+".gz");
      // then rename it as the requested output filename
      if (std::rename((m_file.substr(0, pos)+".gz").c_str(), m_file.c_str()) !=0) {
	std::string error_msg = "Impossible to rename ";
	error_msg += m_file.substr(0, pos)+".gz";
 	error_msg += " as ";
	error_msg += m_file;
	ATH_MSG_ERROR(error_msg.c_str());
    	return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}


/// Helper function to sort GenParticles by descending energy
/// @todo Move into GeneratorUtils (with many friends)
bool cmpGenParticleByEDesc(const HepMC::GenParticle* a, const HepMC::GenParticle* b) {
  return a->momentum().e() > b->momentum().e();
}

inline std::vector<std::string> split(const std::string& input, const std::string& regex) {
  // passing -1 as the submatch index parameter performs splitting
  std::regex re(regex);
  std::sregex_token_iterator
  first{input.begin(), input.end(), re, -1},
    last;
    return {first, last};
}

const HepMC::GenEvent* Rivet_i::checkEvent(const HepMC::GenEvent& event) {
  std::vector<HepMC::GenParticle*> beams;
  HepMC::GenEvent* modEvent = new HepMC::GenEvent(event);

  // overwrite the HEPMC dummy event number with the proper ATLAS event number
  const DataHandle<EventInfo> eventInfo;
  if (!m_needsConversion && StatusCode::SUCCESS == evtStore()->retrieve(eventInfo)) {
    //int run=eventInfo->event_ID()->run_number();
    const unsigned long long eventNumber = eventInfo->event_ID()->event_number();
    modEvent->set_event_number(eventNumber);
  }

  // weight-name cleaning
  const HepMC::WeightContainer& old_wc = event.weights();
  vector<string> old_wnames = old_wc.weight_names();
  if (old_wnames.size()) {
    HepMC::WeightContainer& new_wc = modEvent->weights();
    new_wc.clear();
    vector<pair<string,string> > w_subs = {
      {" nominal ",""},
      {" set = ","_"},
      {" = ","_"},
      {"=",""},
      {",",""},
      {".",""},
      {":",""},
      {" ","_"},
      {"#","num"},
      {"\n","_"},
      {"/","over"}
    };
    std::regex re("(([^()]+))"); // Regex for stuff enclosed by parentheses ()

    // TEMP from Rivet dev branch
    vector<std::regex> select_patterns, deselect_patterns;
    if (m_matchWeights != "") {
      // Compile regex from each string in the comma-separated list
      for (const string& pattern : split(m_matchWeights, ",")) {
        select_patterns.push_back( std::regex(pattern) );
      }
    }
    if (m_unmatchWeights != "") {
      // Compile regex from each string in the comma-separated list
      for (const string& pattern : split(m_unmatchWeights, ",")) {
        deselect_patterns.push_back( std::regex(pattern) );
      }
    }
    // END OF TEMP
    map<string, double> new_name_to_value;
    map<string, string> old_name_to_new_name;
    for (const string& old_name : old_wnames) {
      string wname = string(old_name);
      double value = old_wc[old_name];
      for (const auto& sub : w_subs) {
        size_t start_pos = wname.find(sub.first);
        while (start_pos != std::string::npos) {
          wname.replace(start_pos, sub.first.length(), sub.second);
          start_pos = wname.find(sub.first);
        }
      }
      // Pulling some logic from the Rivet development branch
      // until we have a release with this patch:

      // Check if weight name matches a supplied string/regex and filter to select those only
      bool match = select_patterns.empty();
      for (const std::regex& re : select_patterns) {
        if ( std::regex_match(wname, re) ) {
          match = true;
          break;
        }
      }
      // Check if the remaining weight names match supplied string/regexes and *de*select accordingly
      bool unmatch = false;
      for (const std::regex& re : deselect_patterns) {
        if ( std::regex_match(wname, re) ) { unmatch = true; break; }
      }
      if (!match || unmatch) continue;

      // end of borrowing logic from the Rivet development branch
      new_name_to_value[wname] = value;
      old_name_to_new_name[old_name] = wname;
    }
    auto itEnd = old_name_to_new_name.end();
    for (const string& old_name : old_wnames) {
      if (old_name_to_new_name.find(old_name) == itEnd)  continue;
      const string& new_name = old_name_to_new_name[old_name];
      new_wc[ new_name ] = new_name_to_value[new_name];
    }
    // end of weight-name cleaning
  }

  #ifdef HEPMC_HAS_UNITS
  modEvent->use_units(HepMC::Units::GEV, HepMC::Units::MM);
  #endif

  // work-around for SPGs
  if (modEvent->particles_size() == 1) {
    HepMC::GenEvent::particle_const_iterator p = modEvent->particles_begin();
    modEvent->set_beam_particles(*p, *p);
    return modEvent;
  }
  if (!modEvent->valid_beam_particles()) {
    double etotal = 0;
    //need to find CME of this collision...only needs to be done for AOD files as some events don't appear to have the initial beam particles
    for (HepMC::GenEvent::particle_const_iterator p = modEvent->particles_begin(); p != modEvent->particles_end(); ++p) {
      if (!(*p)->production_vertex() && (*p)->pdg_id() != 0) {
        beams.push_back(*p);
      }
      if( (*p)->status() == 1){
        etotal += (*p)->momentum().e();
      }
    }
    if (beams.size() > 2) std::sort(beams.begin(), beams.end(), cmpGenParticleByEDesc);
    beams.resize(2);
    if (m_patchBeams){
      const double ebeam = 0.5*etotal;
      bool passBeam = (beams[0] && std::abs(beams[0]->momentum().e() - ebeam) < 1e-5);
      passBeam &= (beams[1] && std::abs(beams[1]->momentum().e() - ebeam) < 1e-5);
      if(!passBeam){
        HepMC::GenParticle* b1 = new HepMC::GenParticle(HepMC::FourVector(0,0,ebeam,ebeam),2212,4);
        HepMC::GenParticle* b2 = new HepMC::GenParticle(HepMC::FourVector(0,0,-1*ebeam,ebeam),2212,4);
        HepMC::GenVertex* dummy_v = new HepMC::GenVertex();
        dummy_v->add_particle_in(b1);
        dummy_v->add_particle_in(b2);
        beams[0]=b1;
        beams[1]=b2;
        modEvent->add_vertex(dummy_v);
      }
    }
  } 
  else {
    beams.resize(2);
    beams[0] = modEvent->beam_particles().first;
    beams[1] = modEvent->beam_particles().second;
  }

  double scalefactor = 1.0;

  if (beams[0]->momentum().e() > 50000.0) scalefactor = 0.001;
  if (scalefactor == 1.0 && modEvent->valid_beam_particles()) {
    return modEvent;
  } 
  else {
    if (scalefactor != 1.0) {
      for (HepMC::GenEvent::particle_iterator p = modEvent->particles_begin(); p != modEvent->particles_end(); ++p) {
        const HepMC::FourVector pGeV((*p)->momentum().px() * scalefactor,
                                     (*p)->momentum().py() * scalefactor,
                                     (*p)->momentum().pz() * scalefactor,
                                     (*p)->momentum().e() * scalefactor);
        (*p)->set_momentum(pGeV);
        (*p)->set_generated_mass( (*p)->generated_mass() * scalefactor );
      }
    }
    for (HepMC::GenEvent::particle_const_iterator p = modEvent->particles_begin();
         p != modEvent->particles_end(); ++p) {
      // map beam particle pointers to new event
      if (beams[0]->barcode() == (*p)->barcode()) beams[0]=*p;
      if (beams[1]->barcode() == (*p)->barcode()) beams[1]=*p;
    }
    modEvent->set_beam_particles(beams[0], beams[1]);
    return modEvent;
  }
}
