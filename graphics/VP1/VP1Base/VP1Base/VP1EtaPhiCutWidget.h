/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1EtaPhiCutWidget                  //
//                                                            //
//  Description: Widget for setting cuts on eta, phi and,     //
//               possibly, a third variable (E/P/Pt)          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1ETAPHICUTWIDGET_H
#define VP1ETAPHICUTWIDGET_H

#include "VP1Base/VP1HelperClassBase.h"
#include "VP1Base/VP1Interval.h"

#include <QWidget>

class VP1EtaPhiCutWidget : public QWidget, public VP1HelperClassBase {

  Q_OBJECT

public:

  VP1EtaPhiCutWidget(QWidget * parent = 0, IVP1System * sys = 0);//sys!=0 for messages in gui
  virtual ~VP1EtaPhiCutWidget();

  VP1Interval allowedEta() const;
  QList<VP1Interval> allowedPhi() const;//All off: empty list. All on: list with one entry: ]-inf,inf[

  QByteArray saveState() const;
  void restoreFromState( const QByteArray& );

  void setEtaCutEnabled(bool);
  void setEtaCut(const double&);//sets symmetric eta cut
  void setEtaCut(const double&,const double&);//sets asymmetric eta cut (unless range is symmetric)

  void showEtaCut(bool b);
  void showPhiCut(bool b);

signals:
  void allowedEtaChanged(const VP1Interval&);
  void allowedPhiChanged(const QList<VP1Interval>&);

private:

  class Imp;
  Imp * m_d;
private slots:
  void handleEtaCutSymmetry();
  void possibleChange_allowedEta();
  void possibleChange_allowedPhi();
};

#endif
