/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1AvailEvents                      //
//                                                            //
//  Description: Class for keeping track of available events  //
//               when running VP1 in single-file-per-event    //
//               mode.                                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef IVP1AVAILEVENTS_H
#define IVP1AVAILEVENTS_H
#include <QObject>
#include <QList>
#include "VP1Gui/VP1EventFile.h"

class VP1AvailEvents : public QObject {

  Q_OBJECT

public:

  VP1AvailEvents(int timeCutForNew, QString tmpdir, int maxLocalFilesToKeep = -1, QObject * parent = 0);//maxLocalFilesToKeep<=2 means keep all.
  //timeCutForNew: <0 : no time Cut;
  //                0 : only newest is fresh
  //               >0 : time cut in seconds.
  virtual void init() = 0;//Forbidden to do any real work before init is called (in particular emit signals!).

  virtual ~VP1AvailEvents();

  //Next two returns ordered lists such that first means newer.
  QList<VP1EventFile> allLocalEvents() const;//All events copied to tmp directory
  QList<VP1EventFile> freshEvents() const;//Events that are considered "new", and which have not been investigated yet.
  VP1EventFile newestEvent() const;//The newest event. Might have been investigated.

  void setCurrentEvent(int run,int event);//Call on every new event to update the history.
  int maxLocalFilesToKeep() const;

protected:
  void cleanupAndCheckForEventListChanges();//Call from derived classes to ensure proper emission of XXXEventsChanged() signals.
  void invalidateDirCache(const QString& dir);//Call from derived classes after making changes to files in directory.
  bool inHistory(int run, int event) const;
  QList<VP1EventFile> allEventFilesInDir(const QString& dir) const;
  QList<VP1EventFile> freshEvents(VP1EventFile newestEvt, const QList<VP1EventFile>&) const;//Given timecut and newestEvt, trim down
                                                                                            //list so we only get the "fresh events" left.
  QString tmpDir() const;
  QString tmpLocalFileDir() const;
  QString tmpActiveRetrievalDir();//attempts to create temporary subdirectory in tmpCopyDir which
                                 //can be used for temporary files while downloading/copying, etc.
                                 //returns an empty string in case of problems.

  //Definition of fresh events:
  int timeCutForNew() const;
  bool requireNewestRunNumber() const { return false; /*fixme*/ }
  //Convenience:
  bool isConsideredFresh ( const VP1EventFile& evt, const VP1EventFile& newestEvt ) const;

  void messageDebug(const QString&) const;//Messages to appear on output.
signals:
  void allLocalEventsChanged();
  void freshEventsChanged();
  void message(const QString&) const;//Messages to appear in GUI.

private slots:
  void actualCleanup();
  void actualCheckForEventListChanges();

private:

  class Imp;
  Imp * d;

};

#endif
