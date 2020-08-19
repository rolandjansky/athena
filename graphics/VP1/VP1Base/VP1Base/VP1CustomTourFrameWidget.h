/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1CustomTourFrameWidget            //
//                                                            //
//  Description: Frame in user defined tours.                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2008                           //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1CUSTOMTOURFRAMEWIDGET_H
#define VP1CUSTOMTOURFRAMEWIDGET_H

#include <QFrame>
#include <QByteArray>

class VP1CustomTourFrameWidget : public QFrame {

  Q_OBJECT

public:

  VP1CustomTourFrameWidget(bool camPerspective, QByteArray camState, QWidget * parent = 0);
  VP1CustomTourFrameWidget(QByteArray serialisedFrame, QWidget * parent = 0);
  QByteArray serialise() const;

  virtual ~VP1CustomTourFrameWidget();

  void setStepToEarlierControlsEnabled(bool);
  void setStepToLaterControlsEnabled(bool);

  bool frameIsEnabled() const;
  bool zoomToFrameWithVariableSpeed() const;
  bool zoomToFrameForcedCircular() const;
  double zoomToFrameTime() const;//in seconds
  double stayOnFrameTime() const;//in seconds
  double clipVolumePercentOfATLAS() const;//in percent
  bool camStateIsPerspective() const;
  QByteArray camState() const;

  void setSnapshot(QPixmap);//Call whenever updating the pixmap.
  static int snapShotWidth() { return 90; }
  static int snapShotHeight() { return 90; }

signals:
  void requestStepToEarlier();
  void requestStepToLater();
  void requestDelete();
  void requestShow();
  void requestSwap(VP1CustomTourFrameWidget*,VP1CustomTourFrameWidget*);
  void frameEnableStateChanged();

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void dropEvent(QDropEvent *event);
  void dragEnterEvent(QDragEnterEvent *event);


private slots:
  void buttonClicked();
  void frameEnabledToggled();
private:

  VP1CustomTourFrameWidget( const VP1CustomTourFrameWidget & );
  VP1CustomTourFrameWidget & operator= ( const VP1CustomTourFrameWidget & );
  class Imp;
  Imp * m_d;

};

#endif
