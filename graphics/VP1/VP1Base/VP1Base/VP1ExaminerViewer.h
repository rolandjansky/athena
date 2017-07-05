/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1ExaminerViewer                   //
//                                                            //
//  Description: Examiner Viewer specialisation for VP1       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1EXAMINERVIEWER_H
#define VP1EXAMINERVIEWER_H

#include "VP1Base/VP1String.h"
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <QObject>
class QPixmap;





class VP1ExaminerViewer : public SoQtExaminerViewer, public VP1String {

public:

  VP1ExaminerViewer( QWidget * parent = 0,//This is not really the parent.
		     bool detectorViewButtons = true,
		     const char * name = 0,
		     SbBool embed = TRUE,
		     SoQtFullViewer::BuildFlag flag = BUILD_ALL,
		     SoQtViewer::Type type = BROWSER);

	virtual ~VP1ExaminerViewer();//NB: SoQtExaminerViewer has non-virtual destructor!

	virtual void removeDecorationMenuOption();

	virtual QByteArray saveState();
	virtual void restoreFromState(QByteArray);

	virtual void viewAll();
	virtual void toggleCameraType();
	virtual void setCameraType(SoType type);
	virtual void setSeekMode(SbBool enable);
	virtual void setViewing(SbBool enable);
	virtual void setBufferingType(SoQtViewer::BufferType);
	virtual void setAntialiasing(SbBool smoothing, int numPasses);
	virtual void setTransparencyType(SoGLRenderAction::TransparencyType);
	virtual void setDrawStyle(SoQtViewer::DrawType, SoQtViewer::DrawStyle);
	virtual void showPopupMenu();

	void setAmbientLight(int);//from 0..100
	int ambientLight() const;

	virtual void setSceneGraph(SoNode *);
	virtual SoNode* getSceneGraph();

	void fadeLastRecordedFrameToCurrent(double time_seconds);

	//For consistency, use these instead of setNumPasses:
	bool isAntiAlias() const;
	void setAntiAlias(bool);

	void startTour();
	bool startTourEachEvent() const;
	void startCustomTour();
	void dumpSceneToFile(QString filename="");//empty file names causes file dialog to be launched
	void dumpSceneToVRMLFile(QString filename="");//empty file names causes file dialog to be launched
	void produceSVGImage(QString filename="");//empty file names causes file dialog to be launched
	void produceEPSImage(QString filename="");//empty file names causes file dialog to be launched

	void storeCameraParametersForReset();
	void resetCamera();

	bool currentCamIsPerspective() const;
	QByteArray currentCameraState() const;

	QPixmap getSnapShotFromCamState(bool camStateIsPerspective, QByteArray camState,
			int width, int height, bool transp = false );

	float stereo_offset_camera;
	float stereo_parallax_camera;
	float stereo_offset_viewer;

	//public slots:

	// stereographic view slots
	void setStereoOffsetSlot(float offset);
	float getStereoOffsetSlot();
	SbBool setStereoTypeSlot(SoQtViewer::StereoType type);
	SoQtViewer::StereoType getStereoTypeSlot(void) const;
	void setAnaglyphStereoColorMasksSlot(const SbBool left[3], const SbBool right[3]);
	void getAnaglyphStereoColorMasksSlot( SbBool left[3], SbBool right[3]);
	//  void setStereoEyeSlot(SoQtViewerP::Eye eye);

	void launchStereoEditor();


protected:
	virtual void bottomWheelFinish();
	virtual void bottomWheelMotion(float val);
	virtual void bottomWheelStart();
	virtual void leftWheelFinish();
	virtual void leftWheelMotion(float val);
	virtual void leftWheelStart();
	virtual void rightWheelFinish();
	virtual void rightWheelMotion(float val);
	virtual void rightWheelStart();

	virtual void createViewerButtons(QWidget * parent, SbPList * buttonlist);
	virtual void buildPopupMenu();
	virtual SbBool processSoEvent(const SoEvent * const event);




private:

	class Imp;
	Imp * d;
	friend class VP1ExaminerViewer_SignalCatcher;

};

class VP1ExaminerViewer_SignalCatcher : public QObject {
	Q_OBJECT
	private slots:
	void catchSignal();
	private:
	VP1ExaminerViewer::Imp * d;
	friend class VP1ExaminerViewer::Imp;
};

#endif
