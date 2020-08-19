/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1CameraHelper                  //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: July 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1CAMERAHELPER_H
#define VP1CAMERAHELPER_H

// FIXME: need to include this before any QT header, because QT defines
// a macro `foreach' that collides with an identifier in AthenaKernel.
#include "xAODCore/BaseInfo.h"

#include <QObject>
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbVec3f.h>

// fwd declarations
class SoQtRenderArea;
class VP1ExaminerViewer;
class SoCamera;
class SoNode;
class SbBox3f;
class SoGroup;
class SoPath;

class VP1CameraHelper : public QObject {

  Q_OBJECT

public:

  static void abortAnyCurrentZoom( SoCamera * camera );

  //Returned pointers are only for the case where you want to catch
  //one of the signals that are emitted when the animation finishes.

  //NB: For the vector arguments below, the special value of
  //SbVec3f(999,999,999) means that the cameras current value will be
  //used!!

  //For zoom to persistified camera states:
  static VP1CameraHelper * animatedZoomToCameraState( SoCamera * camera, SoGroup * sceneroot,
						      const QByteArray& camstate,
						      double duration_in_secs = 1.0, double clipVolPercent=100.0, double lastClipVolPercent=100.0, bool varySpeed=true,
						      bool forceCircular = false );

  //Imitate the zoom towards a point (similar to the ones initiated by the user in an examiner viewer):
  static VP1CameraHelper * animatedZoomToPoint( SoCamera * camera, SoGroup * sceneroot,
						SbVec3f, double duration_in_secs = 1.0, double clipVolPercent=100.0, bool varySpeed=true,
						bool forceCircular = false );

  //For the next three methods, slack < 1.0 gives tighter zoom and slack > 1.0 gives looser zoom.
  //The first two are more reliable:
  static VP1CameraHelper * animatedZoomToPath( SoCamera * camera, SoGroup * sceneroot,
					       SoPath * path, double duration_in_secs = 1.0, double clipVolPercent=100.0, double slack = 1.0,
					       const SbVec3f& lookat = SbVec3f(999,999,999),
					       const SbVec3f& upvec = SbVec3f(999,999,999), bool varySpeed=true,
					       bool forceCircular = false );

  static VP1CameraHelper *  animatedZoomToBBox( SoCamera * camera, SoGroup * sceneroot,
						const SbBox3f& box,double duration_in_secs = 1.0, double clipVolPercent=100.0,  double slack = 1.0,
						const SbVec3f& lookat = SbVec3f(999,999,999),
						const SbVec3f& upvec = SbVec3f(999,999,999),bool varySpeed=true,
						bool forceCircular = false );

  //Dont use the following if subtreeroot appears multiple times in your scenegraph:
  static VP1CameraHelper * animatedZoomToSubTree( SoCamera * camera, SoGroup * sceneroot,
						  SoNode*subtreeroot,double duration_in_secs = 1.0, double clipVolPercent=100.0, double lastClipVolPercent=100.0, double slack = 1.0,
						  const SbVec3f& lookat = SbVec3f(999,999,999),
						  const SbVec3f& upvec = SbVec3f(999,999,999), bool varySpeed=true,
						  bool forceCircular = false );

  //For outputting off-screen rendered frames to image files:
//  void setOutputImagesMode(SoQtRenderArea * ra,
  void setOutputImagesMode(VP1ExaminerViewer * ra,
			   QString outputdir,
			   int width = 1024,
			   int height = 768,
			   double fps = 24,
			   QString prefix = "vp1_frame");

  static void getLastAndNextFrameFileNames( QString outputdir,QString prefix,
					    QString& lastOfExistingFiles,//=> returned as empty string if no existing files
					    QString& nextAvailableFile );

  //Force premature end:
  void forceAbort();

signals:
  void animationFinished();//If Bob's your uncle and everything went as expected.
  void animationFinishedAbnormally();//If e.g. the camera was deleted by someone else during animation.
  void clipVolumePercentageOfATLAS(double);//The percentage of the ATLAS vol corresponding the clipping volume

private:
  VP1CameraHelper( SoCamera *, SoGroup * );
  virtual ~VP1CameraHelper();
  class Imp;
  Imp * m_d;
};

#endif
