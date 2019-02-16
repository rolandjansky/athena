/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1TabManager                    //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1TABMANAGER_H
#define VP1TABMANAGER_H

#include <QObject>
#include <QStringList>
#include <QMultiMap>
#include <QPair>
class VP1TabWidget;
class VP1ChannelManager;
class VP1DockWidget;
class IVP1ChannelWidget;
class QPoint;

//NB: The tab manager is allowed to pop up boxes with warnings, information, etc.

class VP1TabManager : public QObject {

  Q_OBJECT

public:

  VP1TabManager(QObject*parent, VP1TabWidget *,VP1ChannelManager*);
  ~VP1TabManager();

  bool hasTab(QString) const;
  bool showTab(QString);
  bool showFirstChannelWithGivenBasename(QString basename);
  QString suggestNewTabName(QString oldtabname) const;
  QString currentTab() const;
  QString currentChannelUniqueName() const;
  IVP1ChannelWidget* selectedChannelWidget() const;
  int nTabs() const;
  void dropOutOfFullScreen();//Does nothing if not in FS mode.

  QList<IVP1ChannelWidget*> allChannels() const;//No particular order.
  const QSet<IVP1ChannelWidget*>& visibleChannels() const;
  const QSet<IVP1ChannelWidget*>& soonVisibleChannels() const;
  bool isVisible(IVP1ChannelWidget*) const;
    //bool isInNextTab();

  void setTabCruiseMode(const bool&);

  QStringList tabList();
  QString channelToTab(IVP1ChannelWidget*);

  void launchStereoEditorCurrentTab();
//  void setAntiAliasingCurrentTab(bool);

  IVP1ChannelWidget * addChannelToTab( QString channelbasename, QString tabname );
public slots:
  void addNewTab( QString, const int& index = -1 );
  void renameTab( QString tabname, QString newtabname );
  void removeTab( QString tabname );// -> Also removes channels obviously.
  void removeChannel(QString channeluniquename);
  void moveChannelToTab(QString channeluniquename,QString tabname);
  void cloneChannelToTab(QString channeluniquename,QString tabname);
  void cloneTab(QString oldtabname,QString newtabname);
  void removeAllTabs();


  void saveConfigurationToFile(QString filename,const bool& askonoverride=true);
  void loadConfigurationFromFile(QString filename,const QMap<QString,QString>& availableplugins);

  void showChannelFullScreen(IVP1ChannelWidget*);
  void showCurrentChannelFullScreen();
  void showTabFullScreen(QString tabname);
  void showCurrentTabFullScreen();

  void showNextTab();
  void showPreviousTab();

  void raiseTabBarContextMenu(int,const QPoint &);
  void setSelectedDockWidget(VP1DockWidget*dw=0);
  void setSelectedChannelWidget(IVP1ChannelWidget*cw=0);//selects the associated dock widget

  //The next two methods/slots are needed to allow channels to remove themselves:
public:
  void removeChannelAfterQueueEmpties(const QString&);
signals:
  void tabListChanged(QStringList);
  void selectedChannelChanged(IVP1ChannelWidget*);//0 if all were unselected
  void visibleChannelsChanged(const QSet<IVP1ChannelWidget*>&vis,const QSet<IVP1ChannelWidget*>&soonvis,const double& soonvisbonus);

protected:
  class Imp;
  Imp * m_d;

  bool eventFilter ( QObject *, QEvent * );
  typedef QPair<QByteArray,QMultiMap<QString,QByteArray> > ChanState;
  void serializeChannelState(IVP1ChannelWidget*,ChanState&state);
  void unserializeChannelState(IVP1ChannelWidget*cw,ChanState tate);

protected slots:
  void currentVisibleChanged();
  void executePendingChannelRemoval();
};

#endif
