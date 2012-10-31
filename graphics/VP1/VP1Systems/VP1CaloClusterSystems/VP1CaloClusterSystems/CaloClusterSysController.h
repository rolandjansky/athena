/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class CaloClusterSysController            //
//                                                            //
//  Description: Calo cluster system controller.              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef CALOCLUSTERSYSCONTROLLER_H
#define CALOCLUSTERSYSCONTROLLER_H

#include "VP1Base/VP1Controller.h"
class SoGroup;

class CaloClusterSysController : public VP1Controller {

  Q_OBJECT

public:

  CaloClusterSysController(IVP1System * sys);
  virtual ~CaloClusterSysController();

  void actualRestoreSettings(VP1Deserialise&);
  int currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  VP1CollectionWidget * collWidget() const;

  ///////////////////////////////////
  //  Access Methods for settings  //
  ///////////////////////////////////

  //To put at start of the system scenegraph:
  SoGroup * drawOptions() const;

  //Interactions (no change signals needed):
  bool printInfoOnClick() const;
  bool printVerboseInfoOnClick() const;
  bool zoomOnClick() const;

  //Scale:
  //The bool indicates if it is logscale(true) or linear scale(false).
  //The double is a parameter, p, used to get the length as a function
  //of energy: Either p*|energy| or p*log(1+|energy|)
  QPair<bool,double> scale() const;

  //General cuts:
  VP1Interval cutAllowedEnergies() const;
  VP1Interval cutAllowedEta() const;
  QList<VP1Interval> cutAllowedPhi() const;//All off: empty list. All on: list with one entry: ]-inf,inf[

  //Display options:
  bool showVolumeOutLines() const;

  //consider Et rather than E? (both for display and cuts):
  bool useTransverseEnergies() const;

  ///////////////////////////////////////
  //  Signals for changes in settings  //
  ///////////////////////////////////////
signals:
  void scaleChanged(const QPair<bool,double>&);
  void cutAllowedEnergiesChanged(const VP1Interval&);
  void cutAllowedEtaChanged(const VP1Interval&);
  void cutAllowedPhiChanged(const QList<VP1Interval>&);
  void showVolumeOutLinesChanged(bool);
  void useTransverseEnergiesChanged(bool);

private:

  class Imp;
  Imp * d;

  //For verbose output:
  template <class T> static QString toString( const T& t ) { return VP1Controller::toString(t); }//unhide base methods
  static QString toString( const QPair<bool,double>& par );
public slots:
  void possibleChange_scale();
private slots:
  void possibleChange_cutAllowedEnergies();
  void possibleChange_cutAllowedEta();
  void possibleChange_cutAllowedPhi();
  void possibleChange_showVolumeOutLines();
  void possibleChange_useTransverseEnergies();
  void etaCutPresetButtonTriggered();
};

#endif
