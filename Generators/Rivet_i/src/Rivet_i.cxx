/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for Athena-Rivet interface

#include "Rivet_i/Rivet_i.h"
#include "Rivet_i/LogLevels.h"

#include "HepMC/GenEvent.h"

#include "GeneratorObjects/McEventCollection.h"
#include "AthenaKernel/errorcheck.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ITHistSvc.h"

#include "Rivet/Rivet.hh"
#include "Rivet/Analysis.hh"
#include "Rivet/Tools/RivetYODA.hh"

#include "TH1D.h"
#include "TGraphAsymmErrors.h"

#include "YODA/ROOTCnv.h"

/// @todo Eliminate Boost?
#include <boost/algorithm/string.hpp>
#include "boost/foreach.hpp"
#ifndef foreach
#define foreach BOOST_FOREACH
#endif

#include <cstdlib>
#include <memory>
using namespace std;


/// Convert to inherit from GenBase, now we're developing Rivet 2 interfacing for R19+ only

Rivet_i::Rivet_i(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_histSvc("THistSvc", name),
  m_analysisHandler(0),
  m_init(false)
{
  // Options
  declareProperty("McEventKey", m_genEventKey="GEN_EVENT");
  declareProperty("Analyses", m_analysisNames);
  declareProperty("CrossSection", m_crossSection=-1.0);
  declareProperty("WeightName", m_weightName="");
  declareProperty("Stream", m_stream="/Rivet");
  declareProperty("RunName", m_runname="");
  declareProperty("HistoFile", m_file="Rivet.yoda");
  declareProperty("AnalysisPath", m_anapath="");
  declareProperty("IgnoreBeamCheck", m_ignorebeams=false);
  declareProperty("SkipFinalize", m_skipfinalize=false);
  declareProperty("DoRootHistos", m_doRootHistos=true);
  declareProperty("RootAsTGraph", m_doRootAsTGraph=false);

  // Service handles
  declareProperty("THistSvc", m_histSvc);
}

string getenv_str(const string& key) {
  char const* val = getenv(key.c_str());
  return val == NULL ? string() : string(val);
}


StatusCode Rivet_i::initialize() {
  ATH_MSG_DEBUG("Rivet_i initializing");
  ATH_MSG_INFO("Using Rivet version " << RIVET_VERSION);

  // Get histo service
  if (m_doRootHistos && m_histSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve service " << m_histSvc);
    return StatusCode::FAILURE;
  }

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
    boost::split(cmtpaths, cmtpath, boost::is_any_of(string(":")));
    const string cmtconfig = getenv_str("CMTCONFIG");
    if (cmtconfig.empty()) {
      ATH_MSG_WARNING("$CMTCONFIG variable not set: finding the main analysis plugin directory will be difficult...");
    } else {
      const string libpath = "/InstallArea/" + cmtconfig + "/lib";
      foreach (const string& p, cmtpaths) {
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
  foreach (const string& ap, anapaths) {
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

  // Set Rivet native log level to match Athena
  Rivet::Log::setLevel("Rivet", rivetLevel(msg().level()));

  // Get all available analysis names
  if (msgLvl(MSG::VERBOSE)) {
    vector<string> analysisNames = Rivet::AnalysisLoader::analysisNames();
    ATH_MSG_VERBOSE("List of available Rivet analyses:");
    foreach (const string& a, analysisNames) ATH_MSG_VERBOSE(" " + a);
  }

  // Add analyses
  foreach (const string& a, m_analysisNames) {
    ATH_MSG_INFO("Loading Rivet analysis " << a);
    m_analysisHandler->addAnalysis(a);
    Rivet::Log::setLevel("Rivet.Analysis."+a, rivetLevel(msg().level()));
    // Rivet::Analysis* analysis = Rivet::AnalysisLoader::getAnalysis(a);
    // if (analysis->needsCrossSection()) {
    //   m_needsCrossSection = true;
    //   if (m_crossSection < 0.0) {
    //     ATH_MSG_FATAL("Analysis " << a << " requires the cross section to be set in the job options");
    //     return StatusCode::FAILURE;
    //   }
    // }
  }

  // Initialise Rivet
  // m_analysisHandler->init();

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
  if (m_crossSection > 0) m_analysisHandler->setCrossSection(m_crossSection);

  // Call Rivet finalize
  if (!m_skipfinalize) m_analysisHandler->finalize();

  // Convert YODA-->ROOT
  if (m_doRootHistos) {
    foreach (const Rivet::AnalysisObjectPtr ao, m_analysisHandler->getData()) {
      // Normalize path name to be usable by ROOT
      const string path = boost::replace_all_copy(ao->path(), "-", "_");
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
  }

  // Write out YODA file (add .yoda suffix if missing)
  if (m_file.find(".yoda") == string::npos) m_file += ".yoda";
  m_analysisHandler->writeData(m_file);

  return StatusCode::SUCCESS;
}


/// Helper function to sort GenParticles by descending energy
/// @todo Move into GeneratorUtils (with many friends)
bool cmpGenParticleByEDesc(const HepMC::GenParticle* a, const HepMC::GenParticle* b) {
  return a->momentum().e() > b->momentum().e();
}


const HepMC::GenEvent* Rivet_i::checkEvent(const HepMC::GenEvent* event) {
  std::vector<HepMC::GenParticle*> beams;
  HepMC::GenEvent* modEvent = new HepMC::GenEvent(*event);

  if(m_weightName != ""){
    if(event->weights().has_key(m_weightName)){
      double weight = event->weights()[m_weightName];
      modEvent->weights().clear();
      modEvent->weights()[m_weightName] = weight;
    }else{
      ATH_MSG_ERROR("Weight named " + m_weightName + " could not be found in the HepMC event!");
      delete modEvent;
      return (HepMC::GenEvent*)0;
    }
  }

  if (!event->valid_beam_particles()) {
    for (HepMC::GenEvent::particle_const_iterator p = event->particles_begin(); p != event->particles_end(); ++p) {
      if (!(*p)->production_vertex() && (*p)->pdg_id() != 0) {
        beams.push_back(*p);
      }
    }
    if (beams.size() > 2) std::sort(beams.begin(), beams.end(), cmpGenParticleByEDesc);
    beams.resize(2);
  } else {
    beams.resize(2);
    beams[0] = event->beam_particles().first;
    beams[1] = event->beam_particles().second;
  }

  double scalefactor = 1.0;
  // ATH_MSG_ALWAYS("BEAM ENERGY = " << beams[0]->momentum().e());
  // ATH_MSG_ALWAYS("UNITS == MEV = " << std::boolalpha << (event->momentum_unit() == HepMC::Units::MEV));
  #ifdef HEPMC_HAS_UNITS
  if (event->momentum_unit() == HepMC::Units::MEV) {
    if (beams[0]->momentum().e() < 50000.0) scalefactor = 1000.0;
  } else if (event->momentum_unit() == HepMC::Units::GEV) {
    if (beams[0]->momentum().e() > 50000.0) scalefactor = 0.001;
  }
  #else
  if (beams[0]->momentum().e() > 50000.0) scalefactor = 0.001;
  #endif

  if (scalefactor == 1.0 && event->valid_beam_particles()) {
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
}
