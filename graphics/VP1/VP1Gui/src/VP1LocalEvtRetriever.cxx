/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1LocalEvtRetriever.h"
#include "VP1Gui/VP1AvailEvtsLocalDir.h"

#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QMutexLocker>

VP1LocalEvtRetriever::VP1LocalEvtRetriever(VP1AvailEvtsLocalDir* availEvts
					   , QString sourcedir
					   , QObject* parent)
  : QThread(parent)
  , m_availEvts(availEvts)
  , m_sourcedir(sourcedir)
  , m_timer(0)
{
}

VP1LocalEvtRetriever::~VP1LocalEvtRetriever()
{
  delete m_timer;
}

void VP1LocalEvtRetriever::run()
{
  m_timer = new QTimer(0);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(updateLocalDirFromSource()),Qt::DirectConnection);
  m_timer->start(3000);
  updateLocalDirFromSource();
  exec();
}

void VP1LocalEvtRetriever::updateLocalDirFromSource()
{
  QMutexLocker locker(&m_mutex);
  updateLocalDir();
}

void VP1LocalEvtRetriever::updateLocalDir()
{
  QList<VP1EventFile> evts =  m_availEvts->allEventFilesInDir(m_sourcedir);
  if (evts.isEmpty()) {
    m_availEvts->cleanupAndCheckForEventListChanges();//In case we e.g. changed to an empty input directory
    return;
  }
  //We don't copy more than enough to give us 3 fresh files. But if
  //there are more available, we will check back in 3 seconds.

  const int nfreshneeded = 3 - m_availEvts->freshEvents().count();
  if (evts.count()>m_availEvts->maxLocalFilesToKeep()+3&&nfreshneeded<=0) {
    //Don't copy anything, but schedule cleanup and check back in 3 seconds:
    m_availEvts->cleanupAndCheckForEventListChanges();
// --- todo?   QTimer::singleShot(3000, this, SLOT(updateLocalDirFromSource()));
    return;
  }

  static unsigned ntmpdlcount = 0;

  QList<VP1EventFile> freshEvtsInSource = m_availEvts->freshEvents(evts.front(), evts);

  QString ad = m_availEvts->tmpActiveRetrievalDir();
  QString tmplocal = m_availEvts->tmpLocalFileDir();
  bool changestmplocal(false), changesad(false);
  if (!ad.isEmpty()&&!tmplocal.isEmpty()) {
    int ncopied(0);
    foreach (VP1EventFile evt, freshEvtsInSource) {
      QString fn = evt.fileName();
      QString basefn = QFileInfo(fn).fileName();
      QString target = tmplocal + basefn;
      if (!QFile::exists(target)) {
	QString tmptarget = ad+basefn+"_"+QString::number(ntmpdlcount++);;
      	if (!QFile::copy(fn,tmptarget)) {
	  m_availEvts->message("Problems copying "+fn+" to "+tmptarget);
	  changesad = true;
	} else {
	  if (!QFile::rename(tmptarget,target)) {
	    m_availEvts->message("Problems moving "+tmptarget+" to "+target+" directory");
	    changesad = true;
	  } else {
	    if (!QFile::exists(target)) {
	      m_availEvts->message("Inconsistent copy of "+basefn+" to local tmp directory");
	    } else {
	      ++ncopied;
	      m_availEvts->messageDebug("Copied "+basefn+" to local tmp directory");//Fixme: verbose!
	      changestmplocal = true;
	    }
	  }
	}
      }
      if (ncopied>=nfreshneeded) {
// -- todo?	QTimer::singleShot(3000, this, SLOT(updateLocalDirFromSource()));
	break;
      }
    }
  } else {
    m_availEvts->message("Problems with temporary local directories.");
  }
  if (changestmplocal)
    m_availEvts->invalidateDirCache(tmplocal);
  if (changesad)
    m_availEvts->invalidateDirCache(ad);
  m_availEvts->cleanupAndCheckForEventListChanges();
}

void VP1LocalEvtRetriever::setSourceDir(QString dir)
{
  QMutexLocker locker(&m_mutex);

  if (!dir.endsWith("/"))
    dir += "/";
  if (m_sourcedir==dir)
    return;

  //Ensure that the next event is from the new directory by removing
  //all cached fresh events:

  while (true) {
    QList<VP1EventFile> freshevts = m_availEvts->freshEvents();
    if (freshevts.isEmpty())
      break;
    foreach(VP1EventFile evt, freshevts) {
      QString fn = evt.fileName();
      if (!QFile::remove(fn))
	m_availEvts->message("ERROR: Could not remove "+fn);
    }
  }

  //Now change directory
  m_sourcedir = dir;
  updateLocalDir();
}
