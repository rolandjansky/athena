/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1CustomStereoEditor             //
//                                                            //
//  Author: Riccardo-Maria BIANCH (rbianchi@cern.ch)          //
//  Initial version: August 2013                              //
//                                                            //
////////////////////////////////////////////////////////////////

#include "ui_vp1customstereoeditor.h"

#include "VP1Base/VP1CustomStereoEditor.h"
#include "VP1Base/VP1ExaminerViewer.h"
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/AnimationSequencer.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Msg.h"

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/SbBox.h>

#include <QShortcut>
#include <QCloseEvent>
#include <QScrollBar>

//____________________________________________________________________
class VP1CustomStereoEditor::Imp {
public:

	VP1CustomStereoEditor * theclass;
	Ui::VP1CustomStereoEditorForm ui;
	VP1ExaminerViewer* viewer;
//	QWidget * frameHolderWidget;
//	QList<VP1CustomTourFrameWidget*> frames;
//	QList<QObject*> objectsToDisableWhenTourUnavailable;
//	void updateFrameListVisuals();
//	void updateFrameStepControlsEnablement();
//	void updateFrameSnapshot(VP1CustomTourFrameWidget*);
//	int countEnabledFrames( int& nEnabledPerspectiveFrames,
//			int& nEnabledOrthographicFrames ) const;
//	void addFrame(VP1CustomTourFrameWidget*frame);

};


// TODO: implement stereo settings save
////____________________________________________________________________
//QByteArray VP1CustomStereoEditor::state() const
//{
//  VP1Serialise s(0/*version*/);
//  QList<QByteArray> frameStates;
//  foreach(VP1CustomTourFrameWidget*frame,d->frames) {
//    frameStates << frame->serialise();
//    s.ignoreWidget(frame);
//  }
//  s.save(frameStates);
//  s.save(d->ui.doubleSpinBox_theta);
//  s.save(d->ui.doubleSpinBox_radius);
//  s.save(d->ui.groupBox_utilityZoom);
//  s.warnUnsaved(this);
//  return s.result();
//}
//
////____________________________________________________________________
//void VP1CustomStereoEditor::setState(QByteArray ba)
//{
//  foreach(VP1CustomTourFrameWidget*frame,d->frames)
//    frame->deleteLater();
//  d->frames.clear();
//
//  VP1Deserialise s(ba);
//  if (s.version()!=0)
//    return;
//  QList<QByteArray> frameStates = s.restore<QList<QByteArray> >();
//  s.restore(d->ui.doubleSpinBox_theta);
//  s.restore(d->ui.doubleSpinBox_radius);
//  s.restore(d->ui.groupBox_utilityZoom);
//  s.warnUnrestored(this);
//
//  d->ui.widget_utilityZoomContents->setVisible(d->ui.groupBox_utilityZoom->isChecked());
//
//  foreach(QByteArray ba2, frameStates)
//    d->addFrame(new VP1CustomTourFrameWidget(ba2));
//  d->updateFrameListVisuals();
//  enabledFrameListChanged();
//}







//____________________________________________________________________
VP1CustomStereoEditor::VP1CustomStereoEditor(VP1ExaminerViewer* viewer)
: QWidget(0,Qt::WindowStaysOnTopHint), d(new Imp)
{
	VP1Msg::messageDebug("VP1CustomStereoEditor::VP1CustomStereoEditor()");

	// set the default anaglyph masks
	leftEyeFilter_def[0] = true; leftEyeFilter_def[1] = false; leftEyeFilter_def[2] = false;
	rightEyeFilter_def[0] = false; rightEyeFilter_def[1] = true; rightEyeFilter_def[2] = true;

	d->theclass = this;
	d->ui.setupUi(this);
	d->ui.groupBox_camera->setChecked(true);
	d->ui.groupBox_viewer->setChecked(false);
	//  d->ui.widget_utilityZoomContents->setVisible(false);

	// a handle on the main viewer widget
	d->viewer = viewer;

	setWindowIcon(QIcon(QString(":/vp1/icons/icons/3d_32x32.png")));

	// set default values
	d->ui.label_focal->setText( QString::number(d->viewer->getCamera()->focalDistance.getValue()) );
	d->ui.doubleSpinBox_offset->setValue(d->viewer->stereo_offset_camera);
	d->ui.doubleSpinBox_parallax->setValue(d->viewer->stereo_parallax_camera);
	d->ui.doubleSpinBox_offset_viewer->setValue(d->viewer->stereo_offset_viewer);


	connect(d->ui.pushButton_focal,SIGNAL(clicked()),this,SLOT(updateFocal()));

	connect(d->ui.pushButton_setToFocal30,SIGNAL(clicked()),this,SLOT(setToFocal30()));


	connect(d->ui.pushButton_defCamera,SIGNAL(clicked()),this,SLOT(buttonClicked()));
	connect(d->ui.pushButton_defViewer,SIGNAL(clicked()),this,SLOT(buttonClicked()));

	connect(d->ui.pushButton_refresh,SIGNAL(clicked()),this,SLOT(buttonClicked()));
	connect(d->ui.pushButton_refreshViewer,SIGNAL(clicked()),this,SLOT(buttonClicked()));

	connect(d->ui.radioButton_standard,SIGNAL(toggled(bool)),this,SLOT(changedView(bool)));
	connect(d->ui.radioButton_left,SIGNAL(toggled(bool)),this,SLOT(changedView(bool)));
	connect(d->ui.radioButton_right,SIGNAL(toggled(bool)),this,SLOT(changedView(bool)));

	connect(d->ui.radioButton_left_viewer,SIGNAL(toggled(bool)),this,SLOT(changedView_viewer(bool)));
	connect(d->ui.radioButton_right_viewer,SIGNAL(toggled(bool)),this,SLOT(changedView_viewer(bool)));
	connect(d->ui.radioButton_standard_viewer,SIGNAL(toggled(bool)),this,SLOT(changedView_viewer(bool)));

	connect(d->ui.radioButton_noStereo,SIGNAL(toggled(bool)),this,SLOT(changedView_viewer(bool)));
	connect(d->ui.radioButton_anaglyph,SIGNAL(toggled(bool)),this,SLOT(changedView_viewer(bool)));
	connect(d->ui.radioButton_quadbuffer,SIGNAL(toggled(bool)),this,SLOT(changedView_viewer(bool)));
	connect(d->ui.radioButton_int_rows,SIGNAL(toggled(bool)),this,SLOT(changedView_viewer(bool)));
	connect(d->ui.radioButton_int_cols,SIGNAL(toggled(bool)),this,SLOT(changedView_viewer(bool)));

	connect(d->ui.doubleSpinBox_imageWidth_meter,SIGNAL(valueChanged(double)),this,SLOT(perform3DCalculations(double)));
	connect(d->ui.doubleSpinBox_imageWidth_inch,SIGNAL(valueChanged(double)),this,SLOT(perform3DCalculations(double)));
	connect(d->ui.doubleSpinBox_offset,SIGNAL(valueChanged(double)),this,SLOT(perform3DCalculations(double)));


	/**
	 * WARNING!!!!!!
	 *
	 * we hide the viewer section for the tag,
	 * because it is still under development.
	 *
	 * Comment out the line below to develop and debug,
	 * and then for the final release.
	 */
	d->ui.groupBox_viewer->hide();

}

//____________________________________________________________________
VP1CustomStereoEditor::~VP1CustomStereoEditor()
{
	//  d->frameHolderWidget->deleteLater();
	//  foreach(VP1CustomTourFrameWidget*frame,d->frames)
	//    frame->deleteLater();
	delete d;
}


//____________________________________________________________________
/*
 * Overriding the QWidget event to
 * update the spinbox with the default or current values every time
 * the editor widget is shown.
 */
void VP1CustomStereoEditor::showEvent(QShowEvent * ev)
{
	// set default values

	d->ui.label_focal->setText( QString::number(d->viewer->getCamera()->focalDistance.getValue()) );

	d->ui.doubleSpinBox_offset->setValue(d->viewer->stereo_offset_camera);
	d->ui.doubleSpinBox_parallax->setValue(d->viewer->stereo_parallax_camera);
	d->ui.doubleSpinBox_offset_viewer->setValue(d->viewer->stereo_offset_viewer);

	d->ui.radioButton_standard->setChecked(true);
	d->ui.radioButton_noStereo->setChecked(true);
	d->ui.radioButton_standard_viewer->setChecked(true);

	d->ui.groupBox_2->setEnabled(false);

	// go on with the default QWidget behavior
	QWidget::showEvent(ev);
}


//____________________________________________________________________
void VP1CustomStereoEditor::closeEvent(QCloseEvent*ev)
{
	ev->ignore();
	hide();
}



//____________________________________________________________________
void VP1CustomStereoEditor::updateFocal()
{
	d->ui.label_focal->setText( QString::number(d->viewer->getCamera()->focalDistance.getValue()) );
}


//____________________________________________________________________
void VP1CustomStereoEditor::setToFocal30()
{
	d->ui.doubleSpinBox_offset->setValue( (d->viewer->getCamera()->focalDistance.getValue() / 30.0) );
	d->ui.doubleSpinBox_offset_viewer->setValue( (d->viewer->getCamera()->focalDistance.getValue() / 30.0) );
}


//____________________________________________________________________
void VP1CustomStereoEditor::perform3DCalculations(double value)
{
	double value_meters = 0.;

	if (d->ui.doubleSpinBox_imageWidth_meter==sender()) {
		// converting meters to inches, and set the new value on the screen
		d->ui.doubleSpinBox_imageWidth_inch->setValue( value * 39.370);
		value_meters = value;
	}
	else if (d->ui.doubleSpinBox_imageWidth_inch==sender()) {
		// converting inches to meters, and set the new value on the screen
		d->ui.doubleSpinBox_imageWidth_meter->setValue( value / 39.370);
		value_meters = value / 39.370;
	}
	else {
		value_meters = d->ui.doubleSpinBox_imageWidth_meter->value();
	}

	// perform S3D calculations
	// --> see: http://www.dashwood3d.com/blog/beginners-guide-to-shooting-stereoscopic-3d/
	double nativeParallax = d->ui.doubleSpinBox_offset->value() / value_meters;
	double minimumObjDistance = d->ui.doubleSpinBox_offset->value() / 30.;

	// update the S3D values
	d->ui.label_nativeParalValue->setText( QString::number(nativeParallax) );
	d->ui.label_minObjDistValue->setText( QString::number(minimumObjDistance) );

}



//____________________________________________________________________
void VP1CustomStereoEditor::changedView(bool checked)
{
	if (checked) {
		SoCamera *camera = d->viewer->getCamera();
		if (d->ui.radioButton_standard->isChecked() ) {
			camera->setStereoMode(SoCamera::MONOSCOPIC);
			VP1Msg::messageDebug("Changed camera STEREO view to STANDARD.");
		}
		else if (d->ui.radioButton_left->isChecked() ) {
			camera->setStereoMode(SoCamera::LEFT_VIEW);
			VP1Msg::messageDebug("Changed camera STEREO view to LEFT EYE.");
		}
		else if (d->ui.radioButton_right->isChecked() ) {
			camera->setStereoMode(SoCamera::RIGHT_VIEW);
			VP1Msg::messageDebug("Changed camera STEREO view to RIGHT EYE.");
		}
		d->viewer->show(); // update the view
	}
}

void VP1CustomStereoEditor::changedView_viewer(bool checked)
{
	if (checked) {
		// we only show the LEFT view in REAL color
		if (d->ui.radioButton_left_viewer->isChecked())
		{
			leftEyeFilter[0] = true; leftEyeFilter[1] = true; leftEyeFilter[2] = true;
			rightEyeFilter[0] = false; rightEyeFilter[1] = false; rightEyeFilter[2] = false;
			d->viewer->setAnaglyphStereoColorMasksSlot(leftEyeFilter, rightEyeFilter);
		}
		// we only show the RIGHT view in REAL color
		if (d->ui.radioButton_right_viewer->isChecked())
		{
			leftEyeFilter[0] = false; leftEyeFilter[1] = false; leftEyeFilter[2] = false;
			rightEyeFilter[0] = true; rightEyeFilter[1] = true; rightEyeFilter[2] = true;
			d->viewer->setAnaglyphStereoColorMasksSlot(leftEyeFilter, rightEyeFilter);
		}
		// we set the standard Red/Cyan anaglyph view
		if (d->ui.radioButton_standard_viewer->isChecked())
		{
			d->viewer->setAnaglyphStereoColorMasksSlot(leftEyeFilter_def, rightEyeFilter_def);
		}
		if (d->ui.radioButton_noStereo->isChecked())
		{
			d->viewer->setStereoTypeSlot(SoQtViewer::STEREO_NONE);
			d->ui.groupBox_2->setEnabled(false);
		}
		if (d->ui.radioButton_anaglyph->isChecked())
		{
			d->viewer->setStereoTypeSlot(SoQtViewer::STEREO_ANAGLYPH);
			d->ui.groupBox_2->setEnabled(true);
		}
		if (d->ui.radioButton_quadbuffer->isChecked())
		{
			d->viewer->setStereoTypeSlot(SoQtViewer::STEREO_QUADBUFFER);
			d->ui.groupBox_2->setEnabled(true);
		}
		if (d->ui.radioButton_int_rows->isChecked())
		{
			d->viewer->setStereoTypeSlot(SoQtViewer::STEREO_INTERLEAVED_ROWS);
			d->ui.groupBox_2->setEnabled(true);
		}
		if (d->ui.radioButton_int_cols->isChecked())
		{
			d->viewer->setStereoTypeSlot(SoQtViewer::STEREO_INTERLEAVED_COLUMNS);
			d->ui.groupBox_2->setEnabled(true);
		}


		d->viewer->show(); // update the view
	}
}



//____________________________________________________________________
void VP1CustomStereoEditor::buttonClicked()
{
	VP1Msg::messageDebug("VP1CustomStereoEditor::buttonClicked() - updating STEREO settings...");

	///////// UPDATING CAMERA
	if (d->ui.pushButton_refresh==sender())
	{
		// get user's values
		double offset(d->ui.doubleSpinBox_offset->value());
		double parallax(d->ui.doubleSpinBox_parallax->value());
		// get current values
		float old_offset = d->viewer->stereo_offset_camera;
		float old_parallax = d->viewer->stereo_parallax_camera;

		SoCamera *camera = d->viewer->getCamera();

		if (old_offset != offset) {
			camera->setStereoAdjustment(offset);
			d->viewer->stereo_offset_camera = offset;
			VP1Msg::messageDebug("Changed CAMERA STEREO offset to: "+QString::number(offset));
		}
		if (old_parallax != parallax) {
			camera->setBalanceAdjustment(parallax);
			d->viewer->stereo_parallax_camera = parallax;
			VP1Msg::messageDebug("Changed CAMERA STEREO balance/parallax to : "+QString::number(parallax));
		}

	}


	//////// UPDATING VIEWER
	else if (d->ui.pushButton_refreshViewer==sender())
	{
		// get user's value
		double offset(d->ui.doubleSpinBox_offset_viewer->value());
		// get current value
		float old_offset = d->viewer->stereo_offset_viewer;

		if (old_offset != offset) {
			d->viewer->setStereoOffsetSlot(offset);
			d->viewer->stereo_offset_viewer = offset;
			VP1Msg::messageDebug("Changed VIEWER STEREO offset to: "+QString::number(offset));
		}
	}

	//////// Setting defaults - VIEWER
	else if (d->ui.pushButton_defCamera==sender())
	{
		// set default values

		/*
		 * Default Interaxial (aka Stereo Base) distance.
		 *
		 * The value is chosen because it's the approximate distance between the human eyes.
		 * See:
		 * - http://doc.coin3d.org/Coin/classSoCamera.html#a6ed2d2527604e86cc8be234b1e429991
		 * - http://www.sky.com/shop/export/sites/www.sky.com/shop/__PDF/3D/Basic_Principles_of_Stereoscopic_3D_v1.pdf
		 * - http://www.dashwood3d.com/blog/beginners-guide-to-shooting-stereoscopic-3d/
		 */
		double defOff = 0.1;

		/*
		 * Default Zero Parallax Plane
		 *
		 * The value 1.0 for the Balance/Parallax, puts the Zero Parallax Plane at the Focal Legth.
		 *
		 * See:
		 * - http://www.dashwood3d.com/blog/beginners-guide-to-shooting-stereoscopic-3d/
		 * - http://www.cafedownloads.com/reviews/r13/3dstereo.html
		 */
		double defParall = 1.0;

		d->ui.doubleSpinBox_offset->setValue(defOff);
		d->ui.doubleSpinBox_parallax->setValue(defParall);

	}


	//////// Setting defaults - VIEWER
	else if (d->ui.pushButton_defViewer==sender())
	{
		// set default values

		/* Default Interaxial (aka Stereo Base) distance.
		 * The value is chosen because it's the approximate distance between the human eyes.
		 * See:
		 * - http://doc.coin3d.org/Coin/classSoCamera.html#a6ed2d2527604e86cc8be234b1e429991
		 * - http://www.sky.com/shop/export/sites/www.sky.com/shop/__PDF/3D/Basic_Principles_of_Stereoscopic_3D_v1.pdf
		 * - http://www.dashwood3d.com/blog/beginners-guide-to-shooting-stereoscopic-3d/
		 */
		double defOff = 0.1;

		d->ui.doubleSpinBox_offset_viewer->setValue(defOff);

	}


}



//
////____________________________________________________________________
//void VP1CustomStereoEditor::Imp::updateFrameSnapshot(VP1CustomTourFrameWidget* frame)
//{
//  QPixmap pm = viewer->getSnapShotFromCamState(frame->camStateIsPerspective(),frame->camState(),
//					       frame->snapShotWidth(),frame->snapShotHeight());
//  if (!pm.isNull())
//    frame->setSnapshot(pm);
//}
//
////____________________________________________________________________
//int VP1CustomStereoEditor::Imp::countEnabledFrames( int& nEnabledPerspectiveFrames,
//						  int& nEnabledOrthographicFrames ) const
//{
//  nEnabledPerspectiveFrames = 0;
//  nEnabledOrthographicFrames = 0;
//  foreach(VP1CustomTourFrameWidget*frame,frames) {
//    if (frame->frameIsEnabled()) {
//      if (frame->camStateIsPerspective())
//	++nEnabledPerspectiveFrames;
//      else
//	++nEnabledOrthographicFrames;
//    }
//  }
//  return nEnabledPerspectiveFrames+nEnabledOrthographicFrames;
//}
//
//
////____________________________________________________________________
//void VP1CustomStereoEditor::enabledFrameListChanged()
//{
//  int nEnabledPerspectiveFrames, nEnabledOrthographicFrames;
//  int n = d->countEnabledFrames( nEnabledPerspectiveFrames,
//				 nEnabledOrthographicFrames );
//  QString s("dummy");
//  bool enable(true);
//  if (n<2) {
//    s = "Add at least two frames for tour";
//    enable=false;
//  } else {
//    if (nEnabledPerspectiveFrames>0&&nEnabledOrthographicFrames>0) {
//      s = "Mixed camera types are forbidden";
//      enable=false;
//    } else {
//      if (nEnabledPerspectiveFrames>0)
//	s="Perspective camera tour";
//      else
//	s="Orthographic camera tour";
//    }
//  }
//  d->ui.label_statustext->setText(s);
//
//  foreach (QObject * o, d->objectsToDisableWhenTourUnavailable) {
//    if (o->isWidgetType())
//      static_cast<QWidget*>(o)->setEnabled(enable);
//    else
//      static_cast<QAction*>(o)->setEnabled(enable);
//  }
//}
//
////____________________________________________________________________
//bool VP1CustomStereoEditor::tourAvailable() const
//{
//  int nEnabledPerspectiveFrames, nEnabledOrthographicFrames;
//  int n = d->countEnabledFrames( nEnabledPerspectiveFrames,
//				 nEnabledOrthographicFrames );
//  if (nEnabledPerspectiveFrames>0&&nEnabledOrthographicFrames>0)
//    return false;
//  return n>=2;
//}
//
////____________________________________________________________________
//bool VP1CustomStereoEditor::tourIsPerspective() const
//{
//  int nEnabledPerspectiveFrames, nEnabledOrthographicFrames;
//  d->countEnabledFrames( nEnabledPerspectiveFrames,
//			 nEnabledOrthographicFrames );
//
//  return nEnabledOrthographicFrames==0;
//}
//
////____________________________________________________________________
//void VP1CustomStereoEditor::addTourToAnimationSequencer(AnimationSequencer&as,bool jumpDirectlyToFirstFrame) const
//{
//  if (!tourAvailable())
//    return;
//  bool firstInDirectJump(jumpDirectlyToFirstFrame);
//  foreach(VP1CustomTourFrameWidget*frame,d->frames) {
//    if (frame->frameIsEnabled()) {
//      double t(frame->zoomToFrameTime());
//      if (firstInDirectJump) {
//	t = 0.0;
//	firstInDirectJump = false;
//      }
//      as.sequence().addFrame(frame->camState(),t,
//			     frame->zoomToFrameWithVariableSpeed(),
//			     frame->zoomToFrameForcedCircular() );
//      if (frame->stayOnFrameTime()>0.0)
//	as.sequence().addFrame(frame->camState(),frame->stayOnFrameTime(),false);
//    }
//  }
//}
//
////____________________________________________________________________
//void VP1CustomStereoEditor::disableObjectWhenTourNotAvailable(QObject *o)
//{
//  if (o)
//    d->objectsToDisableWhenTourUnavailable << o;
//}
