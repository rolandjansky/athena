/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for Athena-Rivet interface

#include "Rivet_i.h"
#include "LogLevels.h"

#include "AtlasHepMC/GenEvent.h"

#include "GeneratorObjects/McEventCollection.h"
#include "GenInterfaces/IHepMCWeightSvc.h"
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



#include <cstdlib>
#include <memory>
#include <regex>

using namespace std;


/// Convert to inherit from GenBase, now we're developing Rivet 2 interfacing for R19+ only

Rivet_i::Rivet_i(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_hepMCWeightSvc("HepMCWeightSvc", name),
  m_analysisHandler(0),
  m_init(false)
{
  // Options
  declareProperty("McEventKey", m_genEventKey="GEN_EVENT");
  declareProperty("Analyses", m_analysisNames);
  declareProperty("CrossSection", m_crossSection=-1.0);
  declareProperty("CrossSectionUncertainty", m_crossSection_uncert=-1.0);
  declareProperty("Stream", m_stream="/Rivet");
  declareProperty("RunName", m_runname="");
  declareProperty("HistoFile", m_file="Rivet.yoda");
  declareProperty("HistoPreload", m_preload="");
  declareProperty("AnalysisPath", m_anapath="");
  declareProperty("IgnoreBeamCheck", m_ignorebeams=false);
  declareProperty("DoRootHistos", m_doRootHistos=false);
  declareProperty("SkipWeights", m_skipweights=false);
  //declareProperty("MatchWeights", m_matchWeights="");
  //declareProperty("UnmatchWeights", m_unmatchWeights="");
  declareProperty("WeightCap", m_weightcap=-1.0);
}

string getenv_str(const string& key) {
  char const* val = getenv(key.c_str());
  return val == NULL ? string() : string(val);
}


StatusCode Rivet_i::initialize() {
  ATH_MSG_DEBUG("Rivet_i initializing");
  ATH_MSG_INFO("Using Rivet version " << Rivet::version());


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
  } else {
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
  m_analysisHandler->skipMultiWeights(m_skipweights); //< Whether to skip weights or not
  //m_analysisHandler->selectMultiWeights(m_matchWeights); //< Only run on a subset of the multi-weights
  //m_analysisHandler->deselectMultiWeights(m_unmatchWeights); //< Veto a subset of the multi-weights
  if (m_weightcap>0) m_analysisHandler->setWeightCap(m_weightcap);

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

  // Get the event collection
  /// @todo Replace with new GenBase functionality
  const McEventCollection* eventCollection;
  StatusCode sc = evtStore()->retrieve(eventCollection, m_genEventKey);
  if (sc.isFailure() || eventCollection == 0) {
    ATH_MSG_ERROR("Unable to retrieve event collection from StoreGate with key " << m_genEventKey);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved event collection from StoreGate with key " << m_genEventKey);
  }

  // Get the first event in the event collection
  /// @todo Actually use back(), for the most recent event, or throw an exception if more than one event found?
  /// @todo Replace with new GenBase const_event() functionality
  const HepMC::GenEvent* event = eventCollection->front();
  if (event == NULL) {
    ATH_MSG_ERROR("Rivet_i received a null HepMC event");
    return StatusCode::FAILURE;
  }

  // Modify the event units etc. if necessary
  const HepMC::GenEvent* checkedEvent = checkEvent(event);
  // ATH_MSG_ALWAYS("CHK1 BEAM ENERGY = " << checkedEvent->beam_particles().first->momentum().e());
  // ATH_MSG_ALWAYS("CHK1 UNITS == MEV = " << std::boolalpha << (checkedEvent->momentum_unit() == HepMC::Units::MEV));

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

  // Set xsec in Rivet
  // Set xsec in Rivet
  double custom_xs = m_crossSection > 0 ? m_crossSection : 1.0;
  double custom_xserr = m_crossSection_uncert > 0 ? m_crossSection_uncert : 0.0;
  m_analysisHandler->setCrossSection({custom_xs, custom_xserr});

  // Call Rivet finalize
  m_analysisHandler->finalize();



  // Write out YODA file (add .yoda suffix if missing)
  if (m_file.find(".yoda") == string::npos) m_file += ".yoda";
  m_analysisHandler->writeData(m_file);

  return StatusCode::SUCCESS;
}


/// Helper function to sort GenParticles by descending energy
/// @todo Move into GeneratorUtils (with many friends)
bool cmpGenParticleByEDesc(HepMC::ConstGenParticlePtr a, HepMC::ConstGenParticlePtr b) {
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

const HepMC::GenEvent* Rivet_i::checkEvent(const HepMC::GenEvent* event) {
  std::vector<HepMC::GenParticlePtr> beams;
  HepMC::GenEvent* modEvent = new HepMC::GenEvent(*event);

  // overwrite the HEPMC dummy event number with the proper ATLAS event number
  const DataHandle<EventInfo> eventInfo;
  if (StatusCode::SUCCESS == evtStore()->retrieve(eventInfo)) {
    //int run=eventInfo->event_ID()->run_number();
    int eventNumber = eventInfo->event_ID()->event_number();
    modEvent->set_event_number(eventNumber);
  }

  // weight-name cleaning
#ifdef HEPMC3
  std::vector<std::string>  w_names = event->weight_names();
  std::vector<std::pair<std::string,std::string> > w_subs = {
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
  for (std::string& wname : w_names) {
    for (const auto& sub : w_subs) {
      size_t start_pos = wname.find(sub.first);
      while (start_pos != std::string::npos) {
        wname.replace(start_pos, sub.first.length(), sub.second);
        start_pos = wname.find(sub.first);
      }
    }
  }
  modEvent->run_info()->set_weight_names(w_names);
#else
  const HepMC::WeightContainer& old_wc = event->weights();
  std::ostringstream stream;
  old_wc.print(stream);
  string str =  stream.str();
  // if it only has one element, 
  // then it doesn't use named weights
  // --> no need for weight-name cleaning
  if (str.size() > 1) {
    vector<string> orig_order(m_hepMCWeightSvc->weightNames().size());
    for (const auto& item : m_hepMCWeightSvc->weightNames()) {
      orig_order[item.second] = item.first;
    }
    map<string, double> new_name_to_value;
    map<string, string> old_name_to_new_name;
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
    for (std::sregex_iterator i = std::sregex_iterator(str.begin(), str.end(), re);
         i != std::sregex_iterator(); ++i ) {
      std::smatch m = *i;
      vector<string> temp = ::split(m.str(), "[,]");
      if (temp.size() == 2 || temp.size() == 3) {
        string wname = temp[0];
        if (temp.size() == 3)  wname += "," + temp[1];
        string old_name = string(wname);
        double value = old_wc[wname];
        for (const auto& sub : w_subs) {
          size_t start_pos = wname.find(sub.first);
          while (start_pos != std::string::npos) {
            wname.replace(start_pos, sub.first.length(), sub.second);
            start_pos = wname.find(sub.first);
          }
        }
        new_name_to_value[wname] = value;
        old_name_to_new_name[old_name] = wname;
      }
    }
    for (const string& old_name : orig_order) {
      const string& new_name = old_name_to_new_name[old_name];
      new_wc[ new_name ] = new_name_to_value[new_name];
    }
    // end of weight-name cleaning
  }
#endif
#ifdef HEPMC3
  if (
  false//!modEvent->valid_beam_particles()//FIXME!
  ) {
    for (auto p: modEvent->particles()) {
      if (!p->production_vertex() && p->pdg_id() != 0) {
        beams.push_back(p);
      }
    }
    if (beams.size() > 2) std::sort(beams.begin(), beams.end(), cmpGenParticleByEDesc);
    beams.resize(2);
  } else {
    beams.resize(2);
    beams[0] = modEvent->beams()[0];
    beams[1] = modEvent->beams()[1];
  }
  double scalefactor = 1.0;
  //ATH_MSG_ALWAYS("BEAM ENERGY = " << beams[0]->momentum().e());
  //ATH_MSG_ALWAYS("UNITS == MEV = " << std::boolalpha << (modEvent->momentum_unit() == HepMC::Units::MEV));
  modEvent->set_units(HepMC3::Units::GEV, HepMC3::Units::MM);
  if (beams[0]->momentum().e() > 50000.0) scalefactor = 0.001;

  if (scalefactor == 1.0 && 
  true//modEvent->valid_beam_particles()//FIXME
  ) {
    return modEvent;
  } else {
    if (scalefactor != 1.0) {
      // ATH_MSG_ALWAYS("RESCALING * " << scalefactor);
      for (auto  p: modEvent->particles()) {
        const HepMC::FourVector pGeV(p->momentum().px() * scalefactor,
                                     p->momentum().py() * scalefactor,
                                     p->momentum().pz() * scalefactor,
                                     p->momentum().e() * scalefactor);
        p->set_momentum(pGeV);
        p->set_generated_mass( p->generated_mass() * scalefactor );
      }
    }
    for (auto p: modEvent->particles()) {
      // map beam particle pointers to new event
      if (HepMC::barcode(beams[0]) == HepMC::barcode(p)) beams[0]=p;
      if (HepMC::barcode(beams[1]) == HepMC::barcode(p)) beams[1]=p;
    }
    modEvent->set_beam_particles(beams[0], beams[1]);
    return modEvent;
  }
#else
  if (!modEvent->valid_beam_particles()) {
    for (HepMC::GenEvent::particle_const_iterator p = modEvent->particles_begin(); p != modEvent->particles_end(); ++p) {
      if (!(*p)->production_vertex() && (*p)->pdg_id() != 0) {
        beams.push_back(*p);
      }
    }
    if (beams.size() > 2) std::sort(beams.begin(), beams.end(), cmpGenParticleByEDesc);
    beams.resize(2);
  } else {
    beams.resize(2);
    beams[0] = modEvent->beam_particles().first;
    beams[1] = modEvent->beam_particles().second;
  }

  double scalefactor = 1.0;
  //ATH_MSG_ALWAYS("BEAM ENERGY = " << beams[0]->momentum().e());
  //ATH_MSG_ALWAYS("UNITS == MEV = " << std::boolalpha << (modEvent->momentum_unit() == HepMC::Units::MEV));
  modEvent->use_units(HepMC::Units::GEV, HepMC::Units::MM);
  if (beams[0]->momentum().e() > 50000.0) scalefactor = 0.001;

  if (scalefactor == 1.0 && modEvent->valid_beam_particles()) {
    return modEvent;
  } else {
    if (scalefactor != 1.0) {
      // ATH_MSG_ALWAYS("RESCALING * " << scalefactor);
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
#endif
}
