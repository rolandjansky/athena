/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class GuideSysController                  //
//                                                            //
//  Description: Guide system controller.                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef GUIDESYSCONTROLLER_H
#define GUIDESYSCONTROLLER_H

#include "VP1Base/VP1Controller.h"
#include "VP1Utils/InDetProjFlags.h"
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbColor4f.h>
class SoMaterial;

class GuideSysController : public VP1Controller {

  Q_OBJECT

public:

  GuideSysController(IVP1System * sys);
  virtual ~GuideSysController();

  void actualRestoreSettings(VP1Deserialise&);
  int currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  ///////////////////////////////////
  //  Access Methods for settings  //
  ///////////////////////////////////

  //Fixme: SoGroup * drawOptions() const;

  //Various materials:
  SoMaterial * lettersMaterial() const;
  SoMaterial * xAxisMaterial() const;
  SoMaterial * yAxisMaterial() const;
  SoMaterial * zAxisMaterial() const;
  SoMaterial * etaCone1Material() const;
  SoMaterial * etaCone2Material() const;
  SoMaterial * etaCone3Material() const;
  SoMaterial * pixelProjSurfMaterial() const;
  SoMaterial * sctProjSurfMaterial() const;
  SoMaterial * trtProjSurfMaterial() const;
  SoMaterial * idTrkVolumesMaterial() const;
  SoMaterial * caloTrkVolumesMaterial() const;
  SoMaterial * msTrkVolumesMaterial() const;

  //Floor:
  bool showFloor() const;
  SbColor4f floorColourAndTransp() const;
  double floorExtent() const;
  double floorSpacing() const;
  double floorHeight() const;

  //Letters:
  bool showLetters() const;
  double lettersZPos() const;
  double lettersVerticalPos() const;

  //Coordinate axis
  bool showAxes() const;
  double axesLength() const;//A negative value means that negative parts will be shown as well
  SbVec3f axesPosition() const;
  double relAxesThickness() const;

  //Grid:
  bool showCartesianGrid() const;
  bool showCylindricalGrid() const;
  SbColor4f gridColourAndTransp() const;
  double gridSpacing() const;
  double gridExtent() const;

  //Eta cones:
  bool showEtaCone1() const;
  bool showEtaCone2() const;
  bool showEtaCone3() const;
  double etaConeValue1() const;
  double etaConeValue2() const;
  double etaConeValue3() const;
  double etaExtent() const;//>0: extent means to a given radius, <0: extent means to a given z.

  //TrackingVolumes
  bool showTrackingVolumes() const;
  bool showInnerDetector() const;
  bool showCalorimeters() const;
  bool showMuonSpectrometer() const;
	
  // Lines
  bool showLines() const;
  double lineLength() const;
  SbVec3f lineDirection() const;
  
  //ID Proj surfs:

  //This is the surfaces the data systems should project their data to:
  InDetProjFlags::InDetProjPartsFlags applicablePixelProjParts() const;
  InDetProjFlags::InDetProjPartsFlags applicableSCTProjParts() const;
  InDetProjFlags::InDetProjPartsFlags applicableTRTProjParts() const;
  //This is the surfaces the guideline system should draw surfaces:
  InDetProjFlags::InDetProjPartsFlags shownPixelProjSurfaces() const;
  InDetProjFlags::InDetProjPartsFlags shownSCTProjSurfaces() const;
  InDetProjFlags::InDetProjPartsFlags shownTRTProjSurfaces() const;

  //Info arriving from other systems:
  void setInDetDetTypesUsingProjections(InDetProjFlags::DetTypeFlags);

  ///////////////////////////////////////
  //  Signals for changes in settings  //
  ///////////////////////////////////////
signals:
  void showFloorChanged(bool);
  void floorColourAndTranspChanged( const SbColor4f& );
  void floorExtentChanged(const double&);
  void floorSpacingChanged(const double&);
  void floorHeightChanged(const double&);
  void showLettersChanged(bool);
  void lettersZPosChanged(const double&);
  void lettersVerticalPosChanged(const double&);
  void showAxesChanged(bool);
  void axesLengthChanged(const double&);
  void axesPositionChanged(const SbVec3f&);
  void relAxesThicknessChanged(const double&);
  void showCartesianGridChanged(bool);
  void showCylindricalGridChanged(bool);
  void gridColourAndTranspChanged( const SbColor4f& );
  void gridSpacingChanged(const double&);
  void gridExtentChanged(const double&);
  void showEtaCone1Changed(bool);
  void showEtaCone2Changed(bool);
  void showEtaCone3Changed(bool);
  void etaConeValue1Changed(const double&);
  void etaConeValue2Changed(const double&);
  void etaConeValue3Changed(const double&);
  void etaExtentChanged(const double&);
  void applicablePixelProjPartsChanged(InDetProjFlags::InDetProjPartsFlags);
  void applicableSCTProjPartsChanged(InDetProjFlags::InDetProjPartsFlags);
  void applicableTRTProjPartsChanged(InDetProjFlags::InDetProjPartsFlags);
  void shownPixelProjSurfacesChanged(InDetProjFlags::InDetProjPartsFlags);
  void shownSCTProjSurfacesChanged(InDetProjFlags::InDetProjPartsFlags);
  void shownTRTProjSurfacesChanged(InDetProjFlags::InDetProjPartsFlags);
  void showTrackingVolumesChanged(bool);
  void showInnerDetectorChanged(bool);
  void showCalorimetersChanged(bool);
  void showMuonSpectrometerChanged(bool);
  void showLinesChanged(bool);
  void lineDirectionChanged(const SbVec3f&);

private:

  class Imp;
  Imp * m_d;

private slots:
  void updateProjPartsCheckboxStates_Pixel();
  void updateProjPartsCheckboxStates_SCT();
  void updateProjPartsCheckboxStates_TRT();
  void possibleChange_showFloor();
  void possibleChange_floorColourAndTransp();
  void possibleChange_floorExtent();
  void possibleChange_floorSpacing();
  void possibleChange_floorHeight();
  void possibleChange_showLetters();
  void possibleChange_lettersZPos();
  void possibleChange_lettersVerticalPos();
  void possibleChange_showAxes();
  void possibleChange_axesLength();
  void possibleChange_axesPosition();
  void possibleChange_relAxesThickness();
  void possibleChange_showCartesianGrid();
  void possibleChange_showCylindricalGrid();
  void possibleChange_gridColourAndTransp();
  void possibleChange_gridSpacing();
  void possibleChange_gridExtent();
  void possibleChange_showEtaCone1();
  void possibleChange_showEtaCone2();
  void possibleChange_showEtaCone3();
  void possibleChange_etaConeValue1();
  void possibleChange_etaConeValue2();
  void possibleChange_etaConeValue3();
  void possibleChange_etaExtent();
  void possibleChange_applicablePixelProjParts();
  void possibleChange_applicableSCTProjParts();
  void possibleChange_applicableTRTProjParts();
  void possibleChange_shownPixelProjSurfaces();
  void possibleChange_shownSCTProjSurfaces();
  void possibleChange_shownTRTProjSurfaces();
  void possibleChange_showTrackingVolumes();
  void possibleChange_showInnerDetector();
  void possibleChange_showCalorimeters();
  void possibleChange_showMuonSpectrometer();
  void possibleChange_showLines();
  void possibleChange_lineDirection();
};


#endif
