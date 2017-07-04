/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1ChannelManager                //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1CHANNELMANAGER_H
#define VP1CHANNELMANAGER_H

// channel manager - manages channel plugins, and knows which are
// available. (also creates/deletes/clones channels, + manages channels ids.)

class IVP1ChannelWidget;
class VP1ExecutionScheduler;
class VP1MainWindow;
#include <QObject>
#include <QStringList>

class VP1ChannelManager : public QObject {

  Q_OBJECT

public:
  VP1ChannelManager(VP1ExecutionScheduler*,VP1MainWindow*);
  ~VP1ChannelManager();

  QString loadPluginFile(QString filename);
  //This does not actually instantiate any channel widgets - it merely
  //instantiates the widget factory from the plugin file (if possible,
  //otherwise it returns an error messages), and notices which
  //channels are provided by this factor (if 0 channels are provided,
  //it also returns an error message). If an empty string was
  //returned, there were no errors.
  bool unloadPluginFile(QString filename);

  QStringList currentPluginFiles() const;

  QStringList channelsInPluginFile(QString filename) const;//An empty returned stringlist is probably because 'filename' never was loaded succesfully.

  QStringList basename2UniqueNames(QString basename) const;

  bool deleteChannel(QString channeluniquename);

  IVP1ChannelWidget * getChannel( QString channelbasename, QString& err );//returns 0 if unsuccesful. Otherwise, 'err' contains an error message.
  unsigned nActive( QString channelbasename ) const;

  IVP1ChannelWidget* uniqueName2Channel(QString uniquename) const;

  bool hasRefreshingSystem(const QString& uniquename) const;

  QStringList uniquenames() const;

  bool uniqueNameExists(QString) const;
  bool baseNameExists(QString) const;
  bool channelWithBasenameIsLoaded(QString basename) const;

  QStringList serializePluginInfo() const;

  QWidget* getController(IVP1ChannelWidget*);

  QString getIconLocation(const QString& channelname, const bool& isbasename=true);//Returns empty string if channelname is not recognised.

  QStringList availableChannelList();

signals:
  void uniquename_channelListChanged(QStringList);//gives a list of channel unique_name's.
  void availableChannelListChanged(QStringList);//gives a list of channel base_name's.
  void availableUnusedChannelListChanged(QStringList);//gives a list of channel base_name's (only the one's that dont appear anywhere).
  void newChannelCreated(IVP1ChannelWidget*);
  void channelUncreated(IVP1ChannelWidget*);

private:
  class Imp;
  Imp * d;
};

#endif

