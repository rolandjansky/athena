/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1CustomTourEditor                 //
//                                                            //
//  Description: Custom tour editor.                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2008                           //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1CUSTOMTOUREDITOR_H
#define VP1CUSTOMTOUREDITOR_H

#include <QWidget>

class VP1CustomTourFrameWidget;
class VP1ExaminerViewer;
class AnimationSequencer;

class VP1CustomTourEditor : public QWidget {

  Q_OBJECT

public:

  VP1CustomTourEditor(VP1ExaminerViewer*);

  void setState(QByteArray);
  QByteArray state() const;

  virtual ~VP1CustomTourEditor();

  void disableObjectWhenTourNotAvailable(QObject*);

  bool tourAvailable() const;//true if the current setup of frames
			     //provides an acceptable tour.
  bool tourIsPerspective() const;

  void addTourToAnimationSequencer(AnimationSequencer&,bool jumpDirectlyToFirstFrame) const;

signals:
  void clipVolumePercentOfATLAS(double);
  void clipVolumeRadiusChanged(double);

public slots:
  void setClipVolumePercentOfATLAS(double);

protected:
  void closeEvent(QCloseEvent*);
private slots:
  void frameStepToEarlier();
  void frameStepToLater();
  void frameDelete();
  void frameShow();
  void buttonClicked();
  void swap(VP1CustomTourFrameWidget*,VP1CustomTourFrameWidget*);
  void enabledFrameListChanged();
private:

  class Imp;
  Imp * m_d;

};

#endif
