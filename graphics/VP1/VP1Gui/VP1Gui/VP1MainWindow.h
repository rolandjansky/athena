/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1MainWindow                    //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1MainWindow_H
#define VP1MainWindow_H

// include the related GUI
#include "../src/ui_vp1mainwindow.h"

// include VP1
#include "VP1Gui/VP1TcpServer.h"
#include "VP1Gui/VP1EventDisplayEditor.h"

// include Qt
#include <QQueue>
#include <QStringList>
#include <QMap>

class VP1ChannelManager;
class VP1TabManager;
class QStringList;
class IVP1ChannelWidget;
//class VP1ConfigForm;
class VP1ExecutionScheduler;
class VP1AvailEvents;
class VP1IncomingMessageDialog;
class VP1PluginDialog;
class QProgressBar;
class QLabel;
class QComboBox;
class VP1StreamMenuUpdater;
class QMutex;

//#include <QtGlobal>
//#if QT_VERSION > QT_VERSION_CHECK(5, 5, 0)
//  class QWebEngineView; // Qt 5.6
//#else
//  class QWebView;
//#endif


struct VP1DirStatusData
{
  VP1DirStatusData()
    : inputDir()
    , dirStatus()
    , enabled(true)
    , bold(false)
  {};

  VP1DirStatusData(QString the_inputDir
		   , QString the_dirStatus
		   , bool the_enabled
		   , bool the_bold)
    : inputDir(the_inputDir)
    , dirStatus(the_dirStatus)
    , enabled(the_enabled)
    , bold(the_bold)
  {};

  QString inputDir;
  QString dirStatus;
  bool enabled;
  bool bold;
};

typedef QMap<QAction*,VP1DirStatusData> VP1DirStatuses;


class VP1MainWindow : public QMainWindow, public Ui::VP1MainWindow
{
  Q_OBJECT

public:
  VP1MainWindow(VP1ExecutionScheduler*,VP1AvailEvents * availEvents,QWidget *parent = 0);
  ~VP1MainWindow();

  bool okToProceedToNextEvent() const;

  // Set next event to the execution scheduler and updates event controls
  // We needed to separate this method from goToNextEvent() in order to be able
  // to skip the bootstrap event in the https mode
  void nextEvent();

  QStringList userRequestedFiles();
  
public slots:
  void loadPluginFile(QString filename);

  void request_addEmptyTab();
  void request_channelInformation();
  void request_printChannel();
  QString request_saveChannelSnapshot(QString xLabel="");
  void request_cruisemodechange();
  void request_saveasConfig();
  void request_saveConfig();
  void request_loadConfig();
  void request_loadPlugin();
  void unloadPlugin_continue();
  void pluginDialogClosed();
  void saveAllCurrentChannels();
  void makeAllChannelsEventDisplay();
  void getAllChannelsIntoSnapshots(QList<QPixmap>& list, QStringList& listNames);
  QPixmap getSingleChannelCustomSnapshot(IVP1ChannelWidget* tab, int width = 0);
  QPixmap getSingleChannelCustomSnapshot(QString tabName, int width = 0);

  void tabListChanged(QStringList);

  void selectedChannelChanged(IVP1ChannelWidget*);

  void loadConfigurationFromFile(QString file);
  void replaceConfigurationFile(QString file);

  void addToMessageBox( const QString&, const QString& extrastyleopts = "",
			const QString& title = "", const QString& titleextrastyleopts = "" );
  void systemAddToMessageBox( const QString& );
  void channelAddToMessageBox( const QString& );
  void helperAddToMessageBox( const QString& );
  void changeFontSize(int);

  //For use when going to a new event (this updates system lists and quit()'s the app):
  void goToNextEvent();
public:
  void setRunEvtNumber(const int& runnumber, const unsigned long long& eventnumber, const unsigned& triggerType = 0, const unsigned& time = 0, const bool& printmessage = true );
  bool mustQuit() const;

  VP1ChannelManager * channelManager() const { return m_channelmanager; }
  VP1TabManager * tabManager() const { return m_tabmanager; }

  bool userRequestedExit() { return m_userRequestedExit; }

  int getRunNumber() { return m_runnumber; }
  unsigned long long getEventNumber() { return m_eventnumber; }
  int getEventTimestamp() { return m_timestamp; }

protected:
  VP1ChannelManager * m_channelmanager;
  VP1TabManager * m_tabmanager;
  QString m_currentconfigfile;
  int m_runnumber;
  unsigned long long m_eventnumber;
  unsigned m_timestamp;
  bool m_betweenevents;

  void closeEvent(QCloseEvent *event);
  bool m_mustquit;

  QWidget* m_dummyemptycontroller;
  VP1ExecutionScheduler*m_scheduler;
  VP1AvailEvents * m_availEvents;

  VP1TcpServer m_tcpserver;
  void listenOnTcp();

  VP1IncomingMessageDialog* m_currentincomingdialog;
  QQueue<VP1ExternalRequest> m_requestqueue;
  bool m_blockallmessages;
  QList<QString> m_messages_blockedsenders;
  QList<VP1ExternalRequest> m_messages_blockedexactmessages;

  void addChannelIconsToComboBox(QComboBox* cb, const bool& isbasenames);

  QStringList m_currentunloadpluginfiles;
  VP1PluginDialog*m_plugindialog;

  void setupStatusBar();
  QLabel*m_statusbarlabel;

  QMap<QString,QString> availablePluginFiles() const;
  QMap<QString,QString> availableFiles(const QString& extension,
				       const QString& pathvar,
				       const QString& instareasubdir,
				       const QString& extradirenvvar,
				       bool currentdir = false) const;

public:
  QProgressBar*progressbar;
protected slots:
  void receivedExternalRequest(VP1ExternalRequest);
  void processEnqueuedRequests();
  void finishedIncomingDialog();
  void updateTcpIcon();
  void postInitUpdates();
  void showMenu_loadPlugin();
  void showMenu_loadPluginItemSelected();
  void showMenu_loadConfFile();
  void showMenu_loadConfFileItemSelected();
  void changeStyleActionTriggered();
  void changeFontSizeActionTriggered();
  void updateCentralStackWidget();
  void quickSetupTriggered();
  void updateEventControls();
  void addEventFile();
  void help_openUserGuide();
  void help_openUserSupport();
  void help_openVP1WebSite();
  void help_openAbout();

  void updateInputDirectoriesStatus();
  void inputDirectoryActionTriggered();

  void launch3DstereoEditor();


protected:
  QAction * m_action_infoAboutLoadedPlugins;
  QMenu * m_menu_loadPlugin;
  QMenu * m_menu_loadConfFile;
  QMenu * m_menu_changeStyle;
  QMenu * m_menu_changeFontSize;
  QAction * m_actionAdd_empty_tab;
  QAction * m_actionSave_current_tabs;
  QAction* m_action_addEventFile;
  QAction* m_action_openUsersGuide;
  QAction* m_action_openUsersSupport;
  QAction* m_action_openVP1Site;
  QAction* m_action_openAbout;

  QList<QAction*> m_inputdiractions;
  VP1DirStatuses m_inputdirstatuses;
  QString m_currentsaveimagepath;
  QString m_currentloadpluginpath;
  QString m_currentStream;

  //Fontsize:
  QFont m_defaultfont;
  double m_defaultfont_pointsize;
  int m_defaultfont_pixelsize;
  const QString m_settingsfile;

  bool m_userRequestedExit;

  QStringList m_userRequestedFiles;

  VP1StreamMenuUpdater* m_streamMenuUpdater;
  QMutex* m_mutex;

//  // Web broser instance to show VP1 documentation
//  #if QT_VERSION > QT_VERSION_CHECK(5, 5, 0)
//    QWebEngineView *m_view; // Qt 5
//  #else
//    QWebView *m_view;
//  #endif

  // Event display editor
  VP1EventDisplayEditor* m_edEditor;
};


#endif

