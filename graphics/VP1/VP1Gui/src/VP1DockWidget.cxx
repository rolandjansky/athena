/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  FilterItem(VP1DockWidget* _dw) : QObject(_dw),dw(_dw) {}
  VP1DockWidget* dw;

  void init() {
    installEventFilterRecursively(dw);
    activationevents.insert(QEvent::ContextMenu);
    //activationevents.insert(QEvent::EnterEditFocus); --> Does not compile.
    activationevents.insert(QEvent::FocusIn);
    activationevents.insert(QEvent::KeyPress);
    activationevents.insert(QEvent::MouseButtonDblClick);
    activationevents.insert(QEvent::MouseButtonPress);
    activationevents.insert(QEvent::TabletPress);
    activationevents.insert(QEvent::Wheel);
  }

protected:
  QSet<QEvent::Type> activationevents;

  bool eventFilter ( QObject * watched, QEvent * event ) {

    if (event->type()==QEvent::ChildAdded) {
      //Maybe the user added a widget, so update the event filters (todo: test that it works):
      foreach(QObject* c, watched->children()) {
	installEventFilterRecursively(c,true);
	installEventFilterRecursively(c);
      }
      return false;
    }

    if ( !dw->d->selected && activationevents.contains(event->type()) ) {
      dw->setSelected();
      dw->wasSelected(dw);
    }
    //     if ( watched==dw && event->type() == QEvent::Enter )
    //       dw->channelWidget()->setFocus(Qt::OtherFocusReason);

    return false;
  }

  void installEventFilterRecursively(QObject * child, const bool& remove=false ) {
    //NB: we install it on all QObjects in the tree rather than just
    //the QWidgets, since we need to watch for ChildAdded events.
    if (child==this)
      return;
    if (remove)
      child->removeEventFilter(this);
    else
      child->installEventFilter(this);
    foreach(QObject* c, child->children())
      installEventFilterRecursively(c,remove);
  }
};

//____________________________________________________________________
VP1DockWidget::Imp::Imp(VP1DockWidget*_dw,IVP1ChannelWidget * cw,VP1TabManager * _tabmanager)
  : filteritem(new FilterItem(_dw)), dw(_dw), frame(0), vboxLayout(0),channelwidget(cw),
    unselectedmargin(0),selected(false),tabmanager(_tabmanager) {}

//____________________________________________________________________
VP1DockWidget::~VP1DockWidget()
{
  delete d->filteritem;
  delete d; d=0;
}

//______________________________________________________________________
QString VP1DockWidget::highlightStyle()
{
  return "background-color: rgba(0, 0, 255, 12%)";
}

//______________________________________________________________________
VP1DockWidget::VP1DockWidget ( IVP1ChannelWidget * cw, VP1TabManager* tm )
  : QDockWidget ( cw->unique_name(), 0 ), d(new Imp(this,cw,tm))
{

  setObjectName("VP1DockWidget:"+cw->name());//For storing/saving the layout of the dock widgets within the dock area.

  setFocusProxy(cw);

  setAllowedAreas(Qt::TopDockWidgetArea);
  setFeatures(VP1DockWidget::DockWidgetMovable | VP1DockWidget::DockWidgetFloatable);

  d->selectedstylesheet="QFrame#VP1DockWidgetFrame { "+highlightStyle()+" } ";

  //Add frame, which will light up on selection:
  d->frame = new QFrame(this);
  d->frame->setObjectName("VP1DockWidgetFrame");
  cw->setObjectName("IVP1ChannelWidget:"+cw->name());
  ensureCWHasParent();//setup the cw in the frame

  //Figure out what margin our drawn frame imposes:
  setSelected();
  int marg_left, marg_top, marg_right, marg_bottom;
  d->frame->getContentsMargins ( &marg_left, &marg_top, &marg_right, &marg_bottom );
  Q_ASSERT(marg_left==marg_top&&marg_left==marg_right&&marg_left==marg_bottom&&"Qt changed its margin behaviour for QFrame!!");
  d->unselectedmargin=marg_left;

  //Channel starts unselected:
  setUnselected();

  connect(cw,SIGNAL(uniqueNameChanged(QString)),this,SLOT(updateTitle()));
  connect(cw,SIGNAL(systemRefreshInfoChanged(QString,int,int)),this,SLOT(systemRefreshInfoChanged(QString,int,int)));

  //Setup a watch for selections:
  d->filteritem->init();

}

//______________________________________________________________________
void VP1DockWidget::ensureCWHasNoParent()
{
  if (!channelWidget()||!channelWidget()->parent())
    return;
  channelWidget()->setParent(0);
  setWidget(0);
  if (d->frame->layout()&&d->frame->layout()->indexOf(d->channelwidget)>=0) {
    assert(d->vboxLayout);
    d->vboxLayout->removeWidget(d->channelwidget);
  }
}

//______________________________________________________________________
void VP1DockWidget::ensureCWHasParent()
{
  if (!channelWidget()||channelWidget()->parent())
    return;
  if (!d->frame->layout()) {
    assert(!d->vboxLayout);
    d->vboxLayout = new QVBoxLayout(0);
    d->frame->setLayout(d->vboxLayout);
    d->vboxLayout->setObjectName("vp1dock-frame-layout");
    d->vboxLayout->setSpacing(0);
  }
  if (d->frame->layout()->indexOf(d->channelwidget)<0)
    d->frame->layout()->addWidget(d->channelwidget);
  channelWidget()->setParent(d->frame);
  setWidget(d->frame);
}

//______________________________________________________________________
void VP1DockWidget::systemRefreshInfoChanged(QString sysrefreshing, int nsysOn, int nsysOnRefreshed)
{
  assert(nsysOnRefreshed<=nsysOn);
  if (nsysOn==nsysOnRefreshed) {
    //    assert(sysrefreshing.isEmpty());
    d->title_refreshpart.clear();
    updateTitle();
    return;
  }
  assert(nsysOnRefreshed<nsysOn);
  if (sysrefreshing.isEmpty()) {
    d->title_refreshpart = " [Refreshed "+QString::number(nsysOnRefreshed)+"/"+QString::number(nsysOn)+"]";
  } else {
    d->title_refreshpart = " [Refreshing "+QString::number(nsysOnRefreshed+1)+"/"+QString::number(nsysOn)+": "+sysrefreshing+" ]";
  }
  updateTitle();
}

//______________________________________________________________________
void VP1DockWidget::updateTitle()
{
  if (d->selected)
    setWindowTitle(">>> "+d->channelwidget->unique_name()+d->title_refreshpart+" <<<");
  else
    setWindowTitle(d->channelwidget->unique_name()+d->title_refreshpart);
}

//______________________________________________________________________
void VP1DockWidget::setSelected()
{
  if (d->selected)
    return;
  d->selected=true;
  d->frame->setStyleSheet(d->selectedstylesheet);
  assert(d->vboxLayout);
  d->vboxLayout->setMargin(0);
  d->frame->setFrameShape(QFrame::StyledPanel);
  updateTitle();
}

//______________________________________________________________________
void VP1DockWidget::setUnselected()
{
  if (!d->selected)
    return;
  d->selected=false;
  d->frame->setStyleSheet("");
  assert(d->vboxLayout);
  d->vboxLayout->setMargin(d->unselectedmargin);
  d->frame->setFrameShape(QFrame::NoFrame);
  updateTitle();
}

//______________________________________________________________________
IVP1ChannelWidget * VP1DockWidget::channelWidget() const
{
  return d->channelwidget;
}

//______________________________________________________________________
bool VP1DockWidget::isSelected() const
{
  return d->selected;
}

//______________________________________________________________________
void VP1DockWidget::contextMenuEvent ( QContextMenuEvent * event )
{
  //Inside the frame, we use the normal behaviour:
  if (d->frame->geometry().contains(event->pos())) {
    QDockWidget::contextMenuEvent ( event );
    return;
  }
  //Outside we show our contextmenu:

  //Construct menu:
  QMenu menu(this);
//   menu.addAction("Channel: "+d->channelwidget->unique_name())->setEnabled(false);
//   menu.addSeparator();

  QAction* pFullScreenAction = menu.addAction("Show &full Screen");
  menu.addSeparator();
  QAction* pRemoveAction = menu.addAction("&Remove channel");
  menu.addSeparator ();
  QAction* pMoveChannelAction = menu.addAction("&Move to tab");

  QStringList tablist = d->tabmanager->tabList();
  QMenu menu_movechan(this);
  if (tablist.count()==1) {
    menu_movechan.addAction("No other tabs available")->setEnabled(false);
  } else {
    QString thistab = d->tabmanager->channelToTab(d->channelwidget);
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
    d->tabmanager->showChannelFullScreen(d->channelwidget);
    return;
  }
  if (selAct==pRemoveAction) {
    d->tabmanager->removeChannelAfterQueueEmpties(d->channelwidget->unique_name());
    return;
  }
  if (selAct->data()=="MOVECHAN") {
    d->tabmanager->moveChannelToTab(d->channelwidget->unique_name(),selAct->text());
    return;
  }
  std::cout<<"ERROR in VP1DockWidget::contextMenuEvent!!!!!"<<std::endl;

}

//______________________________________________________________________
void VP1DockWidget::resizeEvent ( QResizeEvent * event )
{
  d->channelwidget->dockResized();
  QDockWidget::resizeEvent(event);
}
