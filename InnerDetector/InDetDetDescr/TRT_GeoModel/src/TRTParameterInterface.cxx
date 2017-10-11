/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTParameterInterface.h"
#include <iostream>

TRTParameterInterface::TRTParameterInterface() {}

void TRTParameterInterface::ShowValues() {

  //Barrel flange and services add ons
  std::cout << "TRTParameterInterface test: barFlangeZMin = " << barFlangeZMin << std::endl;
  std::cout << "TRTParameterInterface test: barFlangeZMax = " << barFlangeZMax << std::endl;
  std::cout << "TRTParameterInterface test: barFlangeRMin = " << barFlangeRMin << std::endl;
  std::cout << "TRTParameterInterface test: barFlangeRMax = " << barFlangeRMax << std::endl;
  std::cout << "TRTParameterInterface test: barServicesZMin = " << barServicesZMin << std::endl;
  std::cout << "TRTParameterInterface test: barServicesZMax = " << barServicesZMax << std::endl;
  std::cout << "TRTParameterInterface test: barServicesRMin = " << barServicesRMin << std::endl;
  std::cout << "TRTParameterInterface test: barServicesRMax = " << barServicesRMax << std::endl;
  std::cout << "TRTParameterInterface test: includeBarServiceAndFlange = " << includeBarServiceAndFlange << std::endl;
  std::cout << "TRTParameterInterface test: virtualBarrelVolumeLength = " << virtualBarrelVolumeLength << std::endl;

  std::cout << "TRTParameterInterface test: virtualBarrelInnerRadius = " << virtualBarrelInnerRadius << std::endl;
  std::cout << "TRTParameterInterface test: virtualBarrelOuterRadius = " << virtualBarrelOuterRadius << std::endl;
  std::cout << "TRTParameterInterface test: barrelVirtualVolumeZClearance = " << barrelVirtualVolumeZClearance << std::endl;
  std::cout << "TRTParameterInterface test: barrelVirtualVolumeRMinClearance = " << barrelVirtualVolumeRMinClearance << std::endl;
  std::cout << "TRTParameterInterface test: barrelVirtualVolumeRMaxClearance = " << barrelVirtualVolumeRMaxClearance<< std::endl;


  std::cout << "TRTParameterInterface test: digversion = " << digversion << std::endl;
  std::cout << "TRTParameterInterface test: digversionname = " << digversionname << std::endl;
  std::cout << "TRTParameterInterface test: isCosmicRun = " << isCosmicRun << std::endl;
  std::cout << "TRTParameterInterface test: barrelOuterRadiusOfStrawHole = " << barrelOuterRadiusOfStrawHole << std::endl;
  std::cout << "TRTParameterInterface test: barrelLengthOfStraw = " << barrelLengthOfStraw << std::endl;
  std::cout << "TRTParameterInterface test: lengthOfBarrelVolume = " << lengthOfBarrelVolume << std::endl;
  std::cout << "TRTParameterInterface test: lengthOfDeadRegion = " << lengthOfDeadRegion << std::endl;
  std::cout << "TRTParameterInterface test: outerRadiusOfWire = " << outerRadiusOfWire << std::endl;
  std::cout << "TRTParameterInterface test: innerRadiusOfStraw = " << innerRadiusOfStraw << std::endl;
  std::cout << "TRTParameterInterface test: outerRadiusOfStraw = " << outerRadiusOfStraw << std::endl;
  std::cout << "TRTParameterInterface test: barrelLengthOfTwister = " << barrelLengthOfTwister << std::endl;
  std::cout << "TRTParameterInterface test: barrelLengthOfLargeDeadRegion = " << barrelLengthOfLargeDeadRegion << std::endl;
  std::cout << "TRTParameterInterface test: innerRadiusOfBarrelVolume = " << innerRadiusOfBarrelVolume << std::endl;
  std::cout << "TRTParameterInterface test: outerRadiusOfBarrelVolume = " << outerRadiusOfBarrelVolume << std::endl;
  std::cout << "TRTParameterInterface test: thicknessOfBarrelInnerSupport = " << thicknessOfBarrelInnerSupport << std::endl;
  std::cout << "TRTParameterInterface test: thicknessOfBarrelOuterSupport = " << thicknessOfBarrelOuterSupport << std::endl;
  std::cout << "TRTParameterInterface test: barrelOuterRadiusOfCoolingTube = " << barrelOuterRadiusOfCoolingTube << std::endl;
  std::cout << "TRTParameterInterface test: barrelInnerRadiusOfCoolingTube = " << barrelInnerRadiusOfCoolingTube << std::endl;
  std::cout << "TRTParameterInterface test: barrelThicknessOfModuleWalls = " << barrelThicknessOfModuleWalls << std::endl;
  std::cout << "TRTParameterInterface test: barrelNumberOfLayersWithLargeDeadRegion = " << barrelNumberOfLayersWithLargeDeadRegion << std::endl;
  std::cout << "TRTParameterInterface test: nBarrelRings = " << nBarrelRings << std::endl;
  std::cout << "TRTParameterInterface test: nBarrelModules = " << nBarrelModules << std::endl;
  std::cout << "TRTParameterInterface test: nBarrelModulesUsed = " << nBarrelModulesUsed << std::endl;
  std::cout << "TRTParameterInterface test: nBarrelPhi = " << nBarrelPhi << std::endl;

  std::cout << "TRTParameterInterface test: nEndcapPhi = " << nEndcapPhi << std::endl;

  std::cout << "TRTParameterInterface test: endcapNumberOfAWheels = " << endcapNumberOfAWheels << std::endl;
  std::cout << "TRTParameterInterface test: endcapNumberOfBWheels = " << endcapNumberOfBWheels << std::endl;
  std::cout << "TRTParameterInterface test: endcapNumberOfCWheels = " << endcapNumberOfCWheels << std::endl;

  std::cout << "TRTParameterInterface test: nShellCorners = " << nShellCorners << std::endl;
  std::cout << "TRTParameterInterface test: nCoolingTubes = " << nCoolingTubes << std::endl;
  std::cout << "TRTParameterInterface test: innerRadiusOfEndCapVolumeAB = " << innerRadiusOfEndCapVolumeAB << std::endl;
  std::cout << "TRTParameterInterface test: outerRadiusOfEndCapVolumeAB = " << outerRadiusOfEndCapVolumeAB << std::endl;
  std::cout << "TRTParameterInterface test: innerRadiusOfEndCapVolumeC = " << innerRadiusOfEndCapVolumeC << std::endl;
  std::cout << "TRTParameterInterface test: outerRadiusOfEndCapVolumeC = " << outerRadiusOfEndCapVolumeC << std::endl;
  std::cout << "TRTParameterInterface test: lengthOfEndCapVolumeAB = " << lengthOfEndCapVolumeAB << std::endl;
  std::cout << "TRTParameterInterface test: positionOfEndCapVolumeAB = " << positionOfEndCapVolumeAB << std::endl;
  std::cout << "TRTParameterInterface test: lengthOfEndCapVolumeC = " << lengthOfEndCapVolumeC << std::endl;
  std::cout << "TRTParameterInterface test: positionOfEndCapVolumeC = " << positionOfEndCapVolumeC << std::endl;
  std::cout << "TRTParameterInterface test: endcapOuterRadiusOfInnerSupport_wheelAB = " << endcapOuterRadiusOfInnerSupport_wheelAB << std::endl;
  std::cout << "TRTParameterInterface test: endcapInnerRadiusOfOuterSupport_wheelAB = " << endcapInnerRadiusOfOuterSupport_wheelAB << std::endl;
  std::cout << "TRTParameterInterface test: endcapOuterRadiusOfInnerSupport_wheelC = " << endcapOuterRadiusOfInnerSupport_wheelC << std::endl;
  std::cout << "TRTParameterInterface test: endcapInnerRadiusOfOuterSupport_wheelC = " << endcapInnerRadiusOfOuterSupport_wheelC << std::endl;

  std::cout << "TRTParameterInterface test: endCapShiftForEachRotation = " << endCapShiftForEachRotation << std::endl;
  std::cout << "TRTParameterInterface test: endCapInnerRadiusOfSupportA = " << endCapInnerRadiusOfSupportA << std::endl;
  std::cout << "TRTParameterInterface test: endCapInnerRadiusOfSupportB = " << endCapInnerRadiusOfSupportB << std::endl;
  std::cout << "TRTParameterInterface test: endCapInnerRadiusOfSupportC = " << endCapInnerRadiusOfSupportC << std::endl;
  std::cout << "TRTParameterInterface test: endCapOuterRadiusOfSupportA = " << endCapOuterRadiusOfSupportA << std::endl;
  std::cout << "TRTParameterInterface test: endCapOuterRadiusOfSupportB = " << endCapOuterRadiusOfSupportB << std::endl;
  std::cout << "TRTParameterInterface test: endCapOuterRadiusOfSupportC = " << endCapOuterRadiusOfSupportC << std::endl;
  std::cout << "TRTParameterInterface test: endCapRadialThicknessOfInnerSupportA = " << endCapRadialThicknessOfInnerSupportA << std::endl;
  std::cout << "TRTParameterInterface test: endCapRadialThicknessOfInnerSupportB = " << endCapRadialThicknessOfInnerSupportB << std::endl;
  std::cout << "TRTParameterInterface test: endCapRadialThicknessOfInnerSupportC = " << endCapRadialThicknessOfInnerSupportC << std::endl;
  std::cout << "TRTParameterInterface test: endCapRadialThicknessOfOuterSupportA = " << endCapRadialThicknessOfOuterSupportA << std::endl;
  std::cout << "TRTParameterInterface test: endCapRadialThicknessOfOuterSupportB = " << endCapRadialThicknessOfOuterSupportB << std::endl;
  std::cout << "TRTParameterInterface test: endCapRadialThicknessOfOuterSupportC = " << endCapRadialThicknessOfOuterSupportC << std::endl;
  std::cout << "TRTParameterInterface test: endCapRadialDistFromRadToOuterSupportA = " << endCapRadialDistFromRadToOuterSupportA << std::endl;
  std::cout << "TRTParameterInterface test: endCapRadialDistFromRadToOuterSupportB = " << endCapRadialDistFromRadToOuterSupportB << std::endl;
  std::cout << "TRTParameterInterface test: endCapRadialDistFromRadToOuterSupportC = " << endCapRadialDistFromRadToOuterSupportC << std::endl;
  std::cout << "TRTParameterInterface test: endCapMainRadiatorThicknessA = " << endCapMainRadiatorThicknessA << std::endl;
  std::cout << "TRTParameterInterface test: endCapMainRadiatorThicknessB = " << endCapMainRadiatorThicknessB << std::endl;
  std::cout << "TRTParameterInterface test: endCapMainRadiatorThicknessC = " << endCapMainRadiatorThicknessC << std::endl;
  std::cout << "TRTParameterInterface test: endCapThinRadiatorThicknessA = " << endCapThinRadiatorThicknessA << std::endl;
  std::cout << "TRTParameterInterface test: endCapThinRadiatorThicknessB = " << endCapThinRadiatorThicknessB << std::endl;
  std::cout << "TRTParameterInterface test: endCapThinRadiatorThicknessC = " << endCapThinRadiatorThicknessC << std::endl;
  std::cout << "TRTParameterInterface test: endCapMiddleRadiatorThicknessB = " << endCapMiddleRadiatorThicknessB << std::endl;
  std::cout << "TRTParameterInterface test: endCapLengthOfWheelsA = " << endCapLengthOfWheelsA << std::endl;
  std::cout << "TRTParameterInterface test: endCapLengthOfWheelsB = " << endCapLengthOfWheelsB << std::endl;
  std::cout << "TRTParameterInterface test: endCapLengthOfWheelsC = " << endCapLengthOfWheelsC << std::endl;
  std::cout << "TRTParameterInterface test: endCapNumberOfStrawLayersPerWheelA = " << endCapNumberOfStrawLayersPerWheelA << std::endl;
  std::cout << "TRTParameterInterface test: endCapNumberOfStrawLayersPerWheelB = " << endCapNumberOfStrawLayersPerWheelB << std::endl;
  std::cout << "TRTParameterInterface test: endCapNumberOfStrawLayersPerWheelC = " << endCapNumberOfStrawLayersPerWheelC << std::endl;
  std::cout << "TRTParameterInterface test: endCapPositionOfFirstWheelA = " << endCapPositionOfFirstWheelA << std::endl;
  std::cout << "TRTParameterInterface test: endCapPositionOfFirstWheelB = " << endCapPositionOfFirstWheelB << std::endl;
  std::cout << "TRTParameterInterface test: endCapPositionOfFirstWheelC = " << endCapPositionOfFirstWheelC << std::endl;
  std::cout << "TRTParameterInterface test: endCapDistanceBetweenWheelCentersA = " << endCapDistanceBetweenWheelCentersA << std::endl;
  std::cout << "TRTParameterInterface test: endCapDistanceBetweenWheelCentersB = " << endCapDistanceBetweenWheelCentersB << std::endl;
  std::cout << "TRTParameterInterface test: endCapDistanceBetweenWheelCentersC = " << endCapDistanceBetweenWheelCentersC << std::endl;
  std::cout << "TRTParameterInterface test: endcapNumberOfStrawsInStrawLayer_AWheels = " << endcapNumberOfStrawsInStrawLayer_AWheels << std::endl;
  std::cout << "TRTParameterInterface test: endcapNumberOfStrawsInStrawLayer_BWheels = " << endcapNumberOfStrawsInStrawLayer_BWheels << std::endl;
  std::cout << "TRTParameterInterface test: endcapNumberOfStrawsInStrawLayer_CWheels = " << endcapNumberOfStrawsInStrawLayer_CWheels << std::endl;

  for (size_t iiPlane = 0; iiPlane < endCapNumberOfStrawLayersPerWheelA; iiPlane++)
    std::cout << "TRTParameterInterface test: endCapLayerZPositionA["<<iiPlane<<"] = "<< endCapLayerZPositionA[iiPlane] <<std::endl;
  for (size_t iiPlane = 0; iiPlane < endCapNumberOfStrawLayersPerWheelB; iiPlane++)
    std::cout << "TRTParameterInterface test: endCapLayerZPositionB["<<iiPlane<<"] = "<< endCapLayerZPositionB[iiPlane] <<std::endl;
  for (size_t iiPlane = 0; iiPlane < endCapNumberOfStrawLayersPerWheelC; iiPlane++)
    std::cout << "TRTParameterInterface test: endCapLayerZPositionC["<<iiPlane<<"] = "<< endCapLayerZPositionC[iiPlane] <<std::endl;
  for (size_t ii=0; ii < nBarrelRings ; ii++)
    std::cout << "TRTParameterInterface test: barrelNumberOfStrawsInModule["<<ii<<"] = "<< barrelNumberOfStrawsInModule[ii] <<std::endl;
  for (size_t ii=0; ii < nBarrelRings ; ii++)
    std::cout << "TRTParameterInterface test: barrelIndexOfSecondGlobalAlignmentStraw["<<ii<<"] = "
	      << barrelIndexOfSecondGlobalAlignmentStraw[ii] <<std::endl;

  for (size_t ii=0; ii < nBarrelRings ; ii++)
    std::cout << "TRTParameterInterface test: barrelNumberOfStrawLayersInModule["<<ii<<"] = "<< barrelNumberOfStrawLayersInModule[ii] <<std::endl;

  for (size_t ii=0; ii < nBarrelRings; ii++)
    for (size_t jj=0; jj < barrelNumberOfStrawLayersInModule[ii]; jj++)
      std::cout << "TRTParameterInterface test: barrelNumberOfStrawsInStrawLayer["<<ii<<"]["<<jj<<"] = "
		<< barrelNumberOfStrawsInStrawLayer[ii][jj] <<std::endl;

  for (size_t ii=0; ii < nBarrelRings; ii++)
    for (size_t jj=0; jj < nCoolingTubes; jj++)
      std::cout << "TRTParameterInterface test: barrelXOfCoolingTube["<<ii<<"]["<<jj<<"] = "
		<< barrelXOfCoolingTube[ii][jj] <<std::endl;

  for (size_t ii=0; ii < nBarrelRings; ii++)
    for (size_t jj=0; jj < nCoolingTubes; jj++)
      std::cout << "TRTParameterInterface test: barrelYOfCoolingTube["<<ii<<"]["<<jj<<"] = "
		<< barrelYOfCoolingTube[ii][jj] <<std::endl;

  for (size_t ii=0; ii < nBarrelRings; ii++)
    for (size_t jj=0; jj < nShellCorners; jj++)
      std::cout << "TRTParameterInterface test: shellCornerXPosition["<<ii<<"]["<<jj<<"] = "
		<< shellCornerXPosition[ii][jj] <<std::endl;

  for (size_t ii=0; ii < nBarrelRings; ii++)
    for (size_t jj=0; jj < nShellCorners; jj++)
      std::cout << "TRTParameterInterface test: shellCornerYPosition["<<ii<<"]["<<jj<<"] = "
		<< shellCornerYPosition[ii][jj] <<std::endl;

  for (size_t ii=0; ii < nBarrelRings; ii++)
    for (size_t jj=0; jj < barrelNumberOfStrawsInModule[ii]; jj++)
      std::cout << "TRTParameterInterface test: strawXPosition["<<ii<<"]["<<jj<<"] = "
		<< strawXPosition[ii][jj] <<std::endl;

  for (size_t ii=0; ii < nBarrelRings; ii++)
    for (size_t jj=0; jj < barrelNumberOfStrawsInModule[ii]; jj++)
      std::cout << "TRTParameterInterface test: strawYPosition["<<ii<<"]["<<jj<<"] = "
		<< strawYPosition[ii][jj] <<std::endl;

  for (size_t ii=0; ii < nBarrelRings ; ii++)
    std::cout << "TRTParameterInterface test: barrelXOfFirstGlobalAlignmentStraw["<<ii<<"] = "<< barrelXOfFirstGlobalAlignmentStraw[ii] <<std::endl;
  for (size_t ii=0; ii < nBarrelRings ; ii++)
    std::cout << "TRTParameterInterface test: barrelXOfSecondGlobalAlignmentStraw["<<ii<<"] = "<< barrelXOfSecondGlobalAlignmentStraw[ii] <<std::endl;
  for (size_t ii=0; ii < nBarrelRings ; ii++)
    std::cout << "TRTParameterInterface test: barrelYOfFirstGlobalAlignmentStraw["<<ii<<"] = "<< barrelYOfFirstGlobalAlignmentStraw[ii] <<std::endl;
  for (size_t ii=0; ii < nBarrelRings ; ii++)
    std::cout << "TRTParameterInterface test: barrelYOfSecondGlobalAlignmentStraw["<<ii<<"] = "<< barrelYOfSecondGlobalAlignmentStraw[ii] <<std::endl;
}
