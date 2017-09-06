/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP1ChannelWidget                //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//  update: rbianchi@cern.ch - October 2013                //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP1CHANNELWIDGET_H
#define IVP1CHANNELWIDGET_H

#include "VP1Base/VP1String.h"
#include "VP1Base/VP1Msg.h"

#include <QWidget>

#include <set>

class QPixmap;

//NB: When implementing subclasses, remember that the VP1 might
//instantiate several versions of each channel (so stay away from
//static/global variables for instance).

class IVP1System;

class IVP1ChannelWidget : public QWidget, public VP1String {

  Q_OBJECT

public:
  IVP1ChannelWidget(const QString & name, const QString & information, const QString & contact_info);
  virtual void init() = 0;//This is where addSystem calls should be made. No code should be in constructors

  virtual ~IVP1ChannelWidget();
  //The name should be kept shorter than 20 characters.

  //Type of widget:
  enum Type { UNKNOWN = 0, THREEDIMENSIONAL, TWODIMENSIONAL, HISTOGRAM, NUMBERS  };
  virtual Type type() const = 0;//Fixme: as parameter to constructor!

  virtual bool isAccumulator() const;

  void setRunEvtNumber(int runnumber, unsigned long long eventnumber);

protected:
  virtual void create();//Fixme: Abstract!
  virtual void systemRefreshed(IVP1System*);
  virtual void systemErased(IVP1System*);
  virtual void goingToNextEvent(){}
  virtual void uncreate();

  virtual void lastOfActiveSystemsRefreshed();//For triggering some behaviour when all data for event is loaded


  //Fixme:Optional (de)persistification of settings?

  void turnOn(IVP1System*);
  void turnOff(IVP1System*,const bool& immediateErase=true);

  void getRunEvtNumber(int& runnumber, unsigned long long& eventnumber);

  void setEvtTimestamp(unsigned timestamp);
  void getEvtTimestamp(unsigned& timestamp);

protected:
  //If the channel needs a custom control widget to allow the user to
  //control how it presents itself, call the follwing method in create() (and at most once):
  void registerController(QWidget*);

  //Call once for each system in constructor of derived classes:
  void registerSystem(IVP1System*);

public:
  //Override the following if the channel needs to be able to save info in the .vp1 files (states of systems are saved separately):
  virtual QByteArray saveState();
  virtual void restoreFromState(QByteArray);//If channel loaded from .vp1 files, this will be called
                                            //after create() and before any refreshes.

signals:
  //Emitting the following signal makes a message show up in the text window of the application
  void message(QString);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
  const QString& name() const;
  const QString unique_name() const;
  const QString& information() const;
  const QString& contact_info() const;
  virtual QPixmap getSnapshot(bool transp = false, int width = 0, bool batch = false);
  virtual bool setAntiAliasing(bool aa) {VP1Msg::messageVerbose("You should not call this function directly, but one of its implementations in derived classes..."); return aa; };
  virtual bool isAntiAliasing() {VP1Msg::messageVerbose("You should not call this function directly, but one of its implementations in derived classes..."); return false; };

private:
  unsigned cloneID() const;
  QWidget * controllerWidget();
signals:
  void uniqueNameChanged(const QString&);//Do NOT call this method in derived classes!!!
  void systemRefreshInfoChanged(QString sysrefreshing, int nsysOn, int nsysOnRefreshed);//Do NOT call this method in derived classes!!!
private:
  void emitRefreshInfoChanged();
  bool hasRefreshingSystem();
private:
  IVP1ChannelWidget( const IVP1ChannelWidget & );
  IVP1ChannelWidget & operator= ( const IVP1ChannelWidget & );
  class Imp;
  Imp * d;

  void setCloneID(unsigned id);
  const std::set<IVP1System *>& systems();
  friend class VP1ExecutionScheduler;
  friend class VP1Prioritiser;
  friend class VP1ChannelManager;
  friend class VP1TabManager;
  friend class VP1MainWindow;
  friend class IVP1System;
  enum State { CONSTRUCTED=0, READY, UNCREATED };
  void deleteControllers();
  State state() const;
  void setState(const State&);
  void setCanRegister(const bool&controller,const bool&system);
public:
  virtual void dockResized();//Called by the dockwidget to inform about resize events.
};

#define VP1CHANNELNAMEINPLUGIN(cls,nme) nme


Q_DECLARE_INTERFACE(IVP1ChannelWidget,"IVP1ChannelWidget/1.0")

#endif
