/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1Gui/VP1AvailEvtsHttps.h"
#include "VP1Gui/VP1EvtsOnServerInfo.h"
#include "VP1Gui/VP1EventFile.h"
#include "VP1Gui/VP1MD5Sum.h"
#include "VP1Base/VP1Msg.h"

#include <QNetworkAccessManager>
#include <QSslError>
#include <QNetworkCookie>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QTimer>
#include <QBuffer>
#include <QStringMatcher>
#include <QDateTime>

#include <iostream>
#include <QMetaType>

/*
  Stage values:
  0 - get head and check its last_modified header
  1 - get file info
  2 - get event file(s)
 */

class VP1AvailEvtsHttps::Imp {
 public:
  Imp(VP1AvailEvtsHttps*
      ,QString
      ,int);
  ~Imp();  

  void startTimer(int interval=0);

  // Connect signals of the QNetworkReply to slots of the VP1AvailEvtsHttps
  void connectNetworkSignalsToSlots();

  VP1AvailEvtsHttps* m_theclass;
  QString m_fileInfoUrl;
  QString m_baseUrl;
  int m_updateInterval;

  QNetworkAccessManager* m_netmanager;
  QNetworkReply* m_netreply;
  QString m_evtToGet;
  int m_stage;
  QString m_urlLastMod;
  QString m_expectedMD5Sum;
  qint64 m_bytesReceived;
  unsigned m_lastChangeTime;

  QTimer m_stallTimer;
};

VP1AvailEvtsHttps::Imp::Imp(VP1AvailEvtsHttps* theclass
			    ,QString fileInfoUrl
			    ,int updateInterval)
  : m_theclass(theclass)
  , m_fileInfoUrl(fileInfoUrl)
  , m_baseUrl("")
  , m_updateInterval(updateInterval)
  , m_netmanager(0)
  , m_netreply(0)
  , m_evtToGet("")
  , m_stage(0)
  , m_urlLastMod("UNKNOWN")
  , m_expectedMD5Sum("")
  , m_bytesReceived(0)
  , m_lastChangeTime(0)
  , m_stallTimer(0)
{
  QUrl url(m_fileInfoUrl);
  if (url.isValid()) {
    QString path = url.path();
    QString infofilebasename = QFileInfo(path).fileName();
    if (!infofilebasename.isEmpty()&&path.endsWith(infofilebasename)) {
      m_baseUrl=m_fileInfoUrl;
      m_baseUrl.chop(infofilebasename.count());
      if (!QUrl(m_baseUrl).isValid())
	m_baseUrl="";
      else if (!m_baseUrl.endsWith("/"))
	m_baseUrl += "/";
    }
  }

  connect(&m_stallTimer,SIGNAL(timeout()), m_theclass, SLOT(checkForStall()));
  m_stallTimer.start(10000);
}

VP1AvailEvtsHttps::Imp::~Imp()
{
}

void VP1AvailEvtsHttps::Imp::startTimer(int interval)
{
  QTimer::singleShot((interval>0 ? m_updateInterval : 60) ,m_theclass,SLOT(generateHttpsRequest()));
}

void VP1AvailEvtsHttps::Imp::connectNetworkSignalsToSlots()
{
  connect(m_netreply, SIGNAL(finished()),
	  m_theclass, SLOT(finished()));
  connect(m_netreply, SIGNAL(error(QNetworkReply::NetworkError)),
	  m_theclass, SLOT(error(QNetworkReply::NetworkError)));
  connect(m_netreply, SIGNAL(sslErrors(const QList<QSslError>&)),
	  m_theclass, SLOT(sslErrors(const QList<QSslError>&)));
  connect(m_netreply, SIGNAL(downloadProgress(qint64,qint64)),
	  m_theclass, SLOT(dataReadProgress(qint64,qint64)));
  m_bytesReceived = 0;
}

VP1AvailEvtsHttps::VP1AvailEvtsHttps(QString fileinfoUrl,
				     int updateInterval,
				     int timeCutForNew,
				     QString tmpcopydir,
				     int maxLocalFilesToKeep,
				     QObject * parent)
  : VP1AvailEvents(timeCutForNew,tmpcopydir,maxLocalFilesToKeep,parent)
  , d(new Imp(this,fileinfoUrl,updateInterval))
{
}

VP1AvailEvtsHttps::~VP1AvailEvtsHttps()
{
  delete d;
}

void VP1AvailEvtsHttps::init()
{
  // Dummy for now
}
 
void VP1AvailEvtsHttps::start(QNetworkAccessManager* netmanager)
{
  d->m_netmanager = netmanager;
  d->startTimer();
}

QString VP1AvailEvtsHttps::fileinfoLocation()
{
  return d->m_fileInfoUrl;
}

void VP1AvailEvtsHttps::generateHttpsRequest()
{
  QUrl fileInfoUrl(d->m_fileInfoUrl);
  QNetworkRequest netrequest(fileInfoUrl);
  if(d->m_stage==0) {
    /** ---- For logging
    std::cout << "VP1AvailEvtsHttps getHead stage -- " << d->m_stage << " --" << std::endl;
    **/
    d->m_netreply = d->m_netmanager->head(netrequest);
  } else if(d->m_stage==1) {
    /** ---- For logging
    std::cout << "VP1AvailEvtsHttps get stage -- " << d->m_stage << " --" << std::endl;
    **/
    d->m_netreply = d->m_netmanager->get(netrequest);
  } else {
    // stage 2
    /** ---- For logging
    std::cout << "VP1AvailEvtsHttps get stage -- " << d->m_stage << " --" << std::endl;
    **/

    QString activeRetrievalDir = tmpActiveRetrievalDir();
    QString target = activeRetrievalDir +"downloadedfileinfo.txt";
    VP1EvtsOnServerInfo newEvtsOnServerInfo(target);

    if (!newEvtsOnServerInfo.isValid()) {
      message("Problems decoding info in file downloaded from " + d->m_fileInfoUrl);
      QFile::remove(target);
      invalidateDirCache(tmpActiveRetrievalDir());
      d->m_stage = 0;
      d->startTimer(d->m_updateInterval);
      return;
    }

    VP1EventFile evtToGet;
    QString localfiledir = tmpLocalFileDir();

    foreach (VP1EventFile evt, newEvtsOnServerInfo.events(timeCutForNew(),requireNewestRunNumber())) {
      //We are looking for an event which was never seen before and not available locally:
      if (!evt.isValid()||inHistory(evt.runNumber(),evt.eventNumber()))
	continue;
      if (!QFile::exists(localfiledir+evt.fileName())){
	evtToGet=evt;
	break;
      }
    }

    if(evtToGet.isValid()) {
      //Before we get it, let us check that we don't already have 3
      //fresh events locally which are newer than the one we are trying
      //to download:
      QList<VP1EventFile> freshLocalEvents = freshEvents();
      unsigned nNewer(0);
      foreach(VP1EventFile evt,freshLocalEvents) {
	if (evt < evtToGet) {
	  ++nNewer;
	  if (nNewer>=3) {
	    d->m_stage = 0;
	    d->startTimer(d->m_updateInterval);
	    return;
	  }
	}
      }

      QUrl url(d->m_baseUrl + evtToGet.fileName());
      d->m_expectedMD5Sum = evtToGet.md5Sum();
      /** ---- For logging
      std::cout << "         Event : " << url.toString().toStdString() << std::endl; 
      **/
      QNetworkRequest netrequestEvt(url);
      d->m_evtToGet = evtToGet.fileName();
      d->m_netreply = d->m_netmanager->get(netrequestEvt);
    } else {
      d->m_stage = 0;
      d->startTimer(d->m_updateInterval);
      return;
    }
  }
  d->connectNetworkSignalsToSlots();
}

void VP1AvailEvtsHttps::finished()
{
	VP1Msg::messageDebug("VP1AvailEvtsHttps::finished()");
  int sc=-9999;
  QVariant val = d->m_netreply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if(val.type()==QVariant::Int) 
    sc = val.value<int>();
  else if(val.type()==QVariant::Invalid)
    message("No status code obtained while processing " + d->m_netreply->url().toString());
  else if(!val.canConvert<int>())
    message("Cannot convert status code to int while processing " + d->m_netreply->url().toString());
  else 
    sc = val.value<int>();

	VP1Msg::messageDebug("sc: " + QString::number(sc));

  QString lastModified;
  QVariant lastModHeader = d->m_netreply->header(QNetworkRequest::LastModifiedHeader);
  if(lastModHeader.type()!=QVariant::Invalid && 
     lastModHeader.canConvert<QDateTime>()) {
    QDateTime lastModTime = lastModHeader.value<QDateTime>();
    lastModified = lastModTime.toString();
  }

  /** ---- For logging
  std::cout << "VP1AvailEvtsHttps stage -- " << d->m_stage
	    << " -- finished with error=" << (int)d->m_netreply->error() 
	    << ", sc=" << sc 
	    << ", LM=" << lastModified.toStdString() << std::endl;
  **/

  if(d->m_netreply->error()==QNetworkReply::NoError) {
    if(d->m_stage==0) {
      if(!lastModified.isEmpty() && lastModified!=d->m_urlLastMod) {
	// The file info has been modified, go to the stage 1
	d->m_urlLastMod = lastModified;
	d->m_stage = 1; 
      } else {
	// Reuse the already downloaded file info
	d->m_stage = 2;
      }
    }else if(d->m_stage==1) {
      // Write out fileinfo to local file for parsing
      QString activeRetrievalDir = tmpActiveRetrievalDir();
      QString target = activeRetrievalDir +"downloadedfileinfo.txt";
      /** ---- For logging
      std::cout << "  Active Ret Dir: " << activeRetrievalDir.toStdString() << std::endl;
      std::cout << "  Target        : " << target.toStdString() << std::endl;
      **/

      // Delete already existing fileinfo 
      if(QFileInfo(target).exists() && !QFile(target).remove()) {
	message("ERROR: Could not remove the old fileinfo " + target);
	d->m_stage = 0;
	d->startTimer(d->m_updateInterval);
	return;
      }

      // Create new fileinfo
      QFile localfileinfo(target);
      if(!localfileinfo.open(QIODevice::WriteOnly)) {
	message("ERROR: Unable to open " + target + " for writing");
	d->m_stage = 0;
	d->startTimer(d->m_updateInterval);
	return;
      }

      // Wrute received fileinfo to local file
      QByteArray ba = d->m_netreply->readAll();
      localfileinfo.write(ba);
      localfileinfo.close();
      d->m_stage = 2;
    }else if(d->m_stage==2) {
      // Open target file for writing
      QString target = tmpActiveRetrievalDir() + d->m_evtToGet;
      QFile targetFile(target);
      if(!targetFile.open(QIODevice::WriteOnly)) {
	message("ERROR: Unable to open " + target + " for writing");
	d->m_stage = 0;
	d->startTimer(d->m_updateInterval);
	return;
      }

      // Write to file
      QByteArray ba = d->m_netreply->readAll();
      targetFile.write(ba);
      targetFile.close();
      /** ---- For logging
      std::cout << "\t\t" <<  target.toStdString() << " written and closed" << std::endl;
      **/

      // Checksum test
      bool match = VP1MD5Sum::sumMatches(target,d->m_expectedMD5Sum);
      if(!match) {
	message("Checksum did not match");
	QFile::remove(target);
	d->m_stage = 0;
	d->startTimer(d->m_updateInterval);
	return;
      }

      QString finalTarget = tmpLocalFileDir()+d->m_evtToGet;
      if(!QFile::rename(target,finalTarget)) {
	message("ERROR: Could not move " + target + " to " + finalTarget);
	QFile::remove(target);
	QFile::remove(finalTarget);
      } else {
	cleanupAndCheckForEventListChanges();
      }
      
      d->m_stage = 0;
    }
  }
  else
    d->m_stage = 0;

  d->m_netreply->blockSignals(true);
  d->m_netreply->deleteLater();
  d->m_netreply = 0;

  int interval = d->m_stage>0 ? 0 : d->m_updateInterval;
  d->startTimer(interval);
}

void VP1AvailEvtsHttps::error(QNetworkReply::NetworkError err)
{
  message("Error processing " + d->m_netreply->url().toString() 
	  + "\n   ===> Error code: " + QString::number((int)err)
	  + "\n        Error decoding here: http://doc.trolltech.com/4.4/qnetworkreply.html#NetworkError-enum");
}

void VP1AvailEvtsHttps::sslErrors(const QList<QSslError>&) 
{
  /** ---- For logging
  std::cout << "VP1AvailEvtsHttps SSL errors" << std::endl;
  for(int ii=0; ii<errlist.size(); ++ii)
    std::cout << "   SSL * " << (int)errlist.at(ii).error()
	      << ", " << errlist.at(ii).errorString().toStdString() << std::endl;
  **/
  d->m_netreply->ignoreSslErrors();
}

void VP1AvailEvtsHttps::dataReadProgress(qint64 received, qint64)
{
  if(d->m_stage>0) {
    if(received>d->m_bytesReceived) {
      d->m_lastChangeTime = QDateTime::currentDateTime().toTime_t();
      d->m_bytesReceived = received;
    }
  } else {
    d->m_lastChangeTime = QDateTime::currentDateTime().toTime_t();
  }
}

void VP1AvailEvtsHttps::checkForStall()
{
  unsigned currentTime = QDateTime::currentDateTime().toTime_t();
  if (currentTime>d->m_lastChangeTime+10) {
    // Abort current download and go to stage 0
    if(d->m_netreply) {
      d->m_netreply->blockSignals(true);
      delete d->m_netreply;
      d->m_netreply=0;
      d->m_stage = 0;
      d->startTimer(d->m_updateInterval);
    }
  }
}
