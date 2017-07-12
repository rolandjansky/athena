/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP1System                       //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP1SYSTEM_H
#define IVP1SYSTEM_H

#include "VP1Base/VP1String.h"
#include <QObject>
#include <QByteArray>

class IVP1ChannelWidget;
class StoreGateSvc;
class ISvcLocator;
class IToolSvc;
// Base class for all systems:
class IVP1System : public QObject, public VP1String  {

  Q_OBJECT

public:

  // Constructor:
  IVP1System(const QString & name, const QString & information, const QString & contact_info);

  // Destructor:
  virtual ~IVP1System();

protected:
  // Initialize the system:
  virtual void create( StoreGateSvc* detectorStore ) = 0;

  // Refresh event-per-event graphics (sg will be null for standalone apps):
  virtual void refresh( StoreGateSvc* eventStore ) = 0;

  // Erase any event-per-event graphics:
  virtual void erase() = 0;

  // Finalize the system:
  virtual void uncreate();

  // Retrieve label information:
public:
  const QString& name() const;
  const QString& information() const;
  const QString& contact_info() const;

  QWidget * controllerWidget();

protected:
  //If the systems needs a custom control widget to allow the user to
  //control how it presents itself, call the follwing method in
  //create() (and at most once). NB: It is up to a given channel
  //whether to actually show this controller to the user by
  //e.g. incorporating it into its own controller:
  void registerController(QWidget*);

public:
  //Override the following if the system needs to be able to save info in the .vp1 files:
  virtual QByteArray saveState();
  virtual void restoreFromState(QByteArray);//If channel loaded from .vp1 files, this will be called
                                            //after create() and before the first refresh().

  void message(const QString&) const; //calling this method causes a message show up in the text
                         //window of the application (through emission of the sysmessage signal below).
  void messageDebug(const QString&) const;//Output only to stdout.
  static bool verbose() { return s_vp1verbose; }// Returns true if env var VP1_VERBOSE_OUTPUT=1
  void messageVerbose(const QString&) const;//Only outputs when verbose()

  //Overloaded convenience methods, for outputting several lines:
  void message(const QStringList&, const QString& addtoend = "" ) const;
  void messageDebug(const QStringList&, const QString& addtoend = "") const;
  void messageVerbose(const QStringList&, const QString& addtoend = "") const;
  void message(const QString& addtostart, const QStringList&, const QString& addtoend = "" ) const;
  void messageDebug(const QString& addtostart, const QStringList&, const QString& addtoend = "") const;
  void messageVerbose(const QString& addtostart, const QStringList&, const QString& addtoend = "") const;

signals:
  void sysmessage(QString) const;

public://Public for easier use in helper classes.

  void updateGUI();//Please call this occasionally if doing a lot of
		   //heavy calculations, to prevent the GUI from
		   //freezing up (for now we are single-threaded, so
		   //this is what we need to do).

  //Use these to access athena services (it is wise to check that they are not null):
  StoreGateSvc * storeGate() const { return eventStore(); }//OBSOLETE NAME. Use eventStore() instead.
  StoreGateSvc * eventStore() const;
  StoreGateSvc * detectorStore() const;
  ISvcLocator * serviceLocator() const;
  IToolSvc * toolSvc() const;

private:

  IVP1System( const IVP1System & );
  IVP1System & operator= ( const IVP1System & );

  class Imp;
  Imp * d;
  friend class VP1ExecutionScheduler;
  friend class VP1Prioritiser;
  friend class IVP1ChannelWidget;
  friend class VP1MainWindow;
  void disallowUpdateGUI();
  void allowUpdateGUI();
  void setChannel(IVP1ChannelWidget*);

  IVP1ChannelWidget * channel() const;
  void deleteController();
public:
  enum State { CONSTRUCTED=0, REFRESHED, ERASED, UNCREATED };
  enum ActiveState { ON=0, OFF };//Whether it is part of the erase/refresh cycle.
  State state() const;
  ActiveState activeState() const;
private:
  void setState(const State&);
  void setActiveState(const ActiveState&, const bool& donttriggererase=true);
  bool isRefreshing();
  void setRefreshing(const bool&);
  void setCanRegisterController(const bool&);
signals:
  void inactiveSystemTurnedActive();//Do NOT call this method in derived classes!!!
  void needErase();//Do NOT call this method in derived classes!!!

private:
  static bool s_vp1verbose;
};

#endif
