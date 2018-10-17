/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1StreamMenuUpdater.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QTimer>
#include <QDateTime>
#include <QMutexLocker>

VP1StreamMenuUpdater::VP1StreamMenuUpdater(VP1DirStatuses& diractions
					   ,QMutex* mutex
					   ,QObject* parent)
  : QThread(parent)
  , m_inputdiractions(diractions)
  , m_mutex(mutex)
  , m_timer(0)
{
}

VP1StreamMenuUpdater::~VP1StreamMenuUpdater()
{
  delete m_timer;
}

void VP1StreamMenuUpdater::run()
{
  updateInputDirectoriesStatus();
  m_timer = new QTimer(0);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(updateInputDirectoriesStatus()),Qt::DirectConnection);
  m_timer->start(5000);
  exec();
}

void VP1StreamMenuUpdater::updateInputDirectoriesStatus()
{
  QMutexLocker locker(m_mutex);

  foreach (QAction* act,m_inputdiractions.keys()) {
    VP1DirStatusData& dirstatus = m_inputdiractions[act];
    dirstatus.enabled = true;
    dirstatus.bold = false;
    dirstatus.dirStatus = "unknown";

    // Check whether it exists and is really a directory
    QFileInfo fi(dirstatus.inputDir);
    if (!fi.exists()||!fi.isDir()) {
      dirstatus.enabled = false;
      dirstatus.dirStatus = "not found";
    } else {
      // Access the latest_vp1event file in order to get the name of the latest vp1 event file
      // in the directory
      QFile latestFileRef(dirstatus.inputDir + "/latest_vp1event");
      if(latestFileRef.exists()
	 && latestFileRef.open(QIODevice::ReadOnly | QIODevice::Text)) {
	QTextStream textStream(&latestFileRef);
	QString latestFileName = textStream.readLine();
	latestFileRef.close();

	QFileInfo latestFile(latestFileName);
	if(latestFile.exists()) {
	  int secondswithoutupdate = latestFile.lastModified().secsTo(QDateTime::currentDateTime());
	  dirstatus.bold = (secondswithoutupdate<60);
	  if (secondswithoutupdate<120)
	    dirstatus.dirStatus = QString::number(secondswithoutupdate)+" seconds since update";
	  else if (secondswithoutupdate<7200)
	    dirstatus.dirStatus = QString::number(static_cast<int>(secondswithoutupdate/60.0+0.5))+" minutes since update";
	  else
	    dirstatus.dirStatus = QString::number(static_cast<int>(secondswithoutupdate/3600.0+0.5))+" hours since update";
	}
      }
    }
  }
}
