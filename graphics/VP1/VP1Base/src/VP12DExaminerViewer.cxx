/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP12DExaminerViewer            //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//  Major update to support multiple views: July 2007      //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/VP12DExaminerViewer.h"
#include "VP1Base/VP1GraphicsView.h"
#include "ui_vp12dexaminerviewerform.h"
#include <iostream>

//Fixme: If you set seekmode in one view, then even after you use it
//up, the other views will still be in seek mode. I guess we need a
//seekmodedone signal and an endSeekMode slot in vp1graphicsview.

//____________________________________________________________________
class VP12DExaminerViewer::Imp {
public:
  VP12DExaminerViewer * theclass;
  Ui::VP12DExaminerViewerForm * ui;
  bool singleviewmode;

  //Common init stuff for the constructors:
  void init1();//Before the views are setup in the stackwidget
  void init2();//After the views are setup in the stackwidget

  //For some pushbuttons, only the visible view should be
  //connected. Thus use the following when changing view:
  void connectButtonsToView(VP1GraphicsView*);
  void disconnectButtonsFromView(VP1GraphicsView*);

  //Extra info for multi-view mode:
  QList<QPair<QString,QString> > multiviews_viewidAndIconUrl;
  QMap<QString,VP1GraphicsView*> multiviews_viewid2view;
  QMap<QPushButton*,QString> button2viewid;


  //Helpers
  void setModeAllViews(VP1GraphicsView::Mode);


};


//____________________________________________________________________
void VP12DExaminerViewer::Imp::init1()
{
  ui = new Ui::VP12DExaminerViewerForm;
  ui->setupUi(theclass);

  //Clear stack widget:

  while (ui->stackedWidget_views->count())
    ui->stackedWidget_views->removeWidget(ui->stackedWidget_views->currentWidget());

  //Setup mode buttons:
  connect(ui->pushButton_pick,SIGNAL(clicked()),theclass,SLOT(setPickMode()));
  connect(ui->pushButton_changeview,SIGNAL(clicked()),theclass,SLOT(setChangeViewMode()));
  connect(ui->pushButton_seek,SIGNAL(clicked()),theclass,SLOT(setSeekMode()));

  //Hide unused buttons:
  ui->pushButton_zoomplus->hide();
  ui->pushButton_zoomminus->hide();
}

//____________________________________________________________________
void VP12DExaminerViewer::Imp::init2()
{
  if (theclass->view())
    connectButtonsToView(theclass->view());
  else
    std::cout<<"VP12DExaminerViewer::Imp::init2 WARNING: Current View is NULL"<<std::endl;
  //Start in pick mode:
  theclass->setPickMode();

}


//____________________________________________________________________
VP12DExaminerViewer::VP12DExaminerViewer(QWidget * parent, Qt::WindowFlags f)
  : QWidget(parent,f), m_d(new Imp)
{
  m_d->theclass=this;
  m_d->singleviewmode = true;

  m_d->init1();

  //Add graphicsview:
  m_d->ui->stackedWidget_views->insertWidget(m_d->ui->stackedWidget_views->count(),new VP1GraphicsView);

  //Setup viewchangebuttons:
  m_d->ui->widget_viewchangebuttons->hide();

  m_d->init2();
}

//____________________________________________________________________
VP12DExaminerViewer::VP12DExaminerViewer(const QList<QPair<QString,QString> >& views,
					 QWidget * parent, Qt::WindowFlags f)
  : QWidget(parent,f), m_d(new Imp)
{
  if (!views.count())
    std::cout<<"VP12DExaminerViewer ERROR: Using multimode constructor with empty list of requested views!"<<std::endl;


  m_d->theclass=this;
  m_d->singleviewmode = false;

  m_d->init1();

  //Setup layout for the buttons used to change the views:
  QVBoxLayout *layout = new QVBoxLayout;
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  m_d->ui->widget_viewchangebuttons->setLayout(layout);

  //Add graphicsviews:
  m_d->multiviews_viewidAndIconUrl = views;
  VP1GraphicsView * firstview(0);
  for (int i = 0; i<views.count();++i) {

    VP1GraphicsView *  view = new VP1GraphicsView;
    if (!firstview)
      firstview=view;

    m_d->multiviews_viewid2view.insert(views.at(i).first,view);

    m_d->ui->stackedWidget_views->insertWidget(m_d->ui->stackedWidget_views->count(),view);
    //Add button:
    QPushButton* button = new QPushButton(m_d->ui->widget_viewchangebuttons);
    button->setToolTip(views.at(i).first);
    button->setSizePolicy(sizePolicy);
    button->setMinimumSize(QSize(30, 30));
    button->setMaximumSize(QSize(30, 30));
    button->setFocusPolicy(Qt::NoFocus);
    button->setIcon(QIcon(views.at(i).second));
    button->setIconSize(QSize(30, 30));
    button->setCheckable(false);
    layout->addWidget(button);
    //connect button here and put it in button -> view_id map:
    connect(button,SIGNAL(clicked()),this,SLOT(changeViewButtonClicked()));
    m_d->button2viewid.insert(button,views.at(i).first);
  }

  QSpacerItem * spacerItem = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
  layout->addItem(spacerItem);

  if (firstview)
    m_d->ui->stackedWidget_views->setCurrentWidget(firstview);
  else
    std::cout<<"VP12DExaminerViewer WARNING: View is NULL"<<std::endl;

  m_d->init2();

//   VP1GraphicsView a(a);
}

//____________________________________________________________________
void VP12DExaminerViewer::Imp::connectButtonsToView(VP1GraphicsView* view)
{
  connect(ui->pushButton_sethome,SIGNAL(clicked()),view,SLOT(setHome()));
  connect(ui->pushButton_home,SIGNAL(clicked()),view,SLOT(viewHome()));
  connect(ui->pushButton_viewall,SIGNAL(clicked()),view,SLOT(fitViewToContents()));
  connect(ui->pushButton_print,SIGNAL(clicked()),view,SLOT(print()));
}

//____________________________________________________________________
void VP12DExaminerViewer::Imp::disconnectButtonsFromView(VP1GraphicsView*view)
{
  disconnect(ui->pushButton_sethome,SIGNAL(clicked()),view,SLOT(setHome()));
  disconnect(ui->pushButton_home,SIGNAL(clicked()),view,SLOT(viewHome()));
  disconnect(ui->pushButton_viewall,SIGNAL(clicked()),view,SLOT(fitViewToContents()));
  disconnect(ui->pushButton_print,SIGNAL(clicked()),view,SLOT(print()));
}

//____________________________________________________________________
VP12DExaminerViewer::~VP12DExaminerViewer()
{
  delete m_d; m_d=0;
}

//____________________________________________________________________
VP1GraphicsView * VP12DExaminerViewer::view() const {
  return static_cast<VP1GraphicsView *>(m_d->ui->stackedWidget_views->currentWidget());
  //  return m_d->view;
}

//____________________________________________________________________
VP1GraphicsView * VP12DExaminerViewer::view(const QString& view_id) const
{
  if (m_d->singleviewmode||!m_d->multiviews_viewid2view.contains(view_id))
    return 0;

  return m_d->multiviews_viewid2view[view_id];
}

//____________________________________________________________________
void VP12DExaminerViewer::Imp::setModeAllViews(VP1GraphicsView::Mode m)
{
  //Change viewmode for all the views:
  if (singleviewmode) {
    theclass->view()->setMode(m);
  } else {
    QMap<QString,VP1GraphicsView*>::iterator it, itE = multiviews_viewid2view.end();
    for(it = multiviews_viewid2view.begin(); it!=itE; ++it) {
      it.value()->setMode(m);
    }
  }
}

//____________________________________________________________________
void VP12DExaminerViewer::setPickMode()
{
  m_d->ui->pushButton_pick->setChecked(true);
  m_d->ui->pushButton_changeview->setChecked(false);
  m_d->setModeAllViews(VP1GraphicsView::PICK);
}

//____________________________________________________________________
void VP12DExaminerViewer::setChangeViewMode()
{
  m_d->ui->pushButton_pick->setChecked(false);
  m_d->ui->pushButton_changeview->setChecked(true);
  m_d->setModeAllViews(VP1GraphicsView::CHANGEVIEW);
}

//____________________________________________________________________
void VP12DExaminerViewer::setSeekMode()
{
  m_d->setModeAllViews(VP1GraphicsView::SEEK);
}

//____________________________________________________________________
QStringList VP12DExaminerViewer::viewIDs() const
{
  if (m_d->singleviewmode)
    return QStringList();

  QStringList l;
  QMap<QString,VP1GraphicsView*>::iterator it, itE = m_d->multiviews_viewid2view.end();
  for(it = m_d->multiviews_viewid2view.begin(); it!=itE; ++it) {
    l << it.key();
  }

  return l;
}

//____________________________________________________________________
QList<QPair<QString,QString> > VP12DExaminerViewer::viewIDsAndIconUrls() const
{
  if (m_d->singleviewmode)
    return QList<QPair<QString,QString> >();

  return m_d->multiviews_viewidAndIconUrl;
}

//____________________________________________________________________
bool VP12DExaminerViewer::isSingleViewMode() const
{
  return m_d->singleviewmode;
}

//____________________________________________________________________
QString VP12DExaminerViewer::currentView() const
{
  if (m_d->singleviewmode)
    return "";

  VP1GraphicsView* currentview = view();
  if (!currentview)
    return "";

  QMap<QString,VP1GraphicsView*>::iterator it, itE = m_d->multiviews_viewid2view.end();
  for(it = m_d->multiviews_viewid2view.begin(); it!=itE; ++it) {
    if (it.value()==currentview)
      return it.key();
  }

  return "";
}

//____________________________________________________________________
void VP12DExaminerViewer::setShownView(QString viewid)
{
  if (m_d->singleviewmode)
    return;

  if (!m_d->multiviews_viewid2view.contains(viewid))
    return;

  VP1GraphicsView* oldview = view();
  VP1GraphicsView* newview = m_d->multiviews_viewid2view[viewid];
  if (!oldview||!newview)
    return;
  if (oldview==newview)
    return;

  currentViewAboutToChange(newview);

  m_d->disconnectButtonsFromView(oldview);
  m_d->ui->stackedWidget_views->setCurrentWidget(newview);
  m_d->connectButtonsToView(newview);

  currentViewChanged();
}

//____________________________________________________________________
void VP12DExaminerViewer::changeViewButtonClicked()
{
  QPushButton * button = static_cast<QPushButton*>(sender());
  if (!button||!m_d->button2viewid.contains(button))
    return;
  setShownView(m_d->button2viewid[button]);
}
