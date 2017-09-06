/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1Prioritiser                   //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1PRIORITISER_H
#define VP1PRIORITISER_H

#include <QObject>
class IVP1System;
class IVP1ChannelWidget;

class VP1Prioritiser : public QObject {

  Q_OBJECT

public:

  VP1Prioritiser(QObject*parent);
  virtual ~VP1Prioritiser();

  double beginTiming_Refresh(IVP1System*);//Returns an estimate based on earlier measurements
  double elapsedTiming_Refresh();
  double endTiming_Refresh();//Returns timing

  IVP1System* nextErasedActiveSystemByPriority();//Returns 0 if there are no systems with (ActiveState,State)==(ON,ERASED)
  QList<IVP1System*> getSystemsToEraseByPriority();//Returns all systems in REFRESHED state - in prioritised order.

  double estimateRemainingCalcTime() const;

public slots:
  void visibleChannelsChanged(const QSet<IVP1ChannelWidget*>&vis,const QSet<IVP1ChannelWidget*>&soonvis,const double& soonvisbonus);
  void channelCreated(IVP1ChannelWidget*);
  void channelUncreated(IVP1ChannelWidget*);
private:
  class Imp;
  Imp * d;
  void setupSysItr(IVP1ChannelWidget*cw);
};

#endif
