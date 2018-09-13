// -*- C++ -*-

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

#include "Herwig/API/HerwigAPI.h"

#include "PathResolver/PathResolver.h"

#include "boost/thread/thread.hpp"
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"
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
  m_use_seed_from_generatetf(false),
  m_seed_from_generatetf(0),
  m_pdfname_me("UNKNOWN"), m_pdfname_mpi("UNKNOWN") // m_pdfname_ps("UNKONWN"),
{
  declareProperty("RunFile", m_runfile="Herwig7");
  declareProperty("SetupFile", m_setupfile="");

  declareProperty("UseRandomSeedFromGeneratetf", m_use_seed_from_generatetf);
  declareProperty("RandomSeedFromGeneratetf", m_seed_from_generatetf);

  declareProperty("PDFNameME", m_pdfname_me);
  // declareProperty("PDFNamePS", m_pdfname_ps);
  declareProperty("PDFNameMPI", m_pdfname_mpi);

  declareProperty("CrossSectionScaleFactor", m_xsscale=1.0);

  declareProperty("CleanupHerwigScratch", m_cleanup_herwig_scratch);
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
  // Configure the API and print the seed to the log
  if (m_use_seed_from_generatetf){
    ATH_MSG_INFO("Using the random number seed " + to_string(m_seed_from_generatetf) + " provided via Generate_tf.py");
    m_api.seed(m_seed_from_generatetf);
  } else {
    ATH_MSG_INFO("Using the random number seed " + ss_seed.str() + " provided by athena");
    m_api.seed(combined_seed);
  }

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
  ATH_MSG_DEBUG("Successfully loaded Herwig default repository");

  ATH_MSG_INFO("Setting runfile name '"+m_runfile+"'");
  m_api.inputfile(m_runfile);

  ATH_MSG_INFO("starting to prepare the run from runfile '"+m_runfile+"'...");

  // read in a Herwig runfile and obtain the event generator
  m_gen = Herwig::API::prepareRun(m_api);
  ATH_MSG_DEBUG("preparing the run...");

  return StatusCode::SUCCESS;
}



/*!
 *  Run the generator for one event and store the event internally.
 */
StatusCode Herwig7::callGenerator() {
  ATH_MSG_DEBUG("Herwig7 generating an event");
  assert(m_gen);
  m_event = m_gen->shoot();
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
  assert(m_gen);
  cout << "MetaData: generator = Herwig7 " << HWVERSION << endl;
  cout << std::scientific << std::setprecision(5) << "MetaData: cross-section (nb) = " << m_gen->eventHandler()->integratedXSec()*m_xsscale/ThePEG::nanobarn << endl;
  // cout << "MetaData: PDF = " << m_pdfname_me << " (ME); " << m_pdfname_ps << " (shower); " << m_pdfname_mpi << " (MPI)" << endl;
  cout << "MetaData: PDF = " << m_pdfname_me << " (ME); " << m_pdfname_mpi << " (shower/MPI)" << endl;
  m_gen->finalize();
  ThePEG::Repository::cleanup();

  // possibly tidy up working directory
  if (m_cleanup_herwig_scratch && boost::filesystem::is_directory("Herwig-scratch")){

    ATH_MSG_INFO("removing Herwig-scratch folder from "+boost::filesystem::current_path().string());

    // sleep for some time to allow all access to terminate
    boost::this_thread::sleep(boost::posix_time::seconds(5)); /// \todo Think of other way to wait for all access to terminate

    // in case the folder can't be deleted continue with warning
    try {
      boost::filesystem::remove_all("Herwig-scratch");
    } catch (const exception& e) {
      ATH_MSG_WARNING("Failed to delete the folder 'Herwig-scratch': "+string(e.what()));
    }

  }

  return StatusCode::SUCCESS;
}

