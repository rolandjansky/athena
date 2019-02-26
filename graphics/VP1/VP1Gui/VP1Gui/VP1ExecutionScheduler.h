/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1ExecutionScheduler            //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1EXECUTIONSCHEDULER_H
#define VP1EXECUTIONSCHEDULER_H

// include C++
//#include <stddef.h> // this to fix the 'ptrdiff_t' does not name a type error with Qt (http://qt-project.org/forums/viewthread/16992)

// include VP1
#include "VP1Gui/VP1QtApplication.h"

// include Qt
#include <QObject>
#include <QStringList>



class IVP1System;
class IVP1ChannelWidget;

class StoreGateSvc;
class IToolSvc;
class ISvcLocator;
class VP1AvailEvents;

class VP1ExecutionScheduler : public QObject {

  Q_OBJECT

public:

   //Cruise mode:
  enum CruiseMode { NONE,
		    TAB, //Cycles to a new tab at a fixed interval (when it is loaded - gives soonvisible bonus)
 		    EVENT,//Proceeds to a new event at a fixed interval (counting from when all visible channels are refreshed)
 		    BOTH };//Cycles through all tabs and loads next event when they have all been shown.


  void setCruiseMode(const CruiseMode&);

  //Fixme: Need shortcut keys for the cruise modes as well as next/prev. tab (for full screen).


  //init/cleanup:
  static VP1ExecutionScheduler* init(StoreGateSvc* eventStore,
		             StoreGateSvc* detStore,
				     ISvcLocator* svcLocator,
					 IToolSvc*toolSvc,
				     QStringList joboptions,
				     QString initialCruiseMode = "NONE",
					 unsigned initialCruiseSeconds = 10,
				     QString singleEventSource = "",
				     QString singleEventLocalTmpDir = "",
				     unsigned localFileCacheLimit = 10,
				     QStringList availableLocalInputDirectories = QStringList() );
  static void cleanup(VP1ExecutionScheduler*);

  //Call when new event data are available (returns false when the user closes the program)
  bool executeNewEvent(const int& runnumber, const unsigned long long& eventnumber, const unsigned& triggerType = 0, const unsigned& time = 0);//

  VP1ExecutionScheduler(QObject * parent,
			StoreGateSvc*eventStore,StoreGateSvc*detStore,
			ISvcLocator* svcLocator,IToolSvc*toolSvc,
			VP1AvailEvents * availEvents);
  virtual ~VP1ExecutionScheduler();

  void bringFromConstructedToReady(IVP1ChannelWidget*);
  void uncreateAndDelete(IVP1ChannelWidget*);

  bool isRefreshing() const;

  bool hasAllActiveSystemsRefreshed(IVP1ChannelWidget*) const;

  //For VP1Gui:
  QString nextRequestedEventFile() const;

  //For use by whatever logic wants to determine the next event file
  //(probably VP1MainWindow):
  void setNextRequestedEventFile(const QString&);

  QStringList userRequestedFiles();

  QString saveSnaphsotToFile(IVP1System* s, bool batch = false);

signals:
  void refreshingStatusChanged(bool);


private:
  class Imp;
  Imp * m_d;
  void refreshSystem(IVP1System*);
  void eraseSystem(IVP1System*);
  void actualUncreateAndDelete(IVP1ChannelWidget*);

private slots:
  void processSystemForRefresh();
  void updateProgressBarDuringRefresh();
  void channelCreated(IVP1ChannelWidget*);
  void channelUncreated(IVP1ChannelWidget*);
  void startRefreshQueueIfAppropriate();
  void systemNeedErase();

  void performCruise();
  //OPVASK: void abortCruise();
  //Start
};

#endif
