/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1BASE_ANIMATIONSEQUENCE_H
#define VP1BASE_ANIMATIONSEQUENCE_H
#include <vector>
//_________________class AnimationSequence and AnimationSequence::Frame________
//
// These classes allow you to specify a time sequence of camera postions and
// orientations.
//
// Joe Boudreau May 2008
//
//_____________________________________________________________________________
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbLinear.h>

#include <QByteArray>

class AnimationSequence {

 public:

  // Region definitions:
  enum REGION {VERTEX,INDET,CALO,MUON};

  // A frame class:
  class Frame {
  public:
    Frame(const REGION &r, const SbVec3f &d, const SbVec3f & u, const double& t,bool vs, bool fc, double c)
      : time(t), clipVolPercent(c), variableSpeed(vs), forceCircular(fc), reg(r),dir(d),upvec(u){}
    Frame(QByteArray cs, const double& t,bool vs,bool fc, double c)
      : time(t), clipVolPercent(c), variableSpeed(vs), forceCircular(fc),
	camState(cs), reg(VERTEX),dir(SbVec3f(0,0,0)),upvec(SbVec3f(0,0,0)){}

    double  time;   // time
    double  clipVolPercent; // Percentage of ATLAS Vol used as clipping volume.
    bool variableSpeed;
    bool forceCircular;

    //If camstate is not empty, (reg,dir,upvec) defines the frame. Otherwise those three are ignored.
    QByteArray camState;
    REGION  reg; // region
    SbVec3f dir; // direction
    SbVec3f upvec; // direction

  };

  void clearAllFrames() { m_sequence.clear(); }

  // Add a frame:
  void addFrame(REGION reg, const SbVec3f & dir, const SbVec3f & upvec, double t,
		bool variableSpeed = false, bool forceCircular = false, double clip=100.0  ) {
    m_sequence.push_back(Frame(reg, dir, upvec, t, variableSpeed, forceCircular, clip));
  }
  void addFrame(REGION reg, const SbVec3f & dir, double t,
		bool variableSpeed = false, bool forceCircular = false, double clip=100.0  ) {//default upvec along y-axis
    m_sequence.push_back(Frame(reg, dir, SbVec3f(0,1,0), t, variableSpeed, forceCircular, clip));
  }
  void addFrame(QByteArray camState, double t,
		bool variableSpeed = true, bool forceCircular = false, double clip=100.0  ) {//default upvec along y-axis
    m_sequence.push_back(Frame(camState, t, variableSpeed, forceCircular, clip));
  }

  // Get number of frames;
  unsigned int getNumFrames() const { return m_sequence.size();}

  // Get a frame:
  const Frame & getFrame(unsigned int i) const { return m_sequence.at(i);}

 private:

  // Storage for frames:
  std::vector<Frame> m_sequence;

};


#endif
