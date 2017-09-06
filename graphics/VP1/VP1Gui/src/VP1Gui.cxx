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
 : d(new Imp)
{
  d->sg = sg;
  d->detstore = detstore;
  d->svclocator = svclocator;
  d->toolSvc = toolSvc;
  d->initialCruiseMode = QString(initialCruiseMode.c_str()).simplified();
  d->initialCruiseSeconds = initialCruiseSeconds;
  d->singleEventSource = singleEventSource.c_str();
  d->singleEventLocalTmpDir = singleEventLocalTmpDir.c_str();
  d->localFileCacheLimit = localFileCacheLimit;

  std::vector<std::string>::const_iterator it, itE = initialvp1files.end();
  for (it = initialvp1files.begin();it!=itE;++it)
    d->initialvp1files << it->c_str();

  itE = availableLocalInputDirectories.end();
  for (it = availableLocalInputDirectories.begin();it!=itE;++it)
    d->availableLocalInputDirectories << it->c_str();

}

//____________________________________________________________________
VP1Gui::~VP1Gui()
{
  delete d; d=0;
}

//____________________________________________________________________
bool VP1Gui::argumentsAreValid() const
{
  //Athena pointers:

  if (!d->sg) {
    VP1Msg::message("ERROR: Null pointer to event store.");
    return false;
  }
  if (!d->detstore) {
    VP1Msg::message("ERROR: Null pointer to detector store.");
    return false;
  }
  if (!d->svclocator) {
    VP1Msg::message("ERROR: Null pointer to service locator.");
    return false;
  }
  if (!d->toolSvc) {
    VP1Msg::message("ERROR: Null pointer to tool service.");
    return false;
  }

  //Initial files:

  // ...no checks...

  //Cruise mode:
  if (d->initialCruiseMode!="NONE"&&d->initialCruiseMode!="EVENT"&&d->initialCruiseMode!="TAB"&&d->initialCruiseMode!="BOTH") {
      VP1Msg::message("WARNING: unknown initial cruise mode "+d->initialCruiseMode+" (valid are NONE/EVENT/TAB/BOTH). Assuming NONE.");
      d->initialCruiseMode = "NONE";
  }

  //Single-Event-Per-File modes:
  if (!d->singleEventSource.isEmpty()&&!d->singleEventLocalTmpDir.isEmpty()) {
    const bool httpmode = d->singleEventSource.startsWith("http://") || d->singleEventSource.startsWith("https://") ;
    if (httpmode) {
      QUrl url(d->singleEventSource);
      if (!url.isValid()||url.path().isEmpty()) {
	//Fixme: check that path ends with a filename!
	VP1Msg::message("VP1ExecutionScheduler::init ERROR: Source URL ("+d->singleEventSource+") incorrect format!");
	return false;
      }

      // For the https mode check if the platform supports SSL
      if(d->singleEventSource.startsWith("https://") 
	 && !QSslSocket::supportsSsl()) {
	VP1Msg::message("VP1ExecutionScheduler::init ERROR: Unable to retrieve events over Https. The platform does not support SSL");
	return false;
      }
    } else {
      QFileInfo fi_s(d->singleEventSource), fi_l(d->singleEventLocalTmpDir);
      if (!fi_s.exists()||!fi_s.isDir()||!fi_s.isReadable()) {
	VP1Msg::message("ERROR: Source directory ("+d->singleEventSource+") does not exist, is not readable, or is not a directory!");
	return false;
      }
      if (!fi_l.exists()||!fi_l.isDir()||!fi_l.isReadable()||!fi_l.isWritable()) {
	VP1Msg::message("ERROR: Local tmp directory ("+d->singleEventLocalTmpDir+") does not exist, had incorrect permissions, or is not a directory!");
	return false;
      }
      if (fi_l.canonicalFilePath()==fi_s.canonicalFilePath()) {
	VP1Msg::message("ERROR: Local tmp directory ("+d->singleEventLocalTmpDir+") is the same the source directory ("+d->singleEventSource+")");
	return false;
      }
    }
  } else {
    if (!d->singleEventSource.isEmpty()||!d->singleEventLocalTmpDir.isEmpty()) {
      VP1Msg::message("ERROR: Only one of SourceDir and LocalCopyDir set. Set either both or none.");
      return false;
    }
  }

  if (d->localFileCacheLimit==1||d->localFileCacheLimit==2) {
      VP1Msg::message("WARNING: Too low value of localFileCacheLimit. Setting to 3. (Set to 0 to disable removal of locally cached files.");
      d->localFileCacheLimit=3;
  }

  return true;
}

//____________________________________________________________________
bool VP1Gui::hasBeenInitialised()
{
  return d->the_scheduler != 0;
}

//____________________________________________________________________
void VP1Gui::init()
{
  VP1Msg::messageDebug("Start of VP1Gui::init()...");

  assert(!d->the_scheduler&&"Dont init twice!!");
  VP1Msg::message("");
  VP1Msg::message("===================================================");
  VP1Msg::message("               Launching the VP1 GUI");
  VP1Msg::message("===================================================");
  VP1Msg::message("");
  d->the_scheduler = VP1ExecutionScheduler::init(d->sg,d->detstore,d->svclocator,d->toolSvc,
						 d->initialvp1files,
						 d->initialCruiseMode,d->initialCruiseSeconds,
						 d->singleEventSource,d->singleEventLocalTmpDir,
						 d->localFileCacheLimit,
						 d->availableLocalInputDirectories);

  VP1Msg::messageDebug("end of VP1Gui::init().");
}

//____________________________________________________________________
void VP1Gui::cleanup()
{
  assert(d->the_scheduler&&"Cant cleanup before init");
  VP1ExecutionScheduler::cleanup(d->the_scheduler);
}

//____________________________________________________________________
bool VP1Gui::executeNewEvent( const int& run, const uint64_t& event, const unsigned& triggerType, const unsigned& time )
{
	//tests
	int evN = event;
	qulonglong evNqu = event;

	qDebug() << "tests:" << QString::number(event) << QString::number(evN) << QString::number(evNqu);

  VP1Msg::messageDebug("Examining new event ( run# "+QString::number(run)+", event# "+QString::number(event)+" )");

  bool b = d->the_scheduler->executeNewEvent(run,event,triggerType,time);

  // NOTE:
  // we will go on to the lines here below
  // only when the user clicks on the "Next event" button.

  VP1Msg::messageDebug("Leaving event ( run# "+QString::number(run)+", event# "+QString::number(event)+" )"
		       +(nextRequestedEventFile().empty()?QString(""):". Next requested event: "+QString(nextRequestedEventFile().c_str())));

  VP1Msg::messageDebug("end of VP1Gui::executeNewEvent().");

  return b;
}

//____________________________________________________________________
std::string VP1Gui::nextRequestedEventFile() const
{
  return d->the_scheduler->nextRequestedEventFile().toStdString();
}

//____________________________________________________________________
std::vector<std::string> VP1Gui::userRequestedFiles()
{
  std::vector<std::string> retvector;
  if(d->the_scheduler) {
    const QStringList& inplist = d->the_scheduler->userRequestedFiles();
    for(int i=0; i<inplist.size(); ++i) 
      retvector.push_back(inplist.at(i).toStdString());
  }
  return retvector;
}
