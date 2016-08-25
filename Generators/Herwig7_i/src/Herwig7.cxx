// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*! \file Herwig7.cxx
 *  \brief Implementation of the Herwig 7 Athena interface.
 *  \author Daniel Rauch (daniel.rauch@desy.de)
 */

#include "Herwig7_i/Herwig7.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "ThePEG/Repository/EventGenerator.h"
#include "ThePEG/Repository/Repository.h"
#include "ThePEG/Persistency/PersistentIStream.h"
#include "ThePEG/Utilities/DynamicLoader.h"
#include "ThePEG/Utilities/Debug.h"
#include "ThePEG/EventRecord/Event.h"
#include "ThePEG/EventRecord/SubProcess.h"
#include "ThePEG/Handlers/XComb.h"
#include "ThePEG/Handlers/EventHandler.h"
#include "ThePEG/PDF/PartonExtractor.h"
#include "ThePEG/PDF/PDF.h"

#include "PathResolver/PathResolver.h"

#include <boost/algorithm/string.hpp>
#include "boost/foreach.hpp"
#define foreach BOOST_FOREACH


// Setup HepMC traits definition for ThePEG's converter to work
#include "ThePEG/Vectors/HepMCConverter.h"
namespace ThePEG {
  template<>
  struct HepMCTraits<HepMC::GenEvent>
    : public HepMCTraitsBase<HepMC::GenEvent,
                             HepMC::GenParticle,
                             HepMC::GenVertex,
                             HepMC::Polarization,
                             HepMC::PdfInfo>
  {
    static bool hasUnits() {
      #ifdef HEPMC_HAS_UNITS
      return true;
      #else
      return false;
      #endif
    }
  };
}


using namespace std;


// Constructor
Herwig7::Herwig7(const string& name, ISvcLocator* pSvcLocator) :
  GenModule(name, pSvcLocator),
  m_pdfname_me("UNKNOWN"),
  m_pdfname_mpi("UNKNOWN")
{
  declareProperty("Commands", m_herwigCommandVector);
  declareProperty("RunName", m_runname="Atlas");
  declareProperty("InFileDump", m_infiledump="herwigInfileDump.in");
  declareProperty("CrossSectionScaleFactor", m_xsscale=1.0);
}



/*!
 *  \todo Higher-level API to be provided by the Herwig authors to allow for
 *        slimmer interface and for usage of more advanced features such as
 *        the setupfile mechanism for modifying existing runfiles.
 */
StatusCode Herwig7::genInitialize() {
  ATH_MSG_INFO("Herwig7 initialising...");

  // Get random number seeds from Atlas RNG service, and pass them as Hw7 RNG
  // default seeds (they can be overridden with an explicit Hw7 set command in the JO)
  CLHEP::HepRandomEngine* engine = atRndmGenSvc().GetEngine("Herwig7");
  const long* seeds = engine->getSeeds();
  // The RNG service supplies two seeds, but Hw7 only uses one. To avoid the situation
  // where varying one seed has no effect (this already stung us in pre-production
  // job transform tests), we multiply the two seeds and let them wrap around the long
  // type MAX_SIZE:
  int32_t combined_seed = abs(seeds[0] * seeds[1]);
  // Represent the combined seed as a string, so the config system can parse it back to a long ;)
  ostringstream ss_seed;
  ss_seed << combined_seed;
  string atlseed = ss_seed.str();

  // Change repo log level and make sure that config errors result in a program exit
  //ThePEG::Debug::level = 10000;
  ThePEG::Repository::exitOnError() = 1;

  // Horrid runtime ATLAS env variable and CMT path mangling to work out ThePEG module search paths
  char* env1 = getenv("CMTPATH");
  char* env2 = getenv("CMTCONFIG");
  string reposearchpaths;
  if (env1 == 0 || env2 == 0) {
     // Use everything from $DATAPATH and $LD_LIBRARY_PATH:
     const char* datapath = getenv( "DATAPATH" );
     reposearchpaths = datapath;
     std::vector< std::string > datapaths;
     boost::split( datapaths, datapath,
                   boost::is_any_of( std::string( ":" ) ) );
     for( const std::string& p : datapaths ) {
        ThePEG::Repository::appendReadDir( p );
     }
     const char* ldpath = getenv( "LD_LIBRARY_PATH" );
     std::vector< std::string > ldpaths;
     boost::split( ldpaths, ldpath, boost::is_any_of( std::string( ":" ) ) );
     for( const std::string& p : ldpaths ) {
        ThePEG::DynamicLoader::appendPath( p );
     }
  } else {
    vector<string> cmtpaths;
    boost::split(cmtpaths, env1, boost::is_any_of(string(":")));
    const string cmtconfig = env2;
    const string sharepath = "/InstallArea/" + cmtconfig + "/share";
    const string libpath = "/InstallArea/" + cmtconfig + "/lib";
    // Prepend to the repository and loader command file search paths
    foreach (const string& p, cmtpaths) {
      const string cmtsharepath = p + sharepath;
      ATH_MSG_DEBUG("Appending " + cmtsharepath + " to ThePEG repository and command file search paths");
      reposearchpaths = reposearchpaths + (reposearchpaths.length() == 0 ? "" : ":") + cmtsharepath;
      ThePEG::Repository::appendReadDir(cmtsharepath);
      const string cmtlibpath = p + libpath;
      ATH_MSG_DEBUG("Appending " + cmtlibpath + " to ThePEG loader search path");
      ThePEG::DynamicLoader::appendPath(cmtlibpath);
    }
  }
  ATH_MSG_DEBUG("Num of library search paths = " << ThePEG::DynamicLoader::allPaths().size());

  // Use PathResolver to find default Hw7 ThePEG repository file.
  const string repopath = PathResolver::find_file_from_list("HerwigDefaults.rpo", reposearchpaths);
  ATH_MSG_DEBUG("Loading Herwig default repo from " << repopath);
  ThePEG::Repository::load(repopath);
  ATH_MSG_DEBUG("Loaded Herwig default repository");


  // Prepend the defaults to the command vector, so that *everything* gets dumped
  ATH_MSG_DEBUG("Defining default params");
  CommandVector defaultcmds;
  defaultcmds.push_back("cd /Herwig/Generators");
  defaultcmds.push_back("set /Herwig/Generators/LHCGenerator:RandomNumberGenerator:Seed " + atlseed);
  defaultcmds.push_back("set /Herwig/Generators/LHCGenerator:DebugLevel 1");
  defaultcmds.push_back("set /Herwig/Generators/LHCGenerator:PrintEvent 0");
  defaultcmds.push_back("set /Herwig/Generators/LHCGenerator:MaxErrors 1000000");
  defaultcmds.push_back("set /Herwig/Generators/LHCGenerator:NumberOfEvents 1000000000");
  defaultcmds.push_back("set /Herwig/Generators/LHCGenerator:UseStdout Yes");

  // Append config directives from job options to the defaults
  CommandVector cmds = defaultcmds;
  cmds.insert(cmds.begin()+cmds.size(), m_herwigCommandVector.begin(), m_herwigCommandVector.end());

  // Apply the config commands
  ATH_MSG_DEBUG("Processing default and job option commands");
  string commands = "";
  foreach (const string& cmd, cmds) {
    commands += "\n" + cmd;
    const size_t iNonSpace = cmd.find_first_not_of(" ");
    // Only run the command if it's not just whitespace or a comment
    if (iNonSpace != string::npos && cmd.data()[iNonSpace] != '#') {
      ATH_MSG_DEBUG("Herwig7 command: " << cmd);
      const string reply = ThePEG::Repository::exec(cmd, std::cout);
      if (!reply.empty()) {
        if (reply.find("Error") != string::npos) {
          ATH_MSG_ERROR("Herwig7 error: " + reply);
          return StatusCode::FAILURE;
        } else {
          ATH_MSG_INFO("Herwig7 info: " + reply);
        }
      }
    }
  }
  ATH_MSG_DEBUG("Updating repository");
  ThePEG::Repository::update();

  // Dump out the config commands, with an extra saverun to make life easier
  ostringstream ss_cmds;
  ss_cmds << commands << "\n\n"
          << "# Extra saverun for standalone convenience: Athena doesn't execute this\n"
          << "saverun " << m_runname << " /Herwig/Generators/LHCGenerator\n";
  const string dumpcommands = ss_cmds.str();
  ATH_MSG_DEBUG("All job option commands:\n" << dumpcommands);
  if (m_infiledump.length() > 0) {
    ofstream f(m_infiledump.c_str());
    f << dumpcommands;
    f.close();
  }

  // PDF settings checks
  //cout << ThePEG::Repository::exec("get /Herwig/Particles/p+:PDF", std::cout) << endl;
  //cout << ThePEG::Repository::exec("get /Herwig/Particles/p+:PDF:PDFName", std::cout) << endl;

  // Make a "run" object from the config repository.
  ATH_MSG_DEBUG("Getting generator from ThePEG.");
  ThePEG::EGPtr tmpEG = ThePEG::BaseRepository::GetObject<ThePEG::EGPtr>("/Herwig/Generators/LHCGenerator");
  try {
    ATH_MSG_DEBUG("Reducing repository to single LHC generator run");
    m_hw = ThePEG::Repository::makeRun(tmpEG, m_runname);
  } catch (ThePEG::Exception& e) {
    ATH_MSG_ERROR("Exception in ThePEG: " << e.what());
    throw;
  } catch (std::exception& e) {
    ATH_MSG_ERROR("STL exception: " << e.what());
    throw;
  }

  // Write out some run parameters
  const string repo_energy = ThePEG::Repository::exec("get /Herwig/Generators/LHCGenerator:EventHandler:LuminosityFunction:Energy", std::cout);
  ATH_MSG_INFO("Run energy: " + repo_energy + " GeV");
  const string repo_seed = ThePEG::Repository::exec("get /Herwig/Generators/LHCGenerator:RandomNumberGenerator:Seed", std::cout);
  ATH_MSG_INFO("Random seed: " + repo_seed);
  // MPI parameters
  const string repo_intrkt = ThePEG::Repository::exec("get /Herwig/Shower/Evolver:IntrinsicPtGaussian", std::cout);
  ATH_MSG_INFO("Intrinsic kT: " + repo_intrkt + " GeV/c");
  const string repo_ptmatch = ThePEG::Repository::exec("get /Herwig/UnderlyingEvent/MPIHandler:pTmin0", std::cout);
  ATH_MSG_INFO("MPI matching pT0: " + repo_ptmatch + " GeV/c");
  const string repo_invrad = ThePEG::Repository::exec("get /Herwig/UnderlyingEvent/MPIHandler:InvRadius", std::cout);
  ATH_MSG_INFO("Inverse hadron radius: " + repo_invrad + " GeV**2");
  // Kinematic efficiency cuts
  const string repo_jetminkt = ThePEG::Repository::exec("get /Herwig/Cuts/JetKtCut:MinKT", std::cout);
  ATH_MSG_INFO("Minimum jet kT: " + repo_jetminkt + " GeV/c");
  const string repo_lepminkt = ThePEG::Repository::exec("get /Herwig/Cuts/LeptonKtCut:MinKT", std::cout);
  ATH_MSG_INFO("Minimum lepton kT: " + repo_lepminkt + " GeV/c");
  const string repo_photonminkt = ThePEG::Repository::exec("get /Herwig/Cuts/PhotonKtCut:MinKT", std::cout);
  ATH_MSG_INFO("Minimum photon kT: " + repo_photonminkt + " GeV/c");
  // PDFs
  const string me_pdf = ThePEG::Repository::exec("get /Herwig/Particles/p+:PDF:PDFName", std::cout);
  m_pdfname_me = (me_pdf.find("***") == string::npos) ? me_pdf : "UNKNOWN";
  ATH_MSG_INFO("Matrix element PDF: " + m_pdfname_me);
  const string mpi_pdf = ThePEG::Repository::exec("get /Herwig/Shower/ShowerHandler:PDFA:PDFName", std::cout);
  m_pdfname_mpi = (mpi_pdf.find("***") == string::npos && mpi_pdf.find("Error:") == string::npos) ? mpi_pdf : m_pdfname_me;
  ATH_MSG_INFO("MPI PDF: " + m_pdfname_mpi);

  // Initialise the run
  ATH_MSG_DEBUG("Calling Herwig7 initialize()");
  m_hw->initialize();

  return StatusCode::SUCCESS;
}



/*!
 *  Run the generator for one event and store the event internally.
 */
StatusCode Herwig7::callGenerator() {
  ATH_MSG_DEBUG("Herwig7 generating");
  assert(m_hw);
  m_event = m_hw->shoot();
  return StatusCode::SUCCESS;
}



/*!
 *  Fill HepMC event from Herwig's internally stored EventPtr.
 *
 *  \todo Conversion to HepMC format will possibly be provided by the authors
 *        as part of a higher-level API.
 */
StatusCode Herwig7::fillEvt(HepMC::GenEvent* evt) {
  // Convert the Herwig event into the HepMC GenEvent
  ATH_MSG_DEBUG("Converting ThePEG::Event to HepMC::GenEvent");
  ThePEG::HepMCConverter<HepMC::GenEvent>::convert(*m_event, *evt, false, ThePEG::MeV, ThePEG::millimeter);
  ATH_MSG_DEBUG("Converted ThePEG::Event to HepMC::GenEvent");

  // Fill the event number into HepMC event record
  const EventInfo* evtinfo;
  const StatusCode sc = evtStore()->retrieve(evtinfo);
  if (sc.isSuccess()) {
    evt->set_event_number(evtinfo->event_ID()->event_number());
  }

  // Fill event with random seeds from Atlas RNG service
  const long* s = atRndmGenSvc().GetEngine("Herwig7")->getSeeds();
  vector<long> seeds(s, s+2);
  ATH_MSG_DEBUG("Random seeds: " << seeds[0] << ", " << seeds[1]);
  evt->set_random_states(seeds);

  // Add a unit entry to the event weight vector if it's currently empty
  if (evt->weights().empty()) {
    evt->weights().push_back(m_event->weight());
  }

  // Add PDF info manually (for now, until natively supported in the ThePEG converter)
  ATH_MSG_DEBUG("Adding PDF info to HepMC");
  // IDs of the partons going into the primary sub process
  ThePEG::tSubProPtr sub = m_event->primarySubProcess();
  int id1 = sub->incoming().first ->id();
  int id2 = sub->incoming().second->id();
  // Get the event handler
  ThePEG::tcEHPtr eh = ThePEG::dynamic_ptr_cast<ThePEG::tcEHPtr>(m_event->handler());
  // Get the values of x
  double x1 = eh->lastX1();
  double x2 = eh->lastX2();
  // Get the pdfs
  pair<ThePEG::PDF,ThePEG::PDF> pdfs;
  pdfs.first  = eh->pdf<ThePEG::PDF>(sub->incoming().first);
  pdfs.second = eh->pdf<ThePEG::PDF>(sub->incoming().second);
  // Get the scale
  ThePEG::Energy2 scale = eh->lastScale();
  double Q = sqrt(scale/ThePEG::GeV2);
  // Get the values of the pdfs
  double pdf1 = pdfs.first.xfx(sub->incoming().first ->dataPtr(), scale, x1);
  double pdf2 = pdfs.first.xfx(sub->incoming().second->dataPtr(), scale, x2);
  // Create the PDFinfo object
  HepMC::PdfInfo pdfi(id1, id2, x1, x2, Q, pdf1, pdf2);
  evt->set_pdf_info(pdfi);
  ATH_MSG_DEBUG("Added PDF info to HepMC");

  return StatusCode::SUCCESS;
}



/*!
 *  Tidy up, print out run stats, etc.
 */
StatusCode Herwig7::genFinalize() {
  ATH_MSG_INFO("Herwig7 finalizing.");
  assert(m_hw);
  cout << "MetaData: generator = Herwig7 " << HWVERSION << endl;
  cout << std::scientific << std::setprecision(5) << "MetaData: cross-section (nb) = " << m_hw->eventHandler()->integratedXSec()*m_xsscale/ThePEG::nanobarn << endl;
  cout << "MetaData: PDF = " << m_pdfname_me << " (ME); " << m_pdfname_mpi << " (shower/MPI)" << endl;
  m_hw->finalize();
  ThePEG::Repository::cleanup();
  return StatusCode::SUCCESS;
}

