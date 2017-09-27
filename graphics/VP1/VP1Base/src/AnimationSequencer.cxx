/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1Base/AnimationSequencer.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VP1ExaminerViewer.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/VP1QtInventorUtils.h"

#include <QTimer>

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoGroup.h>

#include <iostream>

class AnimationSequencer::Clockwork {

public:

  AnimationSequence          animationSequence;
  mutable unsigned int       animationFrameNumber;
  VP1ExaminerViewer         *viewer;
  SoSphere                  *sphere;

  SoSphere * getRegionSphere(AnimationSequence::REGION, bool perspective);

  bool movieEnabled;
  int movieFPS;
  int movieWidth;
  int movieHeight;
  QString movieOutdir;
  QString frameFileNamePrefix;
  double last_clipVolPercent;
};

void AnimationSequencer::setMovie(bool b)
{
  m_c->movieEnabled = b;
}

void AnimationSequencer::setMovieParameters(QString outdir, QString frameFileNamePrefix, int fps, int width, int height)
{
  m_c->movieFPS = fps;
  m_c->movieWidth = width;
  m_c->movieHeight = height;
  m_c->movieOutdir = outdir;
  m_c->frameFileNamePrefix = frameFileNamePrefix;
}

AnimationSequencer::AnimationSequencer(VP1ExaminerViewer *viewer) :
  m_c(new Clockwork)
{
  m_c->animationFrameNumber=0;
  m_c->viewer=viewer;
  m_c->sphere=NULL;
  m_c->movieEnabled=false;
  m_c->movieFPS=24;
  m_c->movieWidth=100;
  m_c->movieHeight=100;
  m_c->movieOutdir="/tmp/vp1frames";
  m_c->frameFileNamePrefix="vp1_frame";
}

AnimationSequencer::~AnimationSequencer()
{
  if (m_c->sphere) m_c->sphere->unref();
  delete m_c;
}

AnimationSequence & AnimationSequencer::sequence() {
  return m_c->animationSequence;
}

const AnimationSequence & AnimationSequencer::sequence() const {
  return m_c->animationSequence;
}

void AnimationSequencer::startAnimating(bool skipFirstFrame) {

  m_c->animationFrameNumber=(skipFirstFrame?1:0);
  m_c->last_clipVolPercent = 100;
  nextAnimationFrame();
}

void AnimationSequencer::abortAnimation() {
  //Make sure we halt:
  VP1Msg::messageVerbose("AnimationSequencer::abortAnimation.");
  m_c->animationFrameNumber = m_c->animationFrameNumber>=sequence().getNumFrames() + 1;
}

void AnimationSequencer::nextAnimationFrame()
{
  VP1Msg::messageVerbose("AnimationSequencer::nextAnimationFrame.");

  if (m_c->animationFrameNumber>=sequence().getNumFrames()) {
    emit animationFinishedSuccessfully();
    VP1Msg::messageVerbose("animation succeeded!");
    return;
  }

  SoNode * rootnode = m_c->viewer->getSceneGraph();
  if (!rootnode) {
    VP1Msg::messageDebug("AnimationSequencer::nextAnimationFrame WARNING: No scenegraph set. Ignoring.");
    return;
  }
  if ( ! ( rootnode->getTypeId().isDerivedFrom(SoGroup::getClassTypeId())) ) {
    VP1Msg::messageDebug("AnimationSequencer::nextAnimationFrame WARNING: Root node does not derive from SoGroup. Ignoring.");
    return;
  }

//   if (m_c->viewer->getCameraType() == SoPerspectiveCamera::getClassTypeId() ) {
//     //Fix for bad camera:
//     m_c->viewer->toggleCameraType();
//     m_c->viewer->toggleCameraType();
//   }

  SoCamera * camera = m_c->viewer->getCamera();
  if (!camera) {
    VP1Msg::messageDebug("AnimationSequencer::nextAnimationFrame WARNING: Could not get camera. Ignoring.");
    return;
  }

  rootnode->ref();
  SoGroup * root = static_cast<SoGroup*>(rootnode);

  unsigned int i = m_c->animationFrameNumber;
  

  //Get region:
  AnimationSequence::Frame f=sequence().getFrame(i);
  // std::cout<<"Frame: "<<i<<" clipVolumePercentOfATLAS="<<f.clipVolPercent<<std::endl;

  if (f.time==0.0&&!f.camState.isEmpty())
  {
    if (VP1QtInventorUtils::deserializeSoCameraParameters(f.camState,*camera))
      QTimer::singleShot(0, this, SLOT(nextAnimationFrame()));
    else
      QTimer::singleShot(0, this, SLOT(abortAnimation()));
  } else {
    VP1CameraHelper *helper(0);
    if (f.camState.isEmpty()) {
      // std::cout<<"camState empty"<<std::endl;
      camera->ref();
      bool notifyenabled = root->enableNotify(false);
      SoSphere * regionsphere = m_c->getRegionSphere(AnimationSequence::REGION(f.reg),
						   camera->getTypeId().isDerivedFrom(SoPerspectiveCamera::getClassTypeId()));
      camera->unrefNoDelete();
      //Get direction:
      SbVec3f lookat=f.dir, upvec = f.upvec;
      root->insertChild(regionsphere,0);
      VP1Msg::messageVerbose("nextAnimationFrame Initiating zoom to region sphere.");
      helper = VP1CameraHelper::animatedZoomToSubTree(camera,root,regionsphere,f.time,f.clipVolPercent, m_c->last_clipVolPercent,0.1,lookat,upvec,f.variableSpeed,f.forceCircular);

      root->removeChild(regionsphere);

      if (notifyenabled) {
    	  root->enableNotify(true);
    	  root->touch();
      }

    } else {
      // std::cout<<"camState not empty"<<std::endl;
      helper = VP1CameraHelper::animatedZoomToCameraState( camera,root,f.camState,f.time,f.clipVolPercent,  m_c->last_clipVolPercent, f.variableSpeed,f.forceCircular );
    }

    if (m_c->movieEnabled)
      helper->setOutputImagesMode(m_c->viewer, m_c->movieOutdir, m_c->movieWidth,m_c->movieHeight, m_c->movieFPS, m_c->frameFileNamePrefix);

    connect(helper,SIGNAL(animationFinished()), this, SLOT(nextAnimationFrame()));
    connect(helper,SIGNAL(animationFinishedAbnormally()), this, SLOT(abortAnimation()));
    connect(helper,SIGNAL(clipVolumePercentageOfATLAS(double)), this, SIGNAL(clipVolumePercentOfATLAS(double)));

  }
  // std::cout<<" Setting last_clipVolPercent to "<<f.clipVolPercent<<std::endl;
  m_c->last_clipVolPercent = f.clipVolPercent;
  

  rootnode->unrefNoDelete();
  m_c->animationFrameNumber++;

}

SoSphere * AnimationSequencer::Clockwork::getRegionSphere(AnimationSequence::REGION region,bool perspective)
{
  if (!sphere) {
    sphere = new SoSphere;
    sphere->ref();
  }
  double r(30);
  switch (region) {
  case AnimationSequence::VERTEX:
    VP1Msg::messageVerbose("set sphere dimensions for vertex");
    //r = perspective ? 0.5 : 0.5;
    r=0.5;
    break;
  case AnimationSequence::INDET:
    VP1Msg::messageVerbose("set sphere dimensions for indet");
    r = perspective ? 13 : 7;
    break;
  case AnimationSequence::CALO:
    VP1Msg::messageVerbose("set sphere dimensions for calo");
    r = perspective ? 35 : 27;
    break;
  case AnimationSequence::MUON:
  default:
    VP1Msg::messageVerbose("set sphere dimensions for muon");
    r = perspective ? 95 : 73;
    break;
  }
  sphere->radius  = r * 1000.0;
  return sphere;
}
