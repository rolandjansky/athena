/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class  VP1AvailEvtsHttps                  //
//                                                            //
//  Description:    ...                                       //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1AVAILEVTSHTTPS_H
#define VP1AVAILEVTSHTTPS_H

// include C++
//#include <stddef.h> // this to fix the 'ptrdiff_t' does not name a type error with Qt (http://qt-project.org/forums/viewthread/16992)

#include <QObject>
#include <QNetworkReply>

#include "VP1Gui/VP1AvailEvents.h"

class QNetworkAccessManager;

class VP1AvailEvtsHttps : public VP1AvailEvents
{
  Q_OBJECT

public:

  VP1AvailEvtsHttps(QString fileinfoUrl,
		    int updateInterval,
		    int timeCutForNew,
		    QString tmpcopydir,
		    int maxLocalFilesToKeep = -1,
		    QObject * parent = 0);
  virtual ~VP1AvailEvtsHttps();

  virtual void init();

  QString fileinfoLocation();

public slots:
  void start(QNetworkAccessManager* netmanager);

private slots:
  // ---- Https/SSL slots ----
  // Slots of the QNetworkReply
  void finished();
  void error(QNetworkReply::NetworkError);
  void sslErrors(const QList<QSslError>&);
  void dataReadProgress(qint64,qint64);
  void checkForStall();

  // Timer slot
  void generateHttpsRequest();

private:
  class Imp;
  Imp*  d;
};

#endif

