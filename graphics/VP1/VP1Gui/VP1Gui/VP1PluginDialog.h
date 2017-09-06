/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1PluginDialog                  //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1PLUGINDIALOG_H
#define VP1PLUGINDIALOG_H

#include <QDialog>

class VP1ChannelManager;
class VP1ExecutionScheduler;

class VP1PluginDialog : public QDialog {

  Q_OBJECT

public:

  VP1PluginDialog( QWidget*parent, VP1ChannelManager*,VP1ExecutionScheduler*);
  virtual ~VP1PluginDialog();

  QString unloadfile();
private:

  class Imp;
  Imp * d;

private slots:
  void pluginSelected(int i);
  void unloadclicked();
  void updateRefreshInfo();
};

#endif
