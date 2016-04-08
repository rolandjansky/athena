/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataSysController             //
//                                                            //
//  Description: Raw data system controller    .              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATASYSCONTROLLER_H
#define VP1RAWDATASYSCONTROLLER_H

#include "VP1Base/VP1Controller.h"
#include "VP1RawDataSystems/VP1RawDataFlags.h"
class SoGroup;

class VP1RawDataSysController : public VP1Controller {

  Q_OBJECT

public:

  VP1RawDataSysController(IVP1System * sys);
  virtual ~VP1RawDataSysController();

  void actualRestoreSettings(VP1Deserialise&);
  int currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  VP1CollectionWidget * collWidget() const;

  ///////////////////////////////////
  //  Access Methods for settings  //
  ///////////////////////////////////

  //To put at start of the system scenegraph:
  SoGroup * drawOptions() const;

  //Materials:
  SoMaterial * trtHTMaterial() const;
  SoMaterial * bcmHighAttMaterial() const;

  //Interactions (no change signals needed):
  bool printInfoOnClick() const;
  bool printVerboseInfoOnClick() const;
  bool zoomOnClick() const;

  //General cuts:
  VP1Interval cutAllowedEta() const;
  QList<VP1Interval> cutAllowedPhi() const;//All off: empty list. All on: list with one entry: ]-inf,inf[

  //Other cuts:
  VP1RawDataFlags::InDetPartsFlags enabledInDetParts() const;
  bool trtRequireHT() const;
  VP1Interval trtAllowedToT() const;
  unsigned lucidMinQDC() const;

  //Display options:
  bool showVolumeOutLines() const;
  bool useSpecialTRTHTMaterial() const;
  bool useSpecialBCMHighAttMaterial() const;

  ///////////////////////////////////////
  //  Signals for changes in settings  //
  ///////////////////////////////////////
signals:
  void cutAllowedEtaChanged(const VP1Interval&);
  void cutAllowedPhiChanged(const QList<VP1Interval>&);
  void showVolumeOutLinesChanged(bool);
  void useSpecialTRTHTMaterialChanged(bool);
  void useSpecialBCMHighAttMaterialChanged(bool);
  void enabledInDetPartsChanged(VP1RawDataFlags::InDetPartsFlags);
  void trtRequireHTChanged(bool);
  void trtAllowedToTChanged(const VP1Interval&);//nano-seconds
  void lucidMinQDCChanged(unsigned);

private:

  class Imp;
  Imp * d;

//   //For verbose output:
//   template <class T> static QString toString( const T& t ) { return VP1Controller::toString(t); }//unhide base methods
//   static QString toString( const QPair<bool,double>& par );
private slots:
  void possibleChange_cutAllowedEta();
  void possibleChange_cutAllowedPhi();
  void possibleChange_showVolumeOutLines();
  void possibleChange_useSpecialTRTHTMaterial();
  void possibleChange_useSpecialBCMHighAttMaterial();
  void possibleChange_enabledInDetParts();
  void possibleChange_trtRequireHT();
  void possibleChange_trtAllowedToT();
  void possibleChange_lucidMinQDC();
};

#endif
