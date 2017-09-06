/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1StreamMenuUpdater_h
#define VP1StreamMenuUpdater_h

#include "VP1Gui/VP1MainWindow.h"
#include <QThread>

class QMutex;
class QTimer;

class VP1StreamMenuUpdater : public QThread
{
  Q_OBJECT

 public:
  VP1StreamMenuUpdater(VP1DirStatuses&
		       ,QMutex*
		       ,QObject* parent = 0);

  ~VP1StreamMenuUpdater();

 public slots:
  void updateInputDirectoriesStatus();

 protected:
  void run();

 private:
  VP1DirStatuses& m_inputdiractions;
  QMutex* m_mutex;
  QTimer* m_timer;
};

#endif
