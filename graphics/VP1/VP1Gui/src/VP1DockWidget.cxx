/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1DockWidget                  //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Gui/VP1DockWidget.h"
#include "VP1Gui/VP1TabManager.h"
#include "VP1Base/IVP1ChannelWidget.h"

#include <QFrame>
#include <QEvent>
#include <QSet>
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>

#include <cassert>
#include <iostream>

//____________________________________________________________________
class VP1DockWidget::Imp {
public:
  class FilterItem;
  Imp(VP1DockWidget*_dw,IVP1ChannelWidget * cw,VP1TabManager * tabmanager);
  Imp (const Imp&) = delete;
  Imp& operator= (const Imp&) = delete;
  FilterItem* filteritem;
  VP1DockWidget * dw;
  QFrame * frame;
  QVBoxLayout * vboxLayout;
  IVP1ChannelWidget * channelwidget;
  int unselectedmargin;
  QString selectedstylesheet;
  bool selected;
  QString title_refreshpart;
  VP1TabManager* tabmanager;
};

//____________________________________________________________________
class VP1DockWidget::Imp::FilterItem : public QObject {
public:
  FilterItem(VP1DockWidget* dw) : QObject(dw),m_dw(dw) {}
  VP1DockWidget* m_dw;

  void init() {
    installEventFilterRecursively(m_dw);
    m_activationevents.insert(QEvent::ContextMenu);
    //m_activationevents.insert(QEvent::EnterEditFocus); --> Does not compile.
    m_activationevents.insert(QEvent::FocusIn);
    m_activationevents.insert(QEvent::KeyPress);
    m_activationevents.insert(QEvent::MouseButtonDblClick);
    m_activationevents.insert(QEvent::MouseButtonPress);
    m_activationevents.insert(QEvent::TabletPress);
    m_activationevents.insert(QEvent::Wheel);
  }

protected:
  QSet<QEvent::Type> m_activationevents;

  bool eventFilter ( QObject * watched, QEvent * event ) {

    if (event->type()==QEvent::ChildAdded) {
      //Maybe the user added a widget, so update the event filters (todo: test that it works):
      foreach(QObject* c, watched->children()) {
	      installEventFilterRecursively(c,true);
	      installEventFilterRecursively(c);
      }
      return false;
    }

    if ( !m_dw->m_d->selected && m_activationevents.contains(event->type()) ) {
      m_dw->setSelected();
      m_dw->wasSelected(m_dw);
    }
    //     if ( watched==m_dw && event->type() == QEvent::Enter )
    //       m_dw->channelWidget()->setFocus(Qt::OtherFocusReason);

    return false;
  }

  void installEventFilterRecursively(QObject * child, const bool& remove=false ) {
    //NB: we install it on all QObjects in the tree rather than just
    //the QWidgets, since we need to watch for ChildAdded events.
    if (child==this)
      return;
    if (!child){
      std::cerr<<"installEventFilterRecursively: Child is NULL. Aborting."<<std::endl;
      return;
    }
    if (remove)
      child->removeEventFilter(this);
    else
      child->installEventFilter(this);
    foreach(QObject* c, child->children())
      installEventFilterRecursively(c,remove);
  }
};

//____________________________________________________________________
VP1DockWidget::Imp::Imp(VP1DockWidget* the_dw,IVP1ChannelWidget * cw,VP1TabManager * the_tabmanager)
  : filteritem(new FilterItem(the_dw)), dw(the_dw), frame(0), vboxLayout(0),channelwidget(cw),
    unselectedmargin(0),selected(false),tabmanager(the_tabmanager) {}

//____________________________________________________________________
VP1DockWidget::~VP1DockWidget()
{
  delete m_d->filteritem;
  delete m_d; m_d=0;
}

//______________________________________________________________________
QString VP1DockWidget::highlightStyle()
{
  return "background-color: rgba(0, 0, 255, 12%)";
}

//______________________________________________________________________
VP1DockWidget::VP1DockWidget ( IVP1ChannelWidget * cw, VP1TabManager* tm )
  : QDockWidget ( cw->unique_name(), 0 ), m_d(new Imp(this,cw,tm))
{

  setObjectName("VP1DockWidget:"+cw->name());//For storing/saving the layout of the dock widgets within the dock area.

  setFocusProxy(cw);

  setAllowedAreas(Qt::TopDockWidgetArea);
  setFeatures(VP1DockWidget::DockWidgetMovable | VP1DockWidget::DockWidgetFloatable);

  m_d->selectedstylesheet="QFrame#VP1DockWidgetFrame { "+highlightStyle()+" } ";

  //Add frame, which will light up on selection:
  m_d->frame = new QFrame(this);
  m_d->frame->setObjectName("VP1DockWidgetFrame");
  cw->setObjectName("IVP1ChannelWidget:"+cw->name());
  ensureCWHasParent();//setup the cw in the frame

  //Figure out what margin our drawn frame imposes:
  setSelected();
  QMargins marg = m_d->frame->contentsMargins();
  Q_ASSERT(marg.left()==marg.top()&&marg.left()==marg.right()&&marg.left()==marg.bottom()&&"Qt changed its margin behaviour for QFrame!!");
  m_d->unselectedmargin=marg.left();

  //Channel starts unselected:
  setUnselected();

  connect(cw,SIGNAL(uniqueNameChanged(QString)),this,SLOT(updateTitle()));
  connect(cw,SIGNAL(systemRefreshInfoChanged(QString,int,int)),this,SLOT(systemRefreshInfoChanged(QString,int,int)));

  //Setup a watch for selections:
  m_d->filteritem->init();

}

//______________________________________________________________________
void VP1DockWidget::ensureCWHasNoParent()
{
  if (!channelWidget()||!channelWidget()->parent())
    return;
  channelWidget()->setParent(0);
  setWidget(0);
  if (m_d->frame->layout()&&m_d->frame->layout()->indexOf(m_d->channelwidget)>=0) {
    assert(m_d->vboxLayout);
    m_d->vboxLayout->removeWidget(m_d->channelwidget);
  }
}

//______________________________________________________________________
void VP1DockWidget::ensureCWHasParent()
{
  if (!channelWidget()||channelWidget()->parent())
    return;
  if (!m_d->frame->layout()) {
    assert(!m_d->vboxLayout);
    m_d->vboxLayout = new QVBoxLayout(0);
    m_d->frame->setLayout(m_d->vboxLayout);
    m_d->vboxLayout->setObjectName("vp1dock-frame-layout");
    m_d->vboxLayout->setSpacing(0);
  }
  if (m_d->frame->layout()->indexOf(m_d->channelwidget)<0)
    m_d->frame->layout()->addWidget(m_d->channelwidget);
  channelWidget()->setParent(m_d->frame);
  setWidget(m_d->frame);
}

//______________________________________________________________________
void VP1DockWidget::systemRefreshInfoChanged(QString sysrefreshing, int nsysOn, int nsysOnRefreshed)
{
  assert(nsysOnRefreshed<=nsysOn);
  if (nsysOn==nsysOnRefreshed) {
    //    assert(sysrefreshing.isEmpty());
    m_d->title_refreshpart.clear();
    updateTitle();
    return;
  }
  assert(nsysOnRefreshed<nsysOn);
  if (sysrefreshing.isEmpty()) {
    m_d->title_refreshpart = " [Refreshed "+QString::number(nsysOnRefreshed)+"/"+QString::number(nsysOn)+"]";
  } else {
    m_d->title_refreshpart = " [Refreshing "+QString::number(nsysOnRefreshed+1)+"/"+QString::number(nsysOn)+": "+sysrefreshing+" ]";
  }
  updateTitle();
}

//______________________________________________________________________
void VP1DockWidget::updateTitle()
{
  if (m_d->selected)
    setWindowTitle(">>> "+m_d->channelwidget->unique_name()+m_d->title_refreshpart+" <<<");
  else
    setWindowTitle(m_d->channelwidget->unique_name()+m_d->title_refreshpart);
}

//______________________________________________________________________
void VP1DockWidget::setSelected()
{
  if (m_d->selected)
    return;
  m_d->selected=true;
  m_d->frame->setStyleSheet(m_d->selectedstylesheet);
  assert(m_d->vboxLayout);
  m_d->vboxLayout->setMargin(0);
  m_d->frame->setFrameShape(QFrame::StyledPanel);
  updateTitle();
}

//______________________________________________________________________
void VP1DockWidget::setUnselected()
{
  if (!m_d->selected)
    return;
  m_d->selected=false;
  m_d->frame->setStyleSheet("");
  assert(m_d->vboxLayout);
  m_d->vboxLayout->setMargin(m_d->unselectedmargin);
  m_d->frame->setFrameShape(QFrame::NoFrame);
  updateTitle();
}

//______________________________________________________________________
IVP1ChannelWidget * VP1DockWidget::channelWidget() const
{
  return m_d->channelwidget;
}

//______________________________________________________________________
bool VP1DockWidget::isSelected() const
{
  return m_d->selected;
}

//______________________________________________________________________
void VP1DockWidget::contextMenuEvent ( QContextMenuEvent * event )
{
  //Inside the frame, we use the normal behaviour:
  if (m_d->frame->geometry().contains(event->pos())) {
    QDockWidget::contextMenuEvent ( event );
    return;
  }
  //Outside we show our contextmenu:

  //Construct menu:
  QMenu menu(this);
//   menu.addAction("Channel: "+m_d->channelwidget->unique_name())->setEnabled(false);
//   menu.addSeparator();

  QAction* pFullScreenAction = menu.addAction("Show &full Screen");
  menu.addSeparator();
  QAction* pRemoveAction = menu.addAction("&Remove channel");
  menu.addSeparator ();
  QAction* pMoveChannelAction = menu.addAction("&Move to tab");

  QStringList tablist = m_d->tabmanager->tabList();
  QMenu menu_movechan(this);
  if (tablist.count()==1) {
    menu_movechan.addAction("No other tabs available")->setEnabled(false);
  } else {
    QString thistab = m_d->tabmanager->channelToTab(m_d->channelwidget);
    foreach (QString tab, tablist) {
      if (tab!=thistab)
	menu_movechan.addAction(tab)->setData("MOVECHAN");
    }
  }
  pMoveChannelAction->setMenu(&menu_movechan);

  //Execute
  QAction * selAct = menu.exec(event->globalPos());
  //Act on selection:
  if (!selAct)
    return;
  if (selAct==pFullScreenAction) {
    m_d->tabmanager->showChannelFullScreen(m_d->channelwidget);
    return;
  }
  if (selAct==pRemoveAction) {
    m_d->tabmanager->removeChannelAfterQueueEmpties(m_d->channelwidget->unique_name());
    return;
  }
  if (selAct->data()=="MOVECHAN") {
    m_d->tabmanager->moveChannelToTab(m_d->channelwidget->unique_name(),selAct->text());
    return;
  }
  std::cout<<"ERROR in VP1DockWidget::contextMenuEvent!!!!!"<<std::endl;

}

//______________________________________________________________________
void VP1DockWidget::resizeEvent ( QResizeEvent * event )
{
  m_d->channelwidget->dockResized();
  QDockWidget::resizeEvent(event);
}
