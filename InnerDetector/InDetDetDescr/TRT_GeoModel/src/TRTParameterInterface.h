/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_GEOMODEL_TRTPARAMETERINTERFACE_H
#define TRT_GEOMODEL_TRTPARAMETERINTERFACE_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include <string>

namespace InDetDD {
  class DistortedMaterialManager;
}

class TRTParameterInterface  {

protected:
  // This constructor should be called from the constructors of derived classes.
  TRTParameterInterface();

  // Derived classes should implement this using calls to NOVA, Oracle or plain hardcoding, whatever they like.
  virtual void SetValues() = 0;
  // + call it in their constructor.

public:

  void ShowValues();//fixme add dig object
  virtual ~TRTParameterInterface() {};

  ///////////////////////////////////////////////////////////
  //                 Top Level Placements                  //
  ///////////////////////////////////////////////////////////
  virtual const GeoTrf::Transform3D & partTransform(const std::string & partName) const = 0;
  virtual bool partPresent(const std::string & partName) const = 0;

  ///////////////////////////////////////////////////////////
  //                 Version Information                   //
  ///////////////////////////////////////////////////////////
  std::string versionTag{};

  ///////////////////////////////////////////////////////////
  //                 Distorted Material                    //
  ///////////////////////////////////////////////////////////
  virtual InDetDD::DistortedMaterialManager * distortedMatManager() = 0;

  ///////////////////////////////////////////////////////////
  //                    Special Flags                      //
  ///////////////////////////////////////////////////////////
  std::string versionName{};
  std::string layout{};
  std::string versionDescription{};
  bool isCosmicRun{};
  bool initialLayout{};
  bool oldConfiguration{};

  unsigned int digversion{};
  std::string digversionname{};

  ///////////////////////////////////////////////////////////
  //                  Array Boundaries                     //
  ///////////////////////////////////////////////////////////

  unsigned int nBarrelModules{};
  unsigned int nBarrelModulesUsed{};
  unsigned int nBarrelPhi{};

  //  const unsigned int maxStraws;
  //  const unsigned int maxStrawLayers;
  unsigned int nShellCorners{};
  unsigned int nCoolingTubes{};
  //TKclean  const unsigned int nWheels;

  // These needs to be static const since they define the first dimensions of two-dimensional arrays further down:
  static const unsigned int nEndcapPhi = 32;//must be const due to use in factory
  //  unsigned int nEndcapPhi;;//TK: this also??
  //  static const unsigned int nLayers = 3 ;
  static const unsigned int nBarrelRings = 3 ;
  //  const unsigned int nBarrelRings;

  ///////////////////////////////////////////////////////////
  //                  Barrel Parameters                    //
  ///////////////////////////////////////////////////////////

  double barrelOuterRadiusOfStrawHole{};

  double barrelLengthOfStraw{};
  double lengthOfBarrelVolume{};

  double lengthOfDeadRegion{};
  double outerRadiusOfWire{};
  double innerRadiusOfStraw{};
  double outerRadiusOfStraw{};
  double barrelLengthOfTwister{};
  double barrelLengthOfLargeDeadRegion{};

  double innerRadiusOfBarrelVolume{};
  double outerRadiusOfBarrelVolume{};

  double thicknessOfBarrelInnerSupport{};
  double thicknessOfBarrelOuterSupport{};

  double barrelOuterRadiusOfCoolingTube{};
  double barrelInnerRadiusOfCoolingTube{};

  double barrelThicknessOfModuleWalls{};
  unsigned int barrelNumberOfLayersWithLargeDeadRegion{};

  unsigned int *  barrelNumberOfStrawsInModule{nullptr};
  //TKrename  unsigned int * nStraw;

  //  unsigned int * numberOfStrawLayersInWheel;

  unsigned int * barrelNumberOfStrawLayersInModule{nullptr};
  unsigned int * barrelNumberOfStrawsInStrawLayer[nBarrelRings]{nullptr};

  double * barrelXOfCoolingTube[nBarrelRings]{nullptr};
  double * barrelYOfCoolingTube[nBarrelRings]{nullptr};

  double * shellCornerXPosition[nBarrelRings]{nullptr};
  double * shellCornerYPosition[nBarrelRings]{nullptr};

  // The global position of two straws are given.  The first of these
  // two is always the first in the set.  The second varies by layer.
  // The variable barrelIndexOfSecondGlobalAlignmentStraw specifies the straw which is used for the
  // second alignment straw.  The other arrays give the global position
  // of the first and second alignment straw, layer-per-layer.
  unsigned int * barrelIndexOfSecondGlobalAlignmentStraw{nullptr};
  double * barrelXOfFirstGlobalAlignmentStraw{nullptr};
  double * barrelXOfSecondGlobalAlignmentStraw{nullptr};
  double * barrelYOfFirstGlobalAlignmentStraw{nullptr};
  double * barrelYOfSecondGlobalAlignmentStraw{nullptr};
  double * strawXPosition[nBarrelRings]{nullptr};
  double * strawYPosition[nBarrelRings]{nullptr};

  //Barrel flange and services add ons
  double  barFlangeZMin{};
  double  barFlangeZMax{};
  double  barFlangeRMin{};
  double  barFlangeRMax{};
  double  barServicesZMin{};
  double  barServicesZMax{};
  double  barServicesRMin{};
  double  barServicesRMax{};
  double virtualBarrelVolumeLength{};
  bool includeBarServiceAndFlange{};
  bool includeECFoilHeatExchangerAndMembranes{};
  double virtualBarrelInnerRadius{};
  double virtualBarrelOuterRadius{};

  double barrelVirtualVolumeZClearance{};
  double barrelVirtualVolumeRMinClearance{};
  double barrelVirtualVolumeRMaxClearance{};

  ///////////////////////////////////////////////////////////
  //                  EndCap Parameters                    //
  ///////////////////////////////////////////////////////////

  unsigned int endcapNumberOfAWheels{};
  unsigned int endcapNumberOfBWheels{};
  unsigned int endcapNumberOfCWheels{};

  unsigned int endcapNumberOfStrawsInStrawLayer_AWheels{};
  unsigned int endcapNumberOfStrawsInStrawLayer_BWheels{};
  unsigned int endcapNumberOfStrawsInStrawLayer_CWheels{};

  double endcapOuterRadiusOfInnerSupport_wheelAB{};
  double endcapInnerRadiusOfOuterSupport_wheelAB{};
  double endcapOuterRadiusOfInnerSupport_wheelC{};
  double endcapInnerRadiusOfOuterSupport_wheelC{};

  double innerRadiusOfEndCapVolumeAB{};
  double outerRadiusOfEndCapVolumeAB{};

  double innerRadiusOfEndCapVolumeC{};
  double outerRadiusOfEndCapVolumeC{};
  double lengthOfEndCapVolumeAB{};
  double positionOfEndCapVolumeAB{};
  double lengthOfEndCapVolumeC{};
  double positionOfEndCapVolumeC{};

  double endCapShiftForEachRotation{};
  double endCapPhiOfFirstStraw{};

  double endCapInnerRadiusOfSupportA{};
  double endCapInnerRadiusOfSupportB{};
  double endCapInnerRadiusOfSupportC{};
  double endCapOuterRadiusOfSupportA{};
  double endCapOuterRadiusOfSupportB{};
  double endCapOuterRadiusOfSupportC{};
  double endCapRadialThicknessOfInnerSupportA{};
  double endCapRadialThicknessOfInnerSupportB{};
  double endCapRadialThicknessOfInnerSupportC{};
  double endCapRadialThicknessOfOuterSupportA{};
  double endCapRadialThicknessOfOuterSupportB{};
  double endCapRadialThicknessOfOuterSupportC{};
  double endCapRadialDistFromRadToOuterSupportA{};
  double endCapRadialDistFromRadToOuterSupportB{};
  double endCapRadialDistFromRadToOuterSupportC{};
  double endCapMainRadiatorThicknessA{};
  double endCapMainRadiatorThicknessB{};
  double endCapMainRadiatorThicknessC{};
  double endCapThinRadiatorThicknessA{};
  double endCapThinRadiatorThicknessB{};
  double endCapThinRadiatorThicknessC{};
  double endCapMiddleRadiatorThicknessB{};
  double endCapLengthOfWheelsA{};
  double endCapLengthOfWheelsB{};
  double endCapLengthOfWheelsC{};

  unsigned int endCapNumberOfStrawLayersPerWheelA{};
  unsigned int endCapNumberOfStrawLayersPerWheelB{};
  unsigned int endCapNumberOfStrawLayersPerWheelC{};
  double endCapPositionOfFirstWheelC{};
  double endCapPositionOfFirstWheelA[2]{};
  double endCapPositionOfFirstWheelB[2]{};
  double endCapDistanceBetweenWheelCentersC{}; 

  double *endCapLayerZPositionA{nullptr};
  double *endCapLayerZPositionB{nullptr};
  double *endCapLayerZPositionC{nullptr};
  double endCapDistanceBetweenWheelCentersA[2][6]{};
  double endCapDistanceBetweenWheelCentersB[2][14]{};

  double endCapFaradayFoilThickness{};

  double endCapRMinOfHeatExchanger{};
  double endCapRMaxOfHeatExchanger{};
  double endCapHeatExchangerThicknessA{};
  double endCapHeatExchangerThicknessB{};

  double endCapThicknessOfMbrane{};
  double endCapRMinOfMbrane{};
  double endCapRMaxOfMbrane{};
  double endCapZMinOfMbraneWheelA1{};
  double endCapZMinOfMbraneWheelA2{};
  double endCapZMinOfMbraneWheelB1{};
  double endCapZMinOfMbraneWheelB2{};
  double endCapZMaxOfMbraneWheelA1{};
  double endCapZMaxOfMbraneWheelA2{};
  double endCapZMaxOfMbraneWheelB1{};
  double endCapZMaxOfMbraneWheelB2{};
};

#endif
