/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1CustomTourFrameWidget          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2008                           //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1CustomTourFrameWidget.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "ui_vp1customtourframewidget.h"

#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPainter>
#include <QMimeData>
#include <QBuffer>
#include <QDrag>

//____________________________________________________________________
class VP1CustomTourFrameWidget::Imp {
public:
  Imp(VP1CustomTourFrameWidget*theclass);
  VP1CustomTourFrameWidget*theclass;
  void init();//call at end of VP1CustomTourFrameWidget constructor(s)
  Ui::VP1CustomTourFrameWidgetForm ui;
  QPoint dragStartPosition;
  bool camPerspective;
  QByteArray camState;

};

//____________________________________________________________________
VP1CustomTourFrameWidget::Imp::Imp( VP1CustomTourFrameWidget *tc )
  : theclass(tc), camPerspective(true)
{
  ui.setupUi(theclass);
  ui.label_snapshot->setText("");

  int maxheight = static_cast<int>(0.5+QFontMetricsF(ui.pushButton_remove->font()).height()*1.05+2);
  ui.pushButton_remove->setMaximumHeight(maxheight);
  ui.pushButton_show->setMaximumHeight(maxheight);
  ui.checkBox_frameEnabled->setMaximumHeight(maxheight);
  ui.label_snapshot->setMinimumSize(QSize(snapShotWidth(),snapShotHeight()));
  ui.label_snapshot->setMaximumSize(QSize(snapShotWidth(),snapShotHeight()));

  connect(ui.pushButton_moveEarlier,SIGNAL(clicked()),theclass,SLOT(buttonClicked()));
  connect(ui.pushButton_moveLater,SIGNAL(clicked()),theclass,SLOT(buttonClicked()));
  connect(ui.pushButton_remove,SIGNAL(clicked()),theclass,SLOT(buttonClicked()));
  connect(ui.pushButton_show,SIGNAL(clicked()),theclass,SLOT(buttonClicked()));
  connect(ui.checkBox_frameEnabled,SIGNAL(toggled(bool)),theclass,SLOT(frameEnabledToggled()));

  theclass->setAcceptDrops(true);
  ui.label_camtypeicon->setText("");
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::Imp::init()
{
  ui.label_camtypeicon->setPixmap(QPixmap( camPerspective
					   ?":/icons/icons/perspective.png"
					   :":/icons/icons/ortho.png") );

  theclass->frameEnabledToggled();
}

//____________________________________________________________________
VP1CustomTourFrameWidget::VP1CustomTourFrameWidget(bool camPerspective,
						   QByteArray camState,
						   QWidget * parent)
  : QFrame(parent), d(new Imp(this))
{
  d->camPerspective = camPerspective;
  d->camState = camState;
  d->init();
}

//____________________________________________________________________
VP1CustomTourFrameWidget::VP1CustomTourFrameWidget( QByteArray serialisedFrame,
						    QWidget * parent )
  : QFrame(parent), d(new Imp(this))
{
  VP1Deserialise s(serialisedFrame);
  d->camState = s.restoreByteArray();
  d->camPerspective = s.restoreBool();
  s.restore(d->ui.doubleSpinBox_zoomToFrameTime);
  s.restore(d->ui.doubleSpinBox_stayOnFrameTime);
  if (s.version()>0) {
  	s.restore(d->ui.doubleSpinBox_clipVolumePercentOfATLAS);
  }
  s.restore(d->ui.comboBox_approachMethod);
  s.restore(d->ui.checkBox_frameEnabled);
  QPixmap pm = s.restore<QPixmap>();
  if (!pm.isNull())
    d->ui.label_snapshot->setPixmap(pm);
  s.warnUnrestored(this);
  d->init();
}

//____________________________________________________________________
QByteArray VP1CustomTourFrameWidget::serialise() const
{
  VP1Serialise s(1/*version*/);
  s.save(d->camState);
  s.save(d->camPerspective);
  s.save(d->ui.doubleSpinBox_zoomToFrameTime);
  s.save(d->ui.doubleSpinBox_stayOnFrameTime);
  s.save(d->ui.doubleSpinBox_clipVolumePercentOfATLAS);
  s.save(d->ui.comboBox_approachMethod);
  s.save(d->ui.checkBox_frameEnabled);
  s.save(d->ui.label_snapshot->pixmap() ? *(d->ui.label_snapshot->pixmap()) : QPixmap());
  s.warnUnsaved(this);
  return s.result();
}

//____________________________________________________________________
VP1CustomTourFrameWidget::~VP1CustomTourFrameWidget()
{
  delete d;
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::buttonClicked()
{
  if (sender()==d->ui.pushButton_moveEarlier)
    emit requestStepToEarlier();
  else if (sender()==d->ui.pushButton_moveLater)
    emit requestStepToLater();
  else if (sender()==d->ui.pushButton_remove)
    emit requestDelete();
  else if (sender()==d->ui.pushButton_show)
    emit requestShow();
}

//____________________________________________________________________
bool VP1CustomTourFrameWidget::frameIsEnabled() const
{

  return d->ui.checkBox_frameEnabled->isChecked();
}

//____________________________________________________________________
bool VP1CustomTourFrameWidget::zoomToFrameWithVariableSpeed() const
{
  return d->ui.comboBox_approachMethod->currentIndex()==0;
}
//____________________________________________________________________
bool VP1CustomTourFrameWidget::zoomToFrameForcedCircular() const
{
  return d->ui.comboBox_approachMethod->currentIndex()==2;
}

//____________________________________________________________________
double VP1CustomTourFrameWidget::zoomToFrameTime() const
{
  return d->ui.doubleSpinBox_zoomToFrameTime->value();
}

//____________________________________________________________________
double VP1CustomTourFrameWidget::stayOnFrameTime() const
{
  return d->ui.doubleSpinBox_stayOnFrameTime->value();
}

//____________________________________________________________________
double VP1CustomTourFrameWidget::clipVolumePercentOfATLAS() const
{
  return d->ui.doubleSpinBox_clipVolumePercentOfATLAS->value();
}

//____________________________________________________________________
bool VP1CustomTourFrameWidget::camStateIsPerspective() const
{
  return d->camPerspective;
}

//____________________________________________________________________
QByteArray VP1CustomTourFrameWidget::camState() const
{
  return d->camState;
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::setStepToEarlierControlsEnabled(bool b)
{
  d->ui.pushButton_moveEarlier->setVisible(b);
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::setStepToLaterControlsEnabled(bool b)
{
  d->ui.pushButton_moveLater->setVisible(b);
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    d->dragStartPosition = event->pos();
  QFrame::mousePressEvent(event);
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (!(event->buttons() & Qt::LeftButton))
    return;
  if ((event->pos() - d->dragStartPosition).manhattanLength()
      < QApplication::startDragDistance())
    return;
  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;
  mimeData->setData("vp1/customtourframe", QByteArray() );
  drag->setMimeData(mimeData);//drag assumes ownership of mimeData
  QPixmap pm = d->ui.label_snapshot->pixmap() ? *(d->ui.label_snapshot->pixmap()) : QPixmap();
  if (!pm.isNull())
    drag->setPixmap(pm );
  drag->exec(Qt::CopyAction | Qt::MoveAction);
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source()&&event->source()!=this && event->mimeData()->hasFormat("vp1/customtourframe"))
    event->acceptProposedAction();
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::dropEvent(QDropEvent *event)
{
  QByteArray data = event->mimeData()->data("vp1/customtourframe");
  event->acceptProposedAction();
  VP1CustomTourFrameWidget * source = dynamic_cast<VP1CustomTourFrameWidget *>(event->source());
  emit requestSwap(this,source);
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::setSnapshot(QPixmap pm)
{
  if (pm.isNull())
    return;
  d->ui.label_snapshot->setPixmap(pm);
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::frameEnabledToggled()
{
  d->ui.label_snapshot->setEnabled(frameIsEnabled());
  d->ui.label_camtypeicon->setEnabled(frameIsEnabled());
  emit frameEnableStateChanged();
}
