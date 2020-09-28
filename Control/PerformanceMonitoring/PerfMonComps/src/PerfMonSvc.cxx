///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonSvc.cxx
// Implementation file for class PerfMonSvc
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

// Python includes
#include "Python.h"
#include "patchlevel.h"

#ifdef Py_False
#undef Py_False
#define Py_False ( (PyObject*)(void*)&_Py_ZeroStruct )
#endif

#ifdef Py_True
#undef Py_True
#define Py_True ( (PyObject*)(void*)&_Py_TrueStruct )
#endif

// C includes
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/times.h>
#include <string.h>
#include <libgen.h> // for basename
#include <filesystem>

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "GaudiKernel/IIoComponentMgr.h"

#include "CxxUtils/AthDsoCbk.h"
#include "CxxUtils/read_athena_statm.h"

#include "RootUtils/PyAthenaGILStateEnsure.h"

// AIDA includes
#include "AIDA/IBaseHistogram.h"

// PerfMonComps includes
#include "PerfMonSvc.h"
#include "PerfMonUtils.h"
#include "PerfMonAuditor.h"
#include "PerfMonEvent/DataModel.h"
#include "PerfMonEvent/MemStatsHooks.h"

//For PMonSD:
#include "SemiDetHelper.h"

// PyROOT includes
#include "CPyCppyy/PyException.h"


#define PMON_COMP_FMT \
  /* comp-id: /step/comp-name */			\
  "/%s/%s "						\
  /* step-index (0 or 1) */				\
  "%lu "						\
  /* cpu: user sys real rt_cpu */			\
  "%8.3f %8.3f %8.3f %8.3f "				\
  /* mem: vmem rss mall nmall nfree*/			\
  "%8.3f %8.3f %8.3f %lu %lu\n"

#define PMON_IOCOMP_FMT \
  /* comp-id: /step/comp-name */			\
  "/%s/%s "						\
  /* cpu-r: user sys real rt_cpu */			\
  "%8.3f %8.3f %8.3f %8.3f "				\
  /* cpu-rr: user sys real rt_cpu */			\
  "%8.3f %8.3f %8.3f %8.3f "				\
  /* cpu-w: user sys real rt_cpu */			\
  "%8.3f %8.3f %8.3f %8.3f\n"

typedef PerfMon::Tuple                        Tuple;
typedef PerfMon::Tuple::CompStore_t     CompStore_t;
typedef PerfMon::Tuple::IoCompStore_t IoCompStore_t;
typedef PerfMon::Tuple::CompTuple_t     CompTuple_t;
typedef PerfMon::Tuple::IoCompTuple_t IoCompTuple_t;

// define some macros to circumvent the MessageSvc printout limits ------------
#define PMON_MSG_LVL_NOCHK(lvl, lvlname, x)			\
  std::cout << "PerfMonSvc: " << lvlname " " << x << "\n"

#define PMON_MSG_LVL(lvl, lvlname, x)		\
  do {						\
    if (this->msgLvl (lvl)) {			\
      PMON_MSG_LVL_NOCHK(lvl, lvlname, x);	\
    }						\
  } while (0)

#define PMON_VERBOSE(x)        PMON_MSG_LVL(MSG::VERBOSE, "VERBOSE", x)
#define PMON_DEBUG(x)          PMON_MSG_LVL(MSG::DEBUG,   "  DEBUG", x)
#define PMON_INFO(x)     PMON_MSG_LVL_NOCHK(MSG::INFO,    "   INFO", x)
#define PMON_WARNING(x)  PMON_MSG_LVL_NOCHK(MSG::WARNING, "WARNING", x)
#define PMON_ERROR(x)    PMON_MSG_LVL_NOCHK(MSG::ERROR,   "  ERROR", x)
#define PMON_FATAL(x)    PMON_MSG_LVL_NOCHK(MSG::FATAL,   "  FATAL", x)
#define PMON_ALWAYS(x)   PMON_MSG_LVL_NOCHK(MSG::ALWAYS,  " ALWAYS", x)
// ----------------------------------------------------------------------------

namespace {

void throw_py_exception (bool display=true)
{
  if (display) {
    // fetch error
    PyObject* pytype = 0, *pyvalue = 0, *pytrace = 0;
    PyErr_Fetch (&pytype, &pyvalue, &pytrace);
    Py_XINCREF  (pytype);
    Py_XINCREF  (pyvalue);
    Py_XINCREF  (pytrace);
    // restore...
    PyErr_Restore (pytype, pyvalue, pytrace);
    // and print
    PyErr_Print();
  }
  throw CPyCppyy::PyException();
}

  std::string 
  my_basename(const std::string& path)
  {
    char *pathc = strdup(path.c_str());
    char *bname = basename(pathc);
    std::string o(bname);
    free(pathc);
    return o;
  }

  std::string
  my_dirname(const std::string& path)
  {
    char *pathc = strdup(path.c_str());
    char *dname = dirname(pathc);
    std::string o(dname);
    free(pathc);
    return o;
  }
}

struct Units {
  static const int kb =        1024;
  static const int Mb = 1024 * 1024;
};

namespace {
  void    capture(int idx, PerfMon::Component& c);

  extern "C"
  int pmon_dso_cbk(const struct ath_dso_event *evt, void *userdata);

}

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
PerfMonSvc::PerfMonSvc( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthService ( name, pSvcLocator ),
  m_pySvc    ( NULL ),
  m_chrono   ( "ChronoStatSvc/ChronoStatSvc", name ),
  m_ntuple   (      ),
  m_firstVmem(   0. ),
  m_10evtVmem(   0. ),
  m_lastVmem (   0. ),
  m_stream   (  -1  ),
  m_nevts    (   0  ),
  m_nalgs    (   0  ),
  m_corrector(      ),
  m_pmonsd   (   0  ),
  m_pf       ( true )
{

  //
  // Property declaration
  //
  //declareProperty( "Property", m_nProperty, "descr" );

  declareProperty( "DeltaVMemAlert",
                   m_deltaVMemAlert = 1. * Units::Mb,
                   "Maximum allowed variation of virtual memory (in bytes) "
                   "between last print-out and current vmem value, before "
                   "triggering a new print-out" );

  declareProperty( "OutFileName",
                   m_outFileName = "ntuple.pmon.gz",
                   "Name of the output file to hold performance data" );

  declareProperty( "ProfiledAlgs",
                   m_profiledAlgNames,
                   "List of algorithms to be profiled. If empty list "
                   "(default), all the algorithms known to the ApplicationMgr "
                   "are taken." );
  m_profiledAlgNames.declareUpdateHandler( &PerfMonSvc::setupProfiledAlgList,
                                           this );

  declareProperty( "IoContainers",
                   m_ioContainerNames,
                   "List of <CppType>#<SgKey> containers for which one wants "
                   "to gather I/O data" );
  m_ioContainerNames.declareUpdateHandler( &PerfMonSvc::setupIoContainerList,
                                           this );

  declareProperty("EnableDoubleCountingCorrection",
                  m_enableDblCountingCorrection = false,
                  "switch to enable or not the on-the-fly double-counting "
                  "correction");

  declareProperty( "MonLvl",
                   m_monLvl = 0,
                   "monitoring level for monitoring granularity.\n" \
                   " -1: ALL\n"                                     \
                   "  0: fast mode\n"                               \
                   " 10: some level");

  declareProperty( "ShelveFileName",
                   m_shelveFileName = "",
                   "name of the shelve file containing jobo metadata (domain "\
                   "flags, components' list, ...)");

  declareProperty( "SemiDetailedMonitoring",
                   m_semiDetMonLvl = 0,
                   "Whether or not to enable semi-detailed monitoring.\n" \
                   "  0: Disabled\n"                                     \
                   "  1: Enabled\n"                               \
                   "  2: Enabled with full output in logfile");

  declareProperty( "JobStartJiffies",
                   m_jobStartJiffies = "",
                   "Needed internally to time configuration step. Do not modify this property." );

  declareProperty( "ExtraPrintouts",
                   m_extraPrintouts = false,
                   "Trigger print of perf metrics before and after each auditing." );



}

// Destructor
///////////////
PerfMonSvc::~PerfMonSvc()
{
  PMON_DEBUG("Calling destructor");
  if (! m_pf) {
    postFinalize();
  }
}

void
PerfMonSvc::postFinalize() 
{
  
  PMON_DEBUG("Calling postFinalize");
  // make sure we're only called once.
  if (m_pf) {
    PMON_DEBUG("Already been called. Returning");
    return;
  }

  fflush(NULL); //> flushes all output streams...

  RootUtils::PyGILStateEnsure gil;

  auto cwd = std::filesystem::current_path();
  if (!m_workerDir.empty()) {
    // The Python service relies on the current directory being the worker directory.
    // This is the case in athena but not necessarily in other applications (ATR-19462)
    std::filesystem::current_path(m_workerDir);
  }

  if ( m_pySvc ) {
    /// loop over the perf-tools and harvest 'finalize' perf-data
    stopAud( "fin", "PerfMonSlice" );
  }

  if (m_stream>0) {
    close(m_stream);
  }

  if (m_pmonsd) {
    //PerfMonSD report to stdout (with least significant entries
    //collapsed) and a txt file (with all entries). We don't gzip
    //since the file anyway ends up in the gzip'ed tarball of all
    //perfmon files:
    std::string pmonsd_out(m_outFileName);pmonsd_out+=".pmon.pmonsd.txt";
    std::string pmon_tarball(m_outFileName);pmon_tarball+=".pmon.gz";
    m_pmonsd->report(pmonsd_out,pmon_tarball,m_semiDetMonLvl>1);
    delete m_pmonsd;m_pmonsd=0;
  }

  if ( m_pySvc ) {

    for (StatStore_t::const_iterator
	   istat = m_stats.begin(),
	   iend  = m_stats.end();
         istat != iend;
         ++istat) {
      const Stats_t& stats = istat->second;
      PyObject* res = PyObject_CallMethod
        ( m_pySvc,
          const_cast<char*>("_set_stats"),
          const_cast<char*>("sIffffffffffffffffIfffififif"),
          (char*)istat->first.c_str(),
          stats.cpu.nEntries(),
          stats.cpu.flagMean(),
          stats.cpu.flagMeanErr(),
	  stats.cpu_user.flagMean(),
          stats.cpu_user.flagMeanErr(),
	  stats.cpu_sys.flagMean(),
          stats.cpu_sys.flagMeanErr(),
          stats.real.flagMean(),
          stats.real.flagMeanErr(),
          stats.vmem.flagMean(),
          stats.vmem.flagMeanErr(),
          stats.malloc.flagMean(),
          stats.malloc.flagMeanErr(),
          stats.nalloc.flagMean(),
          stats.nalloc.flagMeanErr(),
          stats.rt.flagMean(),
          stats.rt.flagMeanErr(),
          m_nevts,
          m_firstVmem,
          m_10evtVmem,
          m_lastVmem,
	  m_vmemfit_evt2to20.nPoints(),
	  m_vmemfit_evt2to20.slope(),
	  m_vmemfit_evt21to100.nPoints(),
	  m_vmemfit_evt21to100.slope(),
	  m_vmemfit_evt101plus.nPoints(),
	  m_vmemfit_evt101plus.slope()
          );
      if ( 0 == res ) {
        PMON_WARNING("Problem on python side during finalize() !!");
        std::abort();
      }
      Py_DECREF( res );
    }

    // write out data
    {
      PyObject* res = PyObject_CallMethod
        ( m_pySvc,
          const_cast<char*>("finalize"),
          const_cast<char*>("") );
      if ( 0 == res ) {
        PMON_WARNING("Problem on python side during finalize() !!");
        std::abort();
      }
      Py_DECREF( res );
    }
  }
  Py_XDECREF( m_pySvc );

  m_pf = true;

  // Restore working directory
  std::filesystem::current_path(cwd);

  //delete m_ntuple; m_ntuple = 0;
  PerfMon::Tuple::CompTuple_t().swap (m_ntuple.comp);
  PerfMon::Tuple::IoCompTuple_t().swap (m_ntuple.iocomp);
}

/// @c IIoComponent interface
//@{
/** @brief callback method to reinitialize the internal state of
 *         the component for I/O purposes (e.g. upon @c fork(2))
 */
StatusCode
PerfMonSvc::io_reinit()
{
  ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
  if (!iomgr.retrieve().isSuccess()) {
    ATH_MSG_ERROR("Could not retrieve IoComponentMgr !");
    return StatusCode::FAILURE;
  }

  if (!iomgr->io_hasitem(this)) {
    ATH_MSG_ERROR("IoComponentMgr does not know about myself !");
    return StatusCode::FAILURE;
  }

  std::string stream_name = m_outFileName.value()+".pmon.stream";
  std::string orig_stream_name = stream_name;

  if (!iomgr->io_contains(this, stream_name)) {
    ATH_MSG_ERROR("IoComponentMgr does not know about [" << stream_name << "] !");
    return StatusCode::FAILURE;
  }
  if (!iomgr->io_retrieve(this, stream_name).isSuccess()) {
    ATH_MSG_ERROR("Could not retrieve new value for [" <<  stream_name << "] !");
    return StatusCode::FAILURE;
  }
  if (stream_name.empty()) {
    ATH_MSG_DEBUG("io_reinit called in parent process. No actions required.");
    return StatusCode::SUCCESS;
  }

  // io_retrieve gives us a name like 
  //  <amp-root-dir>/<child-pid>/mpid_<worker-id>__<ntuple-name>
  // but many pieces in perfmon assume <ntuple-name> as a name...
  //  -> take the dirname from that file and append the original-basename
  m_workerDir = my_dirname(stream_name);
  stream_name = m_workerDir + "/" + my_basename(orig_stream_name);

  ATH_MSG_INFO("reopening [" << stream_name << "]...");
  fflush(NULL); //> flushes all output streams...
  int close_sc = close(m_stream);
  if (close_sc < 0) {
    ATH_MSG_ERROR("could not close previously open fd [" << m_stream << "]: "
                  << strerror(errno));
    return StatusCode::FAILURE;
  }

  // re-open the previous file
  int old_stream = open(orig_stream_name.c_str(), O_RDONLY);
  if (old_stream < 0) {
    ATH_MSG_ERROR("could not reopen previously open file ["
                  << orig_stream_name << "] !");
    return StatusCode::FAILURE;
  }
  m_stream = open(stream_name.c_str(),
                  O_WRONLY | O_CREAT,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (m_stream < 0) {
    ATH_MSG_ERROR("could not reopen [" << stream_name << "]: "
                  << strerror(errno));
    close (old_stream);
    return StatusCode::FAILURE;
  }
  // copy the content of the previous file over to this new one...
  {
    char line[512];
    int bytes;
    while((bytes = read(old_stream, line, sizeof(line))) > 0)
      write(m_stream, line, bytes);
  }
  // close the old stream
  close_sc = close(old_stream);
  if (close_sc < 0) {
    ATH_MSG_INFO("could not close the re-open old pmon.stream file");
  }
  ATH_MSG_INFO("reopening [" << stream_name << "]... [ok]");
  return StatusCode::SUCCESS;
}
//@}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode PerfMonSvc::initialize()
{
  m_pf = false;
  if (m_semiDetMonLvl>0)
    m_pmonsd = new PMonSD::SemiDetHelper(m_jobStartJiffies);

  // initialize MsgStream
  PMON_INFO("Initializing " << name() << "...");
  PMON_INFO("monitoring level: " << m_monLvl);

  const std::string stream_name = m_outFileName.value()+".pmon.stream";

  PMON_INFO("opening pmon-stream file [" << stream_name << "]...");
  m_stream = open(stream_name.c_str(),
		  O_WRONLY | O_CREAT,
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  if (m_stream<0) {
    PMON_ERROR("could not open pmon-stream file ["
		  << stream_name << "] !");
    return StatusCode::FAILURE;
  }
  {
    // describe data format
    const char* hdr = "#data format: comp-id idx cpu-fields(4) mem-fields(4)\n";
    write(m_stream, hdr, strlen(hdr));
    const char* fmt = "# %d " PMON_COMP_FMT;
    write(m_stream, fmt, strlen(fmt));
  }

  // register all ntuples' blocks...
  for (std::size_t istep=0, imax=PerfMon::Steps.size();
       istep != imax;
       ++istep) {
    const std::string& step = PerfMon::Steps[istep];
    m_ntuple.comp.insert(std::make_pair(step, CompStore_t()));
  }
  // "io" ntuple
  {
    const std::string step = "io";
    m_ntuple.iocomp.insert(std::make_pair(step, IoCompStore_t()));
  }
  // "usr" ntuple
  {
    const std::string step = "usr";
    m_ntuple.comp.insert(std::make_pair(step, CompStore_t()));
  }

  RootUtils::PyGILStateEnsure gil;

  PMON_INFO("Retrieving PyPerfMonSvc object...");
  const std::string moduleName = "PerfMonComps.PyPerfMon";
  PyObject* module = PyImport_ImportModule( const_cast<char*>(moduleName.c_str()) );
  if ( !module || !PyModule_Check(module) ) {
    PMON_ERROR("Could not import [" << moduleName << "] !!");
    ::throw_py_exception();
  }

  {
    PMON_INFO("Retrieving PyPerfMonSvc object...");
    // retrieve the pmon-py-svc
    const std::string fctName = "retrieve_pmon_svc";
    PyObject *py_fct = NULL;
    py_fct = PyDict_GetItemString(PyModule_GetDict(module),
				  const_cast<char*>(fctName.c_str()));
    if (!py_fct) {
      PMON_ERROR("could not retrieve function [" << fctName << "] from "
		    "module [" << moduleName << "] !!");
      ::throw_py_exception();
    }
    const std::string pickle_name = m_outFileName.value()+".props.pkl";
    m_pySvc = PyObject_CallFunction(py_fct,
				    (char*)"s",
				    (char*)pickle_name.c_str());
    if (!m_pySvc) {
      PMON_ERROR("problem calling function [" << fctName << "] from "
		    "module [" << moduleName << "] !!");
      ::throw_py_exception();
    }
    PMON_INFO("Retrieving PyPerfMonSvc object... [OK]");
  }
  // don't need the module anymore
  Py_DECREF ( module );

  {
    PyObject* res = PyObject_CallMethod
      ( m_pySvc,
	const_cast<char*>("initialize"),
	const_cast<char*>("") );
    if ( 0 == res ) {
      PMON_WARNING("Problem on python side during initialize() !!");
      ::throw_py_exception();
    }
    Py_DECREF( res );
  }

  // update our current monitoring state:
  setMonState( PerfMon::State::ini );

  // make sure the list of algorithm names to be profiled is up-to-date
  setupProfiledAlgList( m_profiledAlgNames );
  if ( msgLvl(MSG::VERBOSE) ) {
    std::ostringstream out;
    out << "[ ";
    std::copy( m_profiledAlgNames.value().begin(),
	       m_profiledAlgNames.value().end(),
	       std::ostream_iterator<std::string>( out, " " ) );
    out << "]";
    PMON_VERBOSE("Algorithms: " << out.str());
  }

  // make sure the list of I/O containers to be monitored is up-to-date
  setupIoContainerList( m_ioContainerNames );
  if ( msgLvl(MSG::VERBOSE) ) {
    std::ostringstream out;
    out << "[ ";
    std::copy( m_ioContainerNames.value().begin(),
	       m_ioContainerNames.value().end(),
	       std::ostream_iterator<std::string>( out, " " ) );
    out << "]";
    PMON_VERBOSE("Containers: " << out.str());
  }

  // Set to be listener for end-of-event
  ServiceHandle<IIncidentSvc> incSvc( "IncidentSvc", this->name() );
  if ( !incSvc.retrieve().isSuccess() ) {
    PMON_ERROR("Unable to get the IncidentSvc");
    return StatusCode::FAILURE;
  }
  incSvc->addListener( this, IncidentType::SvcPostFinalize );

  // make sure the correct auditor is configured and running
  if ( !PerfMon::makeAuditor("Athena::PerfMonAuditor",
			     auditorSvc(),
			     msg() ).isSuccess() ) {
    PMON_ERROR("Could not register auditor [Athena::PerfMonAuditor] !!");
    return StatusCode::FAILURE;
  }

  startAud( "ini", "PerfMonSlice" );
  {
    // register myself with the I/O component manager...
    ServiceHandle<IIoComponentMgr> iomgr ("IoComponentMgr", name());
    if ( !iomgr.retrieve().isSuccess() ) {
      ATH_MSG_ERROR ("could not retrieve I/O component manager !");
      return StatusCode::FAILURE;
    }
    if ( !iomgr->io_register(this).isSuccess() ) {
      ATH_MSG_ERROR ("could not register with the I/O component manager !");
      return StatusCode::FAILURE;
    }

    // register output file...
    if ( !iomgr->io_register(this,
                             IIoComponentMgr::IoMode::WRITE,
                             stream_name).isSuccess() ) {
      ATH_MSG_WARNING ("could not register file ["
                       << stream_name << "] with the I/O component manager...");
      return StatusCode::FAILURE;
    }
  }

  {
    // install the pmon dso logger
    PyObject* res = PyObject_CallMethod
      ( m_pySvc,
	const_cast<char*>("install_pmon_dso_logger"),
        const_cast<char*>(""),
        const_cast<char*>(""));
    if ( 0 == res ) {
      PMON_WARNING("Problem during pmon-dso-logger installation");
      ::throw_py_exception();
    }
#if PY_MAJOR_VERSION < 3
    long do_dso = PyInt_AS_LONG(res);
#else
    long do_dso = PyLong_AS_LONG(res);
#endif
    Py_DECREF( res );

    if (do_dso) {
      if (0 != ath_dso_cbk_register(&pmon_dso_cbk, (void*)this)) {
        ATH_MSG_INFO("could not register dso-pmon C-callback");
      }
    } else {
      // nothing to do: dsoMonitoring is disabled.
    }
  }

  startAud( "ini", "PerfMonSvc" );
  return StatusCode::SUCCESS;
}

StatusCode PerfMonSvc::finalize()
{
  PMON_INFO("Finalizing " << name() << "...");

  /// loop over the perf-tools and harvest 'finalize' perf-data
  startAud( "fin", "PerfMonSlice" );

  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode
PerfMonSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IPerfMonSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IPerfMonSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Const methods:
///////////////////////////////////////////////////////////////////

/// @brief return the list of components' names for a given step
std::vector<std::string>
PerfMonSvc::components(const std::string& stepName) const
{
  CompTuple_t::const_iterator itr = m_ntuple.comp.find(stepName);
  if (itr == m_ntuple.comp.end()) {
    return std::vector<std::string>();
  }

  const CompStore_t& store = itr->second;

  std::vector<std::string> comps;
  comps.reserve(store.size());
  for (CompStore_t::const_iterator
	 jtr = store.begin(),
	 jend= store.end();
       jtr != jend;
       ++jtr) {
    comps.push_back(jtr->first);
  }
  return comps;
}

/// @brief return the list of io-components' names
std::vector<std::string>
PerfMonSvc::io_components() const
{
  IoCompTuple_t::const_iterator itr = m_ntuple.iocomp.find("io");
  if (itr == m_ntuple.iocomp.end()) {
    return std::vector<std::string>();
  }

  const IoCompStore_t& store = itr->second;

  std::vector<std::string> comps;
  comps.reserve(store.size());
  for (IoCompStore_t::const_iterator
	 jtr = store.begin(),
	 jend= store.end();
       jtr != jend;
       ++jtr) {
    comps.push_back(jtr->first);
  }
  return comps;
}

/// retrieve the monitored value for the component `compName` and
/// for the step `step` (ini,evt,fin,cbk,...)
const PerfMon::Component*
PerfMonSvc::component(const std::string& stepName,
                      const std::string& compName) const
{
  CompTuple_t::const_iterator itr = m_ntuple.comp.find(stepName);
  if (itr == m_ntuple.comp.end()) {
    return 0;
  }

  const CompStore_t& store = itr->second;
  CompStore_t::const_iterator jtr = store.find(compName);
  if (jtr == store.end()) {
    return 0;
  }
  return &(jtr->second);
}

/// retrieve the I/O monitored value for the I/O component `compName`
const PerfMon::IoContainer*
PerfMonSvc::io_component(const std::string& compName) const
{
  IoCompTuple_t::const_iterator itr = m_ntuple.iocomp.find("io");
  if (itr == m_ntuple.iocomp.end()) {
    return 0;
  }

  const IoCompStore_t& store = itr->second;
  IoCompStore_t::const_iterator jtr = store.find(compName);
  if (jtr == store.end()) {
    return 0;
  }
  return &(jtr->second);
}

/// retrieve the domain name for a given component `compName`
void
PerfMonSvc::domain(const std::string& compName, std::string& domain) const
{
  domain = "N/A";

  RootUtils::PyGILStateEnsure gil;

  PyObject* db = PyObject_CallMethod
    ( m_pySvc,
      const_cast<char*>("domains_db"),
      NULL );
  if ( 0 == db ) {
    PMON_WARNING("Problem on python side for domains_db() !!");
    ::throw_py_exception();
  }

  if (!PyDict_Check(db)) {
    PMON_WARNING("domains_db() did not return a dict");
    Py_DECREF(db);
    ::throw_py_exception();
  }

  PyObject *py_domain = PyDict_GetItemString
    (db,
     const_cast<char*>(compName.c_str()));
  if (!py_domain) {
    PMON_WARNING("domains_db() has no [" << compName << "] component");
    Py_DECREF(db);
    ::throw_py_exception();
  }

#if PY_MAJOR_VERSION < 3
  if (!PyString_Check(py_domain)) {
    PMON_WARNING("domains_db() returned a non-string for component ["
		    << compName << "]");
    Py_DECREF(db);
    ::throw_py_exception();
  }

  domain = std::string(PyString_AS_STRING(py_domain));
  Py_DECREF(db);
#else
  if (!PyUnicode_Check(py_domain)) {
    PMON_WARNING("domains_db() returned a non-unicode for component ["
		    << compName << "]");
    Py_DECREF(db);
    ::throw_py_exception();
  }

  domain = std::string(PyUnicode_AsUTF8(py_domain));
  Py_DECREF(db);
#endif

  return;
}


///////////////////////////////////////////////////////////////////
// Non-const methods:
///////////////////////////////////////////////////////////////////

void PerfMonSvc::handle( const Incident& inc )
{
  if ( msgLvl(MSG::VERBOSE) ) {
    msg(MSG::VERBOSE)
      << "Entering handle(): "
      << endmsg
      << "  Incidence type: " << inc.type()   << endmsg
      << "            from: " << inc.source() << endmsg;
  }

  if ( inc.type() == IncidentType::SvcPostFinalize ) {
    postFinalize();
    return;
  }

  return;
}

// IMonitorSvc interface implementation
// ======================================

void PerfMonSvc::declareInfo( const std::string& /*name*/,
                              const bool& /*var*/,
                              const std::string& /*desc*/,
                              const IInterface* iowner )
{
  const INamedInterface* owner = dynamic_cast<const INamedInterface*>(iowner);
  if ( !owner ) { throw std::bad_cast();
  }

  // const bool *ovar = 0;
  // ovar = m_ntuple["usr"]->add_block<bool>((owner->name()+"."+name).c_str());
  // delete ovar;
  // ovar = &var;

  return;
}

void PerfMonSvc::declareInfo( const std::string& /*name*/,
                              const int& /*var*/,
                              const std::string& /*desc*/,
                              const IInterface* iowner )
{
  const INamedInterface* owner = dynamic_cast<const INamedInterface*>(iowner);
  if ( !owner ) { throw std::bad_cast();
  }

  // const int *ovar = 0;
  // ovar = m_ntuple["usr"]->add_block<int>((owner->name()+"."+name).c_str());
  // delete ovar;
  // ovar = &var;

  return;
}

void PerfMonSvc::declareInfo( const std::string& /*name*/,
                              const long& /*var*/,
                              const std::string& /*desc*/,
                              const IInterface* iowner )
{
  const INamedInterface* owner = dynamic_cast<const INamedInterface*>(iowner);
  if ( !owner ) { throw std::bad_cast();
  }

  // const long *ovar = 0;
  // ovar = m_ntuple["usr"]->add_block<long>((owner->name()+"."+name).c_str());
  // delete ovar;
  // ovar = &var;

  return;
}

void PerfMonSvc::declareInfo( const std::string& /*name*/,
                              const double& /*var*/,
                              const std::string& /*desc*/,
                              const IInterface* iowner )
{
  const INamedInterface* owner = dynamic_cast<const INamedInterface*>(iowner);
  if ( !owner ) { throw std::bad_cast();
  }

  // const double *ovar = 0;
  // ovar = m_ntuple["usr"]->add_block<double>((owner->name()+"."+name).c_str());
  // delete ovar;
  // ovar = &var;

  return;
}

void PerfMonSvc::declareInfo( const std::string& /*name*/,
                              const std::string& /*var*/,
                              const std::string& /*desc*/,
                              const IInterface* iowner )
{
  const INamedInterface* owner = dynamic_cast<const INamedInterface*>(iowner);
  if ( !owner ) { throw std::bad_cast();
  }

  // const std::string *ovar = 0;
  // ovar = m_ntuple["usr"]->add_block<std::string>((owner->name()+"."+name).c_str());
  // delete ovar;
  // ovar = &var;

  return;
}

void PerfMonSvc::declareInfo( const std::string& /*name*/,
                              const std::pair<double,double>& /*var*/,
                              const std::string& /*desc*/,
                              const IInterface* iowner )
{
  const INamedInterface* owner = dynamic_cast<const INamedInterface*>(iowner);
  if ( !owner ) { throw std::bad_cast();
  }

  // typedef std::pair<double,double> Var_t;
  // const Var_t *ovar = 0;
  // ovar = m_ntuple["usr"]->add_block<Var_t>((owner->name()+"."+name).c_str());
  // delete ovar;
  // ovar = &var;

  return;
}

void PerfMonSvc::declareInfo( const std::string& name,
                              const AIDA::IBaseHistogram* var,
                              const  std::string& desc,
                              const IInterface* owner )
{
  std::ostringstream err;
  err << "PerfMonSvc::declareInfo( "
      << "name = [" << name << "], "
      << "hist = [" << var << "], "
      << "descr = [" << desc << "], "
      << "owner = [" << owner << "] )\n"
      << "Not implemented !"
      << std::endl;
  msg(MSG::ERROR) << err.str() << endmsg;
  throw std::runtime_error( err.str() );
}

void PerfMonSvc::declareInfo( const std::string& name,
                              const std::string& format,
                              const void * var, int size,
                              const std::string& desc,
                              const IInterface* owner )
{
  std::ostringstream err;
  err << "PerfMonSvc::declareInfo( "
      << "name = [" << name << "], "
      << "fmt = [" << format << "], "
      << "var = [" << var << "], "
      << "size= [" << size << "], "
      << "descr = [" << desc << "], "
      << "owner = [" << owner << "] )\n"
      << "Not implemented !"
      << std::endl;
  msg(MSG::ERROR) << err.str() << endmsg;
  throw std::runtime_error( err.str() );
}

void
PerfMonSvc::declareInfo(const std::string& name,
                        const StatEntity& var,
                        const std::string& desc,
                        const IInterface* owner)
{
  std::ostringstream err;
  err << "PerfMonSvc::declareInfo( "
      << "name = [" << name << "], "
      << "var = [" << var << "], "
      << "descr = [" << desc << "], "
      << "owner = [" << owner << "] )\n"
      << "Not implemented !"
      << std::endl;
  msg(MSG::ERROR) << err.str() << endmsg;
  throw std::runtime_error( err.str() );
}

void PerfMonSvc::undeclareInfo( const std::string& name,
                                const IInterface* iowner )
{
  const INamedInterface* owner = dynamic_cast<const INamedInterface*>(iowner);
  if ( !owner ) {
    msg(MSG::WARNING)
      << "Requested to undeclare info from un-registered owner ["
      << owner << "] !"
      << endmsg;
    throw std::bad_cast();
  }

  msg(MSG::WARNING)
    << "Could not find nor erase MonInfo of name [" << name << "] "
    << "registered with owner [" << owner << " (" << owner->name() << ")] !"
    << endmsg;
  return;
}

void PerfMonSvc::undeclareAll( const IInterface* iowner )
{
  const INamedInterface* owner = dynamic_cast<const INamedInterface*>(iowner);
  if ( !owner ) {
    msg(MSG::WARNING)
      << "Requested to undeclare info from un-registered owner ["
      << owner << "] !"
      << endmsg;
    throw std::bad_cast();
  }

  msg(MSG::WARNING)
    << "Could not undeclare all infos "
    << "registered with owner [" << owner << " (" << owner->name() << ")] !"
    << endmsg;
}

 std::set<std::string>* PerfMonSvc::getInfos( const IInterface* /*owner*/ )
{
  return 0;
}

///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Const methods:
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Non-const methods:
///////////////////////////////////////////////////////////////////
void PerfMonSvc::setupProfiledAlgList( Gaudi::Details::PropertyBase& /*profiledAlgNames*/ )
{
  return;
}

void PerfMonSvc::setupIoContainerList( Gaudi::Details::PropertyBase& /*ioContainerNames*/ )
{
}

namespace {
const std::string pmon_slice = "PerfMonSlice";
const std::string usr_step_name = "usr";

inline
bool do_mon(int lvl,
	    const std::string& stepName,
	    const std::string& compName)
{
  return (lvl < 0 || lvl > 10 ||
	  compName == pmon_slice   ||
	  stepName == PerfMon::Steps[PerfMon::State::ini] ||
	  stepName == PerfMon::Steps[PerfMon::State::cbk] ||
	  stepName == PerfMon::Steps[PerfMon::State::fin] ||
	  stepName == PerfMon::Steps[PerfMon::State::io]  ||
	  stepName == PerfMon::Steps[PerfMon::State::dso] ||
	  stepName == usr_step_name);
}

inline
bool
do_msg_mon(int lvl,
           const std::string& stepName,
           const std::string& /*compName*/)
{
  return
    ((lvl < 0 || lvl > 10 ) &&
     (stepName == PerfMon::Steps[PerfMon::State::ini] ||
      stepName == PerfMon::Steps[PerfMon::State::cbk] ||
      stepName == PerfMon::Steps[PerfMon::State::fin] ||
	  stepName == PerfMon::Steps[PerfMon::State::dso] ||
      stepName == PerfMon::Steps[PerfMon::State::io]))
    ||
    stepName == usr_step_name;
}

}

void PerfMonSvc::startAud( const std::string& stepName,
                           const std::string& compName )
{
  // Performing performance-monitoring for BeginEvent
  if ( compName == m_compBeginEvent && stepName == "evt" ) {
    static bool s_firstEvt = true;
    if ( s_firstEvt ) {
      s_firstEvt = false;
      stopAud( "ini", "PerfMonSlice" );
      // capture the number of algorithms - here
      ServiceHandle<IAlgManager> mgr("ApplicationMgr", this->name());
      m_nalgs = mgr->getAlgorithms().size();
      m_ntuple.comp["ini"].clear();
      m_ntuple.comp["cbk"].clear();
      m_ntuple.comp["preLoadProxies"].clear();
    }
    startAud( "evt", "PerfMonSlice" );
    return;
  }
  else if ( compName == m_compEndEvent && stepName == "evt" ) {
    // Avoid mismatched start/stop that leads to bogus measurements
    return;
  }

  if (m_extraPrintouts) {
    double vmem,rss;
    PMonSD::get_vmem_rss_kb(vmem,rss);
    PMON_INFO("before:/" << stepName << "/" << compName
	      <<" [vmem="<<vmem/1024.0<<"mb, rss="
	      <<rss/1024.0<<"mb, malloc="<<PMonSD::get_malloc_kb()/1024.0
	      <<"mb, wall="<<PMonSD::get_wall_ms()<<"s]"
	      );
  }
  PMON_DEBUG("start:/" << stepName << "/" << compName);
  if (::do_mon(m_monLvl, stepName, compName)) {
    if (stepName == PerfMon::Steps[PerfMon::State::io]) {
      //io_startAud(stepName, compName);
    } else {
      comp_startAud(stepName, compName);
    }
  }
  if (m_pmonsd)
    m_pmonsd->startAud(stepName,compName,m_nevts);

  return;
}

void PerfMonSvc::stopAud( const std::string& stepName,
                          const std::string& compName )
{
  // Performing performance-monitoring for EndEvent
  if ( compName == m_compEndEvent && stepName == "evt" ) {
    // make sure we update the data from declareInfo...
    poll();
    stopAud( "evt", "PerfMonSlice" );
    return;
  }
  else if ( compName == m_compBeginEvent && stepName == "evt" ) {
    // Avoid mismatched start/stop that leads to bogus measurements
    return;
  }

  if (m_pmonsd)
    m_pmonsd->stopAud(stepName,compName,m_nevts);

  if (::do_mon(m_monLvl, stepName, compName)) {
    if (stepName == PerfMon::Steps[PerfMon::State::io]) {
      //io_stopAud(stepName, compName);
    } else {
      comp_stopAud(stepName, compName);
    }
  }
  if (m_extraPrintouts) {
    double vmem,rss;
    PMonSD::get_vmem_rss_kb(vmem,rss);
    PMON_INFO("after:/" << stepName << "/" << compName
	      <<" [vmem="<<vmem/1024.0<<"mb, rss="
	      <<rss/1024.0<<"mb, malloc="<<PMonSD::get_malloc_kb()/1024.0
	      <<"mb, wall="<<PMonSD::get_wall_ms()<<"s]"
	      );
  }
  return;
}

void PerfMonSvc::poll()
{
  // Tuple_t::iterator itr = m_ntuple.find("usr");
  // if (itr != m_ntuple.end()) {
  //   ntlib::Tuple nt = itr->second;
  //   nt->write();
  // }

  return;
}

/** start collection monitoring cpu+mem data for a given component
 */
void
PerfMonSvc::comp_startAud(const std::string& stepName,
                          const std::string& compName)
{
  PerfMon::Component& c = m_ntuple.comp[stepName][compName];
  ::capture(0, c);

  // we still want to double-count dso's and callbacks since they don't appear in the summary file (maybe they should?)
  const bool step_displayed_in_summary(PerfMon::Steps[PerfMon::State::evt] == stepName ||
				       PerfMon::Steps[PerfMon::State::ini] == stepName ||
				       PerfMon::Steps[PerfMon::State::fin] == stepName );
  if (step_displayed_in_summary && m_enableDblCountingCorrection) {
    c.mem.vmem[0] -= m_corrector.vmem;
    c.mem.rss [0] -= m_corrector.rss;
    c.mem.mall[0] -= m_corrector.mall;
    c.mem.nmall[0]-= m_corrector.nmall;
    c.mem.nfree[0]-= m_corrector.nfree;
  }

  char* buf = 0;
  int buf_sz = asprintf
    (&buf,
     PMON_COMP_FMT,
     stepName.c_str(), compName.c_str(),
     0LU,
     c.cpu.user, c.cpu.sys, c.cpu.real, c.cpu.rt_cpu,
     c.mem.vmem[0], c.mem.rss[0], c.mem.mall[0],
     ((unsigned long)c.mem.nmall[0]),
     ((unsigned long)c.mem.nfree[0])
     );
  write(m_stream, buf, buf_sz);
  free(buf);

  return;
}

void
PerfMonSvc::comp_stopAud(const std::string& stepName,
                         const std::string& compName)
{
  PerfMon::Component& c = m_ntuple.comp[stepName][compName];
  ::capture(1, c);

  bool alertVmem = false;
  if (compName == pmon_slice) {
    if (stepName == PerfMon::Steps[PerfMon::State::evt]) {
      ++m_nevts;
      // vmems are already in kb but m_deltaVMemAlert is in Mb...
      float deltaVmem= (c.mem.vmem[1] - m_lastVmem);
      if (deltaVmem >= (m_deltaVMemAlert/1024.)) {
        alertVmem = true;
        m_lastVmem = c.mem.vmem[1];
      }
    }
    if (m_nevts > 1 ||
        (stepName == PerfMon::Steps[PerfMon::State::ini] ||
         stepName == PerfMon::Steps[PerfMon::State::fin])) {
      Stats_t& stats = m_stats[stepName];
      stats.cpu  += c.cpu.user + c.cpu.sys;
      stats.cpu_user  += c.cpu.user;
      stats.cpu_sys  += c.cpu.sys;
      stats.real += c.cpu.real;
      stats.rt   += c.cpu.rt_cpu;
      stats.vmem += c.mem.vmem[1];
      stats.malloc += c.mem.mall[1] - c.mem.mall[0];
      stats.nalloc += c.mem.nmall[1]- c.mem.nmall[0];
    }

    if ( m_nevts>=2 && stepName == PerfMon::Steps[PerfMon::State::evt]) {
      if (m_nevts<=20) m_vmemfit_evt2to20.addPoint(m_nevts,m_lastVmem);
      else if (m_nevts<=100) m_vmemfit_evt21to100.addPoint(m_nevts,m_lastVmem);
      else m_vmemfit_evt101plus.addPoint(m_nevts,m_lastVmem);
    }
    if ( 1<=m_nevts && m_nevts < 11 &&
        stepName == PerfMon::Steps[PerfMon::State::evt]) {
      switch (m_nevts-1) {
        case 0:
          // update first-evt high-water mark
          m_firstVmem = m_lastVmem;
          break;
        case 9:
          // update 10th-evt high-water mark
          m_10evtVmem = m_lastVmem;
          break;
        default:
          break;
        }
    }
  }

  if (alertVmem  ||
      (m_nevts <= 10 && compName == pmon_slice) ||
      ::do_msg_mon(m_monLvl, stepName, compName))
  {
    char* buf = 0;
    char* hdr = 0;
    asprintf(&hdr, "[%s] ", stepName.c_str());
    //TK: Coverity makes the good point here that alertVmem is only
    //true for "evt" step, so is_evt is always true and we get dead
    //code below.
    //SB: I actually think coverity is a bit confused b/c I do see
    //    printouts like so:
    //      PerfMonSvc:    INFO (null)cpu=  ...
    //    instead of e.g.:
    //      PerfMonSvc:    INFO [dso] cpu=  ...
    //    which would tend to confirm is_evt is not always true.
    const bool is_evt = (stepName == PerfMon::Steps[PerfMon::State::evt]);
    if (is_evt) {
      free(hdr); hdr = 0;
      asprintf(&hdr, "[evt: %3lu] ", (m_nevts>0 ? m_nevts-1 : 0));
    }
    asprintf(&buf,
	     "%s"						\
	     "cpu=%8.3f ms "					\
	     "vmem=%8.3f Mb "					\
	     "dvmem=%8.3f Mb "					\
	     "alloc= +%lu -%lu %8.3f kb "			\
	     " => [/%s/%s]",
	     hdr,
	     c.cpu.user+c.cpu.sys,
	     c.mem.vmem[1]/1024.,
	     c.mem.dVmem()/1024.,
	     ((unsigned long)c.mem.dNMall()),
	     ((unsigned long)c.mem.dNFree()),
	     (float)c.mem.dMall(),
	     stepName.c_str(), compName.c_str()
	     );
    PMON_INFO(buf);
    free(hdr);
    free(buf);
  }
  {

    // we still want to double-count dso's and callbacks since they don't appear in the summary file (maybe they should?)
    const bool step_displayed_in_summary(PerfMon::Steps[PerfMon::State::evt] == stepName ||
					 PerfMon::Steps[PerfMon::State::ini] == stepName ||
					 PerfMon::Steps[PerfMon::State::fin] == stepName );
    if (step_displayed_in_summary && m_enableDblCountingCorrection) {
      c.mem.vmem[1] -= m_corrector.vmem;
      c.mem.rss [1] -= m_corrector.rss;
      c.mem.mall[1] -= m_corrector.mall;
      c.mem.nmall[1]-= m_corrector.nmall;
      c.mem.nfree[1]-= m_corrector.nfree;

      m_corrector.vmem  += (c.mem.vmem[1]  - c.mem.vmem[0]);
      m_corrector.rss   += (c.mem.rss[1]   - c.mem.rss [0]);
      m_corrector.mall  += (c.mem.mall[1]  - c.mem.mall[0]);
      m_corrector.nmall += (c.mem.nmall[1] - c.mem.nmall[0]);
      m_corrector.nfree += (c.mem.nfree[1] - c.mem.nfree[0]);
    }

    char* buf = 0;
    int buf_sz = asprintf
      (&buf,
       PMON_COMP_FMT,
       stepName.c_str(), compName.c_str(),
       1LU,
       c.cpu.user, c.cpu.sys, c.cpu.real, c.cpu.rt_cpu,
       c.mem.vmem[1], c.mem.rss[1], c.mem.mall[1],
       ((unsigned long)c.mem.nmall[1]),
       ((unsigned long)c.mem.nfree[1])
       );
    write(m_stream, buf, buf_sz);
    free(buf);
  }

  if (stepName == PerfMon::Steps[PerfMon::State::evt] &&
      compName == pmon_slice) {
    // trigger the post event I/O monitoring...
    do_io_mon();
  }
}

void
PerfMonSvc::do_io_mon()
{
  const std::vector<std::string>& ionames = m_ioContainerNames.value();
  for (std::size_t i = 0, imax = ionames.size();
       i != imax;
       ++i) {
    const std::string& ioname = ionames[i];
    const std::string p2t = std::string("cObj_")+ioname;
    const std::string p2tr= std::string("cObjR_")+ioname;
    const std::string t2p = std::string("cRep_")+ioname;

    PerfMon::IoContainer& c = m_ntuple.iocomp["io"][ioname];

    // read
    c.r.user = (float)m_chrono->chronoDelta(p2t, IChronoStatSvc::USER);
    c.r.sys  = (float)m_chrono->chronoDelta(p2t, IChronoStatSvc::KERNEL);
    c.r.real = (float)m_chrono->chronoDelta(p2t, IChronoStatSvc::ELAPSED);
    //c.r.rt_cpu = 0.;

    // ROOT-read
    c.rr.user = (float)m_chrono->chronoDelta(p2tr, IChronoStatSvc::USER);
    c.rr.sys  = (float)m_chrono->chronoDelta(p2tr, IChronoStatSvc::KERNEL);
    c.rr.real = (float)m_chrono->chronoDelta(p2tr, IChronoStatSvc::ELAPSED);
    //c.rr.rt_cpu = 0.;

    // write
    c.w.user = (float)m_chrono->chronoDelta(t2p, IChronoStatSvc::USER);
    c.w.sys  = (float)m_chrono->chronoDelta(t2p, IChronoStatSvc::KERNEL);
    c.w.real = (float)m_chrono->chronoDelta(t2p, IChronoStatSvc::ELAPSED);
    //c.w.rt_cpu = 0.;

    char* buf = 0;
    int buf_sz = asprintf
      (&buf,
       PMON_IOCOMP_FMT,
       "io", ioname.c_str(),
       c.r.user,  c.r.sys,  c.r.real,  c.r.rt_cpu,
       c.rr.user, c.rr.sys, c.rr.real, c.rr.rt_cpu,
       c.w.user,  c.w.sys,  c.w.real,  c.w.rt_cpu
       );
    write(m_stream, buf, buf_sz);
    free(buf);
  } //> loop over io-containers

  PerfMon::IoContainer& c = m_ntuple.iocomp["io"]["PerfMonSliceIo"];
  const std::string ioname = "commitOutput";
  c.w.user = (float)m_chrono->chronoDelta(ioname, IChronoStatSvc::USER);
  c.w.sys  = (float)m_chrono->chronoDelta(ioname, IChronoStatSvc::KERNEL);
  c.w.real = (float)m_chrono->chronoDelta(ioname, IChronoStatSvc::ELAPSED);

  {
    char* buf = 0;
    int buf_sz = asprintf
      (&buf,
       PMON_IOCOMP_FMT,
       "io", "PerfMonSliceIo",
       c.r.user,  c.r.sys,  c.r.real,  c.r.rt_cpu,
       c.rr.user, c.rr.sys, c.rr.real, c.rr.rt_cpu,
       c.w.user,  c.w.sys,  c.w.real,  c.w.rt_cpu
       );
    write(m_stream, buf, buf_sz);
    free(buf);
  }
  return;
}

///////////////////////////////////////////////////////////////////
// Private data:
///////////////////////////////////////////////////////////////////

#include <fstream>
#include <sstream>
#include "GaudiKernel/Timing.h"
#ifdef __APPLE__
#include <mach/task.h>
#include <mach/mach_init.h>
#include <unistd.h>
#endif

///// local utils
namespace {

  // const System::TimeType MyUnit = System::microSec;
  const System::TimeType MyUnit = System::milliSec;
  void capture(int idx, PerfMon::Component& c)
  {
    // capture cpu data
    PerfMon::Cpu& cpu = c.cpu;
    if (idx==0) {
      c = PerfMon::Component();
    }
    cpu.user = static_cast<float>(System::userTime    (MyUnit)) - cpu.user;
    cpu.sys  = static_cast<float>(System::kernelTime  (MyUnit)) - cpu.sys;
    cpu.real = static_cast<float>(System::ellapsedTime(MyUnit)) - cpu.real;
    cpu.rt_cpu= PerfMon::rt_cpu() - cpu.rt_cpu; //> in millisecs

    // capture mem data
    PerfMon::Mem& mem = c.mem;
    athena_statm s = read_athena_statm();
    static long  pg_size = sysconf(_SC_PAGESIZE);
    mem.vmem[idx] = static_cast<float>(s.vm_pages * pg_size / (float)Units::kb);
    mem.rss[idx]  = static_cast<float>(s.rss_pages* pg_size / (float)Units::kb);
    mem.mall[idx] = static_cast<float>(PerfMon::MemStats::nbytes()/Units::kb);
    mem.nmall[idx]= static_cast<float>(PerfMon::MemStats::nmallocs());
    mem.nfree[idx]= static_cast<float>(PerfMon::MemStats::nfrees());
  }

  int
  pmon_dso_cbk(const struct ath_dso_event *evt, void *userdata)
  {
    if (userdata) {
      PerfMonSvc *svc = (PerfMonSvc*)userdata;
      const std::string compName(evt->fname ? evt->fname : "NULL");
      if (evt->step == 0) {
        svc->startAud(PerfMon::Steps[PerfMon::State::dso], compName);
      } else if (evt->step == 1) {
        svc->stopAud(PerfMon::Steps[PerfMon::State::dso], compName);
      } else {
        // fallthrough.
        // FIXME: warn ?
      }
    }
    return 0;
  }

}
