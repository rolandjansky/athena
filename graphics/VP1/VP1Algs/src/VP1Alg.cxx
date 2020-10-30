/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1Algs/VP1Alg.h"

#include "VP1Gui/VP1Gui.h"
#include "VP1UtilsBase/VP1FileUtilities.h"

#include <xAODEventInfo/EventInfo.h>
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIncidentSvc.h"

#include <vector>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstdint> // for uint64_t (unsigned long long)
#include <cstdlib> //For setenv
 

//____________________________________________________________________
VP1Alg::VP1Alg(const std::string& name, ISvcLocator* svcLocator):
  AthAlgorithm(name, svcLocator),
  m_toolSvc(0),m_vp1gui(0)
{
  declareProperty("InitiallyLoadedVP1Files",m_initialvp1files);
  declareProperty("InitialCruiseMode",m_initialCruiseMode="NONE");
  declareProperty("InitialCruiseModePeriod",m_initialCruiseSeconds=10);

  declareProperty("NoGui",m_noGui=false);

  //  **** MF ****
  declareProperty("MultipleFilesON",m_mfOn=false);
  declareProperty("MFNumFileLimit",m_mfLimit=10);
  declareProperty("MFSourceDir",m_mfSourceDir="");
  declareProperty("MFLocalCopyDir",m_mfLocalCopyDir="");
  declareProperty("MFAvailableLocalInputDirectories",m_mfAvailableLocalInputDirectories);


  // Two ways of running in multiple files mode:
  //
  // 1) Source is directory on filesystem (e.g. for use in the control room).
  //      * MFSourceDir indicates the directory where files can be found.
  //      * MFLocalCopyDir indicates the temporary directory where
  //        files are copied before we tell pool to get them there.
  // 2) Source is web directory
  //      * MFSourceDir indicates http url to small text file (to be
  //        parsed by VP1Gui/VP1EvtsOnServerInfo), with a list of
  //        available files (assumed to be in the same dir).
  //        Example value: "http://pcatdwww.cern.ch/atlas-point1/vp1events/fileinfo.txt"
  //      * MFLocalCopyDir has same meaning as before, but this time a
  //        subdirectory of it, "active_downloads", will be used to
  //        download files before they are subsequently moved to
  //        MFLocalCopyDir itself.

  //  **** MF ****

  // Pick up patched Coin
  ::setenv("LCGPATCH_COINMULTISELECT","1",1);
}

//____________________________________________________________________
VP1Alg::~VP1Alg()
{
}

//____________________________________________________________________
StatusCode VP1Alg::initialize()
{
  msg(MSG::INFO) << " in initialize() " << endmsg;

  //ToolSvc
  StatusCode status = service("ToolSvc",m_toolSvc);
  if (status.isFailure()||!m_toolSvc) {
    msg(MSG::ERROR) << " Unable to get ToolSvc!" << endmsg;
    return status;
  }

  std::vector<std::string>::iterator it, itE = m_initialvp1files.end();
  for (it = m_initialvp1files.begin();it!=itE;++it) {
    std::string file = PathResolver::find_file (*it, "DATAPATH");
    if (file=="")
      file = PathResolver::find_file (*it+".vp1", "DATAPATH");
    if (file!="")
      *it = file;
  }

  // use the incident service to register a handler
  IIncidentSvc* incsvc = 0;
  status = service("IncidentSvc", incsvc, true);

  if(status.isFailure() || incsvc==0) {
    msg(MSG::WARNING) << "Unable to get IncidentSvc! MF mechanism is disabled" << endmsg;
    return StatusCode::SUCCESS;
  }

  std::string endfilekey("EndTagFile");
  incsvc->addListener(this, endfilekey, 0);
  msg(MSG::DEBUG) << "Added listener on "<<endfilekey << endmsg;
  
  //Create VP1 gui object and see if it considers settings to be valid.
  m_vp1gui = new VP1Gui(&(*evtStore()),&(*detStore()),serviceLocator(),m_toolSvc,
			m_initialvp1files,
			m_initialCruiseMode,m_initialCruiseSeconds,
			(m_mfOn ? m_mfSourceDir : ""),
			(m_mfOn ? m_mfLocalCopyDir : ""),
			m_mfLimit,
			( m_mfOn ? m_mfAvailableLocalInputDirectories : std::vector<std::string>() ) );
  if (!m_vp1gui->argumentsAreValid()) {
    delete m_vp1gui;
    m_vp1gui = 0;
    exit(1);//Should we do this? It is kind of nice to not have to dig through the logfile to find the failure.
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//____________________________________________________________________
StatusCode VP1Alg::execute()
{

    // Here you do everything that needs to be done on every single
    // events, e.g. read input variables, apply cuts, and fill
    // histograms and trees, if any.  

  msg(MSG::DEBUG) <<" in execute() " << endmsg;

  if (!m_vp1gui)
    return StatusCode::FAILURE;

  if (!m_noGui&&!m_vp1gui->hasBeenInitialised()) {
    m_vp1gui->init();//Launch!
  }




  // Get L1 trigger type from the EventInfo
  unsigned int trigType = 0;
  const EventInfo*  evt;
  StatusCode statusEvt = evtStore()->retrieve(evt);
  if(statusEvt.isSuccess()) {
   const TriggerInfo* trig = evt->trigger_info();
   trigType = trig ? trig->level1TriggerType() : 0;
  }

  // retrieve the eventInfo object from the event store
  const xAOD::EventInfo *eventInfo = nullptr;
  StatusCode status = evtStore()->retrieve (eventInfo, "EventInfo");
  
  if(status.isSuccess()) {
    
    // Get run/event number:
	const unsigned long long eventNumber = eventInfo->eventNumber();
    const uint32_t           runNumber   = eventInfo->runNumber();
    msg(MSG::DEBUG) << " Got run number = " << runNumber
	<< ", event number = " << eventNumber << endmsg;
    
    // Get time stamp:
    uint32_t time = eventInfo->timeStamp();//0 means no info.

    if (m_noGui||m_vp1gui->executeNewEvent(runNumber,eventNumber,trigType,time)) {
      return StatusCode::SUCCESS;
    } else {
      msg(MSG::INFO) << " Ending application gracefully." << endmsg;
      return StatusCode::FAILURE;
    }
  return StatusCode::SUCCESS;
  };

  msg(MSG::WARNING) << " Unable to retrieve EventInfo from StoreGate. Skipping" << endmsg;
  return StatusCode::FAILURE;

}

//____________________________________________________________________
StatusCode VP1Alg::finalize()
{
  msg(MSG::INFO) <<" in finalize() " << endmsg;

  if (!m_vp1gui)
    return StatusCode::FAILURE;

  if (!m_noGui)
    m_vp1gui->cleanup();
  delete m_vp1gui;

  return StatusCode::SUCCESS;
}

//____________________________________________________________________
void VP1Alg::handle(const Incident& inc)
{
  msg(MSG::INFO) << "Handling incident '" << inc.type() << "'" << endmsg;

  if (!m_vp1gui) {
    msg(MSG::INFO) << "Aborting due to null VP1Gui pointer." << endmsg;
    return;
  }

  const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
  if(fileInc == 0) {
    msg(MSG::WARNING) << " Unable to cast to file incident" << endmsg;
    return;
  }
  else
    msg(MSG::DEBUG) << " Casting to file incident successful" << endmsg;

  // Locate the EventSelector
  ServiceHandle<IEvtSelector> pEvtSelector("EventSelector", this->name());
  StatusCode sc = pEvtSelector.retrieve();

  if(!sc.isSuccess() || 0 == pEvtSelector) {
    msg(MSG::WARNING) << "Could not find EventSelector" << endmsg;
    return;
  }
  else
    msg(MSG::DEBUG) << " Got EventSelector" << endmsg;

  IProperty* propertyServer = dynamic_cast<IProperty*>(pEvtSelector.operator->());
  if (!propertyServer) {
    msg(MSG::WARNING) << "Could not get propertyServer" << endmsg;
    return;
  }

  std::vector<std::string> vect;
  StringArrayProperty inputCollections("InputCollections", vect);

  sc = propertyServer->getProperty(&inputCollections);
  if(!sc.isSuccess()) {
    msg(MSG::INFO) << "Could not get InputCollections property" << endmsg;
    return;
  }
  else
    msg(MSG::DEBUG) << " Got InputCollections property" << endmsg;

  std::vector<std::string>::const_iterator iter = inputCollections.value().begin();
  std::vector<std::string>::const_iterator last = inputCollections.value().end();
  for(; iter != last; iter++) {
    if(*iter != fileInc->fileName())
      vect.push_back(*iter);
  }

  if(m_mfOn) {
    std::string strNewFileName = m_vp1gui->nextRequestedEventFile();

    if (strNewFileName.empty())
      return;
    if (!VP1FileUtilities::fileExistsAndReadable(strNewFileName)) {
      msg(MSG::WARNING) << " File requested by VP1 does not exists or is not readable: "<<strNewFileName<<". Ending." << endmsg;
      return;
    }

    vect.push_back(strNewFileName);
    msg(MSG::INFO) << " Setting next event file: " << strNewFileName<< endmsg;
  } 
  else {
    std::vector<std::string> strNewFileNames = m_vp1gui->userRequestedFiles();
    for(unsigned i=0; i<strNewFileNames.size(); ++i) {
      const std::string& strNewFileName = strNewFileNames[i];
      if (strNewFileName.empty())
	continue;
      if (!VP1FileUtilities::fileExistsAndReadable(strNewFileName)) {
	msg(MSG::WARNING) << " File requested by VP1 does not exists or is not readable: " << strNewFileName << endmsg;
	continue;
      }
      vect.push_back(strNewFileName);
      msg(MSG::INFO) << " Setting next event file: " << strNewFileName<< endmsg;
    }
  }

  StringArrayProperty newInputCollections("InputCollections", vect);

  if(propertyServer->setProperty(newInputCollections)!=StatusCode::SUCCESS)
    msg(MSG::WARNING) << "Could not set new InputCollections property" << endmsg;
  else
    msg(MSG::DEBUG) << " InputCollections property set" << endmsg;
}

