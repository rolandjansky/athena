/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHISECTIONWIDGET_H
#define PHISECTIONWIDGET_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class PhiSectionWidget                             //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial VP1 version: September 2007                                //
//  Major update: January 2009                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/VP1HelperClassBase.h"
#include "VP1Base/VP1Interval.h"
#include "VP1Base/VP1Msg.h"

#include <QGraphicsView>
#include <QByteArray>

class PhiSectionWidget : public QGraphicsView, public VP1HelperClassBase {


  Q_OBJECT

public:

  PhiSectionWidget(QWidget * parent,IVP1System * sys = 0);
  virtual ~PhiSectionWidget();

  int numberOfSectors() const;
  void setNumberOfSectors(int,bool forceAllEnabled = false);//(default is 12)

  bool allSectorsOn() const;
  bool allSectorsOff() const;

  //Return disjoint intervals of allowed phi values. If all sectors
  //are off, an empty list is returned. If all are on, a single
  //interval of 0..2*M_PI is returned:
  QList<VP1Interval> enabledPhiRanges() const;

  QString enabledRangesToString() const;

  //This further imposes a condition of phi in [phi_min,phi_max]:
  QList<VP1Interval> enabledPhiRanges( double phi_min, double phi_max ) const;

  QByteArray state() const;
  void setState(QByteArray);

  void launchContextMenu(QPoint);
  void setSectorStatus(int,bool);

  //Convenience methods for dealing with "sectors" outside the widget:
  bool virtualSectorEnabled(int iSector, int nSectors) const;
  QVector<bool> virtualSectorsEnabled(int nSectors) const;
  static int phiToVirtualSectorIndex(double phi,int nSectors);
  void setAllowedNumberOfSectors(QList<int> allowedNSectors, bool allowCustom = false);//If current number is not
                                                                                       //among the allowed values,
                                                                                       //the lowest of the allowed
                                                                                       //sectors will be chosen
                                                                                       //(unless allowCustom=true).
                                                                                       //Call ignored if invalid list.

signals:
  void enabledPhiRangesChanged(const QList<VP1Interval>&);

protected:
  void mousePressEvent(QMouseEvent *);
  void resizeEvent(QResizeEvent *);
  void mouseMoveEvent(QMouseEvent *event);
  void dropEvent(QDropEvent *event);
  void dragEnterEvent(QDragEnterEvent *event);
  void dragLeaveEvent(QDragLeaveEvent * event);
  void dragMoveEvent(QDragMoveEvent * event);
private:

  class Imp;
  Imp * d;

};

#endif
