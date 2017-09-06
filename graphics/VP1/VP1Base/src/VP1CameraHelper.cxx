/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1CameraHelper                //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: July 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VP1ExaminerViewer.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1QtInventorUtils.h"

#include <Inventor/SbSphere.h>

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoGroup.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>

#include <Inventor/sensors/SoTimerSensor.h>

#include <QDir>
#include <QFile>
#include <QPixmap>

#include <map>
#include <iostream>

//____________________________________________________________________
class VP1CameraHelper::Imp {
public:
  VP1CameraHelper* theclass;
  SoCamera * camera;
  SoGroup * sceneroot;
  QByteArray targetCamState;
  bool forceCircular;
  static void seeksensorCB(void * data, SoSensor *);
  void getCameraCoordinateSystem(SoCamera * cameraarg,SoNode * root,
				 SbMatrix & matrix,SbMatrix & inverse);

  // Seek functionality
  SoTimerSensor * seeksensor;
  float seekperiod;
  SbBool inseekmode;
  SbBool seektopoint;
  float seekdistance;
  SbBool seekdistanceabs;
  //
  SbVec3f camerastartposition, cameraendposition;
  SbRotation camerastartorient, cameraendorient;
  //
  SoSFFloat camerastart_nearDistance, cameraend_nearDistance;
  SoSFFloat camerastart_farDistance, cameraend_farDistance;
  SoSFFloat camerastart_focalDistance, cameraend_focalDistance;
  bool camera_isperspective;
  SoSFFloat camerastart_ortho_height, cameraend_ortho_height;
  
  bool varySpeed;
  double clipVol_startPercentage, clipVol_percentage, clipVol_endPercentage;

  //Movie making mode:
//  SoQtRenderArea * renderArea; // original
  VP1ExaminerViewer * renderArea; // test
  QString outputdir;
  QString prefix;
  double fps;
  int ntotframes;
  int iframe;
  int width;
  int height;

  //Variables to detect if we should abort, if e.g. the user started
  //changing the view by hand:
  SbRotation last_cameraorient;
  SbVec3f last_cameraposition;
  SoSFFloat last_camera_ortho_height;
  double clipVol_lastPercentage;
  void updateLastVars();
  bool lastParsChanged() const;

  void startSeekTimer( double duration_in_secs );

  //
  SoSearchAction * searchaction;
  SoGetMatrixAction * matrixaction;

  //For keeping track of current seeks and whether the camera is invalidated while seeking:
  int32_t camera_ref;
  bool instance_invalid;//This will be set if the camera/sceneroot ref count changes during the lifetime of the VP1CameraHelper.
  bool instanceInvalid();

  bool getCameraParametersForBBox( const SbBox3f & box,
				   SoSFVec3f & position, SoSFFloat & nearDistance,
				   SoSFFloat & farDistance, SoSFFloat & focalDistance,
				   bool&isPerspective,
				   SoSFFloat & height /*Target height in case of orthographic camera*/);

  //Convenience method for construction camera orientation from
  //vectors giving view and "up" directions:
  static SbRotation cameraOrientation( SbVec3f dir, SbVec3f up);

  static std::map<SoCamera*,VP1CameraHelper*> helpers;//All helpers register themselves here.
  //This allows new instances working on the same camera to abort the old ones.

  //zoom to persistified state:
  void actual_animatedZoomToCameraState( const QByteArray& camstate,
					 double duration_in_secs );

  //Imitate the zoom towards a point (similar to the ones initiated by the user in an examiner viewer):
  void actual_animatedZoomToPoint(SbVec3f,double duration_in_secs = 1.0 );

  //For the next three methods, slack < 1.0 gives tighter zoom and slack > 1.0 gives looser zoom.
  //The first two are more reliable:
  void actual_animatedZoomToPath(SoPath * path,double duration_in_secs = 1.0, double slack = 1.0,
			  const SbVec3f& lookat = SbVec3f(999,999,999), const SbVec3f& upvec = SbVec3f(999,999,999) );
  void actual_animatedZoomToBBox(const SbBox3f& box,double duration_in_secs = 1.0, double slack = 1.0,
			  const SbVec3f& lookat = SbVec3f(999,999,999), const SbVec3f& upvec = SbVec3f(999,999,999) );

  //Dont use the following if subtreeroot appears multiple times in your scenegraph:
  void actual_animatedZoomToSubTree(SoNode*subtreeroot,double duration_in_secs = 1.0, double slack = 1.0,
			     const SbVec3f& lookat = SbVec3f(999,999,999), const SbVec3f& upvec = SbVec3f(999,999,999) );

  void die(bool abnormal = true)
  {
    VP1Msg::messageVerbose("VP1CameraHelper::Imp::die(abnormal="+VP1Msg::str(abnormal)+")");
    if (seeksensor&&seeksensor->isScheduled())
      seeksensor->unschedule();
    if (abnormal)
      theclass->animationFinishedAbnormally();
    else
      theclass->animationFinished();
    theclass->deleteLater();
  }

};

//____________________________________________________________________
void VP1CameraHelper::forceAbort()
{
  std::map<SoCamera*,VP1CameraHelper*>::iterator it = Imp::helpers.find(d->camera);
  if (it!=Imp::helpers.end())
    Imp::helpers.erase(it);
  d->instance_invalid = true;
  d->camera->unrefNoDelete();
  d->camera = 0;
}

std::map<SoCamera*,VP1CameraHelper*> VP1CameraHelper::Imp::helpers;

//____________________________________________________________________
//void VP1CameraHelper::setOutputImagesMode( SoQtRenderArea * ra,
void VP1CameraHelper::setOutputImagesMode( VP1ExaminerViewer * ra,
					   QString outputdir,
					   int width,
					   int height,
					   double fps,
					   QString prefix )
{
  if (!ra) {
    VP1Msg::messageDebug("VP1CameraHelper::setOutputImagesMode ERROR: Bad input. Null render area.");
    return;
  }
  if (fps<1) {
    VP1Msg::messageDebug("VP1CameraHelper::setOutputImagesMode ERROR: Bad input. Bad FPS = "+QString::number(fps));
    return;
  }
  if (!QDir(outputdir).exists()) {
    VP1Msg::messageDebug("VP1CameraHelper::setOutputImagesMode ERROR: Bad input. Output dir "+outputdir+" does not exist.");
    return;
  }
  if (width<1||width>4000||height<1||height>4000) {
    VP1Msg::messageDebug("VP1CameraHelper::setOutputImagesMode ERROR: Bad input. Bad dimensions: "
			 +QString::number(width)+"x"+QString::number(height));
    return;
  }
  if (prefix.isEmpty()) {
    VP1Msg::messageDebug("VP1CameraHelper::setOutputImagesMode ERROR: Bad input. Empty prefix.");
    return;
  }
  d->fps = fps;
  d->outputdir = outputdir;
  d->width = width;
  d->height = height;
  d->prefix = prefix;
  d->renderArea = ra;
}


//____________________________________________________________________
VP1CameraHelper::VP1CameraHelper(SoCamera * camera, SoGroup * sceneroot)
  : QObject(0), d(new Imp)
{
  d->theclass=this;
  d->camera=camera;
  d->forceCircular = false;
  if (camera) {
    std::map<SoCamera*,VP1CameraHelper*>::iterator it = Imp::helpers.find(camera);
    if (it!=Imp::helpers.end())
      it->second->forceAbort();
    Imp::helpers[camera] = this;
  }
  d->sceneroot=sceneroot;

  d->fps=-1;
  d->ntotframes = -1;
  d->width = 100;
  d->height = 100;
  d->iframe = -1;
  d->renderArea = 0;

  d->seekdistance = 50.0f;
  d->seekdistanceabs = FALSE;
  d->seektopoint = TRUE;
  d->seekperiod = 2.0f;
  d->inseekmode = FALSE;
  d->seeksensor = new SoTimerSensor(Imp::seeksensorCB, d);

  d->searchaction = new SoSearchAction;
  d->matrixaction = new SoGetMatrixAction(SbViewportRegion(100,100));

  d->camera->ref();
  d->sceneroot->ref();
  d->camera_ref=d->camera->getRefCount();
  d->instance_invalid = false;

  if (!camera||!sceneroot)
    d->instance_invalid=true;

}


//____________________________________________________________________
VP1CameraHelper::~VP1CameraHelper()
{
  if (d->seeksensor->isScheduled())
    d->seeksensor->unschedule();
  delete d->seeksensor;
  d->searchaction->reset(); delete d->searchaction;
  delete d->matrixaction;
  SoCamera * cam = d->camera;
  if (d->sceneroot)
    d->sceneroot->unref();
  delete d; d=0;
  if (cam) {
    std::map<SoCamera*,VP1CameraHelper*>::iterator it = Imp::helpers.find(cam);
    if (it!=Imp::helpers.end())
      Imp::helpers.erase(it);
    cam->unref();
  }
}

//____________________________________________________________________
void VP1CameraHelper::abortAnyCurrentZoom( SoCamera * camera )
{
  std::map<SoCamera*,VP1CameraHelper*>::iterator it = Imp::helpers.find(camera);
  VP1CameraHelper* helper(0);
  if (it!=Imp::helpers.end()) {
    helper = it->second;
  }
  if (helper)
    helper->forceAbort();
}


//____________________________________________________________________
void VP1CameraHelper::Imp::updateLastVars()
{
  last_cameraposition=camera->position.getValue();
  last_cameraorient = camera->orientation.getValue();
  if (!camera_isperspective)
    last_camera_ortho_height=static_cast<SoOrthographicCamera*>(camera)->height;
  clipVol_lastPercentage = clipVol_percentage;
}

//____________________________________________________________________
bool VP1CameraHelper::Imp::lastParsChanged() const
{
  if (camera->position.getValue()!=last_cameraposition
      ||camera->orientation.getValue()!=last_cameraorient)
    return true;
  if (!camera_isperspective
      &&static_cast<SoOrthographicCamera*>(camera)->height!=last_camera_ortho_height)
      return true;
  return false;
}

//____________________________________________________________________
//STATIC:
VP1CameraHelper * VP1CameraHelper::animatedZoomToCameraState( SoCamera * camera, SoGroup * sceneroot,
							      const QByteArray& camstate,
							      double duration_in_secs, double clipVolPercent, double lastClipVolPercent, bool varySpeed,
							      bool forceCircular )
{
  // std::cout<<"VP1CameraHelper::animatedZoomToCameraState - clipVol%="<<clipVolPercent<<" lastClipVolPercent"<<lastClipVolPercent<<std::endl;
  // std::cout<<"clipVol_lastPercentage%="<<d->clipVol_lastPercentage<<", clipVol_percentage%="<<d->clipVol_percentage
  //   <<", clipVol_startPercentage%="<<d->clipVol_startPercentage<<", clipVol_endPercentage%="<<d->clipVol_endPercentage<<std::endl;
  VP1CameraHelper * helper = new VP1CameraHelper(camera,sceneroot);
  helper->d->actual_animatedZoomToCameraState( camstate,duration_in_secs );
  helper->d->varySpeed=varySpeed;
  helper->d->clipVol_startPercentage=lastClipVolPercent;
  helper->d->clipVol_percentage=lastClipVolPercent;
  helper->d->clipVol_endPercentage=clipVolPercent;
  helper->d->forceCircular = forceCircular;
  return helper;
}

//____________________________________________________________________
void VP1CameraHelper::Imp::actual_animatedZoomToCameraState( const QByteArray& camstate,
							     double duration_in_secs )
{
  if (!camera||camstate==QByteArray()||duration_in_secs<0.0) {
    die();
    return;
  }

  if (instanceInvalid()) {
    if (seeksensor->isScheduled())
      seeksensor->unschedule();
    die();
    return;
  }

  targetCamState = camstate;

  const bool perspective(camera->getTypeId().isDerivedFrom(SoPerspectiveCamera::getClassTypeId()));
  SoCamera * targetcam = perspective ? static_cast<SoCamera*>(new SoPerspectiveCamera)
                                     : static_cast<SoCamera*>(new SoOrthographicCamera);
  targetcam->ref();
  QByteArray ba = camstate;
  if (!VP1QtInventorUtils::deserializeSoCameraParameters( ba, *targetcam )) {
    targetcam->unref();
    die();
    return;
  }

  camera_isperspective = perspective;
  camerastartposition = camera->position.getValue();
  camerastartorient = camera->orientation.getValue();
  camerastart_nearDistance = camera->nearDistance.getValue();
  camerastart_farDistance = camera->farDistance.getValue();
  camerastart_focalDistance = camera->focalDistance.getValue();
  cameraendposition = targetcam->position.getValue();
  cameraendorient = targetcam->orientation.getValue();
  cameraend_nearDistance = targetcam->nearDistance.getValue();
  cameraend_farDistance = targetcam->farDistance.getValue();
  cameraend_focalDistance = targetcam->focalDistance.getValue();
  if (!perspective) {
    camerastart_ortho_height = static_cast<SoOrthographicCamera*>(camera)->height.getValue();
    cameraend_ortho_height = static_cast<SoOrthographicCamera*>(targetcam)->height.getValue();
  }
  targetcam->unref();

  startSeekTimer( duration_in_secs );
}


//____________________________________________________________________
//STATIC:
VP1CameraHelper*  VP1CameraHelper::animatedZoomToPath( SoCamera * camera, SoGroup * sceneroot,
						       SoPath * path,double duration_in_secs, double /*clipVolPercent*/, double slack,
						       const SbVec3f& lookat, const SbVec3f& upvec, bool varySpeed,
						       bool forceCircular )
{  
  VP1CameraHelper * helper = new VP1CameraHelper(camera,sceneroot);
  helper->d->actual_animatedZoomToPath( path,duration_in_secs, slack, lookat, upvec );
  helper->d->varySpeed=varySpeed;
  helper->d->forceCircular = forceCircular;
  return helper;
}

//____________________________________________________________________
void VP1CameraHelper::Imp::actual_animatedZoomToPath( SoPath * path,double duration_in_secs, double slack,
						      const SbVec3f& lookat, const SbVec3f& upvec )
{
  if (!path||instanceInvalid()) {
    die();
    return;
  }

  if (slack<=0.0||duration_in_secs<0.0) {
    die();
    return;
  }

  if (instanceInvalid()) {
    if (seeksensor->isScheduled())
      seeksensor->unschedule();
    die();
    return;
  }
  SbViewportRegion dummyvp;
  SoGetBoundingBoxAction mybboxaction(dummyvp);
  mybboxaction.apply(path);
  SbBox3f box = mybboxaction.getBoundingBox();

  actual_animatedZoomToBBox(box, duration_in_secs, slack, lookat,upvec);
}

//____________________________________________________________________
//STATIC:
VP1CameraHelper * VP1CameraHelper::animatedZoomToSubTree(SoCamera * camera, SoGroup * sceneroot,
							 SoNode*subtreeroot,double duration_in_secs, double /* clipVolPercent*/, double /*lastClipVolPercent*/, double slack,
							 const SbVec3f& lookat, const SbVec3f& upvec, bool varySpeed,
							 bool forceCircular )
{  
  VP1CameraHelper * helper = new VP1CameraHelper(camera,sceneroot);
  helper->d->actual_animatedZoomToSubTree( subtreeroot,duration_in_secs,slack,lookat,upvec );
  helper->d->varySpeed=varySpeed;
  helper->d->forceCircular = forceCircular;
  return helper;
}

//____________________________________________________________________
void VP1CameraHelper::Imp::actual_animatedZoomToSubTree( SoNode*subtreeroot,double duration_in_secs, double slack,
						    const SbVec3f& lookat, const SbVec3f& upvec)
{
  if (!subtreeroot||instanceInvalid()) {
    die();
    return;
  }

  if (slack<=0.0||duration_in_secs<0.0) {
    die();
    return;
  }

  if (instanceInvalid()) {
    die();
    return;
  }

  SoSearchAction mysearchaction;
  mysearchaction.setNode(subtreeroot);
  mysearchaction.apply(sceneroot);
  SoPath * mypath = mysearchaction.getPath();
  if (!mypath) {
    VP1Msg::messageDebug("VP1CameraHelper WARNING: Cancelled operation due to failure to locate path to node.");
    return;
  }
  mypath->ref();
  actual_animatedZoomToPath( mypath, duration_in_secs, slack, lookat,upvec );
  mypath->unref();
  mysearchaction.reset();
}

//____________________________________________________________________
//STATIC:
VP1CameraHelper * VP1CameraHelper::animatedZoomToBBox(SoCamera * camera, SoGroup * sceneroot,
						      const SbBox3f& box,double duration_in_secs, double /*clipVolPercent*/, double slack,
						      const SbVec3f& lookat, const SbVec3f& upvec , bool varySpeed,
						      bool forceCircular )
{
  
  VP1CameraHelper * helper = new VP1CameraHelper(camera,sceneroot);
  helper->d->actual_animatedZoomToBBox( box,duration_in_secs,slack,lookat,upvec );
  helper->d->varySpeed=varySpeed;
  helper->d->forceCircular = forceCircular;
  return helper;
}

//____________________________________________________________________
void VP1CameraHelper::Imp::actual_animatedZoomToBBox( const SbBox3f& box,double duration_in_secs, double slack,
						      const SbVec3f& lookat, const SbVec3f& upvec )
{
  if (instanceInvalid()) {
    die();
    return;
  }

  if (slack<=0.0||duration_in_secs<0.0) {
    die();
    return;
  }

  if (instanceInvalid()) {
    die();
    return;
  }

  if (box.isEmpty()) {
    die();
    return;
  }
  SbBox3f actualbbox = box;
  if (slack!=1.0) {
    float minx, miny, minz, maxx, maxy, maxz;
    actualbbox.getBounds (minx, miny, minz, maxx, maxy, maxz);
    float dx(maxx-minx), cx(0.5*(maxx+minx));
    float dy(maxy-miny), cy(0.5*(maxy+miny));
    float dz(maxz-minz), cz(0.5*(maxz+minz));
    actualbbox.setBounds (cx-0.5*slack*dx, cy-0.5*slack*dy, cz-0.5*slack*dz,
			  cx+0.5*slack*dx, cy+0.5*slack*dy, cz+0.5*slack*dz);
  }



  SoSFVec3f target_position;
  SoSFFloat target_nearDistance;
  SoSFFloat target_farDistance;
  SoSFFloat target_focalDistance;
  bool currentcamera_isperspective;
  SoSFFloat target_ortho_height;

  camerastartorient = camera->orientation.getValue();

  bool changeorient(false);
  if (lookat==SbVec3f(999,999,999)&&upvec==SbVec3f(999,999,999)) {
    cameraendorient = camerastartorient;//Same orientation
  } else {
    changeorient=true;
    //At least one of view direction and upwards direction is set by user
    SbVec3f actual_lookat(lookat);
    if (lookat==SbVec3f(999,999,999)) {
      //Use present view direction of camera
      SbRotation camrot = camera->orientation.getValue();
      actual_lookat = SbVec3f(0.0f, 0.0f, -1.0f); // init to default view direction vector
      camrot.multVec(actual_lookat, actual_lookat);
    }
    SbVec3f actual_upvec(upvec);
    if (upvec==SbVec3f(999,999,999)) {
      //Use present upwards direction of camera
      SbRotation camrot = camera->orientation.getValue();
      actual_upvec = SbVec3f(0.0f, 1.0f, 0.0f); // init to default up vector
      camrot.multVec(actual_upvec, actual_upvec);
      //       { float x,y,z; actual_upvec.getValue(x,y,z);
      // 	//std::cout<<"USE PRESENT CAMERA UPVEC ( "<<x<<", "<<y<<", "<<z<<" )"<<std::endl;
      //       }
    }
    //Use actual_lookat and actual_upvec to construct an orientation
    //of the camera (refer to the SoCamera doc. for how it should be
    //defined):
    cameraendorient = Imp::cameraOrientation(actual_lookat,actual_upvec);
  }


  if (changeorient)
    camera->orientation.setValue(cameraendorient);
  if (!getCameraParametersForBBox( actualbbox,
				      target_position, target_nearDistance,
				      target_farDistance, target_focalDistance,
				      currentcamera_isperspective,target_ortho_height)) {
    die();
    return;
  }
  camerastartposition = camera->position.getValue();
  cameraendposition = target_position.getValue();

  camerastart_nearDistance = camera->nearDistance.getValue();
  cameraend_nearDistance = target_nearDistance;
  camerastart_farDistance = camera->farDistance.getValue();
  cameraend_farDistance = target_farDistance;
  camerastart_focalDistance = camera->focalDistance.getValue();
  cameraend_focalDistance = target_focalDistance;
  camera_isperspective = currentcamera_isperspective;
  if (camera_isperspective) {
    camerastart_ortho_height = 1.0f;//Irrelevant value since not needed (we could cast to orthographic camera and get the real value).
    cameraend_ortho_height = camerastart_ortho_height;
  } else {
    SoOrthographicCamera * orthocam =static_cast<SoOrthographicCamera*>(camera);
    if (!orthocam) {
      if (changeorient)
	camera->orientation.setValue(camerastartorient);
      die();
      return;
    }
    camerastart_ortho_height = orthocam->height.getValue();
    cameraend_ortho_height = target_ortho_height;
  }
  if (changeorient)
    camera->orientation.setValue(camerastartorient);

  //Start seek:

  startSeekTimer(duration_in_secs);
}


//____________________________________________________________________
//STATIC:
VP1CameraHelper * VP1CameraHelper::animatedZoomToPoint(SoCamera * camera, SoGroup * sceneroot,
						       SbVec3f targetpoint,double duration_in_secs, double /*clipVolPercent*/, bool varySpeed,
						       bool forceCircular )
{  
  VP1CameraHelper * helper = new VP1CameraHelper(camera,sceneroot);
  helper->d->actual_animatedZoomToPoint( targetpoint,duration_in_secs );
  helper->d->varySpeed=varySpeed;
  helper->d->forceCircular = forceCircular;
  return helper;
}

//____________________________________________________________________
void VP1CameraHelper::Imp::startSeekTimer( double duration_in_secs )
{
  seekperiod = static_cast<float>(duration_in_secs);

  if (seeksensor->isScheduled())
    seeksensor->unschedule();

  seeksensor->setBaseTime(SbTime::getTimeOfDay());
  seeksensor->schedule();

  updateLastVars();

}

//____________________________________________________________________
void VP1CameraHelper::Imp::actual_animatedZoomToPoint( SbVec3f targetpoint,double duration_in_secs )
{
  if (instanceInvalid()) {
    die();
    return;
  }

  if (duration_in_secs<0.0) {
    die();
    return;
  }

  if (instanceInvalid()) {
    die();
    return;
  }

  camerastartposition = camera->position.getValue();
  camerastartorient = camera->orientation.getValue();

  //The following variables are not needed in seekToPoint mode, so
  //give them values indicating they should not be updated:

  camerastart_nearDistance = camera->nearDistance.getValue();
  cameraend_nearDistance = camerastart_nearDistance;
  camerastart_farDistance = camera->farDistance.getValue();
  cameraend_farDistance = camerastart_farDistance;
  camerastart_focalDistance = camera->focalDistance.getValue();
  cameraend_focalDistance = camerastart_focalDistance;
  //It actually doesnt matter if we set this one:
  camera_isperspective=!(camera->getTypeId() == SoOrthographicCamera::getClassTypeId());
  camerastart_ortho_height = 1.0f;//Irrelevant value since not needed (we could cast to orthographic camera and get the real value).
  cameraend_ortho_height = camerastart_ortho_height;

  // move point to the camera coordinate system, consider
  // transformations before camera in the scene graph
  SbMatrix cameramatrix, camerainverse;
  getCameraCoordinateSystem(camera,
			       sceneroot,
			       cameramatrix,
			       camerainverse);
  camerainverse.multVecMatrix(targetpoint, targetpoint);
  float fd = seekdistance;
  if (!seekdistanceabs)
    fd *= (targetpoint - camera->position.getValue()).length()/100.0f;
  camera->focalDistance = fd;

  SbVec3f dir = targetpoint - camerastartposition;
  dir.normalize();

  // find a rotation that rotates current camera direction into new
  // camera direction.
  SbVec3f olddir;
  camera->orientation.getValue().multVec(SbVec3f(0, 0, -1), olddir);
  SbRotation diffrot(olddir, dir);
  cameraendposition = targetpoint - fd * dir;
  cameraendorient = camera->orientation.getValue() * diffrot;

  startSeekTimer(duration_in_secs);
}


//____________________________________________________________________
void VP1CameraHelper::Imp::seeksensorCB(void * data, SoSensor * s)
{
	VP1Msg::messageVerbose("VP1CameraHelper::Imp::seeksensorCB()");

  VP1CameraHelper::Imp * d = (VP1CameraHelper::Imp *)data;
  if (!d||!s)
    return;
  if (d->instanceInvalid() || d->lastParsChanged()) {
    d->die();
    return;
  }

  SoTimerSensor * sensor = (SoTimerSensor *)s;

  float t(0);

  if (d->fps>0) {
    //fixed number of frames and output
    if (d->ntotframes<0) {
      //first time: figure out total number of frames
      d->ntotframes = std::max(1,static_cast<int>((d->fps)*(d->seekperiod)+0.5));
      d->iframe = 0;
    }
    //Example: 5 total frames should give times 0.2, 0.4, 0.6, 0.8,
    //1.0. We should _not_ include 0.0 here. Otherwise chained zooms
    //will get a frame at both the old 1.0 and the new 0.0.
    t = ( (d->iframe+1==d->ntotframes) ? 1.0f : static_cast<float>((d->iframe+1)*1.0/(d->ntotframes*1.0)) );
    ++(d->iframe);
  } else {
    //normal animation:
    SbTime currenttime = SbTime::getTimeOfDay();
    t = float((currenttime - sensor->getBaseTime()).getValue()) / d->seekperiod;
  }

  if ((t > 1.0f) || (d->fps<=0&&(t + sensor->getInterval().getValue()/d->seekperiod > 1.0f))) t = 1.0f;
  
  d->clipVol_percentage = d->clipVol_startPercentage+(d->clipVol_endPercentage - d->clipVol_startPercentage)*t;
  emit d->theclass->clipVolumePercentageOfATLAS(d->clipVol_percentage);

  bool end = (t == 1.0f);

  if (d->varySpeed)
    t = (float) ((1.0 - cos(M_PI*t)) * 0.5);//Map t to slow down motion in the beginning and end of the animation.

  if (d->camera->position.getValue()!=d->cameraendposition) {
    double rBegin=d->camerastartposition.length();
    double rEnd=d->cameraendposition.length();
    double lengthRatio = rEnd/rBegin;
    SbVec3f u = d->camerastartposition; u.normalize();
    SbVec3f v = d->cameraendposition; v.normalize();
    double angle = acos(u.dot(v));
    if (d->forceCircular||(lengthRatio>0.99 && lengthRatio<1.01 && angle<0.95*M_PI && angle>0.05*M_PI)) {
      double r = rBegin + (rEnd-rBegin)*t;
      SbVec3f w = u.cross(v);
      SbRotation R(w,t*angle);
      SbVec3f z;
      R.multVec(u,z);
      z*=r;
      d->camera->position=z;
    } else {
      d->camera->position = d->camerastartposition + (d->cameraendposition - d->camerastartposition) * t;
    }
  }
  if (d->camera->orientation.getValue()!=d->cameraendorient)
    d->camera->orientation = SbRotation::slerp(d->camerastartorient,d->cameraendorient,t);
  //TKTEST: Comment out the next lines to fix camera??>
  //Other thing to try: Something about disable notify while setting multiple pars??

  //TEST  if (d->camera->nearDistance!=d->cameraend_nearDistance)
  //TEST    d->camera->nearDistance.setValue(d->camerastart_nearDistance.getValue()*(1-t) + d->cameraend_nearDistance.getValue()*t);
  //TEST  if (d->camera->farDistance!=d->cameraend_farDistance)
  //TEST    d->camera->farDistance.setValue(d->camerastart_farDistance.getValue()*(1-t) + d->cameraend_farDistance.getValue()*t);
  if (d->camera->focalDistance!=d->cameraend_focalDistance)
    d->camera->focalDistance.setValue(d->camerastart_focalDistance.getValue()*(1-t) + d->cameraend_focalDistance.getValue()*t);
  if (!d->camera_isperspective) {
    SoOrthographicCamera * orthocam =static_cast<SoOrthographicCamera*>(d->camera);
    if (orthocam&&orthocam->height!=d->cameraend_ortho_height)
      orthocam->height.setValue(d->camerastart_ortho_height.getValue()*(1-t) + d->cameraend_ortho_height.getValue()*t);
  }

  d->updateLastVars();

  if (d->ntotframes>0) {
    QString dummy, filename;
    VP1CameraHelper::getLastAndNextFrameFileNames( d->outputdir,d->prefix,dummy, filename );
    if (d->seekperiod>0.0) {
      VP1Msg::messageVerbose("VP1CameraHelper: Attempting to create image "+filename);
      QPixmap pm = VP1QtInventorUtils::renderToPixmap(d->renderArea,
						      d->width,d->height,false);
      if (pm.isNull()) {
	VP1Msg::messageDebug("VP1CameraHelper ERROR: Could not create image");
      } else {
	if (!pm.save(filename))
	  VP1Msg::messageDebug("VP1CameraHelper ERROR: Could not save image file "+filename);
      }
    }
  }

  if (end) {
    if (d->targetCamState!=QByteArray()) {
      if (!VP1QtInventorUtils::deserializeSoCameraParameters( d->targetCamState, *(d->camera) )) {
	d->die();
	return;
      }
    }
    d->die(false/*non-abnormal end!*/);
  }

}

//____________________________________________________________________
void VP1CameraHelper::getLastAndNextFrameFileNames( QString outputdir,QString prefix,
						    QString& lastOfExistingFiles,
						    QString& nextAvailableFile )
{
  lastOfExistingFiles="";
  QString filename;
  int i(0);
  while (true) {
    filename=outputdir+"/"+prefix+"_"+QString::number(i++).rightJustified(6,'0')+".png";
    if (!QFile::exists(filename)) {
      nextAvailableFile=filename;
      return;
    }
    lastOfExistingFiles = filename;
  }
}

//____________________________________________________________________
bool VP1CameraHelper::Imp::instanceInvalid()
{
  if (instance_invalid)
    return true;
  if (!camera||camera_ref!=camera->getRefCount()) {
    instance_invalid=true;
    VP1Msg::messageDebug("VP1CameraHelper WARNING: Cancelled operation due to invalid camera.");
    return true;
  }
  if (sceneroot->getRefCount()<=1) {
    instance_invalid=true;
    VP1Msg::messageDebug("VP1CameraHelper WARNING: Cancelled operation due to sceneroot being unref'ed.");
    return true;
  }

  return false;
}

//____________________________________________________________________
void VP1CameraHelper::Imp::getCameraCoordinateSystem(SoCamera * cameraarg,
						     SoNode * root,
						     SbMatrix & matrix,
						     SbMatrix & inverse)
{
  searchaction->reset();
  searchaction->setSearchingAll(TRUE);
  searchaction->setInterest(SoSearchAction::FIRST);
  searchaction->setNode(cameraarg);
  searchaction->apply(root);

  matrix = inverse = SbMatrix::identity();
  if (searchaction->getPath()) {
    matrixaction->apply(searchaction->getPath());
    matrix = matrixaction->getMatrix();
    inverse = matrixaction->getInverse();
  }
  searchaction->reset();
}

//____________________________________________________________________
bool VP1CameraHelper::Imp::getCameraParametersForBBox( const SbBox3f & box,
						       SoSFVec3f & position, SoSFFloat & nearDistance,
						       SoSFFloat & farDistance, SoSFFloat & focalDistance,
						       bool&isPerspective,
						       SoSFFloat & height /*Target height in case of orthographic camera*/ )
{
  float aspect = camera->aspectRatio.getValue();

 if (box.isEmpty())
    return false;
  if (camera->getTypeId() == SoPerspectiveCamera::getClassTypeId()) {
    isPerspective=true;
  } else if (camera->getTypeId() == SoOrthographicCamera::getClassTypeId()) {
    isPerspective=false;
  } else {
    VP1Msg::messageDebug("VP1CameraHelper WARNING: Unknown camera type. Animation aborted.");
    return false;
  }

  SbVec3f cameradirection;
  camera->orientation.getValue().multVec(SbVec3f(0, 0, -1), cameradirection);
  position.setValue(box.getCenter() + -cameradirection);

  // Get the radius of the bounding sphere.
  SbSphere bs;
  bs.circumscribe(box);
  float radius = bs.getRadius();

  if (isPerspective) {
    // Make sure that everything will still be inside the viewing volume
    // even if the aspect ratio "favorizes" width over height.
    float aspectradius = radius / (aspect < 1.0f ? aspect : 1.0f);

    SoPerspectiveCamera * perspcam =static_cast<SoPerspectiveCamera*>(camera);
    if (!perspcam)
      return false;
    // Move the camera to the edge of the bounding sphere, while still
    // pointing at the scene.
    SbVec3f direction = position.getValue() - box.getCenter();
    (void) direction.normalize(); // we know this is not a null vector
    float movelength =
      aspectradius + (aspectradius/float(atan(perspcam->heightAngle.getValue())));
    position.setValue(box.getCenter() + direction * movelength);
  } else {
    // Make sure that everything will still be inside the viewing volume
    // even if the aspect ratio "favorizes" width over height.
    if (aspect < 1.0f)
      height = 2 * radius / aspect;
    else
      height = 2 * radius;

    // Move the camera to the edge of the bounding sphere, while still
    // pointing at the scene.
    SbVec3f direction = position.getValue() - box.getCenter();
    (void) direction.normalize(); // we know this is not a null vector
    position.setValue(box.getCenter() + direction * radius);
  }

  // Set up the clipping planes according to the slack value (a value
  // of 1.0 will yield clipping planes that are tangent to the
  // bounding sphere of the scene).
  float distance_to_midpoint = (position.getValue() - box.getCenter()).length();
  nearDistance = distance_to_midpoint - radius;
  farDistance = distance_to_midpoint + radius;


  // The focal distance is simply the distance from the camera to the
  // scene midpoint. This field is not used in rendering, its just
  // provided to make it easier for the user to do calculations based
  // on the distance between the camera and the scene.
  focalDistance = distance_to_midpoint;

  return true;
}

//____________________________________________________________________
SbRotation VP1CameraHelper::Imp::cameraOrientation( SbVec3f dir, SbVec3f up)
{
  const float epsilon = 0.00001f;

  //Input vectors must be non-zero:
  if (dir.sqrLength()==0.0f||up.sqrLength()==0.0f) {
    VP1Msg::messageDebug("VP1CameraHelper Warning: Asked to determine camera orientation from "
			 "view and up direction vectors that are not both non-zero");
    return SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);//Default rotation (as in SoCamera.cpp)
  }

  //Normalize input:
  dir.normalize();
  up.normalize();

  //Direction and upvector corresponding to identity rotation:
  SbVec3f dir0(0.0f, 0.0f, -1.0f), up0(0.0f, 1.0f, 0.0f);

  //First we construct the rotation which takes dir0 into dir:
  SbRotation rot(dir0,dir);

  //dir and up should not be parallel:
  if (fabs(dir.dot(up))>1.0f-epsilon) {
    VP1Msg::messageDebug("VP1CameraHelper Warning: Asked to determine camera orientation from "
			 "view and up direction vectors that are essentially parallel."
			 " Choosing new arbitrary up direction.");
    up = dir.cross(SbVec3f(1.0f,1.0f,0.0f));
    up.normalize();
    if (fabs(dir.dot(up))>1.0f-epsilon) {
      up = dir.cross(SbVec3f(1.0f,0.0f,1.0f));
      up.normalize();
      if (fabs(dir.dot(up))>1.0f-epsilon) {
	VP1Msg::messageDebug("VP1CameraHelper Warning: Unable to find useful up-vector.");
	return rot;
      }
    }
  }

  //The second rotation should rotate around dir (to leave it
  //unchanged), and should take, in the plane orthogonal to dir, (the
  //projection of) rot*up0 into (the projection of) up. This is achieved
  //by first putting the two up vectors into the plane orthogonal to
  //dir, and then simply finding the rotation which brings up0 into
  //up:

  //Put up vectors into plane orthogonal to dir.
  up = up - dir*(dir.dot(up));
  if (up.length()<epsilon) {
    //up and dir are parallel?
    VP1Msg::messageDebug("VP1CameraHelper Warning: Can't determine correct up direction (1).");
    return rot;
  }
  up.normalize();

  rot.multVec(up0,up0);
  up0 = up0 - dir*(dir.dot(up0));
  if (up0.length()<epsilon) {
    //up0 and dir are parallel?
    VP1Msg::messageDebug("VP1CameraHelper Warning: Can't determine correct up direction (2).");
    return rot;
  }
  up0.normalize();

  //Done:
  return rot*SbRotation(up0,up);
}
