/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1Gui                         //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Gui/VP1Gui.h"
#include "VP1Gui/VP1ExecutionScheduler.h"
#include "VP1Base/VP1Msg.h"

#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QSslSocket>

#include <cassert>
#include <iostream>

//____________________________________________________________________
class VP1Gui::Imp {
public:
  Imp() : the_scheduler(nullptr), sg(nullptr), detstore(nullptr),
    svclocator(nullptr), toolSvc(nullptr),
    initialvp1files{},
    initialCruiseSeconds{},
    localFileCacheLimit{}{
    //nop
  }
  VP1ExecutionScheduler * the_scheduler;

  //We hold the arguments here until init is called:
  StoreGateSvc * sg;
  StoreGateSvc * detstore;
  ISvcLocator * svclocator;
  IToolSvc * toolSvc;
  QStringList initialvp1files;
  QString initialCruiseMode;
  unsigned initialCruiseSeconds;
  QString singleEventSource;
  QString singleEventLocalTmpDir;
  unsigned localFileCacheLimit;
  QStringList availableLocalInputDirectories;
};


//____________________________________________________________________
VP1Gui::VP1Gui(StoreGateSvc* sg,StoreGateSvc* detstore,
	       ISvcLocator* svclocator,IToolSvc*toolSvc,
	       const std::vector<std::string>& initialvp1files,
	       const std::string& initialCruiseMode, unsigned initialCruiseSeconds,
	       const std::string& singleEventSource, const std::string& singleEventLocalTmpDir,
	       unsigned localFileCacheLimit,
	       const std::vector<std::string>& availableLocalInputDirectories )
 : m_d(new Imp)
{
  m_d->sg = sg;
  m_d->detstore = detstore;
  m_d->svclocator = svclocator;
  m_d->toolSvc = toolSvc;
  m_d->initialCruiseMode = QString(initialCruiseMode.c_str()).simplified();
  m_d->initialCruiseSeconds = initialCruiseSeconds;
  m_d->singleEventSource = singleEventSource.c_str();
  m_d->singleEventLocalTmpDir = singleEventLocalTmpDir.c_str();
  m_d->localFileCacheLimit = localFileCacheLimit;

  std::vector<std::string>::const_iterator it, itE = initialvp1files.end();
  for (it = initialvp1files.begin();it!=itE;++it)
    m_d->initialvp1files << it->c_str();

  itE = availableLocalInputDirectories.end();
  for (it = availableLocalInputDirectories.begin();it!=itE;++it)
    m_d->availableLocalInputDirectories << it->c_str();

}

//____________________________________________________________________
VP1Gui::~VP1Gui()
{
  delete m_d; m_d=0;
}

//____________________________________________________________________
bool VP1Gui::argumentsAreValid() const
{
  //Athena pointers:

  if (!m_d->sg) {
    VP1Msg::message("ERROR: Null pointer to event store.");
    return false;
  }
  if (!m_d->detstore) {
    VP1Msg::message("ERROR: Null pointer to detector store.");
    return false;
  }
  if (!m_d->svclocator) {
    VP1Msg::message("ERROR: Null pointer to service locator.");
    return false;
  }
  if (!m_d->toolSvc) {
    VP1Msg::message("ERROR: Null pointer to tool service.");
    return false;
  }

  //Initial files:

  // ...no checks...

  //Cruise mode:
  if (m_d->initialCruiseMode!="NONE"&&m_d->initialCruiseMode!="EVENT"&&m_d->initialCruiseMode!="TAB"&&m_d->initialCruiseMode!="BOTH") {
      VP1Msg::message("WARNING: unknown initial cruise mode "+m_d->initialCruiseMode+" (valid are NONE/EVENT/TAB/BOTH). Assuming NONE.");
      m_d->initialCruiseMode = "NONE";
  }

  //Single-Event-Per-File modes:
  if (!m_d->singleEventSource.isEmpty()&&!m_d->singleEventLocalTmpDir.isEmpty()) {
    const bool httpmode = m_d->singleEventSource.startsWith("http://") || m_d->singleEventSource.startsWith("https://") ;
    if (httpmode) {
      QUrl url(m_d->singleEventSource);
      if (!url.isValid()||url.path().isEmpty()) {
	//Fixme: check that path ends with a filename!
	VP1Msg::message("VP1ExecutionScheduler::init ERROR: Source URL ("+m_d->singleEventSource+") incorrect format!");
	return false;
      }

      // For the https mode check if the platform supports SSL
      if(m_d->singleEventSource.startsWith("https://") 
	 && !QSslSocket::supportsSsl()) {
	VP1Msg::message("VP1ExecutionScheduler::init ERROR: Unable to retrieve events over Https. The platform does not support SSL");
	return false;
      }
    } else {
      QFileInfo fi_s(m_d->singleEventSource), fi_l(m_d->singleEventLocalTmpDir);
      if (!fi_s.exists()||!fi_s.isDir()||!fi_s.isReadable()) {
	VP1Msg::message("ERROR: Source directory ("+m_d->singleEventSource+") does not exist, is not readable, or is not a directory!");
	return false;
      }
      if (!fi_l.exists()||!fi_l.isDir()||!fi_l.isReadable()||!fi_l.isWritable()) {
	VP1Msg::message("ERROR: Local tmp directory ("+m_d->singleEventLocalTmpDir+") does not exist, had incorrect permissions, or is not a directory!");
	return false;
      }
      if (fi_l.canonicalFilePath()==fi_s.canonicalFilePath()) {
	VP1Msg::message("ERROR: Local tmp directory ("+m_d->singleEventLocalTmpDir+") is the same the source directory ("+m_d->singleEventSource+")");
	return false;
      }
    }
  } else {
    if (!m_d->singleEventSource.isEmpty()||!m_d->singleEventLocalTmpDir.isEmpty()) {
      VP1Msg::message("ERROR: Only one of SourceDir and LocalCopyDir set. Set either both or none.");
      return false;
    }
  }

  if (m_d->localFileCacheLimit==1||m_d->localFileCacheLimit==2) {
      VP1Msg::message("WARNING: Too low value of localFileCacheLimit. Setting to 3. (Set to 0 to disable removal of locally cached files.");
      m_d->localFileCacheLimit=3;
  }

  return true;
}

//____________________________________________________________________
bool VP1Gui::hasBeenInitialised()
{
  return m_d->the_scheduler != 0;
}

//____________________________________________________________________
void VP1Gui::init()
{
  VP1Msg::messageDebug("Start of VP1Gui::init()...");

  assert(!m_d->the_scheduler&&"Dont init twice!!");
  VP1Msg::message("");
  VP1Msg::message("===================================================");
  VP1Msg::message("               Launching the VP1 GUI");
  VP1Msg::message("===================================================");
  VP1Msg::message("");
  m_d->the_scheduler = VP1ExecutionScheduler::init(m_d->sg,
		                 m_d->detstore,
						 m_d->svclocator,
						 m_d->toolSvc,
						 m_d->initialvp1files,
						 m_d->initialCruiseMode,m_d->initialCruiseSeconds,
						 m_d->singleEventSource,m_d->singleEventLocalTmpDir,
						 m_d->localFileCacheLimit,
						 m_d->availableLocalInputDirectories);

  VP1Msg::messageDebug("end of VP1Gui::init().");
}

//____________________________________________________________________
void VP1Gui::cleanup()
{
  assert(m_d->the_scheduler&&"Cant cleanup before init");
  VP1ExecutionScheduler::cleanup(m_d->the_scheduler);
}

//____________________________________________________________________
bool VP1Gui::executeNewEvent( const int& run, const uint64_t& event, const unsigned& triggerType, const unsigned& time )
{
  VP1Msg::messageDebug("Examining new event ( run# "+QString::number(run)+", event# "+QString::number(event)+" )");



  bool b = m_d->the_scheduler->executeNewEvent(run,event,triggerType,time);

  // NOTE:
  // we will go on to the lines here below
  // only when the user clicks on the "Next event" button.

  VP1Msg::messageDebug("Leaving event ( run# "+QString::number(run)+", event# "+QString::number(event)+" )"
		       +(nextRequestedEventFile().empty()?QString(""):". Next requested event file: "+QString(nextRequestedEventFile().c_str())));

  VP1Msg::messageDebug("end of VP1Gui::executeNewEvent().");

  return b;
}

//____________________________________________________________________
std::string VP1Gui::nextRequestedEventFile() const
{
  return m_d->the_scheduler->nextRequestedEventFile().toStdString();
}

//____________________________________________________________________
std::vector<std::string> VP1Gui::userRequestedFiles()
{
  std::vector<std::string> retvector;
  if(m_d->the_scheduler) {
    const QStringList& inplist = m_d->the_scheduler->userRequestedFiles();
    for(int i=0; i<inplist.size(); ++i) 
      retvector.push_back(inplist.at(i).toStdString());
  }
  return retvector;
}
