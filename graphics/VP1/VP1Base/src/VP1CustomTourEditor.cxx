/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1CustomTourEditor               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2008                           //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1CustomTourEditor.h"
#include "VP1Base/VP1CustomTourFrameWidget.h"
#include "ui_vp1customtoureditor.h"

#include "VP1Base/VP1ExaminerViewer.h"
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/AnimationSequencer.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/SbBox.h>

#include <QShortcut>
#include <QCloseEvent>
#include <QScrollBar>

#include <iostream>

//____________________________________________________________________
class VP1CustomTourEditor::Imp {
public:
  VP1CustomTourEditor * theclass;
  Ui::VP1CustomTourEditorForm ui;
  VP1ExaminerViewer* viewer;
  QWidget * frameHolderWidget;
  QList<VP1CustomTourFrameWidget*> frames;
  QList<QObject*> objectsToDisableWhenTourUnavailable;
  void updateFrameListVisuals();
  void updateFrameStepControlsEnablement();
  void updateFrameSnapshot(VP1CustomTourFrameWidget*);
  int countEnabledFrames( int& nEnabledPerspectiveFrames,
			  int& nEnabledOrthographicFrames ) const;
  void addFrame(VP1CustomTourFrameWidget*frame);

};


//____________________________________________________________________
QByteArray VP1CustomTourEditor::state() const
{
  VP1Serialise s(0/*version*/);
  QList<QByteArray> frameStates;
  foreach(VP1CustomTourFrameWidget*frame,m_d->frames) {
    frameStates << frame->serialise();
    s.ignoreWidget(frame);
  }
  s.save(frameStates);
  s.save(m_d->ui.doubleSpinBox_theta);
  s.save(m_d->ui.doubleSpinBox_radius);
  s.save(m_d->ui.groupBox_utilityZoom);
  s.warnUnsaved(this);
  return s.result();
}

//____________________________________________________________________
void VP1CustomTourEditor::setState(QByteArray ba)
{
  foreach(VP1CustomTourFrameWidget*frame,m_d->frames)
    frame->deleteLater();
  m_d->frames.clear();

  VP1Deserialise s(ba);
  if (s.version()!=0)
    return;
  QList<QByteArray> frameStates = s.restore<QList<QByteArray> >();
  s.restore(m_d->ui.doubleSpinBox_theta);
  s.restore(m_d->ui.doubleSpinBox_radius);
  s.restore(m_d->ui.groupBox_utilityZoom);
  s.warnUnrestored(this);

  m_d->ui.widget_utilityZoomContents->setVisible(m_d->ui.groupBox_utilityZoom->isChecked());

  foreach(QByteArray ba2, frameStates)
    m_d->addFrame(new VP1CustomTourFrameWidget(ba2));
  m_d->updateFrameListVisuals();
  enabledFrameListChanged();
}


//____________________________________________________________________
VP1CustomTourEditor::VP1CustomTourEditor(VP1ExaminerViewer* viewer)
  : QWidget(0,Qt::WindowStaysOnTopHint), m_d(new Imp)
{

  m_d->theclass = this;
  m_d->ui.setupUi(this);
  m_d->ui.groupBox_utilityZoom->setChecked(false);
  m_d->ui.widget_utilityZoomContents->setVisible(false);
  m_d->viewer = viewer;
  setWindowIcon(QIcon(QString(":/vp1/icons/icons/3d_32x32.png")));
  m_d->frameHolderWidget = new QWidget;
  m_d->ui.scrollArea->setWidgetResizable(true);
  m_d->ui.scrollArea->setWidget(m_d->frameHolderWidget);

  connect(new QShortcut(QKeySequence(Qt::Key_Escape), this),SIGNAL(activated()),this,SLOT(hide()));
  connect(m_d->ui.pushButton_close,SIGNAL(clicked()),this,SLOT(hide()));

  connect(m_d->ui.pushButton_addCurrentView,SIGNAL(clicked()),this,SLOT(buttonClicked()));
  connect(m_d->ui.pushButton_refreshPreviews,SIGNAL(clicked()),this,SLOT(buttonClicked()));
  connect(m_d->ui.pushButton_execute,SIGNAL(clicked()),this,SLOT(buttonClicked()));
  connect(m_d->ui.pushButton_utilityZoomShow,SIGNAL(clicked()),this,SLOT(buttonClicked()));

  m_d->objectsToDisableWhenTourUnavailable << m_d->ui.pushButton_execute;

  enabledFrameListChanged();
}

//____________________________________________________________________
VP1CustomTourEditor::~VP1CustomTourEditor()
{
  m_d->frameHolderWidget->deleteLater();
  foreach(VP1CustomTourFrameWidget*frame,m_d->frames)
    frame->deleteLater();
  delete m_d;
}

//____________________________________________________________________
void VP1CustomTourEditor::closeEvent(QCloseEvent*ev)
{
  ev->ignore();
  hide();
}

//____________________________________________________________________
void VP1CustomTourEditor::Imp::addFrame(VP1CustomTourFrameWidget*frame)
{
  // std::cout<<"VP1CustomTourEditor::Imp::addFrame %= "<<frame->clipVolumePercentOfATLAS()<<std::endl;
  frames << frame;
  connect(frame,SIGNAL(requestStepToEarlier()),theclass,SLOT(frameStepToEarlier()));
  connect(frame,SIGNAL(requestStepToLater()),theclass,SLOT(frameStepToLater()));
  connect(frame,SIGNAL(requestDelete()),theclass,SLOT(frameDelete()));
  connect(frame,SIGNAL(requestShow()),theclass,SLOT(frameShow()));
  connect(frame,SIGNAL(requestSwap(VP1CustomTourFrameWidget*,VP1CustomTourFrameWidget*)),
	  theclass,SLOT(swap(VP1CustomTourFrameWidget*,VP1CustomTourFrameWidget*)));
  connect(frame,SIGNAL(frameEnableStateChanged()),theclass,SLOT(enabledFrameListChanged()));
}

//____________________________________________________________________
void VP1CustomTourEditor::Imp::updateFrameListVisuals()
{
  bool save = frameHolderWidget->updatesEnabled();
  if (save)
    frameHolderWidget->setUpdatesEnabled(false);
  int sliderpos=ui.scrollArea->verticalScrollBar()->value();
  foreach(QObject * o, frameHolderWidget->children()) {
    if (o->isWidgetType()) {
      static_cast<QWidget*>(o)->setVisible(false);
      o->setParent(0);
    }
  }
  delete frameHolderWidget->layout();
  QVBoxLayout * vlayout = new QVBoxLayout;
  foreach(VP1CustomTourFrameWidget*frame,frames) {
    vlayout->addWidget(frame);
   frame->setParent(frameHolderWidget);
    frame->setVisible(true);
  }
  vlayout->addStretch(1);
  frameHolderWidget->setLayout(vlayout);
  updateFrameStepControlsEnablement();
  sliderpos=std::max(ui.scrollArea->verticalScrollBar()->minimum(),
		     std::min(ui.scrollArea->verticalScrollBar()->maximum(),sliderpos));
  ui.scrollArea->verticalScrollBar()->setValue(sliderpos);
  theclass->enabledFrameListChanged();
  if (save)
    frameHolderWidget->setUpdatesEnabled(true);

}

//____________________________________________________________________
void VP1CustomTourEditor::frameStepToEarlier()
{
  int iframe(m_d->frames.indexOf(dynamic_cast<VP1CustomTourFrameWidget*>(sender())));
  if (iframe<=0)
    return;//not found or already at start
  m_d->frames.swap(iframe-1,iframe);
  m_d->updateFrameListVisuals();
}

//____________________________________________________________________
void VP1CustomTourEditor::frameStepToLater()
{
  int iframe(m_d->frames.indexOf(dynamic_cast<VP1CustomTourFrameWidget*>(sender())));
  if (iframe<0)
    return;
  if (iframe>=m_d->frames.count()-1)
    return;//already at end
  m_d->frames.swap(iframe,iframe+1);
  m_d->updateFrameListVisuals();
}

//____________________________________________________________________
void VP1CustomTourEditor::frameDelete()
{
  VP1CustomTourFrameWidget * frame = dynamic_cast<VP1CustomTourFrameWidget*>(sender());
  if (!frame)
    return;
  m_d->frames.removeAll(frame);
  frame->deleteLater();
  m_d->updateFrameStepControlsEnablement();
  enabledFrameListChanged();
}

//____________________________________________________________________
void VP1CustomTourEditor::frameShow()
{
  VP1CustomTourFrameWidget * frame = dynamic_cast<VP1CustomTourFrameWidget*>(sender());
  if (!frame)
    return;

  if (frame->camStateIsPerspective()==m_d->viewer->currentCamIsPerspective()) {
    //Zoom there:
    SoCamera * cam = m_d->viewer->getCamera();
    SoNode * root = m_d->viewer->getSceneGraph();
    if (!cam||!root||!root->getTypeId().isDerivedFrom(SoGroup::getClassTypeId()))
      return;

    if (m_d->viewer->isAnimating())
      m_d->viewer->stopAnimating();
    VP1CameraHelper::animatedZoomToCameraState( cam,static_cast<SoGroup*>(root),frame->camState(),
		        frame->zoomToFrameTime(),
		        frame->clipVolumePercentOfATLAS(),
						frame->zoomToFrameWithVariableSpeed(),
						frame->zoomToFrameForcedCircular() );
  } else {
    //Go there directly:
    m_d->viewer->toggleCameraType();
    SoCamera * cam = m_d->viewer->getCamera();
    if (!cam)
      return;
    QByteArray ba(frame->camState());
    VP1QtInventorUtils::deserializeSoCameraParameters(ba,*cam);
  }
}

//____________________________________________________________________
void VP1CustomTourEditor::Imp::updateFrameStepControlsEnablement()
{
  for (int i = 0; i < frames.count(); ++i) {
    VP1CustomTourFrameWidget*frame = frames.at(i);
    bool earlier(true), later(true);
    if (i==0)
      earlier = false;
    if (i==frames.count()-1)
      later = false;
    frame->setStepToEarlierControlsEnabled(earlier);
    frame->setStepToLaterControlsEnabled(later);
  }
}

//____________________________________________________________________
void VP1CustomTourEditor::swap( VP1CustomTourFrameWidget * frame1,
			        VP1CustomTourFrameWidget * frame2 )
{
  int iframe1(m_d->frames.indexOf(frame1));
  int iframe2(m_d->frames.indexOf(frame2));
  if ( iframe1==iframe2
       ||iframe1<0
       ||iframe2<0
       ||iframe1>=m_d->frames.count()
       ||iframe2>=m_d->frames.count() )
    return;
  m_d->frames.swap(iframe1,iframe2);
  m_d->updateFrameListVisuals();
}

//____________________________________________________________________
void VP1CustomTourEditor::buttonClicked()
{
  if (m_d->ui.pushButton_addCurrentView==sender()) {
    VP1CustomTourFrameWidget * frame = new VP1CustomTourFrameWidget(m_d->viewer->currentCamIsPerspective(),
								    m_d->viewer->currentCameraState());
    m_d->addFrame(frame);
    m_d->updateFrameSnapshot(frame);
    m_d->updateFrameListVisuals();
  } else if (m_d->ui.pushButton_refreshPreviews==sender()) {
    foreach(VP1CustomTourFrameWidget*frame,m_d->frames)
      m_d->updateFrameSnapshot(frame);
  } else if (m_d->ui.pushButton_execute==sender()) {
    m_d->viewer->startCustomTour();
  } else if (m_d->ui.pushButton_utilityZoomShow==sender()) {
    SoCamera * cam = m_d->viewer->getCamera();
    SoNode * root = m_d->viewer->getSceneGraph();
    if (!cam||!root||!root->getTypeId().isDerivedFrom(SoGroup::getClassTypeId()))
      return;

    double r(m_d->ui.doubleSpinBox_radius->value()*1000.0/*meters*/);
    double theta(m_d->ui.doubleSpinBox_theta->value()/180.0*M_PI);
    theta += M_PI;
    while (theta<0) theta += 2*M_PI;
    while (theta>2*M_PI) theta -= 2*M_PI;
    double a(r/sqrt(3.0));
    SbBox3f box(-a,-a,-a,a,a,a);
    SbVec3f lookat = SbVec3f(sin(theta),0,cos(theta));
    SbVec3f upvec = SbVec3f(0,1,0);
    bool notifyenabled = cam->enableNotify(false);
    m_d->viewer->resetCamera();
    VP1CameraHelper::animatedZoomToBBox( cam,static_cast<SoGroup*>(root),
					 box, 0.0, 100.0, 1.0,lookat,upvec);
    if (notifyenabled)
      cam->enableNotify(true);//Don't touch here - to avoid showing
			      //the reset cam state (the camera helper
			      //will touch it when modifying it)
  }
}

//____________________________________________________________________
void VP1CustomTourEditor::Imp::updateFrameSnapshot(VP1CustomTourFrameWidget* frame)
{
  QPixmap pm = viewer->getSnapShotFromCamState(frame->camStateIsPerspective(),frame->camState(),
					       frame->snapShotWidth(),frame->snapShotHeight());
  if (!pm.isNull())
    frame->setSnapshot(pm);
}

//____________________________________________________________________
int VP1CustomTourEditor::Imp::countEnabledFrames( int& nEnabledPerspectiveFrames,
						  int& nEnabledOrthographicFrames ) const
{
  nEnabledPerspectiveFrames = 0;
  nEnabledOrthographicFrames = 0;
  foreach(VP1CustomTourFrameWidget*frame,frames) {
    if (frame->frameIsEnabled()) {
      if (frame->camStateIsPerspective())
	++nEnabledPerspectiveFrames;
      else
	++nEnabledOrthographicFrames;
    }
  }
  return nEnabledPerspectiveFrames+nEnabledOrthographicFrames;
}


//____________________________________________________________________
void VP1CustomTourEditor::enabledFrameListChanged()
{
  int nEnabledPerspectiveFrames, nEnabledOrthographicFrames;
  int n = m_d->countEnabledFrames( nEnabledPerspectiveFrames,
				 nEnabledOrthographicFrames );
  QString s("dummy");
  bool enable(true);
  if (n<2) {
    s = "Add at least two frames for tour";
    enable=false;
  } else {
    if (nEnabledPerspectiveFrames>0&&nEnabledOrthographicFrames>0) {
      s = "Mixed camera types are forbidden";
      enable=false;
    } else {
      if (nEnabledPerspectiveFrames>0)
	s="Perspective camera tour";
      else
	s="Orthographic camera tour";
    }
  }
  m_d->ui.label_statustext->setText(s);

  foreach (QObject * o, m_d->objectsToDisableWhenTourUnavailable) {
    if (o->isWidgetType())
      static_cast<QWidget*>(o)->setEnabled(enable);
    else
      static_cast<QAction*>(o)->setEnabled(enable);
  }
}

//____________________________________________________________________
bool VP1CustomTourEditor::tourAvailable() const
{
  int nEnabledPerspectiveFrames, nEnabledOrthographicFrames;
  int n = m_d->countEnabledFrames( nEnabledPerspectiveFrames,
				 nEnabledOrthographicFrames );
  if (nEnabledPerspectiveFrames>0&&nEnabledOrthographicFrames>0)
    return false;
  return n>=2;
}

//____________________________________________________________________
bool VP1CustomTourEditor::tourIsPerspective() const
{
  int nEnabledPerspectiveFrames, nEnabledOrthographicFrames;
  m_d->countEnabledFrames( nEnabledPerspectiveFrames,
			 nEnabledOrthographicFrames );

  return nEnabledOrthographicFrames==0;
}

//____________________________________________________________________
void VP1CustomTourEditor::addTourToAnimationSequencer(AnimationSequencer& as,bool jumpDirectlyToFirstFrame) const
{
  if (!tourAvailable())
    return;
  bool firstInDirectJump(jumpDirectlyToFirstFrame);
  foreach(VP1CustomTourFrameWidget*frame,m_d->frames) {
    if (frame->frameIsEnabled()) {
      double t(frame->zoomToFrameTime());
      if (firstInDirectJump) {
        t = 0.0;
        firstInDirectJump = false;
      }

      // std::cout<<"addTourToAnimationSequencer %="<<frame->clipVolumePercentOfATLAS()<<std::endl;
      as.sequence().addFrame(frame->camState(),t,
                             frame->zoomToFrameWithVariableSpeed(),frame->zoomToFrameForcedCircular(),
                             frame->clipVolumePercentOfATLAS() );
      if (frame->stayOnFrameTime()>0.0)
        as.sequence().addFrame(frame->camState(),frame->stayOnFrameTime(),false, false, frame->clipVolumePercentOfATLAS());
    }
  }
}

void VP1CustomTourEditor::setClipVolumePercentOfATLAS(double percent){
  emit clipVolumePercentOfATLAS( percent );

  // Let's assume ATLAS is 40m long. So emit this.
  emit clipVolumeRadiusChanged( percent * 400 );
}


//____________________________________________________________________
void VP1CustomTourEditor::disableObjectWhenTourNotAvailable(QObject *o)
{
  if (o)
    m_d->objectsToDisableWhenTourUnavailable << o;
}
