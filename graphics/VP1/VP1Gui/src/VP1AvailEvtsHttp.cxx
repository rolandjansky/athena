/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1AvailEvtsHttp                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1AvailEvtsHttp.h"
#include "VP1Gui/VP1HttpGetFile.h"
#include "VP1Gui/VP1EvtsOnServerInfo.h"
#include "VP1Gui/VP1WebWatcher.h"
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QUrl>

//____________________________________________________________________
class VP1AvailEvtsHttp::Imp {
public:
  VP1AvailEvtsHttp * theclass;
  QString fileinfoUrl;
  QString baseUrl;
  VP1HttpGetFile httpgetfile_events;
  VP1HttpGetFile httpgetfile_fileinfo;
  VP1WebWatcher * webwatcher_fileinfo;
  VP1EvtsOnServerInfo * evtsOnServer;
  QTimer * examineEvtsOnServerTimer;
  void restartExamineEvtsOnServerTimer() {
    if (!examineEvtsOnServerTimer) {
      examineEvtsOnServerTimer = new QTimer(theclass);
      connect(  examineEvtsOnServerTimer, SIGNAL(timeout()), theclass, SLOT(examineEvtsOnServer()));
    }
    examineEvtsOnServerTimer->start(3000);
  }
  void getFileInfoLater() {
    QTimer::singleShot(10000, theclass, SLOT(fileInfoChanged()));
  }
  static unsigned ntmpdlcount;
};

unsigned VP1AvailEvtsHttp::Imp::ntmpdlcount = 0;

//____________________________________________________________________
VP1AvailEvtsHttp::VP1AvailEvtsHttp( QString fileinfoUrl,
				    int updateInterval,
				    int timeCutForNew,
				    QString tmpcopydir,
				    int maxLocalFilesToKeep,
				    QObject * parent )
  : VP1AvailEvents(timeCutForNew,tmpcopydir,maxLocalFilesToKeep,parent), d(new Imp)
{
  d->theclass = this;
  d->examineEvtsOnServerTimer = 0;
  d->fileinfoUrl = fileinfoUrl;
  QUrl url(fileinfoUrl);
  if (url.isValid()) {
    QString path = url.path();
    QString infofilebasename = QFileInfo(path).fileName();
    if (!infofilebasename.isEmpty()&&path.endsWith(infofilebasename)) {
      d->baseUrl=fileinfoUrl;
      d->baseUrl.chop(infofilebasename.count());
      if (!QUrl(d->baseUrl).isValid())
	d->baseUrl="";
      else if (!d->baseUrl.endsWith("/"))
	d->baseUrl += "/";
    }
  }

  d->evtsOnServer = 0;
  connect(&(d->httpgetfile_fileinfo),SIGNAL(downloadSuccessful(const QString&,const QString&,const QString&)),
	  this,SLOT(fileInfoDownloadSuccessful(const QString&,const QString&)));
  connect(&(d->httpgetfile_fileinfo),SIGNAL(downloadFailed(const QString&,const QString&,const QString&,const QString&)),
	  this,SLOT(fileInfoDownloadFailed(const QString&,const QString&,const QString&)));
  connect(&(d->httpgetfile_events),SIGNAL(downloadSuccessful(const QString&,const QString&,const QString&)),
	  this,SLOT(eventFileDownloadSuccessful(const QString&,const QString&,const QString&)));
  connect(&(d->httpgetfile_events),SIGNAL(downloadFailed(const QString&,const QString&,const QString&,const QString&)),
	  this,SLOT(eventFileDownloadFailed(const QString&,const QString&,const QString&)));

  d->webwatcher_fileinfo = new VP1WebWatcher(updateInterval*1000,this);
  connect(d->webwatcher_fileinfo,SIGNAL(urlChanged(const QString&)),this,SLOT(fileInfoChanged()));
}

//____________________________________________________________________
void VP1AvailEvtsHttp::init()
{
  d->webwatcher_fileinfo->addUrl(d->fileinfoUrl);//A signal will be triggered the first time the webwatcher knows anything about the url.
  d->restartExamineEvtsOnServerTimer();
}

//____________________________________________________________________
VP1AvailEvtsHttp::~VP1AvailEvtsHttp()
{
  delete d->evtsOnServer;
  delete d;
}

//____________________________________________________________________
void VP1AvailEvtsHttp::fileInfoChanged()
{
  if (d->webwatcher_fileinfo->lastResult(d->fileinfoUrl)!=VP1WebWatcher::EXISTS) {
    message("Problems investigating "+d->fileinfoUrl);
    message("   ===> "+d->webwatcher_fileinfo->lastResultToString(d->fileinfoUrl));
    return;
  }
  QString ad = tmpActiveRetrievalDir();
  if (ad.isEmpty()) {
    message("Error: No temporary retrieval directory set!");
    d->getFileInfoLater();//Avoid stalling forever (however this is an unlikely error).
    return;
  }
  QString target = ad+"downloadedfileinfo.txt";
  if ( QFileInfo(target).exists() && !QFile(target).remove() ) {
    message("ERROR: Could not remove old "+target);
    d->getFileInfoLater();//Avoid stalling forever in case user fixes error (i.e. fixes permissions).
    return;
  }
  QString err = d->httpgetfile_fileinfo.startDownload( d->fileinfoUrl,target);
  if (!err.isEmpty()) {
    message("Problems starting download to get file-info file: "+err);
    d->getFileInfoLater();//Avoid stalling forever in case user fixes error (i.e. fixes net connection).
  }
}

//____________________________________________________________________
void VP1AvailEvtsHttp::fileInfoDownloadSuccessful( const QString& urltofile, const QString& localtargetfile )
{
  VP1EvtsOnServerInfo * newEvtsOnServerInfo = new VP1EvtsOnServerInfo(localtargetfile);
  if (!newEvtsOnServerInfo->isValid()) {
    message("Problems decoding info in file downloaded from "+urltofile);
    message("Error: "+newEvtsOnServerInfo->error());
    delete newEvtsOnServerInfo;
    QFile::remove(localtargetfile);
    d->getFileInfoLater();//Avoid stalling forever in case this is temporary.
    invalidateDirCache(tmpActiveRetrievalDir());
    return;
  }
  delete d->evtsOnServer;
  d->evtsOnServer = newEvtsOnServerInfo;

  examineEvtsOnServer();
}

//____________________________________________________________________
void VP1AvailEvtsHttp::examineEvtsOnServer()
{
  d->restartExamineEvtsOnServerTimer();//To ensure we don't get events too closely spaced here.
  if (d->httpgetfile_events.numberOfPendingDownloads()>3)
    return;
  if (!d->evtsOnServer)
    return;

  //Start download of one of the files from the server?
  VP1EventFile evtToGet;
  QString localfiledir = tmpLocalFileDir();
  if (localfiledir.isEmpty()) {
    message("Problems with temporary local event file directory.");
    return;
  }

  foreach (VP1EventFile evt, d->evtsOnServer->events(timeCutForNew(), requireNewestRunNumber() )) {
    //We are looking for an event which was never seen before and not available locally:
    if (!evt.isValid()||inHistory(evt.runNumber(),evt.eventNumber()))
      continue;
    if ( !QFile::exists ( localfiledir+evt.fileName() ) ) {
      evtToGet=evt;
      break;
    }
  }

  if (!evtToGet.isValid()&&d->examineEvtsOnServerTimer)
    d->examineEvtsOnServerTimer->stop();//No need to check back regularly until we get new information.

  if (evtToGet.isValid()) {
    //Before we get it, let us check that we don't already have 3
    //fresh events locally which are newer than the one we are trying
    //to download:
    QList<VP1EventFile> freshLocalEvents = freshEvents();
    unsigned nNewer(0);
    foreach(VP1EventFile evt,freshLocalEvents) {
      if (evt < evtToGet) {
	++nNewer;
	if (nNewer>=3) {
	  return;
	}
      }
    }
    QString ad = tmpActiveRetrievalDir();
    if (!ad.isEmpty()) {
      QString url = d->baseUrl+evtToGet.fileName();
      QString target = ad+evtToGet.fileName()+"_"+QString::number(Imp::ntmpdlcount++);
      //A quick check that we are not already downloading that file currently:
      if (!d->httpgetfile_events.isDownloading(url)&&!d->httpgetfile_events.isDownloadingTo(target)) {
	QString err = d->httpgetfile_events.startDownload( url,target,evtToGet.md5Sum(),localfiledir+evtToGet.fileName());
	if (!err.isEmpty()) {
	  message("Problems starting download of :" +url);
	  message("   => "+err);
	}
      } else {
	//We are already downloading url - abort silently.
	return;
      }
    } else {
      message("Problems with temporary local download directory.");
    }
  }
  invalidateDirCache(tmpActiveRetrievalDir());//Fixme: remove from here???
}


//____________________________________________________________________
void VP1AvailEvtsHttp::fileInfoDownloadFailed( const QString& error, const QString& urltofile,const QString& localtargetfile )
{
  message("Problems downloading "+urltofile+": "+error);
  QFile::remove(localtargetfile);
  invalidateDirCache(tmpActiveRetrievalDir());
  d->getFileInfoLater();//Avoid stalling forever in case this is temporary.
}

//____________________________________________________________________
void VP1AvailEvtsHttp::eventFileDownloadSuccessful( const QString&, const QString& localtargetfile, const QString& data )
{
  invalidateDirCache(tmpLocalFileDir());
  invalidateDirCache(tmpActiveRetrievalDir());

  if (!QFile::rename(localtargetfile,data)) {
    message("Error: Could not move "+localtargetfile+" to "+data);
    QFile::remove(localtargetfile);
    QFile::remove(data);
    d->getFileInfoLater();//Avoid stalling forever in case this is temporary.
    return;
  }
  cleanupAndCheckForEventListChanges();
}

//____________________________________________________________________
void VP1AvailEvtsHttp::eventFileDownloadFailed( const QString& error, const QString& urltofile,const QString& localtargetfile )
{
  message("Problems downloading "+urltofile+": "+error);
  QFile::remove(localtargetfile);
  d->getFileInfoLater();//Avoid stalling forever in case this is temporary.
  invalidateDirCache(tmpActiveRetrievalDir());
}
