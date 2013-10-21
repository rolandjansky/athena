/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class GeoSysController                    //
//                                                            //
//  Description: Controller for the geometry system.          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: October 2008                             //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef GEOSYSCONTROLLER_H
#define GEOSYSCONTROLLER_H

#include "VP1Base/VP1Controller.h"
#include "VP1GeometrySystems/VP1GeoFlags.h"
class VolumeHandle;
class SoGroup;
class SoPickStyle;
class VP1GeoTreeView;
class PhiSectionWidget;
class ZappedVolumeListModel;

class GeoSysController : public VP1Controller {

  Q_OBJECT

public:

  GeoSysController(IVP1System * sys);
  virtual ~GeoSysController();

  //Change defaults (call right after constructor):
  void setGeometrySelectable(bool);
  void setZoomToVolumeOnClick(bool);
  void setOrientViewToMuonChambersOnClick(bool);
  void setAutoAdaptMuonChambersToEventData(bool);
  void setComplexity(double);
  
  // Labels
//  void setAvailableLabels(IVP1System* sys, QStringList providedLabels);
  void setLabelsEnabled(bool t0s, bool hits); //!< controls whether the checkboxes are enabled or not. Set in e.g. VP1SysConf.cxx, via VP1GeometrySystem

  void actualRestoreSettings(VP1Deserialise&);
  int  currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  void setLastSelectedVolume(VolumeHandle*);
  VolumeHandle* lastSelectedVolume() const;

  QCheckBox * subSystemCheckBox(VP1GeoFlags::SubSystemFlag) const;//Checkbox will be disabled
                                                                        //if corresponding geometry
                                                                        //is not present in job.

  SoGroup * drawOptions() const;
  SoPickStyle * pickStyle() const;
  VP1GeoTreeView * volumeTreeBrowser() const;
  PhiSectionWidget * phiSectionWidget() const;
  ZappedVolumeListModel * zappedVolumeListModel() const;

  ///////////////////////////////////
  //  Access Methods for settings  //
  ///////////////////////////////////

  //Those only queried on-demand:
  bool printInfoOnClick_Shape() const;
  bool printInfoOnClick_Material() const;
  bool printInfoOnClick_CopyNumber() const;
  bool printInfoOnClick_Transform() const;
  bool printInfoOnClick_Tree() const;
  bool printInfoOnClick_Mass() const;
  bool printInfoOnClick_MuonStationInfo() const;
  bool orientViewToMuonChambersOnClick() const;
  bool zoomToVolumeOnClick() const;
  bool autoAdaptMuonChambersToEventData() const;

  //Settings with change signals:

  float transparency() const;
  VP1GeoFlags::MuonChamberAdaptionStyleFlags muonChamberAdaptionStyle() const;
  bool showVolumeOutLines() const;
  int labels() const;
  QList<int> labelPosOffset() ; //!< Offset in x,y,z

  ///////////////////////////////////////
  //  Signals for changes in settings  //
  ///////////////////////////////////////
signals:

  //Change signals:
  void transparencyChanged(float);
  void muonChamberAdaptionStyleChanged(VP1GeoFlags::MuonChamberAdaptionStyleFlags);
  void showVolumeOutLinesChanged(bool);
  void labelsChanged(int);
  void labelPosOffsetChanged(QList<int>);

  //Signals without state:
  void adaptMuonChambersToEventData();
  void autoAdaptPixelsOrSCT(bool,bool,bool,bool,bool,bool);//pixel,brl,ecA,ecC,bcmA,bcmC
  void resetSubSystems(VP1GeoFlags::SubSystemFlags);
  void autoExpandByVolumeOrMaterialName(bool,QString);//volname: (false,namestr), matname: (true,namestr)
  void actionOnAllNonStandardVolumes(bool);//true: zap, false: expand.
  void volumeStateChangeRequested(VolumeHandle*,VP1GeoFlags::VOLSTATE);//Might not be used atm.
  void volumeResetRequested(VolumeHandle*);

  void saveMaterialsToFile(QString,bool);//(filename,onlyChangedMaterials)
  void loadMaterialsFromFile(QString);//filename

private:

  class Imp;
  Imp * d;

private slots:
  void possibleChange_transparency();
  void possibleChange_muonChamberAdaptionStyle();
  void possibleChange_showVolumeOutLines();
  void updatePickStyle();
  void saveMaterialsRequested();
  void loadMaterialsRequested();
  void emit_adaptMuonChambersToEventData();
  void emit_autoAdaptPixelsOrSCT();
  void emit_autoExpandByVolumeOrMaterialName();
  void emit_actionOnAllNonStandardVolumes();
  void emit_resetSubSystems();
  void possibleChange_labels();
  void possibleChange_labelPosOffset();
};

#endif
