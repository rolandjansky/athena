/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL includes
#include <iterator>

#include "GaudiKernel/MsgStream.h"
#include "FileStager/FileStagerAlg.h"

#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "FileStager/TStageManager.h"
#include "FileStager/TCopyFile.h"
#include <TFile.h>
#include <TTree.h>
#include <TStopwatch.h>
#include <TH1D.h>
#include <TFile.h>

FileStagerAlg::FileStagerAlg(const std::string& name, ISvcLocator* pSvcLocator) 
 : Algorithm(name, pSvcLocator)
 , m_pipeLength(1)
 , m_verbose(false)
 , m_verboseWait(true)
 , m_firstFileAlreadyStaged(false)
 , m_treeName("CollectionTree")
 , m_infilePrefix("gridcopy://")
 , m_outfilePrefix("file:")
 , m_cpCommand("lcg-cp")
 , m_baseTmpdir("")
 , m_logfileDir("")
 , m_keepLogfiles(false)
 , m_storeStats(false)
 , m_prevFile("")
 , m_stopwatch(0)
 , m_waittime(0)
 , m_waithist(0)
{
  // Declare the properties
  declareProperty("PipeLength", m_pipeLength);
  declareProperty("VerboseStager", m_verbose);
  declareProperty("VerboseWaiting", m_verboseWait);
  declareProperty("FirstFileAlreadyStaged",m_firstFileAlreadyStaged);
  declareProperty("TreeName",m_treeName);
  declareProperty("InfilePrefix", m_infilePrefix);
  declareProperty("OutfilePrefix", m_outfilePrefix);
  declareProperty("CpCommand", m_cpCommand);
  declareProperty("BaseTmpdir", m_baseTmpdir);
  declareProperty("CpArguments", m_cpArg, "vector of cp arguments");  
  declareProperty("InputCollections", m_inCollection, "vector of input files");  
  declareProperty("OutputCollections", m_outCollection, "vector of output files");  
  declareProperty("LogfileDir", m_logfileDir);
  declareProperty("KeepLogfiles", m_keepLogfiles);
  declareProperty("StoreStatistics", m_storeStats);

  m_stopwatch = new TStopwatch();
  m_waithist = new TH1D("stagehist","Time for next file to finish staging (s)",600,0.,60.);

  // file statistics about number of bytes processed by root
  TCopyFile::PrintInfo();
}


FileStagerAlg::~FileStagerAlg()
{
  if (m_stopwatch) delete m_stopwatch;
  if (m_waithist) delete m_waithist;

  // file statistics about number of bytes processed by root
  TCopyFile::PrintInfo();

  // release last file 
  // ... assuming is has been closed by pool ;-)

  //TStageManager& manager(TStageManager::instance());
  //manager.releaseAll();
}


StatusCode 
FileStagerAlg::initialize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << name() << "Initialize()" << endmsg;

  // use the incident service to register
  IIncidentSvc* incsvc = 0;
  StatusCode status = service("IncidentSvc", incsvc, true);

  if(status.isFailure() || incsvc==0) {
    log << MSG::WARNING << "Unable to get IncidentSvc! MF mechanism is disabled" << endmsg;
    return StatusCode::SUCCESS;
  }

  incsvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
  incsvc->addListener(this, "EndInputFile", 0);

  log << MSG::DEBUG << "Added listeners on begin and end of input files." << endmsg;

  // configure the stager
  configStager();
  // fill up the stager
  loadStager();

  setupNextFile();

  log << MSG::DEBUG << name() << "Initialize() successful" << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode 
FileStagerAlg::execute() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << name() << "Execute()" << endmsg;

  log << MSG::DEBUG << name() << "Execute() successful" << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode 
FileStagerAlg::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << name() << "Finalize()" << endmsg;
  log << MSG::WARNING << "Total wait time = " << m_waittime << " s." << endmsg;

  if (m_storeStats) {
    std::string fileName = "filestager_stats.root";
    log << MSG::WARNING << "Writing filestager statistics to file <" << fileName << ">" << endmsg;
    TFile *ff = new TFile(fileName.c_str(),"recreate");
    m_waithist->Write();
    ff->Close();
    delete ff;
  }

  // release remaining staged files in destructor

  log << MSG::DEBUG << name() << "Finalize() successful" << endmsg;
  return StatusCode::SUCCESS;
}


void
FileStagerAlg::configStager()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "configStager()" << endmsg;

  // itr for looping over input files
  m_fItr = m_inCollection.begin();

  // stager settings
  TStageManager& manager(TStageManager::instance());

  manager.setPipeLength(m_pipeLength);
  manager.verbose(m_verbose);
  manager.verboseWait(m_verboseWait);
  manager.firstFileAlreadyStaged(m_firstFileAlreadyStaged);
  manager.keepLogfiles(m_keepLogfiles);

  if (!m_infilePrefix.empty())
    manager.setInfilePrefix(m_infilePrefix.c_str());
  if (!m_outfilePrefix.empty())
    manager.setOutfilePrefix(m_outfilePrefix.c_str());
  if (!m_cpCommand.empty())
    manager.setCpCommand(m_cpCommand.c_str());  
  if (!m_baseTmpdir.empty())
    manager.setBaseTmpdir(m_baseTmpdir.c_str());
  if (!m_logfileDir.empty())
    manager.setLogfileDir(m_logfileDir.c_str());
  
  for (int i=0; i<int(m_cpArg.size()); ++i)
    manager.addCpArg(m_cpArg[i].c_str());
}


void
FileStagerAlg::loadStager()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "loadStager()" << endmsg;

  TStageManager& manager(TStageManager::instance());

  m_outCollection.clear();
  std::vector< std::string >::iterator itr = m_inCollection.begin();

  // ensure deletion of first staged file
  if (!m_inCollection.empty())
    m_prevFile = m_inCollection[0].c_str();

  // add files and start staging ...
  for (; itr!=m_inCollection.end(); ++itr) {
    manager.addToList(itr->c_str());
    std::string outColl = manager.getTmpFilename(itr->c_str());
    m_outCollection.push_back( outColl );
  }
}


void
FileStagerAlg::releasePrevFile()
{
  // release previous file (not previous file) to avoid possible conflicts w/ pool
  if (!m_prevFile.empty()) {
    TStageManager& manager(TStageManager::instance());
    manager.releaseFile(m_prevFile.c_str());
  }

  if (m_fItr!=m_inCollection.end()) { 
    m_prevFile = *m_fItr; 
  }
}


void
FileStagerAlg::setupNextFile()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "setupNextFile()" << endmsg;

  if (m_fItr!=m_inCollection.end()) {
    TStageManager& manager(TStageManager::instance());

    // wait till file finishes staging ...
    m_stopwatch->Start();
    (void) manager.getFile(m_fItr->c_str());
    m_stopwatch->Stop();

    // collect wait time statistics
    m_waithist->Fill(m_stopwatch->RealTime());
    m_waittime += m_stopwatch->RealTime();

    log << MSG::DEBUG << "Time to wait for <" << m_fItr->c_str() << "> = " << m_stopwatch->RealTime() << " s." << endmsg;

    ++m_fItr;
  }
}


void 
FileStagerAlg::handle(const Incident& inc) 
{
  MsgStream log(msgSvc(), name());

  const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
  if (fileInc == 0) {
      log << MSG::ERROR << " Unable to get FileName from BeginInputFile/EndInputFile incident" << endmsg;
      return;
  }

  const std::string fileName = fileInc->fileName();
  log << MSG::DEBUG << "handle() " << inc.type() << " for " << fileName << endmsg;

  if (inc.type() == "BeginInputFile") {
      // do nothing
  } else if (inc.type() == "EndInputFile") {
      releasePrevFile();
      // do nothing with current file to avoid conflicts with pool
      setupNextFile();
  } 
}

