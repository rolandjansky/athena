/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1LocalEvtRetriever_h
#define VP1LocalEvtRetriever_h

#include <QThread>
#include <QString>
#include <QMutex>

class VP1AvailEvtsLocalDir;
class QTimer;

class VP1LocalEvtRetriever : public QThread
{
  Q_OBJECT

 public:
  VP1LocalEvtRetriever(VP1AvailEvtsLocalDir*
		       ,QString
		       ,QObject* parent = 0);
  ~VP1LocalEvtRetriever();

  void setSourceDir(QString);

 public slots:
  void updateLocalDirFromSource();

 protected:
  void run();

 private:
  void updateLocalDir(); // Called by updateLocalDirFromSource() and by setSourceDir(QString)
                         // Does the actual work. Has been introduced in order to avoid mutex deadlocks

  VP1AvailEvtsLocalDir* m_availEvts;
  QString               m_sourcedir;
  QTimer*               m_timer;
  QMutex                m_mutex;
};

#endif
