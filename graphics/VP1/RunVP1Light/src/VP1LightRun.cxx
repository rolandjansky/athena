/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Author: Riccardo.Maria.Bianchi@cern.ch, Apr 2017
// Update: Apr 2019

#include "RunVP1Light/VP1LightRun.h"

#include "VP1Gui/VP1Gui.h"
#include "VP1UtilsBase/VP1FileUtilities.h"

// #include "EventInfo/EventInfo.h"
// #include "EventInfo/TriggerInfo.h"
// #include "EventInfo/EventID.h"
// #include "PathResolver/PathResolver.h"

// C++ includes
#include <vector>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstdint> // for uint64_t (unsigned long long)
#include <cstdlib> //For setenv
#include <chrono>
// #include <ctime>

using namespace std;


class StoreGateSvc;
class ISvcLocator;
class IToolSvc;

VP1LightRun::VP1LightRun(const std::string& name) :
  m_vp1gui(0)
{
	std::cout << "VP1LightRun - name: " << name << std::endl;
	m_noGui=false;

  // Pick up patched Coin
  ::setenv("LCGPATCH_COINMULTISELECT","1",1);
}

//____________________________________________________________________
VP1LightRun::~VP1LightRun()
{
}

//____________________________________________________________________
bool VP1LightRun::initialize()
{
  // msg(MSG::DEBUG) << " in initialize() " << endreq;
  std::cout << " in initialize() " << std::endl;




  std::vector<std::string>::iterator it, itE = m_initialvp1files.end();
  for (it = m_initialvp1files.begin();it!=itE;++it) {
  }

  // dummy objects
  StoreGateSvc* evtStore = nullptr;
  StoreGateSvc* detStore = nullptr;
  ISvcLocator*  serviceLocator = nullptr;
  IToolSvc*     toolSvc = nullptr;

  // create VP1 GUI!
  m_vp1gui = new VP1Gui( evtStore, detStore, serviceLocator, toolSvc,
			m_initialvp1files,
			m_initialCruiseMode,m_initialCruiseSeconds,
			(m_mfOn ? m_mfSourceDir : ""),
			(m_mfOn ? m_mfLocalCopyDir : ""),
			m_mfLimit,
			( m_mfOn ? m_mfAvailableLocalInputDirectories : std::vector<std::string>() ) );

#ifdef JFBCOMMENTOUT
  if (!m_vp1gui->argumentsAreValid()) {
    std::cout << "arguments not valid..." << std::endl;
    delete m_vp1gui;
    m_vp1gui = 0;
    exit(1);//Should we do this? It is kind of nice to not have to dig through the logfile to find the failure.
    return false; // StatusCode::FAILURE;
  }
  std::cout << "OK! Returning TRUE..." << std::endl;
#endif
  return true; //StatusCode::SUCCESS;
}

//____________________________________________________________________
bool VP1LightRun::execute()
{
  // msg(MSG::DEBUG) <<" in execute() " << endreq;
  std::cout <<"VP1LightRun:: in execute() " << std::endl;

  // std::cout << "m_vp1gui: " << m_vp1gui << " - m_noGui: " << m_noGui << " - m_vp1gui->hasBeenInitialised(): " << m_vp1gui->hasBeenInitialised() << std::endl;
  if (!m_vp1gui) {
    std::cout << "no vp1gui. Returning..." << std::endl;
    return false; //StatusCode::FAILURE;
  }

  if ( !m_noGui &&  !m_vp1gui->hasBeenInitialised() ) {
    std::cout << "Launching VP1...!!" << std::endl;
    m_vp1gui->init();//Launch!
  }

  // const EventInfo*  evt;
  // StatusCode status = evtStore()->retrieve(evt);
  // if(status.isSuccess()) {
  if(true) {
    // Get run/event number:
  	const uint64_t eventNumber = 0; //evt->event_ID()->event_number();
    int runNumber = 0; //evt->event_ID()->run_number();
    // msg(MSG::DEBUG) << " Got run number = " << runNumber << ", event number = " << eventNumber << endreq;
    std::cout << " Got run number = " << runNumber << ", event number = " << eventNumber << std::endl;

    // Get time stamp:
    // unsigned time = evt->event_ID()->time_stamp();//0 means no info.
    std::chrono::time_point<std::chrono::system_clock> now;
    now = std::chrono::system_clock::now();
    unsigned time =  std::chrono::duration_cast<std::chrono::seconds>( now.time_since_epoch() ).count();
    std::cout << " Got timestamp = " << time << std::endl;

    // Get L1 trigger type
    // TriggerInfo* _trig = evt->trigger_info();
    // unsigned int trigType = _trig ? _trig->level1TriggerType() : 0;
    unsigned int trigType = 0; //_trig ? _trig->level1TriggerType() : 0;

    if (m_noGui||m_vp1gui->executeNewEvent(runNumber,eventNumber,trigType,time)) {
      return true; //StatusCode::SUCCESS;
    } else {
      // msg(MSG::INFO) << " Ending application gracefully." << endreq;
      std::cout << " Ending application gracefully." << std::endl;
      return false; //StatusCode::FAILURE;
    }
  };

  // msg(MSG::WARNING) << " Unable to retrieve EventInfo from StoreGate. Skipping" << endreq;

  std::cout << "VP1LightRun::execute() returns TRUE..." << std::endl;
  return true; //StatusCode::SUCCESS;

}

//____________________________________________________________________
bool VP1LightRun::finalize()
{
  // msg(MSG::DEBUG) <<" in finalize() " << endreq;
  std::cout << " in finalize() " << std::endl;

  if (!m_vp1gui)
    return false; //StatusCode::FAILURE;

  if (!m_noGui)
    m_vp1gui->cleanup();
  delete m_vp1gui;

  return true; // StatusCode::SUCCESS;
}

//____________________________________________________________________
void VP1LightRun::handle()
{
  std::cout << " in handle() " << std::endl;

  // msg(MSG::INFO) << "Handling incident '" << inc.type() << "'" << endreq;

  if (!m_vp1gui) {
    // msg(MSG::INFO) << "Aborting due to null VP1Gui pointer." << endreq;
    std::cout << "Aborting due to null VP1Gui pointer." << std::endl;
    return;
  }

  std::vector<std::string> vect;
  // StringArrayProperty inputCollections("InputCollections", vect);
  //
  // sc = propertyServer->getProperty(&inputCollections);
  // if(!sc.isSuccess()) {
  //   msg(MSG::INFO) << "Could not get InputCollections property" << endreq;
  //   return;
  // }
  // else
  //   msg(MSG::DEBUG) << " Got InputCollections property" << endreq;
  //
  // std::vector<std::string>::const_iterator iter = inputCollections.value().begin();
  // std::vector<std::string>::const_iterator last = inputCollections.value().end();
  // for(; iter != last; iter++) {
  //   if(*iter != fileInc->fileName())
  //     vect.push_back(*iter);
  // }

  if(m_mfOn) {
    std::string strNewFileName = m_vp1gui->nextRequestedEventFile();

    if (strNewFileName.empty())
      return;
    if (!VP1FileUtilities::fileExistsAndReadable(strNewFileName)) {
      // msg(MSG::WARNING) << " File requested by VP1 does not exists or is not readable: "<<strNewFileName<<". Ending." << endreq;
      cout << " File requested by VP1 does not exists or is not readable: "<<strNewFileName<<". Ending." << endl;
      return;
    }

    vect.push_back(strNewFileName);
    // msg(MSG::INFO) << " Setting next event file: " << strNewFileName<< endreq;
    cout << " Setting next event file: " << strNewFileName<< endl;
  }
  else {
    std::vector<std::string> strNewFileNames = m_vp1gui->userRequestedFiles();
    for(unsigned i=0; i<strNewFileNames.size(); ++i) {
      const std::string& strNewFileName = strNewFileNames[i];
      if (strNewFileName.empty())
	continue;
      if (!VP1FileUtilities::fileExistsAndReadable(strNewFileName)) {
	// msg(MSG::WARNING) << " File requested by VP1 does not exists or is not readable: " << strNewFileName << endreq;
	cout << " File requested by VP1 does not exists or is not readable: " << strNewFileName << endl;
	continue;
      }
      vect.push_back(strNewFileName);
      // msg(MSG::INFO) << " Setting next event file: " << strNewFileName<< endreq;
      cout << " Setting next event file: " << strNewFileName<< endl;
    }
  }

  // StringArrayProperty newInputCollections("InputCollections", vect);
  //
  // if(propertyServer->setProperty(newInputCollections)!=StatusCode::SUCCESS)
  //   msg(MSG::WARNING) << "Could not set new InputCollections property" << endreq;
  // else
  //   msg(MSG::DEBUG) << " InputCollections property set" << endreq;
}
