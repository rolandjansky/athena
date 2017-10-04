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
  : QFrame(parent), m_d(new Imp(this))
{
  m_d->camPerspective = camPerspective;
  m_d->camState = camState;
  m_d->init();
}

//____________________________________________________________________
VP1CustomTourFrameWidget::VP1CustomTourFrameWidget( QByteArray serialisedFrame,
						    QWidget * parent )
  : QFrame(parent), m_d(new Imp(this))
{
  VP1Deserialise s(serialisedFrame);
  m_d->camState = s.restoreByteArray();
  m_d->camPerspective = s.restoreBool();
  s.restore(m_d->ui.doubleSpinBox_zoomToFrameTime);
  s.restore(m_d->ui.doubleSpinBox_stayOnFrameTime);
  if (s.version()>0) {
  	s.restore(m_d->ui.doubleSpinBox_clipVolumePercentOfATLAS);
  }
  s.restore(m_d->ui.comboBox_approachMethod);
  s.restore(m_d->ui.checkBox_frameEnabled);
  QPixmap pm = s.restore<QPixmap>();
  if (!pm.isNull())
    m_d->ui.label_snapshot->setPixmap(pm);
  s.warnUnrestored(this);
  m_d->init();
}

//____________________________________________________________________
QByteArray VP1CustomTourFrameWidget::serialise() const
{
  VP1Serialise s(1/*version*/);
  s.save(m_d->camState);
  s.save(m_d->camPerspective);
  s.save(m_d->ui.doubleSpinBox_zoomToFrameTime);
  s.save(m_d->ui.doubleSpinBox_stayOnFrameTime);
  s.save(m_d->ui.doubleSpinBox_clipVolumePercentOfATLAS);
  s.save(m_d->ui.comboBox_approachMethod);
  s.save(m_d->ui.checkBox_frameEnabled);
  s.save(m_d->ui.label_snapshot->pixmap() ? *(m_d->ui.label_snapshot->pixmap()) : QPixmap());
  s.warnUnsaved(this);
  return s.result();
}

//____________________________________________________________________
VP1CustomTourFrameWidget::~VP1CustomTourFrameWidget()
{
  delete m_d;
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::buttonClicked()
{
  if (sender()==m_d->ui.pushButton_moveEarlier)
    emit requestStepToEarlier();
  else if (sender()==m_d->ui.pushButton_moveLater)
    emit requestStepToLater();
  else if (sender()==m_d->ui.pushButton_remove)
    emit requestDelete();
  else if (sender()==m_d->ui.pushButton_show)
    emit requestShow();
}

//____________________________________________________________________
bool VP1CustomTourFrameWidget::frameIsEnabled() const
{

  return m_d->ui.checkBox_frameEnabled->isChecked();
}

//____________________________________________________________________
bool VP1CustomTourFrameWidget::zoomToFrameWithVariableSpeed() const
{
  return m_d->ui.comboBox_approachMethod->currentIndex()==0;
}
//____________________________________________________________________
bool VP1CustomTourFrameWidget::zoomToFrameForcedCircular() const
{
  return m_d->ui.comboBox_approachMethod->currentIndex()==2;
}

//____________________________________________________________________
double VP1CustomTourFrameWidget::zoomToFrameTime() const
{
  return m_d->ui.doubleSpinBox_zoomToFrameTime->value();
}

//____________________________________________________________________
double VP1CustomTourFrameWidget::stayOnFrameTime() const
{
  return m_d->ui.doubleSpinBox_stayOnFrameTime->value();
}

//____________________________________________________________________
double VP1CustomTourFrameWidget::clipVolumePercentOfATLAS() const
{
  return m_d->ui.doubleSpinBox_clipVolumePercentOfATLAS->value();
}

//____________________________________________________________________
bool VP1CustomTourFrameWidget::camStateIsPerspective() const
{
  return m_d->camPerspective;
}

//____________________________________________________________________
QByteArray VP1CustomTourFrameWidget::camState() const
{
  return m_d->camState;
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::setStepToEarlierControlsEnabled(bool b)
{
  m_d->ui.pushButton_moveEarlier->setVisible(b);
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::setStepToLaterControlsEnabled(bool b)
{
  m_d->ui.pushButton_moveLater->setVisible(b);
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    m_d->dragStartPosition = event->pos();
  QFrame::mousePressEvent(event);
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (!(event->buttons() & Qt::LeftButton))
    return;
  if ((event->pos() - m_d->dragStartPosition).manhattanLength()
      < QApplication::startDragDistance())
    return;
  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;
  mimeData->setData("vp1/customtourframe", QByteArray() );
  drag->setMimeData(mimeData);//drag assumes ownership of mimeData
  QPixmap pm = m_d->ui.label_snapshot->pixmap() ? *(m_d->ui.label_snapshot->pixmap()) : QPixmap();
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
  m_d->ui.label_snapshot->setPixmap(pm);
}

//____________________________________________________________________
void VP1CustomTourFrameWidget::frameEnabledToggled()
{
  m_d->ui.label_snapshot->setEnabled(frameIsEnabled());
  m_d->ui.label_camtypeicon->setEnabled(frameIsEnabled());
  emit frameEnableStateChanged();
}
