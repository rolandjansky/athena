/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _AnimationSequencer_h_
#define _AnimationSequencer_h_
//____________________________________________________________
//
// A sequencer for animating the camera...
//
// Joe Boudreau May 2008
//____________________________________________________________
#include "VP1Base/AnimationSequence.h"

#include <QObject>

class VP1ExaminerViewer;

class AnimationSequencer : public QObject {

  Q_OBJECT

 public:

  // Constructor
  AnimationSequencer(VP1ExaminerViewer *viewer);

  // Destructor
  ~AnimationSequencer();

  // Get the animation sequence (read/write)
  AnimationSequence & sequence();

  // Get the animation sequence (read only)
  const AnimationSequence & sequence() const;

  // Start animating:
  void startAnimating(bool skipFirstFrame = false);

  //For frame-by-frame output:
  void setMovie(bool);
  void setMovieParameters(QString outdir, QString frameFileNamePrefix, int fps, int width, int height);

signals:
  void animationFinishedSuccessfully();
  void clipVolumePercentOfATLAS(double);

private slots:

  // Next animation frame:
  void nextAnimationFrame();
  void abortAnimation();

private:

  class Clockwork;
  Clockwork *c;

};

#endif
