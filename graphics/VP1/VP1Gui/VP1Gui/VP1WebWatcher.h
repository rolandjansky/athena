/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1WebWatcher                       //
//                                                            //
//  Description: Http analogy of QFileSystemWatcher, which    //
//               uses http HEAD requests (in a subthread) at  //
//               regular intervals to monitor for changes     //
//               in files located on a webserver.             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1WEBWATCHER_H
#define VP1WEBWATCHER_H

#include <QObject>
#include <QStringList>


class QDateTime;

class VP1WebWatcher : public QObject {

  Q_OBJECT

public:

  VP1WebWatcher( int recheckInterval_ms = 30000, QObject * parent = 0 );
  VP1WebWatcher( const QStringList& urls, int recheckInterval_ms = 30000, QObject * parent = 0 );
  virtual ~VP1WebWatcher();

  void addUrl(const QString&);
  void addUrls(const QStringList&);

  QStringList urls() const;
  bool isWatchingUrl(const QString&) const;

  void removeUrl(const QString&);
  void removeUrls(const QStringList&);

  //Get more information:
  enum RESULT { UNKNOWN,//Might be because no http request was yet done.
		INVALID_URL,
		CONNECTION_PROBLEMS,//Various problems (server not found, no net connection, etc.)
		NOT_ON_SERVER,//Request returned 404 (file not found on server)
		EXISTS };//File was found on server
  RESULT lastResult(const QString& url);
  QString lastResultToString(const QString& url);

  QDateTime lastModTime(const QString& url);//Always invalid if lastResult(url)!=EXISTS (might be invalid for EXISTS as well).

signals:
  void urlChanged(const QString& url );

private slots:
  void httpRequestDone(bool);
protected:
  void timerEvent(QTimerEvent*);
private:
  class Imp;
  Imp * d;

};

#endif
