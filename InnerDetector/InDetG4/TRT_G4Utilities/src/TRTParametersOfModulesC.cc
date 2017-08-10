/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTParametersOfModulesC.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <cmath>
#include <CLHEP/Vector/ThreeVector.h>
#include <fstream>

  // Called by TRTConstructionOfModulesC::TRTConstructionOfModulesC

TRTParametersOfModulesC::TRTParametersOfModulesC() : m_msg("TRTParametersOfModulesC")
{
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesC" << endmsg;

  DefineParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesC done" << endmsg;
}


  // Called by TRTConstructionOfModulesC::~TRTConstructionOfModulesC

TRTParametersOfModulesC::~TRTParametersOfModulesC()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesC" << endmsg;

  delete [] xLocalOfHolesC;
  delete [] zLocalOfHolesC;
  delete [] xOfCoolingTubesC;
  delete [] zOfCoolingTubesC;
  delete [] xOfHolesForCoolingTubesC;
  delete [] zOfHolesForCoolingTubesC;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesC done" << endmsg;
}


  // Called by TRTParametersOfModulesC

void TRTParametersOfModulesC::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesC::DefineParameters" << endmsg;

    // Distances between corners of shell C:
  int numberOfShellCorners = pParameters->GetInteger("NumberOfShellCorners");
  double* xOfShellCornersC = new double[numberOfShellCorners];
  double* yOfShellCornersC = new double[numberOfShellCorners];
  pParameters->GetDoubleArray("XOfShellCornersC", numberOfShellCorners,
    xOfShellCornersC);
  pParameters->GetDoubleArray("YOfShellCornersC", numberOfShellCorners,
    yOfShellCornersC);

  double deltaX12 = xOfShellCornersC[0] - xOfShellCornersC[1];
  double deltaX14 = xOfShellCornersC[0] - xOfShellCornersC[3];
  double deltaX23 = xOfShellCornersC[1] - xOfShellCornersC[2];
  double deltaX24 = xOfShellCornersC[1] - xOfShellCornersC[3];
  double deltaX34 = xOfShellCornersC[2] - xOfShellCornersC[3];

  double deltaY12 = yOfShellCornersC[0] - yOfShellCornersC[1];
  double deltaY14 = yOfShellCornersC[0] - yOfShellCornersC[3];
  double deltaY23 = yOfShellCornersC[1] - yOfShellCornersC[2];
  double deltaY24 = yOfShellCornersC[1] - yOfShellCornersC[3];
  double deltaY34 = yOfShellCornersC[2] - yOfShellCornersC[3];

  double distance12 = std::sqrt(deltaX12 * deltaX12 + deltaY12 * deltaY12);
  double distance14 = std::sqrt(deltaX14 * deltaX14 + deltaY14 * deltaY14);
  double distance23 = std::sqrt(deltaX23 * deltaX23 + deltaY23 * deltaY23);
  double distance24 = std::sqrt(deltaX24 * deltaX24 + deltaY24 * deltaY24);
  double distance34 = std::sqrt(deltaX34 * deltaX34 + deltaY34 * deltaY34);

  double side = (distance14 + distance23 + distance24) / 3.;

    // Parameters of first trapezoid of shell C:
  baseOfShellTrd1C = distance12;
  heightOfShellTrd1C = std::sqrt(side * side - distance12 * distance12 / 4.);

    // Parameters of second trapezoid of shell C:
  baseOfShellTrd2C = distance34;
  heightOfShellTrd2C = std::sqrt(side * side - distance34 * distance34 / 4.);

  double alpha1 = std::atan(heightOfShellTrd1C / (baseOfShellTrd1C / 2.));
  double beta2 = std::atan((baseOfShellTrd2C / 2.) / heightOfShellTrd2C);
  double gamma = alpha1 + beta2;

  xOfShellTrd2C = -baseOfShellTrd1C / 2. + (heightOfShellTrd2C / 2.) *
    std::cos(gamma);
  zOfShellTrd2C = -heightOfShellTrd1C / 2. + (heightOfShellTrd2C / 2.) *
    std::sin(gamma);
  phiOfShellTrd2C = gamma + M_PI / 2.;

    // Parameters of shell C:
  double xOfMiddlePointOfSide12 = (xOfShellCornersC[0] +
    xOfShellCornersC[1]) / 2.;
  double yOfMiddlePointOfSide12 = (yOfShellCornersC[0] +
    yOfShellCornersC[1]) / 2.;
  x0OfShellC = (xOfMiddlePointOfSide12 + xOfShellCornersC[3]) / 2.;
  y0OfShellC = (yOfMiddlePointOfSide12 + yOfShellCornersC[3]) / 2.;
  double phi0 = std::atan((xOfShellCornersC[0] - xOfShellCornersC[1]) /
    (yOfShellCornersC[1] - yOfShellCornersC[0]));
  phi0OfShellC = -M_PI / 2. + phi0;

    // Parameters of modules C:
  numberOfModulesC = pParameters->GetInteger("NumberOfModulesInRing");
  lengthOfModuleC = pParameters->GetDouble("LengthOfBarrelVolume");

    // Parameters of radiator C:
  double thicknessOfModuleWalls =
    pParameters->GetDouble("ThicknessOfModuleWalls");
  baseOfRadiatorTrd1C = baseOfShellTrd1C - thicknessOfModuleWalls *
    (2. / std::tan(alpha1) +  1. /  std::sin(alpha1));
  heightOfRadiatorTrd1C = baseOfRadiatorTrd1C * std::tan(alpha1) / 2.;

  double alpha2 = std::atan(heightOfShellTrd2C / (baseOfShellTrd2C / 2.));

  baseOfRadiatorTrd2C = baseOfShellTrd2C - thicknessOfModuleWalls *
    (2. / std::tan(alpha2) +  1. /  std::sin(alpha2));
  heightOfRadiatorTrd2C = baseOfShellTrd2C * std::tan(alpha2) / 2.;

  double xOfRadiatorTrd2InShellTrd2C = (baseOfRadiatorTrd2C -
    baseOfShellTrd2C) / 2. + thicknessOfModuleWalls / std::tan(alpha2);
  double zOfRadiatorTrd2InShellTrd2C = (heightOfRadiatorTrd2C -
    heightOfShellTrd2C) / 2. + thicknessOfModuleWalls;

  xOfRadiatorC = (baseOfRadiatorTrd1C - baseOfShellTrd1C) / 2. +
    thicknessOfModuleWalls / std::tan(alpha1);
  zOfRadiatorC = (heightOfRadiatorTrd1C - heightOfShellTrd1C) / 2. +
    thicknessOfModuleWalls;

  double phi = 3. * M_PI / 2. - gamma;

  xOfRadiatorTrd2C = xOfRadiatorTrd2InShellTrd2C * std::cos(phi) +
    zOfRadiatorTrd2InShellTrd2C * std::sin(phi) + xOfShellTrd2C - xOfRadiatorC;
  zOfRadiatorTrd2C = zOfRadiatorTrd2InShellTrd2C * std::cos(phi) -
    xOfRadiatorTrd2InShellTrd2C * std::sin(phi) + zOfShellTrd2C - zOfRadiatorC;
  phiOfRadiatorTrd2C = phiOfShellTrd2C;

    // Parameters of holes C:
  numberOfHolesC = pParameters->GetInteger("NumberOfStrawsC");
  double* xOfHolesC = new double[numberOfHolesC];
  double* yOfHolesC = new double[numberOfHolesC];
  pParameters->GetDoubleArray("XOfHolesC", numberOfHolesC, xOfHolesC);
  pParameters->GetDoubleArray("YOfHolesC", numberOfHolesC, yOfHolesC);
  int i;
  if (xOfHolesC[0] != 0.)
    for (i = 0; i < numberOfHolesC; ++i)
      xOfHolesC[i] -= xOfHolesC[0];
  if (yOfHolesC[0] != 0.)
    for (i = 0; i < numberOfHolesC; ++i)
      yOfHolesC[i] -= yOfHolesC[0];

  double xGlobalOfHole1C = pParameters->GetDouble("XGlobalOfHole1C");
  double yGlobalOfHole1C = pParameters->GetDouble("YGlobalOfHole1C");
  double xGlobalOfHole23C = pParameters->GetDouble("XGlobalOfHole23C");
  double yGlobalOfHole23C = pParameters->GetDouble("YGlobalOfHole23C");

  double startingPhi = CLHEP::Hep3Vector(-xOfHolesC[22],
    yOfHolesC[22], 0.).phi();
  if (startingPhi < 0.)
    startingPhi += M_PI * 2.;
  double globalPhi = CLHEP::Hep3Vector(xGlobalOfHole23C - xGlobalOfHole1C,
    yGlobalOfHole23C - yGlobalOfHole1C, 0.).phi();
  if (globalPhi < 0.)
    globalPhi += M_PI * 2.;
  double deltaPhi = startingPhi - globalPhi;

  double* xGlobalOfHolesC = new double[numberOfHolesC];
  double* yGlobalOfHolesC = new double[numberOfHolesC];

  double sinDeltaPhi = std::sin(deltaPhi);
  double cosDeltaPhi = std::cos(deltaPhi);

  for (i = 0; i < numberOfHolesC; ++i)
  {
    xGlobalOfHolesC[i] = -xOfHolesC[i] * cosDeltaPhi +
      yOfHolesC[i] * sinDeltaPhi + xGlobalOfHole1C;
    yGlobalOfHolesC[i] = yOfHolesC[i] * cosDeltaPhi +
      xOfHolesC[i] * sinDeltaPhi + yGlobalOfHole1C;
  }

  xLocalOfHolesC = new double[numberOfHolesC];
  zLocalOfHolesC = new double[numberOfHolesC];

  double sinPhi0 = std::sin(phi0);
  double cosPhi0 = std::cos(phi0);

  for (i = 0; i < numberOfHolesC; ++i)
  {
    double temporaryX = -yGlobalOfHolesC[i] + y0OfShellC;
    double temporaryZ = xGlobalOfHolesC[i] - x0OfShellC;

    xLocalOfHolesC[i] = temporaryX * cosPhi0 + temporaryZ * sinPhi0 -
      xOfRadiatorC;
    zLocalOfHolesC[i] = temporaryZ * cosPhi0 - temporaryX * sinPhi0 -
      zOfRadiatorC;
  }

    // Parameters of cooling tubes:
  numberOfCoolingTubesC = pParameters->GetInteger("NumberOfCoolingTubes");
  double distanceToCoolingTube =
    pParameters->GetDouble("DistanceToCoolingTube");

  xOfCoolingTubesC = new double[numberOfCoolingTubesC];
  zOfCoolingTubesC = new double[numberOfCoolingTubesC];

  xOfCoolingTubesC[0] = baseOfShellTrd1C / 2. - distanceToCoolingTube *
    (1. / std::sin(alpha1) + 1. / std::tan(alpha1));
  zOfCoolingTubesC[0] = -heightOfShellTrd1C / 2. + distanceToCoolingTube;

  double xOfCoolingTubeInShellTrd2C = baseOfShellTrd2C / 2. -
    distanceToCoolingTube * (1. / std::sin(alpha2) + 1. / std::tan(alpha2));
  double zOfCoolingTubeInShellTrd2C = -heightOfShellTrd2C / 2. +
    distanceToCoolingTube;

  xOfCoolingTubesC[1] = xOfCoolingTubeInShellTrd2C * std::cos(phi) +
    zOfCoolingTubeInShellTrd2C * std::sin(phi) + xOfShellTrd2C;
  zOfCoolingTubesC[1] = zOfCoolingTubeInShellTrd2C * std::cos(phi) -
    xOfCoolingTubeInShellTrd2C * std::sin(phi) + zOfShellTrd2C;

  if (!pParameters->GetInteger("ParameterisedStrawsInModulesBC"))
  {
    for (i = 0; i < numberOfCoolingTubesC; ++i)
    {
      xOfCoolingTubesC[i] -= xOfRadiatorC;
      zOfCoolingTubesC[i] -= zOfRadiatorC;
    }

    xOfHolesForCoolingTubesC = NULL;
    zOfHolesForCoolingTubesC = NULL;
  }
  else
  {
    radiusOfHoleForCoolingTubeC =
      pParameters->GetDouble("OuterRadiusOfCoolingTube");

    xOfHolesForCoolingTubesC = new double[numberOfCoolingTubesC];
    zOfHolesForCoolingTubesC = new double[numberOfCoolingTubesC];

    xOfHolesForCoolingTubesC[0] = xOfCoolingTubesC[0] - xOfRadiatorC;
    zOfHolesForCoolingTubesC[0] = zOfCoolingTubesC[0] - zOfRadiatorC;

    xOfHolesForCoolingTubesC[1] = baseOfRadiatorTrd2C / 2. -
      (distanceToCoolingTube - thicknessOfModuleWalls) * (1. / std::sin(alpha2) +
      1. / std::tan(alpha2));
    zOfHolesForCoolingTubesC[1] = -heightOfRadiatorTrd2C / 2. +
      distanceToCoolingTube - thicknessOfModuleWalls;
  }

  delete [] xOfShellCornersC;
  delete [] yOfShellCornersC;
  delete [] xOfHolesC;
  delete [] yOfHolesC;

  if (pParameters->GetInteger("PrintParametersOfModulesC"))
    PrintParameters(xGlobalOfHolesC, yGlobalOfHolesC);

  delete [] xGlobalOfHolesC;
  delete [] yGlobalOfHolesC;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesC::DefineParameters done" << endmsg;
}


  // Called by DefineParameters

void TRTParametersOfModulesC::PrintParameters(double* xGlobalOfHolesC,
  double* yGlobalOfHolesC) const
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesC::PrintParameters" << endmsg;

  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersOfModulesC::PrintParameters *****" << std::endl;

  output << "Parameters of first trapezoid of shell C:" << std::endl;
  output << "  baseOfShellTrd1C=" << baseOfShellTrd1C << " mm" << std::endl;
  output << "  heightOfShellTrd1C=" << heightOfShellTrd1C << " mm" << std::endl;

  output << std::endl << "Parameters of second trapezoid of shell C:"
         << std::endl;
  output << "  baseOfShellTrd2C=" << baseOfShellTrd2C << " mm" << std::endl;
  output << "  heightOfShellTrd2C=" << heightOfShellTrd2C << " mm" << std::endl;
  output << "  xOfShellTrd2C=" << xOfShellTrd2C << " mm" << std::endl;
  output << "  zOfShellTrd2C=" << zOfShellTrd2C << " mm" << std::endl;
  output << "  phiOfShellTrd2C=" << phiOfShellTrd2C << " rad" << std::endl;

  output << std::endl << "Parameters of shell C:" << std::endl;
  output << "  x0OfShellC=" << x0OfShellC << " mm" << std::endl;
  output << "  y0OfShellC=" << y0OfShellC << " mm" << std::endl;
  output << "  phi0OfShellC=" << phi0OfShellC << " rad" << std::endl;

  output << std::endl << "Parameters of modules C:" << std::endl;
  output << "  numberOfModulesC=" << numberOfModulesC << std::endl;
  output << "  lengthOfModuleC=" << lengthOfModuleC << " mm" << std::endl;

  output << std::endl << "Parameters of first trapezoid of radiator C:"
         << std::endl;
  output << "  baseOfRadiatorTrd1C=" << baseOfRadiatorTrd1C << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd1C=" << heightOfRadiatorTrd1C << " mm"
         << std::endl;

  output << std::endl << "Parameters of second trapezoid of radiator C:"
         << std::endl;
  output << "  baseOfRadiatorTrd2C=" << baseOfRadiatorTrd2C << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd2C=" << heightOfRadiatorTrd2C << " mm"
         << std::endl;
  output << "  xOfRadiatorTrd2C=" << xOfRadiatorTrd2C << " mm" << std::endl;
  output << "  zOfRadiatorTrd2C=" << zOfRadiatorTrd2C << " mm" << std::endl;
  output << "  phiOfRadiatorTrd2C=" << phiOfRadiatorTrd2C << " rad"
         << std::endl;

  output << std::endl << "Parameters of radiator C:" << std::endl;
  output << "  xOfRadiatorC=" << xOfRadiatorC << " mm" << std::endl;
  output << "  zOfRadiatorC=" << zOfRadiatorC << " mm" << std::endl;

  output << std::endl << "Parameters of straw holes C:" << std::endl;
  output << "  numberOfHolesC=" << numberOfHolesC << std::endl;
  int i;
  if (pParameters->GetInteger("PrintParametersOfModulesC") == 2)
  {
    output << std::endl;
    for (i = 0; i < numberOfHolesC; ++i)
      output << "  xLocalOfHolesC[" << i << "]=" << xLocalOfHolesC[i] << " mm"
             << std::endl;

    output << std::endl;
    for (i = 0; i < numberOfHolesC; ++i)
      output << "  zLocalOfHolesC[" << i << "]=" << zLocalOfHolesC[i] << " mm"
             << std::endl;
  }

  output << std::endl << "Parameters of corner straw holes C:" << std::endl;
  output << "  xGlobalOfHole1=" << xGlobalOfHolesC[0] << " mm" << std::endl;
  output << "  xGlobalOfHole23=" << xGlobalOfHolesC[22] << " mm" << std::endl;
  output << "  xGlobalOfHole24=" << xGlobalOfHolesC[23] << " mm" << std::endl;
  output << "  xGlobalOfHole765=" << xGlobalOfHolesC[764] << " mm" << std::endl;
  output << "  xGlobalOfHole766=" << xGlobalOfHolesC[765] << " mm" << std::endl;
  output << "  xGlobalOfHole793=" << xGlobalOfHolesC[792] << " mm" << std::endl;
  output << "  yGlobalOfHole1=" << yGlobalOfHolesC[0] << " mm" << std::endl;
  output << "  yGlobalOfHole23=" << yGlobalOfHolesC[22] << " mm" << std::endl;
  output << "  yGlobalOfHole24=" << yGlobalOfHolesC[23] << " mm" << std::endl;
  output << "  yGlobalOfHole765=" << yGlobalOfHolesC[764] << " mm" << std::endl;
  output << "  yGlobalOfHole766=" << yGlobalOfHolesC[765] << " mm" << std::endl;
  output << "  yGlobalOfHole793=" << yGlobalOfHolesC[792] << " mm" << std::endl;

  output << std::endl << "Parameters of cooling tubes C:" << std::endl;
  output << "  numberOfCoolingTubesC=" << numberOfCoolingTubesC << std::endl
         << std::endl;

  for (i = 0; i < numberOfCoolingTubesC; ++i)
    output << "  xOfCoolingTubesC[" << i << "]=" << xOfCoolingTubesC[i] << " mm"
           << std::endl;
  output << std::endl;

  for (i = 0; i < numberOfCoolingTubesC; ++i)
    output << "  zOfCoolingTubesC[" << i << "]=" << zOfCoolingTubesC[i] << " mm"
           << std::endl;

  if (pParameters->GetInteger("ParameterisedStrawsInModulesBC"))
  {
    output << std::endl << "Parameters of holes in radiator for cooling tubes"
           << " C:" << std::endl;
    output << "  radiusOfHoleForCoolingTubeC=" << radiusOfHoleForCoolingTubeC
           << " mm" << std::endl << std::endl;
    for (i = 0; i < numberOfCoolingTubesC; ++i)
      output << "  xOfHolesForCoolingTubesC[" << i << "]="
             << xOfHolesForCoolingTubesC[i] << " mm" << std::endl;
    output << std::endl;

    for (i = 0; i < numberOfCoolingTubesC; ++i)
      output << "  zOfHolesForCoolingTubesC[" << i << "]="
             << zOfHolesForCoolingTubesC[i] << " mm" << std::endl;
  }

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesC::PrintParameters done" << endmsg;
}
