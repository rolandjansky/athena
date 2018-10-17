/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1CustomStereoEditor               //
//                                                            //
//  Description: Custom STEREOGRAPHIC VIEW editor.            //
//                                                            //
//  Author: Riccardo-Maria BIANCHI (rbianchi@cern.ch)         //
//  Initial version: August 2013                              //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1CUSTOMSTEREOEDITOR_H
#define VP1CUSTOMSTEREOEDITOR_H

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbBasic.h>

#include <QWidget>

class VP1ExaminerViewer;

//class VP1CustomTourFrameWidget;
//class AnimationSequencer;

class VP1CustomStereoEditor : public QWidget {

  Q_OBJECT

public:

  VP1CustomStereoEditor(VP1ExaminerViewer*);

  void setState(QByteArray);
  QByteArray state() const;

  virtual ~VP1CustomStereoEditor();

//  void disableObjectWhenTourNotAvailable(QObject*);

//  bool tourAvailable() const;//true if the current setup of frames
			     //provides an acceptable tour.
//  bool tourIsPerspective() const;

//  void addTourToAnimationSequencer(AnimationSequencer&,bool jumpDirectlyToFirstFrame) const;

protected:
  void closeEvent(QCloseEvent*);
private slots:
//  void frameStepToEarlier();
//  void frameStepToLater();
//  void frameDelete();
//  void frameShow();
  void buttonClicked();
  void changedView(bool checked);
  void changedView_viewer(bool checked);
  void showEvent(QShowEvent*ev);
  void updateFocal();
  void setToFocal30();
  void perform3DCalculations(double value);

//  void swap(VP1CustomTourFrameWidget*,VP1CustomTourFrameWidget*);
//  void enabledFrameListChanged();
private:

  class Imp;
  Imp * m_d;

  // arrays to store the anaglyph filters
  SbBool m_leftEyeFilter[3];
  SbBool m_rightEyeFilter[3];

  // default values for normal Anaglyph red-cyan view filters
//  SbBool leftEyeFilter_def[3] = {true, false, false};
//  SbBool rightEyeFilter_def[3] = {false, true, true};
  SbBool m_leftEyeFilter_def[3];
  SbBool m_rightEyeFilter_def[3];
};

#endif
