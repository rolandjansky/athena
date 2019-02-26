/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1TabManager                  //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Gui/VP1TabManager.h"
#include "VP1Gui/VP1ChannelManager.h"
#include "VP1Gui/VP1DockWidget.h"

#include "VP1Base/VP1TabWidget.h"
#include "VP1Base/VP1TabBar.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/IVP1ChannelWidget.h"
#include "VP1Base/IVP13DStandardChannelWidget.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Msg.h"

#include <QEvent>
#include <QPoint>
#include <QSet>
#include <QFileInfo>
#include <QMap>
#include <QTimer>
#include <QBuffer>
#include <QByteArray>
#include <QMessageBox>
#include <QMenu>
#include <QKeyEvent>
#include <QMainWindow>
#include <QApplication>
#include <QInputDialog>

#include <map>
#include <set>
#include <cassert>
#include <iostream>//fixme



//_______________________________________________________________________
class VP1TabManager::Imp {
public:
  //Constructor, tabwidget & channel manager:
  Imp(VP1TabManager*tm,VP1TabWidget* tw, VP1ChannelManager* cm)
    : tabmanager(tm), tabwidget(tw),channelmanager(cm),
      dontEmitVisibilityChanges{},
      selecteddockwidget(0),
      fullscreen_dockwidget(0),fullscreen_channelwidget(0),
      fullscreen_tabindex(-1),fullscreen_tab(0),tabcruisemode(false) {}

  VP1TabManager* tabmanager;
  VP1TabWidget * tabwidget;
  VP1ChannelManager* channelmanager;

  //Convenience:
  QMainWindow * channel2tab(IVP1ChannelWidget*cw);
  QMainWindow * name2tab(QString tabname);//Returns 0 if not found

  //The actual bookkeeping maps:
  std::map<QString,QMainWindow *> name_2_tab;
  std::map<IVP1ChannelWidget*,VP1DockWidget*> channelwidget_2_dockwidget;
  std::map<QMainWindow*,std::set<IVP1ChannelWidget*> > tab_2_channelwidgets;

  //For sanity checking input, and providing appropriate warnings.
  //Fixme: Check also max number of chars.
  bool checkTabnameNotEmpty(QString tabname) const;
  bool checkTabnameExists(QString tabname) const;
  bool checkTabnameNotExists(QString tabname) const;

  bool checkChannelNameExists(QString channelbasename, const bool& isuniquename, const bool& checkfornonexistance = false) const;

  bool dontEmitVisibilityChanges;
  QSet<IVP1ChannelWidget*> lastvisible;
  QSet<IVP1ChannelWidget*> lastsoonvisible;

  VP1DockWidget* selecteddockwidget;
  //Fullscreen of individual channels:
  VP1DockWidget* fullscreen_dockwidget;
  IVP1ChannelWidget* fullscreen_channelwidget;
  //Fullscreen of tabs:
  QString fullscreen_tabname;
  int fullscreen_tabindex;
  QMainWindow* fullscreen_tab;
  std::set<VP1DockWidget*> fullscreen_floatingdocks;

  QMap<QString,QStringList> serializeTabAndChannelConfigInfo() const;
  void serializeTabAndChannelConfigInfo(QMap<QString,QMultiMap<QString,ChanState> >& tab2channels,
					QMap<QString,QByteArray>&tab2arrangements) const;

  QMainWindow* previousTab();
  QMainWindow* nextTab();
  bool tabcruisemode;

  QStringList channelsInTab(QMainWindow* tab);
  QStringList channelsInTab(const QString&);

  QString channelWithPendingRemoval;
};

//_______________________________________________________________________
VP1TabManager::VP1TabManager( QObject* parent, VP1TabWidget * tw, VP1ChannelManager* cm )
  : QObject(parent), m_d(new Imp(this,tw,cm)) {

  m_d->dontEmitVisibilityChanges=false;
  tw->setTabReorderingEnabled(true);
  connect(tw->getVP1TabBar(),SIGNAL(contextMenu( int, const QPoint& )),
	  this,SLOT(raiseTabBarContextMenu(int,const QPoint &)));

  connect(tw,SIGNAL(currentChanged(int)),this,SLOT(currentVisibleChanged()));
}



//_______________________________________________________________________
VP1TabManager::~VP1TabManager(){
  while (m_d->tabwidget->count()>0) {
    removeTab( m_d->tabwidget->tabText(0) );
  }
  delete m_d;
}


//___________________________________________________________________________________
void  VP1TabManager::launchStereoEditorCurrentTab(){
	VP1Msg::messageVerbose("VP1TabManager::launchStereoEditorCurrentTab()");
  IVP13DStandardChannelWidget* channel = dynamic_cast<IVP13DStandardChannelWidget*>(selectedChannelWidget());
	if (channel ) {
		channel->launchStereoEditor();
	}
}



////___________________________________________________________________________________
//void VP1TabManager::setAntiAliasingCurrentTab(bool aa)
//{
//	VP1Msg::messageVerbose("VP1TabManager::setAntiAliasingCurrentTab()");
//
//	if (dynamic_cast<IVP13DStandardChannelWidget*>(selectedChannelWidget()) ) {
//		IVP13DStandardChannelWidget* channel = dynamic_cast<IVP13DStandardChannelWidget*>(selectedChannelWidget());
//		channel->setAntiAliasing(aa);
//	}
//}


//___________________________________________________________________________________
bool  VP1TabManager::Imp::checkTabnameExists(QString tabname) const {
  if (!checkTabnameNotEmpty(tabname))
    return false;
  if (name_2_tab.find(tabname)==name_2_tab.end()) {
    QMessageBox::critical(0, "Error - Tab "+tabname+" does not exists",
			  "Tab <i>"+tabname+"</i> does not exists.",QMessageBox::Ok,QMessageBox::Ok);
    return false;
  }
  return true;
}

//___________________________________________________________________________________
bool  VP1TabManager::Imp::checkTabnameNotExists(QString tabname) const {
  if (!checkTabnameNotEmpty(tabname))
    return false;
  if (name_2_tab.find(tabname)!=name_2_tab.end()) {
    QMessageBox::critical(0, "Error - Tab "+tabname+" already exists",
			  "Tab <i>"+tabname+"</i> already exists.",QMessageBox::Ok,QMessageBox::Ok);
    return false;
  }
  return true;
}

//___________________________________________________________________________________
bool VP1TabManager::Imp::checkTabnameNotEmpty(QString tabname) const {
  if (tabname.isEmpty()) {
    QMessageBox::critical(0, "Error - Empty tab name provided",
			  "Empty tab name provided.",QMessageBox::Ok,QMessageBox::Ok);
    return false;
  }
  return true;
}

//___________________________________________________________________________________
bool VP1TabManager::Imp::checkChannelNameExists(QString channelname, const bool& isuniquename, const bool& checkfornonexistance) const {
  if (channelname.isEmpty()) {
    QMessageBox::critical(0, "Error - Empty channel name provided",
			  "Empty channel name provided.",QMessageBox::Ok,QMessageBox::Ok);
    return false;
  }
  if ( checkfornonexistance == ( isuniquename ? channelmanager->uniqueNameExists(channelname)
				 : channelmanager->baseNameExists(channelname) ) ) {
    QString tmp = ( checkfornonexistance ? "already":"does not" );
    QMessageBox::critical(0, "Error - Channel "+channelname+" "+tmp+" exists",
			  "Channel <i>"+channelname+"</i> "+tmp+" exists.",QMessageBox::Ok,QMessageBox::Ok);
    return false;
  }
  return true;
}

//_______________________________________________________________________
QStringList VP1TabManager::Imp::channelsInTab(QMainWindow* tab)
{
  assert(tab_2_channelwidgets.find(tab)!=tab_2_channelwidgets.end());
  if (tab_2_channelwidgets.find(tab)==tab_2_channelwidgets.end())
    return QStringList();
  std::set<IVP1ChannelWidget*>::const_iterator it, itE = tab_2_channelwidgets[tab].end();

  QStringList l;
  for (it = tab_2_channelwidgets[tab].begin();it!=itE;++it) {
    l << (*it)->unique_name();
  }

  return l;
}

//_______________________________________________________________________
QStringList VP1TabManager::Imp::channelsInTab(const QString& tabname)
{
  //Get list of (unique) names of channels in this tab:
  assert(name_2_tab.find(tabname)!=name_2_tab.end());
  if (name_2_tab.find(tabname)==name_2_tab.end())
    return QStringList();
  return channelsInTab(name_2_tab[tabname]);
}


//_______________________________________________________________________
void VP1TabManager::addNewTab(QString tabname,const int& index)
{
  if (!m_d->checkTabnameNotExists(tabname)) return;
  bool save = m_d->dontEmitVisibilityChanges;
  m_d->dontEmitVisibilityChanges=true;

  QMainWindow * t = new QMainWindow();
  t->setDockNestingEnabled(true);
  if (index==-1)
    m_d->tabwidget->addTab(t,tabname);
  else
    m_d->tabwidget->insertTab(index,t,tabname);
  //NB: Do not call: t->setParent(m_d->tabwidget);
  m_d->name_2_tab[tabname]=t;
  m_d->tab_2_channelwidgets[t] = std::set<IVP1ChannelWidget*>();
  tabListChanged(tabList());
  m_d->dontEmitVisibilityChanges=save;
  currentVisibleChanged();
}

//___________________________________________________________________________________
void VP1TabManager::dropOutOfFullScreen()
{
  if (!(m_d->fullscreen_dockwidget||m_d->fullscreen_tab))
    return;

  bool save = m_d->dontEmitVisibilityChanges;
  m_d->dontEmitVisibilityChanges=true;
  qApp->removeEventFilter(this);
  QWidget * focuswidget = qApp->focusWidget();

  if (m_d->fullscreen_dockwidget) {
    assert(m_d->fullscreen_dockwidget&&m_d->fullscreen_channelwidget);
    m_d->fullscreen_dockwidget->ensureCWHasParent();
    m_d->fullscreen_channelwidget->showNormal();//fixme? Do inside dockwidget?
    m_d->fullscreen_dockwidget=0;
    m_d->fullscreen_channelwidget=0;
  } else {
    assert(m_d->fullscreen_tab);
    QWidget * dummywidget=m_d->tabwidget->widget(m_d->fullscreen_tabindex);
    m_d->tabwidget->removeTab(m_d->fullscreen_tabindex);
    m_d->tabwidget->insertTab(m_d->fullscreen_tabindex, m_d->fullscreen_tab, m_d->fullscreen_tabname );
    delete dummywidget;
    m_d->tabwidget->setCurrentIndex(m_d->fullscreen_tabindex);
    //Make dockwidgets in the tab floatable again
    assert(m_d->tab_2_channelwidgets.find(m_d->fullscreen_tab)!=m_d->tab_2_channelwidgets.end());
    std::set<IVP1ChannelWidget*>::const_iterator it = m_d->tab_2_channelwidgets[m_d->fullscreen_tab].begin();
    std::set<IVP1ChannelWidget*>::const_iterator itE = m_d->tab_2_channelwidgets[m_d->fullscreen_tab].end();
    for (;it!=itE;++it) {
      assert(m_d->channelwidget_2_dockwidget.find(*it)!=m_d->channelwidget_2_dockwidget.end());
      VP1DockWidget * dock = m_d->channelwidget_2_dockwidget[*it];
      dock->setFeatures(VP1DockWidget::DockWidgetMovable|VP1DockWidget::DockWidgetFloatable);
      if (m_d->fullscreen_floatingdocks.find(dock)!=m_d->fullscreen_floatingdocks.end())
	dock->setFloating(true);
    }
    m_d->fullscreen_tabname = "";
    m_d->fullscreen_tabindex = -1;
    m_d->fullscreen_tab = 0;
    m_d->fullscreen_floatingdocks.clear();
  }
  if (focuswidget&&!focuswidget->hasFocus())
    focuswidget->setFocus(Qt::OtherFocusReason);
  m_d->dontEmitVisibilityChanges=save;
  currentVisibleChanged();
}

//___________________________________________________________________________________
bool VP1TabManager::eventFilter ( QObject *, QEvent * event ) {

  //When in fullscreen mode we monitor all events in the application
  //to look for ESC (leave fullscreen) and to prevent any toplevel
  //widgets from getting closed by e.g. the user pressing ALT+F4.

  if (event->type()==QEvent::Close) {
    event->ignore();
    return true;
  }

  if (event->type()!=QEvent::KeyPress)
    return false;

  QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
  assert(keyEvent);
  if (keyEvent->key()!=Qt::Key_Escape)
    return false;

  dropOutOfFullScreen();

  //Eat the event:
  return true;
}

//___________________________________________________________________________________
void VP1TabManager::setSelectedDockWidget(VP1DockWidget*dw) {

	VP1Msg::messageDebug("VP1TabManager::setSelectedDockWidget()");

  if ( m_d->selecteddockwidget == dw )
    return;

  if (m_d->selecteddockwidget)
    m_d->selecteddockwidget->setUnselected();

  m_d->selecteddockwidget=dw;
  if (dw)
    dw->setSelected();

  selectedChannelChanged( dw ? dw->channelWidget() : 0 );
}

//___________________________________________________________________________________
void VP1TabManager::setSelectedChannelWidget(IVP1ChannelWidget*cw)
{
  if (!cw) {
    setSelectedDockWidget(0);
    return;
  }
  assert(m_d->channelwidget_2_dockwidget.find(cw)!=m_d->channelwidget_2_dockwidget.end());
  setSelectedDockWidget(m_d->channelwidget_2_dockwidget[cw]);
}

//___________________________________________________________________________________
void VP1TabManager::showChannelFullScreen(IVP1ChannelWidget*cw) {
  dropOutOfFullScreen();

  assert(m_d->channelwidget_2_dockwidget.find(cw)!=m_d->channelwidget_2_dockwidget.end());
  VP1DockWidget* dock = m_d->channelwidget_2_dockwidget[cw];
  bool save = m_d->dontEmitVisibilityChanges;
  m_d->dontEmitVisibilityChanges=true;
  m_d->fullscreen_dockwidget=dock;
  m_d->fullscreen_channelwidget=cw;
  cw->hide();
  dock->ensureCWHasNoParent();
  cw->showFullScreen();
  qApp->installEventFilter(this);
  m_d->dontEmitVisibilityChanges=save;
  currentVisibleChanged();


}

//___________________________________________________________________________________
void VP1TabManager::showCurrentChannelFullScreen()
{
  if(!m_d->selecteddockwidget)
    return;
  showChannelFullScreen(m_d->selecteddockwidget->channelWidget());
}

//___________________________________________________________________________________
QString VP1TabManager::currentChannelUniqueName() const
{
  if(!m_d->selecteddockwidget)
    return "";
  else
    return m_d->selecteddockwidget->channelWidget()->unique_name();
}

//___________________________________________________________________________________
void VP1TabManager::showTabFullScreen(QString tabname)
{
  int index = 0;
  for (; index < m_d->tabwidget->count(); ++index) {
    if (m_d->tabwidget->tabText(index)==tabname)
      break;
  }
  if (index >= m_d->tabwidget->count())
    return;
  dropOutOfFullScreen();
  //Test that we are not already fs:
  assert(m_d->fullscreen_tabname.isEmpty());
  assert(m_d->fullscreen_tabindex==-1);
  assert(!m_d->fullscreen_tab);
  assert(m_d->fullscreen_floatingdocks.empty());

  bool save = m_d->dontEmitVisibilityChanges;
  m_d->dontEmitVisibilityChanges=true;

  //Set tab info:
  m_d->fullscreen_tabindex = index;
  m_d->fullscreen_tabname = m_d->tabwidget->tabText(m_d->fullscreen_tabindex);
  assert(m_d->name_2_tab.find(m_d->fullscreen_tabname)!=m_d->name_2_tab.end());
  m_d->fullscreen_tab = m_d->name_2_tab[m_d->fullscreen_tabname];
  //Dock all dockwidgets in the tab that are currently floating, and make them unfloatable:
  assert(m_d->tab_2_channelwidgets.find(m_d->fullscreen_tab)!=m_d->tab_2_channelwidgets.end());
  std::set<IVP1ChannelWidget*>::const_iterator it = m_d->tab_2_channelwidgets[m_d->fullscreen_tab].begin();
  std::set<IVP1ChannelWidget*>::const_iterator itE = m_d->tab_2_channelwidgets[m_d->fullscreen_tab].end();
  for (;it!=itE;++it) {
    assert(m_d->channelwidget_2_dockwidget.find(*it)!=m_d->channelwidget_2_dockwidget.end());
    VP1DockWidget * dock = m_d->channelwidget_2_dockwidget[*it];
    if (dock->isFloating()) {
      m_d->fullscreen_floatingdocks.insert(dock);
      dock->setFloating(false);
    }
    dock->setFeatures(VP1DockWidget::DockWidgetMovable);
  }

  //Remove tab, put a dummy in its place, and go fullscreen:
  m_d->tabwidget->removeTab(m_d->fullscreen_tabindex);
  m_d->fullscreen_tab->hide();
  m_d->fullscreen_tab->setParent(0);
  m_d->tabwidget->insertTab ( m_d->fullscreen_tabindex, new QWidget(), m_d->fullscreen_tabname );
  m_d->fullscreen_tab->showFullScreen();
   //Needs an extra update apparently, to get selection frame properly enlarged
  it = m_d->tab_2_channelwidgets[m_d->fullscreen_tab].begin();
  itE = m_d->tab_2_channelwidgets[m_d->fullscreen_tab].end();
  for (;it!=itE;++it) {
    assert(m_d->channelwidget_2_dockwidget.find(*it)!=m_d->channelwidget_2_dockwidget.end());
    VP1DockWidget * dock = m_d->channelwidget_2_dockwidget[*it];
    dock->update();
  }

  qApp->installEventFilter(this);
  m_d->dontEmitVisibilityChanges=save;
  currentVisibleChanged();

}

//___________________________________________________________________________________
void VP1TabManager::showCurrentTabFullScreen()
{
  if (m_d->tabwidget->count()==0)
    return;
  showTabFullScreen(m_d->tabwidget->tabText(m_d->tabwidget->currentIndex()));
}



//___________________________________________________________________________________
IVP1ChannelWidget* VP1TabManager::selectedChannelWidget() const {
  return m_d->selecteddockwidget ? m_d->selecteddockwidget->channelWidget() : 0;
}

//_______________________________________________________________________
IVP1ChannelWidget * VP1TabManager::addChannelToTab(QString channelbasename,QString tabname) {

  if (m_d->channelwidget_2_dockwidget.size()&&VP1QtUtils::environmentVariableIsOn("VP1_DISALLOW_MULTIPLE_CHANNELS")) {
    QMessageBox::critical(0, "Error - Not allowed to open channel",
			  "The possibility to launch multiple channels has been disabled by the environment variable VP1_DISALLOW_MULTIPLE_CHANNELS."
			  " This was likely set since some badly written 3D drivers have been known to cause crashes when showing multiple 3D views."
			  "\n In order to launch a new channel you must thus first remove the already active channel."
			  "\n To disable this behaviour don't set VP1_DISALLOW_MULTIPLE_CHANNELS, or set it to \"0'.",QMessageBox::Ok,QMessageBox::Ok);
    return 0;
  }

  if (!m_d->checkTabnameExists(tabname)) return 0;
  if (!m_d->checkChannelNameExists(channelbasename, false)) return 0;

  //Then we get the channel:
  QString err;
  IVP1ChannelWidget * cw = m_d->channelmanager->getChannel( channelbasename,err );
  if (!err.isEmpty()) {
    QMessageBox::critical(0, "Error - could not get channel: "+channelbasename,
			  "Could not get channel: "+channelbasename
			  +"\n\nReason: "+err,QMessageBox::Ok,QMessageBox::Ok);

    return 0;
  }
  assert(cw);
  bool save = m_d->dontEmitVisibilityChanges;
  m_d->dontEmitVisibilityChanges=true;
  cw->setUpdatesEnabled(false);

  //Everything went well - go ahead and add the channel to the tab.
  VP1DockWidget *dock = new VP1DockWidget(cw,this);
  connect(dock,SIGNAL(topLevelChanged(bool)),this,SLOT(currentVisibleChanged()));
  connect(dock,SIGNAL(wasSelected(VP1DockWidget*)),this,SLOT(setSelectedDockWidget(VP1DockWidget*)));

  QMainWindow * tab = m_d->name_2_tab[tabname];
  tab->addDockWidget(Qt::TopDockWidgetArea, dock);

  assert(m_d->channelwidget_2_dockwidget.find(cw)==m_d->channelwidget_2_dockwidget.end());
  m_d->channelwidget_2_dockwidget[cw]=dock;
  assert(m_d->tab_2_channelwidgets.find(tab)!=m_d->tab_2_channelwidgets.end());
  m_d->tab_2_channelwidgets[tab].insert(cw);
  cw->setUpdatesEnabled(true);
  m_d->dontEmitVisibilityChanges=save;
  currentVisibleChanged();
  return cw;
}

//_______________________________________________________________________
void VP1TabManager::renameTab( QString tabname, QString newtabname ) {
  if (!m_d->checkTabnameExists(tabname)) return;
  if (!m_d->checkTabnameNotExists(newtabname)) return;

  QMainWindow * tab = m_d->name_2_tab[tabname];

  m_d->name_2_tab.erase(m_d->name_2_tab.find(tabname));
  m_d->name_2_tab[newtabname]=tab;
  int tabindex = m_d->tabwidget->indexOf(tab);
  assert(tabindex!=-1);
  m_d->tabwidget->setTabText ( tabindex, newtabname );
  tabListChanged(tabList());
}

//_______________________________________________________________________
QStringList VP1TabManager::tabList() {
  QStringList l;
  for (int i = 0; i < m_d->tabwidget->count(); ++i) {
    l<<m_d->tabwidget->tabText(i);
  }
  return l;
}


//_______________________________________________________________________
void VP1TabManager::removeTab( QString tabname ) {

  if (!m_d->checkTabnameExists(tabname)) return;

  bool save = m_d->dontEmitVisibilityChanges;
  m_d->dontEmitVisibilityChanges=true;

  assert(m_d->name_2_tab.find(tabname)!=m_d->name_2_tab.end());
  QMainWindow * tab = m_d->name_2_tab[tabname];
  assert(tab);
  assert(m_d->tab_2_channelwidgets.find(tab)!=m_d->tab_2_channelwidgets.end());

  //First, we turn off updates on all channel widgets in the tab (this
  //prevents some crashes due to poorly written channels):
  std::set<IVP1ChannelWidget*>::const_iterator it = m_d->tab_2_channelwidgets[tab].begin();
  std::set<IVP1ChannelWidget*>::const_iterator itE = m_d->tab_2_channelwidgets[tab].end();
  for (;it!=itE;++it) {
    (*it)->setUpdatesEnabled(false);//Fixme: do this on detach/reattach also?
    (*it)->hide();
  }

  while(m_d->tab_2_channelwidgets[tab].size()>0) {
    IVP1ChannelWidget * channelwidget = *(m_d->tab_2_channelwidgets[tab].begin());
    removeChannel(channelwidget->unique_name());
  }

  //Remove the tab from the tabwidget (this does not actually delete the tab)
  m_d->name_2_tab.erase(m_d->name_2_tab.find(tabname));
  int i = m_d->tabwidget->indexOf(tab);
  assert(i!=-1);
  m_d->tabwidget->removeTab(i);

  delete tab;

  tabListChanged(tabList());
  m_d->dontEmitVisibilityChanges=save;
  currentVisibleChanged();
}


//_______________________________________________________________________
void VP1TabManager::removeAllTabs()
{
	VP1Msg::messageDebug("VP1TabManager::removeAllTabs()");
	foreach(QString tab, tabList() ) {
		removeTab(tab);
	}
}


//___________________________________________________________________________________
void VP1TabManager::removeChannel(QString channeluniquename) {

	VP1Msg::messageDebug("VP1TabManager::removeChannel()");

  if (!m_d->checkChannelNameExists(channeluniquename, true)) return;

  IVP1ChannelWidget* cw = m_d->channelmanager->uniqueName2Channel(channeluniquename);
  assert(cw);

  //Unselect if selected:
  if (m_d->selecteddockwidget&&cw==m_d->selecteddockwidget->channelWidget())
    setSelectedDockWidget(0);

  QString bn = cw->name();
  cw->setUpdatesEnabled(false);

  assert(m_d->channelwidget_2_dockwidget.find(cw)!=m_d->channelwidget_2_dockwidget.end());
  VP1DockWidget * dw = m_d->channelwidget_2_dockwidget[cw];

  //Update maps:
  assert(m_d->channelwidget_2_dockwidget.find(cw)!=m_d->channelwidget_2_dockwidget.end());
  m_d->channelwidget_2_dockwidget.erase(m_d->channelwidget_2_dockwidget.find(cw));

  //And remove dock from tab.
  QMainWindow * tab = m_d->channel2tab(cw);
  assert(tab);

//   int itab = m_d->tabwidget->indexOf(tab);
//   assert(itab!=-1);
//   QString tabname = m_d->tabwidget->tabText(itab);
  //  m_d->tabwidget->removeTab(itab);
  tab->setUpdatesEnabled(false);
  tab->removeDockWidget(dw);


  //Update map:
  assert(m_d->tab_2_channelwidgets[tab].find(cw)!=m_d->tab_2_channelwidgets[tab].end());
  m_d->tab_2_channelwidgets[tab].erase(m_d->tab_2_channelwidgets[tab].find(cw));

  //delete channel
  cw->hide();
  dw->ensureCWHasNoParent();
  bool ok=m_d->channelmanager->deleteChannel(cw->unique_name());
  assert(ok);
  _UNUSED(ok);//To avoid compile warning in opt mode.

  //delete dock widget
  delete dw;

  tab->setUpdatesEnabled(true);
  //  m_d->tabwidget->insertTab(itab,tab,tabname);

  if (m_d->selecteddockwidget==dw) {
    m_d->selecteddockwidget=0;
    selectedChannelChanged(0);
  }

  currentVisibleChanged();
}

//___________________________________________________________________________________
QMainWindow * VP1TabManager::Imp::channel2tab(IVP1ChannelWidget*cw) {
  std::map<QMainWindow*,std::set<IVP1ChannelWidget*> >::iterator it = tab_2_channelwidgets.begin();
  std::map<QMainWindow*,std::set<IVP1ChannelWidget*> >::iterator itE = tab_2_channelwidgets.end();
  for (;it!=itE;++it) {
    if (it->second.find(cw)!=it->second.end())
      return it->first;
  }
  return 0;
}


//___________________________________________________________________________________
QMainWindow * VP1TabManager::Imp::name2tab(QString tabname)
{
  if (name_2_tab.find(tabname)==name_2_tab.end())
    return 0;
  return name_2_tab[tabname];
}

//Fixme: check for empty input strings in all methods where applicable!

//___________________________________________________________________________________
void VP1TabManager::moveChannelToTab(QString channeluniquename,QString tabname) {

  if (!m_d->checkTabnameExists(tabname)) return;
  if (!m_d->checkChannelNameExists(channeluniquename, true)) return;

  QMainWindow * tab_new = m_d->name_2_tab[tabname];
  assert(tab_new);
  IVP1ChannelWidget* cw = m_d->channelmanager->uniqueName2Channel(channeluniquename);
  assert(cw);

  assert(m_d->channelwidget_2_dockwidget.find(cw)!=m_d->channelwidget_2_dockwidget.end());

  VP1DockWidget* dw = m_d->channelwidget_2_dockwidget[cw];
  assert(dw);
  QMainWindow * tab_old = m_d->channel2tab(cw);
  assert(tab_old);

  if (tab_old==tab_new)//Fixme: Make warning here instead, and ensure that the interface does not allow this.
    return;

  tab_old->removeDockWidget(dw);
  dw->setParent(tab_new);
  tab_new->addDockWidget(Qt::TopDockWidgetArea, dw);
  dw->show();

  //Update tab_2_channelwidgets:
  assert(m_d->tab_2_channelwidgets.find(tab_old)!=m_d->tab_2_channelwidgets.end());
  assert(m_d->tab_2_channelwidgets.find(tab_new)!=m_d->tab_2_channelwidgets.end());
  assert(m_d->tab_2_channelwidgets[tab_old].find(cw)!=m_d->tab_2_channelwidgets[tab_old].end());
  assert(m_d->tab_2_channelwidgets[tab_new].find(cw)==m_d->tab_2_channelwidgets[tab_new].end());//dies!
  m_d->tab_2_channelwidgets[tab_old].erase(m_d->tab_2_channelwidgets[tab_old].find(cw));
  m_d->tab_2_channelwidgets[tab_new].insert(cw);

}

//___________________________________________________________________________________
void VP1TabManager::cloneChannelToTab(QString channeluniquename,QString tabname) {
  if (!m_d->checkTabnameExists(tabname)) return;
  if (!m_d->checkChannelNameExists(channeluniquename, true)) return;
  IVP1ChannelWidget* cw = m_d->channelmanager->uniqueName2Channel(channeluniquename);
  assert(cw);
  IVP1ChannelWidget * newcw = addChannelToTab(cw->name(),tabname);
  if (newcw) {
    ChanState state;
    serializeChannelState(cw,state);
    unserializeChannelState(newcw,state);
  }
}

//___________________________________________________________________________________
void VP1TabManager::cloneTab(QString oldtabname,QString newtabname)
{
  if (!m_d->checkTabnameExists(oldtabname)) return;
  if (!m_d->checkTabnameNotExists(newtabname)) return;

  addNewTab(newtabname);

  assert(m_d->name_2_tab.find(newtabname)!=m_d->name_2_tab.end());

  QMainWindow * oldtab = m_d->name_2_tab[oldtabname];

  assert(m_d->tab_2_channelwidgets.find(oldtab)!=m_d->tab_2_channelwidgets.end());
  std::set<IVP1ChannelWidget*>::const_iterator it = m_d->tab_2_channelwidgets[oldtab].begin();
  std::set<IVP1ChannelWidget*>::const_iterator itE = m_d->tab_2_channelwidgets[oldtab].end();

  for (;it!=itE;++it) {
    IVP1ChannelWidget * newcw = addChannelToTab((*it)->name(),newtabname);
    if (newcw) {
      ChanState state;
      serializeChannelState(*it,state);
      unserializeChannelState(newcw,state);
    }
  }

  QMainWindow * newtab = m_d->name_2_tab[newtabname];
  QByteArray state = oldtab->saveState();
  if (!newtab->restoreState(state))
    QMessageBox::warning(0, "Warning - Problems cloning channel arrangement",
			 "Problems cloning channel arrangement.",QMessageBox::Ok,QMessageBox::Ok);
}



//___________________________________________________________________________________
void VP1TabManager::saveConfigurationToFile(QString filename,const bool& askonoverride) {
  if (filename.isEmpty()) {
    QMessageBox::critical(0, "Error - Empty file name provided",
			  "Empty file name provided.",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }
  //If file exists, ask to overwrite.
  QFileInfo fi(filename);
  if (fi.exists()) {
    if (!fi.isWritable()) {
      QMessageBox::critical(0, "Error - could not save to file "+filename,
			    "Could not save to file: <i>"+filename+"</i>"
			    +"<br/><br/>Reason: File exists already and is write protected",QMessageBox::Ok,QMessageBox::Ok);
      return;
    }
    if (askonoverride) {
      int ret = QMessageBox::question(0, "File already exists: "+filename,
				      "The file <i>"+filename+"</i> already exists.\n"
				      "Override this file?",
				      QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
      if (ret==QMessageBox::Cancel)
	return;
    }
  }

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::critical(0, "Error - problems writing to file "+filename,
			    "Problems writing to file: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
      return;
  }

  //Serialise :
  QMap<QString,QMultiMap<QString,ChanState> > tab2channels;
  QMap<QString,QByteArray> tab2arrangements;
  m_d->serializeTabAndChannelConfigInfo(tab2channels, tab2arrangements);

  //Put into bytearray:
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  buffer.open(QIODevice::WriteOnly);
  QDataStream out(&buffer);
  out<<QString("This is an automatically generated config file for VP1. [cfg version 003]" );
  out<<m_d->channelmanager->serializePluginInfo();
  out<<tab2channels;
  out<<tab2arrangements;
  out<<tabList();//For tab *order*
  out<<QString("This is the end of the automatically generated config file for VP1." );
  buffer.close();

  QDataStream outfile(&file);
  outfile<<qCompress(byteArray).toBase64();


}

//___________________________________________________________________________________
void VP1TabManager::loadConfigurationFromFile(QString filename,const QMap<QString,QString>& availableplugins) {
  if (filename.isEmpty()) {
    QMessageBox::critical(0, "Error - Empty file name provided",
			  "Empty file name provided.",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }
  QFileInfo fi(filename);
  if (!fi.exists()) {
    QMessageBox::critical(0, "Error - file does not exists: "+filename,
			  "File does not exists: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }
  if (!fi.isReadable()) {
    QMessageBox::critical(0, "Error - file is not readable: "+filename,
			  "File is not readable: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }
  //open file
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::critical(0, "Error - problems opening file "+filename,
			    "Problems opening file: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
      return;
  }

  //Read:
  QString head,foot;
  QStringList pluginfiles;
  QMap<QString,QMultiMap<QString,ChanState> > tab2channels;
  QMap<QString,QByteArray> tab2arrangements;
  QStringList tabs_orded;

  QByteArray byteArray64;
  QDataStream infile(&file);
  infile >> byteArray64;

  QByteArray byteArray = qUncompress(QByteArray::fromBase64(byteArray64));

  QBuffer buffer(&byteArray);
  buffer.open(QIODevice::ReadOnly);
  QDataStream in(&buffer);
  in >> head;
  in >> pluginfiles;
  in >> tab2channels;
  in >> tab2arrangements;
  in >> tabs_orded;
  in >> foot;
  buffer.close();

  if (head!="This is an automatically generated config file for VP1. [cfg version 003]"
	||foot!="This is the end of the automatically generated config file for VP1."
      ||tab2channels.count()!=tab2arrangements.count()) {
    QMessageBox::critical(0, "Error - file not in correct format: "+filename,
			  "File not in correct format: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
    return;
  }

  QStringList pf_withproblem;
  QStringList errormessage;

#ifdef __APPLE__
  QString sharedlibsuffix_correct = ".dylib";
  QString sharedlibsuffix_wrong = ".so";
#else
  QString sharedlibsuffix_correct = ".so";
  QString sharedlibsuffix_wrong = ".dylib";
#endif
  
  foreach(QString pf,pluginfiles) {
    if (pf.endsWith(sharedlibsuffix_wrong)) {
      pf.replace(sharedlibsuffix_wrong, sharedlibsuffix_correct);
    } 
    
    if (!availableplugins.contains(pf)) {
      pf_withproblem<<pf;
      errormessage<<"Could not locate " + pf;
      continue;
    }
    QString pfabsolute = availableplugins[pf];
    QString err;
    if (m_d->channelmanager->channelsInPluginFile(pfabsolute).empty())
      err = m_d->channelmanager->loadPluginFile(pfabsolute);
    if (!err.isEmpty()) {
      pf_withproblem<<pfabsolute;
      errormessage<<err;
    }
  }

  if (!pf_withproblem.empty()) {
    assert(pf_withproblem.count()==errormessage.count());
    QString tmp;
    for (int i = 0; i<pf_withproblem.count();++i) {
      tmp += pf_withproblem.value(i)+": "+errormessage.value(i)+"\n";
    }
    QMessageBox::warning(0, "Warning - could not load all plugins",
			 "Some or more plugins did not load properly:\n\n"+tmp,QMessageBox::Ok,QMessageBox::Ok);
  }

  QString channelsFailureMsg;
  QString tabArrangementFailureMsg;

  QString lastaddedtab;

  foreach (QString newtabname_infile, tabs_orded) {
    //Check format:
    if (!tab2channels.contains(newtabname_infile)||!tab2arrangements.contains(newtabname_infile)) {
      QMessageBox::critical(0, "Error - file not in correct format: "+filename,
			    "File not in correct format: <i>"+filename+"</i>",QMessageBox::Ok,QMessageBox::Ok);
      return;
    }

    //We are about to add a new tab. If there is already one with
    //that name we remove it if it is empty:
    QMainWindow * existingtab = m_d->name2tab(newtabname_infile);
    if (existingtab) {
      if (m_d->tab_2_channelwidgets.find(existingtab)==m_d->tab_2_channelwidgets.end()
	  ||m_d->tab_2_channelwidgets[existingtab].empty())
	removeTab(newtabname_infile);
    }

    //Special case: If there are presently no loaded channels, and
    //only one existing tab named 'My Tab', then we remove that one
    if (m_d->channelwidget_2_dockwidget.empty()
	&&m_d->name_2_tab.size()==1
	&&m_d->name_2_tab.begin()->first=="My Tab")
      removeTab("My Tab");

    QString newtabname = suggestNewTabName(newtabname_infile);
    addNewTab(newtabname);
    if (!m_d->checkTabnameExists(newtabname))
      return;
    lastaddedtab=newtabname;

    QMapIterator<QString,ChanState> it( tab2channels.value(newtabname_infile) );

    while (it.hasNext()) {
      it.next();
      //it.key(): Channel base name.
      //it.value(): Channel state info.
      if (!m_d->channelmanager->baseNameExists(it.key())) {
	channelsFailureMsg += it.key()+" (tab "+newtabname+")"+"\n";
      } else {
	IVP1ChannelWidget * cw = addChannelToTab( it.key(), newtabname );
	if (cw)
	  unserializeChannelState(cw,it.value());
	else
	  channelsFailureMsg += it.key()+" (tab "+newtabname+")"+"\n";
      }
    }

    //Setup layout state:
    assert(m_d->name_2_tab.find(newtabname)!=m_d->name_2_tab.end());
    QMainWindow * tab = m_d->name_2_tab.find(newtabname)->second;
    QByteArray state = tab2arrangements.value(newtabname_infile);

    if (!state.isEmpty()) {
      if (!tab->restoreState(state,0/*version*/)) {
	tabArrangementFailureMsg += newtabname+"\n";
      }
    }
  }
  if (!channelsFailureMsg.isEmpty()) {
    QMessageBox::warning(0, "Warning - could not start all channels",
			 "Some or more channels were not available to start:\n\n"
			 +channelsFailureMsg,QMessageBox::Ok,QMessageBox::Ok);
  }
  if (!tabArrangementFailureMsg.isEmpty()) {
    QMessageBox::warning(0, "Warning - could not arrange channels in all tabs",
			 "The arrangement of channels could not be properly determined withing the following tabs:\n\n"
			 +tabArrangementFailureMsg,QMessageBox::Ok,QMessageBox::Ok);
  }
  if (!lastaddedtab.isEmpty())
    showTab(lastaddedtab);
}

//___________________________________________________________________________________
void VP1TabManager::Imp::serializeTabAndChannelConfigInfo(QMap<QString,QMultiMap<QString,ChanState> >& tab2channels,
							  QMap<QString,QByteArray>& tab2arrangements) const {

  tab2channels.clear();
  tab2arrangements.clear();
  for (int i = 0; i< tabwidget->count();++i) {
    QString tabname = tabwidget->tabText(i);
    assert(name_2_tab.find(tabname)!=name_2_tab.end());
    QMainWindow * tab = name_2_tab.find(tabname)->second;
    QMultiMap<QString,ChanState> channelsinfo;
    std::map<QMainWindow*,std::set<IVP1ChannelWidget*> >::const_iterator itcws=tab_2_channelwidgets.find(tab);
    assert(itcws!=tab_2_channelwidgets.end());
    std::set<IVP1ChannelWidget*>::const_iterator it = itcws->second.begin();
    std::set<IVP1ChannelWidget*>::const_iterator itE = itcws->second.end();
    for (;it!=itE;++it) {
      ChanState chanstate;
      tabmanager->serializeChannelState(*it,chanstate);
      channelsinfo.insert((*it)->name(),chanstate);
    }
    tab2channels.insert(tabname,channelsinfo);


    tab2arrangements.insert(tabname,tab->saveState(0/*version*/));
  }
}

//___________________________________________________________________________________
QString VP1TabManager::suggestNewTabName(QString oldtabname) const {
  QString newtabname=oldtabname;
  int i = 1;
  while (m_d->name_2_tab.find(newtabname)!=m_d->name_2_tab.end())
    newtabname=oldtabname+" "+QString::number(++i);
  return newtabname;
}

//___________________________________________________________________________________
QString VP1TabManager::currentTab() const {
  if (m_d->tabwidget->count()==0)
    return "";
  return m_d->tabwidget->tabText(m_d->tabwidget->currentIndex());
}

//___________________________________________________________________________________
int VP1TabManager::nTabs() const {
  return m_d->tabwidget->count();
}

//___________________________________________________________________________________
QMainWindow* VP1TabManager::Imp::nextTab(){
  if (tabwidget->count()<=1)
    return 0;
  std::cout<<"currentindex:"<<tabwidget->currentIndex()<<std::endl;
  std::cout<<"nextindex:"<<(tabwidget->currentIndex() + 1) % tabwidget->count()<<std::endl;
  std::cout<<"ntabs:"<<tab_2_channelwidgets.size()<<std::endl;
  QMainWindow*nexttab=static_cast<QMainWindow*>(tabwidget->widget((tabwidget->currentIndex() + 1) % tabwidget->count()));
  assert(nexttab);
  assert(tab_2_channelwidgets.find(nexttab)!=tab_2_channelwidgets.end());
//   if (tab_2_channelwidgets.find(nexttab)==tab_2_channelwidgets.end())
//     return fullscreen_tab;
//   else
  return nexttab;
}

//___________________________________________________________________________________
QMainWindow* VP1TabManager::Imp::previousTab(){
  if (tabwidget->count()<=1)
    return 0;
  int newindex = tabwidget->currentIndex() - 1;
  if (newindex<0)
    newindex += tabwidget->count();
  QMainWindow*prevtab = static_cast<QMainWindow*>(tabwidget->widget(newindex % tabwidget->count()));
  if (tab_2_channelwidgets.find(prevtab)==tab_2_channelwidgets.end())
    return fullscreen_tab;
  else
    return prevtab;
}

//___________________________________________________________________________________
bool VP1TabManager::showTab(QString tabname){
  int itarget(-1);
  for (int i = 0; i < m_d->tabwidget->count(); ++i) {
    if (m_d->tabwidget->tabText(i)==tabname) {
      itarget = i;
      break;
    }
  }
  if (itarget<0)
    return false;
  if (itarget==m_d->tabwidget->currentIndex())
    return true;

  bool save = m_d->dontEmitVisibilityChanges;
  m_d->dontEmitVisibilityChanges=true;

  bool fullscreentab = (m_d->fullscreen_tab!=0);
  if (m_d->fullscreen_channelwidget||m_d->fullscreen_tab)
    dropOutOfFullScreen();

  m_d->tabwidget->setCurrentIndex(itarget);
  if (fullscreentab)
    showCurrentTabFullScreen();
  m_d->dontEmitVisibilityChanges=save;
  currentVisibleChanged();
  return true;
}


//___________________________________________________________________________________
void VP1TabManager::showNextTab(){
  if (m_d->tabwidget->count()<=1)
    return;
  assert(!m_d->fullscreen_channelwidget);
  if (m_d->fullscreen_tab) {
    bool save = m_d->dontEmitVisibilityChanges;
    m_d->dontEmitVisibilityChanges=true;
    dropOutOfFullScreen();
    showNextTab();
    showCurrentTabFullScreen();
    m_d->dontEmitVisibilityChanges=save;
  }
  m_d->tabwidget->setCurrentIndex((m_d->tabwidget->currentIndex() + 1) % m_d->tabwidget->count());
}

//___________________________________________________________________________________
void VP1TabManager::showPreviousTab() {
  assert(!m_d->fullscreen_channelwidget);
  assert(!m_d->fullscreen_tab);//Fixme: as above.
  if (m_d->tabwidget->count()<=1)
    return;
  int newindex = m_d->tabwidget->currentIndex() - 1;
  if (newindex<0)
    newindex += m_d->tabwidget->count();
  m_d->tabwidget->setCurrentIndex( newindex % m_d->tabwidget->count());
}

//___________________________________________________________________________________
void VP1TabManager::raiseTabBarContextMenu(int i,const QPoint & p) {

  QString tabname = m_d->tabwidget->tabText(i);

  QMenu menu(m_d->tabwidget);

  //Construct menu:
  //   menu.addAction("Tab: "+tabname)->setEnabled(false);
  //   menu.addSeparator ();
  QAction* pFullScreenAction = menu.addAction("Show &full Screen");
  menu.addSeparator();
  QAction* pRenameAction = menu.addAction("Re&name tab");
  QAction* pDeleteAction = menu.addAction("&Remove tab");
  menu.addSeparator ();
  QAction* pInsertNewAction = menu.addAction("&Insert new tab");
  QAction* pCloneAction = menu.addAction("&Clone tab");
  menu.addSeparator ();
  QAction* pAddChannelAction = menu.addAction("&Add channel");
  QAction* pRemoveChannelAction = menu.addAction("R&emove channel");

  QMenu menu_addchan(m_d->tabwidget);
  QStringList chnls = m_d->channelmanager->availableChannelList();
  if (chnls.empty()) {
    menu_addchan.addAction("No channels available")->setEnabled(false);
  } else {
    foreach (QString chnl, chnls) {
      QString iconloc = m_d->channelmanager->getIconLocation(chnl, true);
      QAction* pChnlAct;
      if (iconloc.isEmpty())
	pChnlAct = menu_addchan.addAction(chnl);
      else
	pChnlAct = menu_addchan.addAction(QIcon(iconloc),chnl);
      pChnlAct->setData("ADDCHAN");
    }
  }
  pAddChannelAction->setMenu(&menu_addchan);
  QStringList chnls_rem = m_d->channelsInTab(tabname);
  if (chnls_rem.empty())
    pFullScreenAction->setEnabled(false);
  QMenu menu_remchan(m_d->tabwidget);
  if (chnls_rem.empty()) {
    menu_remchan.addAction("No channels in tab")->setEnabled(false);
  } else {
    foreach (QString chnl, chnls_rem) {
      QString iconloc = m_d->channelmanager->getIconLocation(chnl, false);
      QAction* pChnlAct;
      if (iconloc.isEmpty())
	pChnlAct = menu_remchan.addAction(chnl);
      else
	pChnlAct = menu_remchan.addAction(QIcon(iconloc),chnl);
      pChnlAct->setData("REMCHAN");
    }
  }
  pRemoveChannelAction->setMenu(&menu_remchan);


  //Execute
  QAction * selAct = menu.exec(p);
  //
  if (!selAct)
    return;
  if (selAct==pFullScreenAction) {
    showTabFullScreen(tabname);
    return;
  }
  if (selAct==pRenameAction) {
    bool ok;
    QString text = QInputDialog::getText( 0, "Rename tab '"+tabname+"'","Rename tab '"+tabname+"' to:",
					  QLineEdit::Normal, tabname, &ok );
    if (!ok||text==tabname)
      return;
    renameTab(tabname,text);
    return;
  }
  if (selAct==pDeleteAction) {
    removeTab( tabname );
    return;
  }
  if (selAct==pInsertNewAction) {
    bool ok;
    QString newtabname = QInputDialog::getText( 0, "New Tab Name","New tab name:",
						QLineEdit::Normal, suggestNewTabName("My Tab"), &ok );
    if (!ok||newtabname.isEmpty())
      return;
    addNewTab(newtabname,i+1);
    return;
  }
  if (selAct==pCloneAction) {
    bool ok;
    QString newtabname = QInputDialog::getText( 0, "Cloning '"+tabname+"'.","Cloning '"+tabname+"'. Name of new tab:",
						QLineEdit::Normal, suggestNewTabName(tabname), &ok );
    if (!ok||newtabname.isEmpty())
      return;
    cloneTab(tabname,newtabname);
    return;
  }
  if (selAct==pAddChannelAction) {
    return;
  }
  //Only option left is channels...
  if (selAct->data()=="ADDCHAN") {
    addChannelToTab( selAct->text(), tabname );
    return;
  }
  if (selAct->data()=="REMCHAN") {
    removeChannel( selAct->text() );
    return;
  }
  std::cout<<"ERROR in VP1TabManager::raiseTabBarContextMenu!!!!!"<<std::endl;
}

//___________________________________________________________________________________
void VP1TabManager::currentVisibleChanged() {
  if (m_d->dontEmitVisibilityChanges)
    return;

  QSet<IVP1ChannelWidget*> visible;
  QSet<IVP1ChannelWidget*> soonvisible;
  double soonbonus(0.0);

  /////////////////////////////////////////////////////////////////////
  ////////////////////////////   VISIBLE   ////////////////////////////
  /////////////////////////////////////////////////////////////////////

  if (m_d->fullscreen_dockwidget) {
    //Thats an easy one:
    visible << m_d->fullscreen_dockwidget->channelWidget();
  } else if (m_d->fullscreen_tab) {
    //Add all channel widgets from this tab:
    assert(m_d->tab_2_channelwidgets.find(m_d->fullscreen_tab)!=m_d->tab_2_channelwidgets.end());
    std::set<IVP1ChannelWidget*>::iterator  it,itE;
    itE = m_d->tab_2_channelwidgets[m_d->fullscreen_tab].end();
    for (it = m_d->tab_2_channelwidgets[m_d->fullscreen_tab].begin();it!=itE;++it) {
      visible << *it;
    }
    std::cout<<"fullscreen tab:"<<m_d->fullscreen_tab<<std::endl;
  } else {
    //Go through the tabs, if it is the current tab we add all dock
    //widgets, otherwise we add those that are floating:
    QWidget * currentTab = m_d->tabwidget->currentWidget();
    std::map<QMainWindow*,std::set<IVP1ChannelWidget*> >::iterator tabIt,tabItE = m_d->tab_2_channelwidgets.end();
    std::set<IVP1ChannelWidget*>::iterator  it,itE;
    for (tabIt=m_d->tab_2_channelwidgets.begin();tabIt!=tabItE;++tabIt) {
      //Loop over channels;
      it = tabIt->second.begin();
      itE = tabIt->second.end();
      for (;it!=itE;++it) {
	if (currentTab==tabIt->first) {
	  //Add all:
	  visible << *it;
	} else {
	  if (!m_d->fullscreen_tab) {
	    //Add only if floating and not a fullscreen tab:
	    assert(m_d->channelwidget_2_dockwidget.find(*it)!=m_d->channelwidget_2_dockwidget.end());
	    if (m_d->channelwidget_2_dockwidget[*it]->isFloating())
	      visible << *it;
	  }
	}
      }
    }
  }

  /////////////////////////////////////////////////////////////////////
  /////////////////////////   SOON VISIBLE   //////////////////////////
  /////////////////////////////////////////////////////////////////////

  if (m_d->fullscreen_dockwidget) {
    assert(!m_d->tabcruisemode);
    //Soon visible: All floating channels (not this one) and all channels in the present tab.
    soonbonus = 2000;//We are afterall quite sure here what will soon be visible.
    QWidget * currentTab = m_d->tabwidget->currentWidget();
    std::map<QMainWindow*,std::set<IVP1ChannelWidget*> >::iterator tabIt, tabItE = m_d->tab_2_channelwidgets.end();
    std::set<IVP1ChannelWidget*>::iterator  it,itE;
    for (tabIt=m_d->tab_2_channelwidgets.begin();tabIt!=tabItE;++tabIt) {
      //Loop over channels;
      itE = tabIt->second.end();
      for (it=tabIt->second.begin();it!=itE;++it) {
	if (currentTab==tabIt->first) {
	  //Add all except the fullscreen'ed one:
	  if (*it!=m_d->fullscreen_dockwidget->channelWidget())
	    soonvisible << *it;
	} else {
	  //Add only if floating:
	  assert(m_d->channelwidget_2_dockwidget.find(*it)!=m_d->channelwidget_2_dockwidget.end());
	  if (m_d->channelwidget_2_dockwidget[*it]->isFloating())
	    soonvisible << *it;
	}
      }
    }
  } else if (m_d->fullscreen_tab) {
    if (m_d->tabcruisemode&&m_d->tab_2_channelwidgets.size()>1) {
      //Everything in the next tab.
      soonbonus = 10000;//Very important that the stuff in the next tab gets a high priority!
      QMainWindow * nexttab = m_d->nextTab();
      assert(nexttab);
      std::cout<<"fullscreen tab:"<<m_d->fullscreen_tab<<std::endl;
      std::cout<<"nexttab:"<<nexttab<<std::endl;
      assert(m_d->tab_2_channelwidgets.find(nexttab)!=m_d->tab_2_channelwidgets.end());
      std::set<IVP1ChannelWidget*>::iterator  it,itE=m_d->tab_2_channelwidgets[nexttab].end();
      for(it=m_d->tab_2_channelwidgets[nexttab].begin();it!=itE;++it){
	soonvisible << *it;
      }
    } else {
      //All floating channels not in the present tab
      soonbonus = 1000;//We are rather sure here what will soon be visible.
      QWidget * currentTab = m_d->tabwidget->currentWidget();
      std::map<QMainWindow*,std::set<IVP1ChannelWidget*> >::iterator tabIt, tabItE = m_d->tab_2_channelwidgets.end();
      std::set<IVP1ChannelWidget*>::iterator  it,itE;
      for (tabIt=m_d->tab_2_channelwidgets.begin();tabIt!=tabItE;++tabIt) {
	//Loop over channels;
	itE = tabIt->second.end();
	for (it=tabIt->second.begin();it!=itE;++it) {
	  if (m_d->fullscreen_tab!=tabIt->first&&currentTab!=tabIt->first) {
	    //Add only if floating:
	    assert(m_d->channelwidget_2_dockwidget.find(*it)!=m_d->channelwidget_2_dockwidget.end());
	    if (m_d->channelwidget_2_dockwidget[*it]->isFloating())
	      soonvisible << *it;
	  }
	}
      }
    }
  } else {
    //Add everything non-floating from the next tab.
    soonbonus = 200;//A weak guess (fixme: Stronger if tab cruise mode).
    if (m_d->tabwidget->count()>=2) {
      int nexttabindex = (m_d->tabwidget->currentIndex() + 1) % m_d->tabwidget->count();
      QMainWindow* tab = static_cast<QMainWindow*>(m_d->tabwidget->widget(nexttabindex));
      assert(tab);
      assert(m_d->tab_2_channelwidgets.find(tab)!=m_d->tab_2_channelwidgets.end());
      std::set<IVP1ChannelWidget*>::iterator  it,itE=m_d->tab_2_channelwidgets[tab].end();
      for (it=m_d->tab_2_channelwidgets[tab].begin();it!=itE;++it) {
	assert(m_d->channelwidget_2_dockwidget.find(*it)!=m_d->channelwidget_2_dockwidget.end());
	if (!m_d->channelwidget_2_dockwidget[*it]->isFloating())
	  soonvisible << *it;
      }
    }
  }


  assert(soonbonus>0.0);

#ifndef NDEBUG
  //There should be no intersection between visible and soon visible!
  QSet<IVP1ChannelWidget*> tmp = visible;
  tmp.intersect(soonvisible);
  assert(tmp.empty());
#endif

  if (visible!=m_d->lastvisible||soonvisible!=m_d->lastsoonvisible) {
    m_d->lastvisible=visible;
    m_d->lastsoonvisible=soonvisible;
    visibleChannelsChanged(visible,soonvisible,soonbonus);

    //To ensure that we dont show controllers for channels that are
    //not visible:
    if (m_d->selecteddockwidget) {
      if (!visible.contains(m_d->selecteddockwidget->channelWidget()))
	setSelectedDockWidget(0);
    }

    //To make sure we automatically select a channel if it is the only
    //one visible:
    if (visible.count()==1&&(!m_d->selecteddockwidget||*(visible.begin())!=m_d->selecteddockwidget->channelWidget())) {
      assert(m_d->channelwidget_2_dockwidget.find(*(visible.begin()))!=m_d->channelwidget_2_dockwidget.end());
      setSelectedDockWidget(m_d->channelwidget_2_dockwidget[*(visible.begin())]);
    }
  }
}

//___________________________________________________________________________________
QString VP1TabManager::channelToTab(IVP1ChannelWidget*cw)
{
  QMainWindow * tab = m_d->channel2tab(cw);
  if (!tab)
    return "VP1TabManager::channelToTab ERROR: Unknown CW";

  std::map<QString,QMainWindow *>::const_iterator it, itE = m_d->name_2_tab.end();
  for ( it = m_d->name_2_tab.begin(); it!=itE; ++it ) {
    if (it->second == tab)
      return it->first;
  }
  return "";
}

//___________________________________________________________________________________
bool VP1TabManager::hasTab(QString tabname) const
{
  return m_d->name_2_tab.find(tabname)!=m_d->name_2_tab.end();
}

//___________________________________________________________________________________
bool VP1TabManager::showFirstChannelWithGivenBasename(QString basename) {

  std::set<IVP1ChannelWidget*>::const_iterator it2,it2E;

  std::map<QMainWindow*,std::set<IVP1ChannelWidget*> >::const_iterator
    it = m_d->tab_2_channelwidgets.begin(), itE=m_d->tab_2_channelwidgets.end();
  for (;it!=itE;++it) {
    it2=it->second.begin();
    it2E=it->second.end();
    for (;it2!=it2E;++it2) {
      if ((*it2)->name()==basename) {
	//Figure out the tabname:
	std::map<QString,QMainWindow *>::const_iterator it3,it3E;
	it3=m_d->name_2_tab.begin();it3E=m_d->name_2_tab.end();
	for (;it3!=it3E;++it3) {
	  if (it3->second==it->first)
	    return showTab(it3->first);
	}
	//Something is wrong.
	return false;
      }
    }
  }
  return false;
}


//___________________________________________________________________________________
QList<IVP1ChannelWidget*> VP1TabManager::allChannels() const
{
  QList<IVP1ChannelWidget*> l;
  std::map<IVP1ChannelWidget*,VP1DockWidget*>::iterator it, itE(m_d->channelwidget_2_dockwidget.end());
  for(it=m_d->channelwidget_2_dockwidget.begin();it!=itE;++it) {
    l <<it->first;
  }
  return l;
}


//___________________________________________________________________________________
const QSet<IVP1ChannelWidget*>& VP1TabManager::visibleChannels() const
{
  return m_d->lastvisible;
}
//___________________________________________________________________________________
const QSet<IVP1ChannelWidget*>& VP1TabManager::soonVisibleChannels() const
{
  return m_d->lastsoonvisible;
}


//___________________________________________________________________________________
bool VP1TabManager::isVisible(IVP1ChannelWidget*cw) const
{
  return m_d->lastvisible.contains(cw);
}

//___________________________________________________________________________________
void VP1TabManager::setTabCruiseMode(const bool& b) {
  if (m_d->tabcruisemode==b)
    return;
  bool save = m_d->dontEmitVisibilityChanges;
  m_d->dontEmitVisibilityChanges = true;
  m_d->tabcruisemode = b;
//   if (m_d->tabcruisemode) {
//     assert(!fullscreen_dockwidget);//We cant be in FS single-channel mode.
//     if (fullscreen_tab) {
//       //Dock all floating (in other tabs) immediately (fixme: only when they need to be shown!.
//     } else {
//       //normal mode
//     }
//   } else {
//   }
  //Soonvisible might have changed - so emit changes:
  m_d->dontEmitVisibilityChanges=save;
  currentVisibleChanged();
}




//___________________________________________________________________________________
void VP1TabManager::removeChannelAfterQueueEmpties(const QString& chnlun)
{
  m_d->channelWithPendingRemoval = chnlun;
  QTimer::singleShot(0, this, SLOT(executePendingChannelRemoval()));
}

//___________________________________________________________________________________
void VP1TabManager::executePendingChannelRemoval()
{
  if (m_d->channelWithPendingRemoval.isEmpty())
    return;
  removeChannel(m_d->channelWithPendingRemoval);
  m_d->channelWithPendingRemoval="";
}

//___________________________________________________________________________________
void VP1TabManager::serializeChannelState(IVP1ChannelWidget*cw,ChanState&state)
{
  QMap<QString,QByteArray> sysstate;
  std::set<IVP1System*>::const_iterator it, itE = cw->systems().end();
  for (it=cw->systems().begin();it!=itE;++it) {
    unsigned nVP1Serialise = VP1Serialise::numberOfInstantiations();
    sysstate.insertMulti((*it)->name(),(*it)->saveState());
    if (nVP1Serialise==VP1Serialise::numberOfInstantiations())
      VP1Msg::messageDebug("VP1TabManager WARNING: System "+(*it)->name()
			   +" did not use VP1Serialise in implementation of saveState()");
  }

  state.first = cw->saveState();
  state.second = sysstate;
}

//___________________________________________________________________________________
void VP1TabManager::unserializeChannelState(IVP1ChannelWidget*cw,ChanState state)
{
  if(!state.first.isEmpty())
    cw->restoreFromState(state.first);

  QList<QString> storedSystems = state.second.keys();

  std::set<IVP1System*>::const_iterator itsys, itsysE = cw->systems().end();
  for (itsys=cw->systems().begin();itsys!=itsysE;++itsys) {
    QString name = (*itsys)->name();
    //Look for the first entry with this system name in the list. If it is there - use it and remove it:
    QMultiMap<QString,QByteArray>::iterator it_state = state.second.find(name);
    if (it_state != state.second.end()) {
      storedSystems.removeAll(name);
//       //Lets make sure that we advance it_state to the LAST of the elements with the same key:
//       QMultiMap<QString,QByteArray>::iterator it_state_tmp = it_state;
//       while ( it_state_tmp != state.second.end() && it_state_tmp.key() == (*itsys)->name() ) {
// 	it_state=it_state_tmp;
// 	++it_state_tmp;
//       }
      //Fixme ^^^: The above has not been tested on any channel which
      //actually has multiple similar systems. So might be buggy.

      //Use it and remove it from array:
      if (!it_state.value().isEmpty()) {
	unsigned nVP1Deserialise = VP1Deserialise::numberOfInstantiations();
	(*itsys)->restoreFromState(it_state.value());
	if (nVP1Deserialise==VP1Deserialise::numberOfInstantiations())
	  VP1Msg::messageDebug("VP1TabManager WARNING: System "+(*itsys)->name()
			       +" did not use VP1Deserialise in implementation of restoreFromState()");
      }
      //state.second.erase(it_state);
    } else {
      std::cout<<"VP1TabManager::unserializeChannelState Warning: Did not find state data for system "<<name.toStdString()<<std::endl;
    }
  }
  foreach (QString name,storedSystems)
    std::cout<<"VP1TabManager::unserializeChannelState Warning: Did not use stored configuration for system "<<name.toStdString()<<std::endl;
}
