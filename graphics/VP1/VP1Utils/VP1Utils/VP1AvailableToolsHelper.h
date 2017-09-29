/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1AvailableToolsHelper             //
//                                                            //
//  Description: For monitoring the available tools of given  //
//               tool types, and for automatically taking     //
//               care of the state of associated gui elements //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1AVAILABLETOOLSHELPER_H
#define VP1AVAILABLETOOLSHELPER_H

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>
#include <QStringList>
class IToolSvc;
class IVP1System;
class QComboBox;
class QWidget;

class VP1AvailableToolsHelper : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:

  VP1AvailableToolsHelper(IToolSvc*, QObject * parent = 0);//Use this to get error messages output to stdout.
  VP1AvailableToolsHelper(IVP1System*, QObject * parent = 0);//Use this to also get error messages output in VP1 message area.
  virtual ~VP1AvailableToolsHelper();

  /////////////////////////////////////////////////
  //  Change the currently monitored tool-types: //
  /////////////////////////////////////////////////

  void addMonitoredType(const QString&, const QStringList& ignoreList = QStringList());//List of (wildcarded) expressions of tool names to ignore (i.e. "*Fatras*")
  void addMonitoredTypes(const QStringList&, const QStringList& ignoreList = QStringList());
  void removeMonitoredType(const QString&);
  void removeMonitoredTypes(const QStringList&);
  void clearMonitoredTypes(const QString&);
  QStringList monitoredTypes() const;

  /////////////////////////////////////////////////////////////////////
  //  Get list of available tools (calls update behind the scenes):  //
  /////////////////////////////////////////////////////////////////////

  QStringList availableTools() const;

  //Convenience methods:
  void setComboBoxData(QComboBox*);//Will be filled with a list of available tools (disabled if none are available);
  void disableIfNoTools(QWidget*);//widget will be disabled (enabled) if the list of available tools is empty (not empty).
  void handleComboBox(QComboBox*);//Calls setComboBoxData on the combobox whenever the list changes.
  void handleEnabledState(QWidget*);//Calls disableIfNoTools on the widget whenever the list changes.

  void scheduleImmediateUpdate() const;//Updates immediately after event queue clears (NB: Not necessary in future Gaudi versions).

signals:
  void availableToolsChanged(const QStringList&) const;

protected:
  void timerEvent ( QTimerEvent * event );
private:
  class Imp;
  Imp * m_d;
  VP1AvailableToolsHelper( const VP1AvailableToolsHelper & );
  VP1AvailableToolsHelper & operator= ( const VP1AvailableToolsHelper & );

private slots:
  void widgetDeleted(QObject*);//To automatically remove deleted widgets from internal lists.
  void update() const;//Called internally every 2 seconds. In future gaudi versions we can instead register callbacks to the toolSvc.
};

#endif
