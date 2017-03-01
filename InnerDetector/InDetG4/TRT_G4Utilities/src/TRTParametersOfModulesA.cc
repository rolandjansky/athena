/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTParametersOfModulesA.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <cmath>
#include <CLHEP/Vector/ThreeVector.h>
#include <fstream>

  // Called by TRTConstructionOfModulesA::TRTConstructionOfModulesA

TRTParametersOfModulesA::TRTParametersOfModulesA() : m_msg("TRTParametersOfModulesA")
{
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesA" << endmsg;

  DefineParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesA done" << endmsg;
}


  // Called by TRTConstructionOfModulesA::~TRTConstructionOfModulesA

TRTParametersOfModulesA::~TRTParametersOfModulesA()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesA" << endmsg;

  delete [] xLocalOfHolesA;
  delete [] zLocalOfHolesA;
  delete [] xOfCoolingTubesA;
  delete [] zOfCoolingTubesA;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesA done" << endmsg;
}


  // Called by TRTParametersOfModulesA

void TRTParametersOfModulesA::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesA::DefineParameters"	<< endmsg;

    // Distances between corners of shell A:
  int numberOfShellCorners = pParameters->GetInteger("NumberOfShellCorners");
  double* xOfShellCornersA = new double[numberOfShellCorners];
  double* yOfShellCornersA = new double[numberOfShellCorners];
  pParameters->GetDoubleArray("XOfShellCornersA", numberOfShellCorners,
    xOfShellCornersA);
  pParameters->GetDoubleArray("YOfShellCornersA", numberOfShellCorners,
    yOfShellCornersA);

  double deltaX12 = xOfShellCornersA[0] - xOfShellCornersA[1];
  double deltaX14 = xOfShellCornersA[0] - xOfShellCornersA[3];
  double deltaX23 = xOfShellCornersA[1] - xOfShellCornersA[2];
  double deltaX24 = xOfShellCornersA[1] - xOfShellCornersA[3];
  double deltaX34 = xOfShellCornersA[2] - xOfShellCornersA[3];

  double deltaY12 = yOfShellCornersA[0] - yOfShellCornersA[1];
  double deltaY14 = yOfShellCornersA[0] - yOfShellCornersA[3];
  double deltaY23 = yOfShellCornersA[1] - yOfShellCornersA[2];
  double deltaY24 = yOfShellCornersA[1] - yOfShellCornersA[3];
  double deltaY34 = yOfShellCornersA[2] - yOfShellCornersA[3];

  double distance12 = std::sqrt(deltaX12 * deltaX12 + deltaY12 * deltaY12);
  double distance14 = std::sqrt(deltaX14 * deltaX14 + deltaY14 * deltaY14);
  double distance23 = std::sqrt(deltaX23 * deltaX23 + deltaY23 * deltaY23);
  double distance24 = std::sqrt(deltaX24 * deltaX24 + deltaY24 * deltaY24);
  double distance34 = std::sqrt(deltaX34 * deltaX34 + deltaY34 * deltaY34);

  double side = (distance14 + distance23 + distance24) / 3.;

    // Parameters of first trapezoid of shell A:
  baseOfShellTrd1A = distance12;
  heightOfShellTrd1A = std::sqrt(side * side - distance12 * distance12 / 4.);

    // Parameters of second trapezoid of shell A:
  baseOfShellTrd2A = distance34;
  heightOfShellTrd2A = std::sqrt(side * side - distance34 * distance34 / 4.);

  double alpha1 = std::atan(heightOfShellTrd1A / (baseOfShellTrd1A / 2.));
  double beta2 = std::atan((baseOfShellTrd2A / 2.) / heightOfShellTrd2A);
  double gamma = alpha1 + beta2;

  xOfShellTrd2A = -baseOfShellTrd1A / 2. + (heightOfShellTrd2A / 2.) *
    std::cos(gamma);
  zOfShellTrd2A = -heightOfShellTrd1A / 2. + (heightOfShellTrd2A / 2.) *
    std::sin(gamma);
  phiOfShellTrd2A = gamma + M_PI / 2.;

    // Parameters of shell A:
  double xOfMiddlePointOfSide12 = (xOfShellCornersA[0] +
    xOfShellCornersA[1]) / 2.;
  double yOfMiddlePointOfSide12 = (yOfShellCornersA[0] +
    yOfShellCornersA[1]) / 2.;
  x0OfShellA = (xOfMiddlePointOfSide12 + xOfShellCornersA[3]) / 2.;
  y0OfShellA = (yOfMiddlePointOfSide12 + yOfShellCornersA[3]) / 2.;
  double phi0 = std::atan((xOfShellCornersA[0] - xOfShellCornersA[1]) /
    (yOfShellCornersA[1] - yOfShellCornersA[0]));
  phi0OfShellA = -M_PI / 2. + phi0;

    // Parameters of modules A:
  numberOfModulesA = pParameters->GetInteger("NumberOfModulesInRing");
  lengthOfModuleA = pParameters->GetDouble("LengthOfBarrelVolume");

    // Parameters of radiator A:
  double thicknessOfModuleWalls =
    pParameters->GetDouble("ThicknessOfModuleWalls");
  baseOfRadiatorTrd1A = baseOfShellTrd1A - thicknessOfModuleWalls *
    (2. / std::tan(alpha1) +  1. /  std::sin(alpha1));
  heightOfRadiatorTrd1A = baseOfRadiatorTrd1A * std::tan(alpha1) / 2.;

  double alpha2 = std::atan(heightOfShellTrd2A / (baseOfShellTrd2A / 2.));

  baseOfRadiatorTrd2A = baseOfShellTrd2A - thicknessOfModuleWalls *
    (2. / std::tan(alpha2) +  1. /  std::sin(alpha2));
  heightOfRadiatorTrd2A = baseOfShellTrd2A * std::tan(alpha2) / 2.;

  double xOfRadiatorTrd2InShellTrd2A = (baseOfRadiatorTrd2A -
    baseOfShellTrd2A) / 2. + thicknessOfModuleWalls / std::tan(alpha2);
  double zOfRadiatorTrd2InShellTrd2A = (heightOfRadiatorTrd2A -
    heightOfShellTrd2A) / 2. + thicknessOfModuleWalls;

  xOfRadiatorA = (baseOfRadiatorTrd1A - baseOfShellTrd1A) / 2. +
    thicknessOfModuleWalls / std::tan(alpha1);
  zOfRadiatorA = (heightOfRadiatorTrd1A - heightOfShellTrd1A) / 2. +
    thicknessOfModuleWalls;

  double phi = 3. * M_PI / 2. - gamma;

  xOfRadiatorTrd2A = xOfRadiatorTrd2InShellTrd2A * std::cos(phi) +
    zOfRadiatorTrd2InShellTrd2A * std::sin(phi) + xOfShellTrd2A - xOfRadiatorA;
  zOfRadiatorTrd2A = zOfRadiatorTrd2InShellTrd2A * std::cos(phi) -
    xOfRadiatorTrd2InShellTrd2A * std::sin(phi) + zOfShellTrd2A - zOfRadiatorA;
  phiOfRadiatorTrd2A = phiOfShellTrd2A;

    // Parameters of holes A:
  numberOfHolesA = pParameters->GetInteger("NumberOfStrawsA");
  double* xOfHolesA = new double[numberOfHolesA];
  double* yOfHolesA = new double[numberOfHolesA];
  pParameters->GetDoubleArray("XOfHolesA", numberOfHolesA, xOfHolesA);
  pParameters->GetDoubleArray("YOfHolesA", numberOfHolesA, yOfHolesA);
  int i;
  if (xOfHolesA[0] != 0.)
    for (i = 0; i < numberOfHolesA; ++i)
      xOfHolesA[i] -= xOfHolesA[0];
  if (yOfHolesA[0] != 0.)
    for (i = 0; i < numberOfHolesA; ++i)
      yOfHolesA[i] -= yOfHolesA[0];
  for (i = 0; i < numberOfHolesA; ++i)
    xOfHolesA[i] = -xOfHolesA[i];

  double xGlobalOfHole1A = pParameters->GetDouble("XGlobalOfHole1A");
  double yGlobalOfHole1A = pParameters->GetDouble("YGlobalOfHole1A");
  double xGlobalOfHole15A = pParameters->GetDouble("XGlobalOfHole15A");
  double yGlobalOfHole15A = pParameters->GetDouble("YGlobalOfHole15A");

  double startingPhi = CLHEP::Hep3Vector(xOfHolesA[14], yOfHolesA[14], 0.).phi();
  if (startingPhi < 0.)
    startingPhi += M_PI * 2.;
  double globalPhi = CLHEP::Hep3Vector(xGlobalOfHole15A - xGlobalOfHole1A,
    yGlobalOfHole15A - yGlobalOfHole1A, 0.).phi();
  if (globalPhi < 0.)
    globalPhi += M_PI * 2.;
  double deltaPhi = startingPhi - globalPhi;

  double* xGlobalOfHolesA = new double[numberOfHolesA];
  double* yGlobalOfHolesA = new double[numberOfHolesA];

  double sinDeltaPhi = std::sin(deltaPhi);
  double cosDeltaPhi = std::cos(deltaPhi);

  for (i = 0; i < numberOfHolesA; ++i)
  {
    xGlobalOfHolesA[i] = xOfHolesA[i] * cosDeltaPhi +
      yOfHolesA[i] * sinDeltaPhi + xGlobalOfHole1A;
    yGlobalOfHolesA[i] = yOfHolesA[i] * cosDeltaPhi -
      xOfHolesA[i] * sinDeltaPhi + yGlobalOfHole1A;
  }

  xLocalOfHolesA = new double[numberOfHolesA];
  zLocalOfHolesA = new double[numberOfHolesA];

  double sinPhi0 = std::sin(phi0);
  double cosPhi0 = std::cos(phi0);

  for (i = 0; i < numberOfHolesA; ++i)
  {
    double temporaryX = -yGlobalOfHolesA[i] + y0OfShellA;
    double temporaryZ = xGlobalOfHolesA[i] - x0OfShellA;

    xLocalOfHolesA[i] = temporaryX * cosPhi0 + temporaryZ * sinPhi0 -
      xOfRadiatorA;
    zLocalOfHolesA[i] = temporaryZ * cosPhi0 - temporaryX * sinPhi0 -
      zOfRadiatorA;
  }

    // Parameters of cooling tubes:
  numberOfCoolingTubesA = pParameters->GetInteger("NumberOfCoolingTubes");
  double distanceToCoolingTube =
    pParameters->GetDouble("DistanceToCoolingTube");

  xOfCoolingTubesA = new double[numberOfCoolingTubesA];
  zOfCoolingTubesA = new double[numberOfCoolingTubesA];

  xOfCoolingTubesA[0] = baseOfShellTrd1A / 2. - distanceToCoolingTube *
    (1. / std::sin(alpha1) + 1. / std::tan(alpha1));
  zOfCoolingTubesA[0] = -heightOfShellTrd1A / 2. + distanceToCoolingTube;

  double xOfCoolingTubeInShellTrd2A = baseOfShellTrd2A / 2. -
    distanceToCoolingTube * (1. / std::sin(alpha2) + 1. / std::tan(alpha2));
  double zOfCoolingTubeInShellTrd2A = -heightOfShellTrd2A / 2. +
    distanceToCoolingTube;

  xOfCoolingTubesA[1] = xOfCoolingTubeInShellTrd2A * std::cos(phi) +
    zOfCoolingTubeInShellTrd2A * std::sin(phi) + xOfShellTrd2A;
  zOfCoolingTubesA[1] = zOfCoolingTubeInShellTrd2A * std::cos(phi) -
    xOfCoolingTubeInShellTrd2A * std::sin(phi) + zOfShellTrd2A;

  for (i = 0; i < numberOfCoolingTubesA; ++i)
  {
    xOfCoolingTubesA[i] -= xOfRadiatorA;
    zOfCoolingTubesA[i] -= zOfRadiatorA;
  }

    // Number of short straws A:
  int numberOfLayersWithShortStrawsA =
    pParameters->GetInteger("NumberOfLayersWithShortStrawsA");
  int* numberOfShortStrawsInLayersA =
    new int[numberOfLayersWithShortStrawsA];
  pParameters->GetPartOfIntegerArray("NumberOfStrawsInLayersA",
    numberOfLayersWithShortStrawsA, numberOfShortStrawsInLayersA);
  numberOfShortStrawsA = 0;
  for (i = 0; i < numberOfLayersWithShortStrawsA; ++i)
    numberOfShortStrawsA += numberOfShortStrawsInLayersA[i];

  delete [] xOfShellCornersA;
  delete [] yOfShellCornersA;
  delete [] xOfHolesA;
  delete [] yOfHolesA;
  delete [] numberOfShortStrawsInLayersA;

  if (pParameters->GetInteger("PrintParametersOfModulesA"))
    PrintParameters(xGlobalOfHolesA, yGlobalOfHolesA);

  delete [] xGlobalOfHolesA;
  delete [] yGlobalOfHolesA;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesA::DefineParameters done" << endmsg;
}


  // Called by DefineParameters

void TRTParametersOfModulesA::PrintParameters(double* xGlobalOfHolesA,
  double* yGlobalOfHolesA) const
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesA::PrintParameters" << endmsg;

  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersOfModulesA::PrintParameters *****" << std::endl;

  output << "Parameters of first trapezoid of shell A:" << std::endl;
  output << "  baseOfShellTrd1A=" << baseOfShellTrd1A << " mm" << std::endl;
  output << "  heightOfShellTrd1A=" << heightOfShellTrd1A << " mm" << std::endl;

  output << std::endl << "Parameters of second trapezoid of shell A:"
         << std::endl;
  output << "  baseOfShellTrd2A=" << baseOfShellTrd2A << " mm" << std::endl;
  output << "  heightOfShellTrd2A=" << heightOfShellTrd2A << " mm" << std::endl;
  output << "  xOfShellTrd2A=" << xOfShellTrd2A << " mm" << std::endl;
  output << "  zOfShellTrd2A=" << zOfShellTrd2A << " mm" << std::endl;
  output << "  phiOfShellTrd2A=" << phiOfShellTrd2A << " rad" << std::endl;

  output << std::endl << "Parameters of shell A:" << std::endl;
  output << "  x0OfShellA=" << x0OfShellA << " mm" << std::endl;
  output << "  y0OfShellA=" << y0OfShellA << " mm" << std::endl;
  output << "  phi0OfShellA=" << phi0OfShellA << " rad" << std::endl;

  output << std::endl << "Parameters of modules A:" << std::endl;
  output << "  numberOfModulesA=" << numberOfModulesA << std::endl;
  output << "  lengthOfModuleA=" << lengthOfModuleA << " mm" << std::endl;

  output << std::endl << "Parameters of first trapezoid of radiator A:"
         << std::endl;
  output << "  baseOfRadiatorTrd1A=" << baseOfRadiatorTrd1A << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd1A=" << heightOfRadiatorTrd1A << " mm"
         << std::endl;

  output << std::endl << "Parameters of second trapezoid of radiator A:"
         << std::endl;
  output << "  baseOfRadiatorTrd2A=" << baseOfRadiatorTrd2A << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd2A=" << heightOfRadiatorTrd2A << " mm"
         << std::endl;
  output << "  xOfRadiatorTrd2A=" << xOfRadiatorTrd2A << " mm" << std::endl;
  output << "  zOfRadiatorTrd2A=" << zOfRadiatorTrd2A << " mm" << std::endl;
  output << "  phiOfRadiatorTrd2A=" << phiOfRadiatorTrd2A << " rad"
         << std::endl;

  output << std::endl << "Parameters of radiator A:" << std::endl;
  output << "  xOfRadiatorA=" << xOfRadiatorA << " mm" << std::endl;
  output << "  zOfRadiatorA=" << zOfRadiatorA << " mm" << std::endl;

  output << std::endl << "Parameters of straw holes A:" << std::endl;
  output << "  numberOfHolesA=" << numberOfHolesA << std::endl;
  int i;
  if (pParameters->GetInteger("PrintParametersOfModulesA") == 2)
  {
    output << std::endl;
    for (i = 0; i < numberOfHolesA; ++i)
      output << "  xLocalOfHolesA[" << i << "]=" << xLocalOfHolesA[i] << " mm"
             << std::endl;

    output << std::endl;
    for (i = 0; i < numberOfHolesA; ++i)
      output << "  zLocalOfHolesA[" << i << "]=" << zLocalOfHolesA[i] << " mm"
             << std::endl;
  }

  output << std::endl << "Parameters of corner straw holes A:" << std::endl;
  output << "  xGlobalOfHole1=" << xGlobalOfHolesA[0] << " mm" << std::endl;
  output << "  xGlobalOfHole15=" << xGlobalOfHolesA[14] << " mm" << std::endl;
  output << "  xGlobalOfHole16=" << xGlobalOfHolesA[15] << " mm" << std::endl;
  output << "  xGlobalOfHole311=" << xGlobalOfHolesA[310] << " mm" << std::endl;
  output << "  xGlobalOfHole312=" << xGlobalOfHolesA[311] << " mm" << std::endl;
  output << "  xGlobalOfHole329=" << xGlobalOfHolesA[328] << " mm" << std::endl;
  output << "  yGlobalOfHole1=" << yGlobalOfHolesA[0] << " mm" << std::endl;
  output << "  yGlobalOfHole15=" << yGlobalOfHolesA[14] << " mm" << std::endl;
  output << "  yGlobalOfHole16=" << yGlobalOfHolesA[15] << " mm" << std::endl;
  output << "  yGlobalOfHole311=" << yGlobalOfHolesA[310] << " mm" << std::endl;
  output << "  yGlobalOfHole312=" << yGlobalOfHolesA[311] << " mm" << std::endl;
  output << "  yGlobalOfHole329=" << yGlobalOfHolesA[328] << " mm" << std::endl;

  output << std::endl << "Parameters of cooling tubes A:" << std::endl;
  output << "  numberOfCoolingTubesA=" << numberOfCoolingTubesA << std::endl
         << std::endl;

  for (i = 0; i < numberOfCoolingTubesA; ++i)
    output << "  xOfCoolingTubesA[" << i << "]=" << xOfCoolingTubesA[i] << " mm"
           << std::endl;
  output << std::endl;

  for (i = 0; i < numberOfCoolingTubesA; ++i)
    output << "  zOfCoolingTubesA[" << i << "]=" << zOfCoolingTubesA[i] << " mm"
           << std::endl;

  output << std::endl << "Parameters of straws A:" << std::endl;
  output << "  numberOfShortStrawsA=" << numberOfShortStrawsA << std::endl;

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesA::PrintParameters done" << endmsg;
}
