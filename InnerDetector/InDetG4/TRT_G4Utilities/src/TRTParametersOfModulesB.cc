/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTParametersOfModulesB.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <cmath>
#include <CLHEP/Vector/ThreeVector.h>
#include <fstream>

  // Called by TRTConstructionOfModulesB::TRTConstructionOfModulesB

TRTParametersOfModulesB::TRTParametersOfModulesB(): m_msg("TRTParametersOfModulesB")
{
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesB" << endmsg;

  DefineParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesB done" << endmsg;
}


  // Called by TRTConstructionOfModulesB::~TRTConstructionOfModulesB

TRTParametersOfModulesB::~TRTParametersOfModulesB()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesB" << endmsg;

  delete [] xLocalOfHolesB;
  delete [] zLocalOfHolesB;
  delete [] xOfCoolingTubesB;
  delete [] zOfCoolingTubesB;
  delete [] xOfHolesForCoolingTubesB;
  delete [] zOfHolesForCoolingTubesB;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesB done" << endmsg;
}


  // Called by TRTParametersOfModulesB

void TRTParametersOfModulesB::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesB::DefineParameters" << endmsg;

    // Distances between corners of shell B:
  int numberOfShellCorners = pParameters->GetInteger("NumberOfShellCorners");
  double* xOfShellCornersB = new double[numberOfShellCorners];
  double* yOfShellCornersB = new double[numberOfShellCorners];
  pParameters->GetDoubleArray("XOfShellCornersB", numberOfShellCorners,
    xOfShellCornersB);
  pParameters->GetDoubleArray("YOfShellCornersB", numberOfShellCorners,
    yOfShellCornersB);

  double deltaX12 = xOfShellCornersB[0] - xOfShellCornersB[1];
  double deltaX13 = xOfShellCornersB[0] - xOfShellCornersB[2];
  double deltaX14 = xOfShellCornersB[0] - xOfShellCornersB[3];
  double deltaX23 = xOfShellCornersB[1] - xOfShellCornersB[2];
  double deltaX34 = xOfShellCornersB[2] - xOfShellCornersB[3];

  double deltaY12 = yOfShellCornersB[0] - yOfShellCornersB[1];
  double deltaY13 = yOfShellCornersB[0] - yOfShellCornersB[2];
  double deltaY14 = yOfShellCornersB[0] - yOfShellCornersB[3];
  double deltaY23 = yOfShellCornersB[1] - yOfShellCornersB[2];
  double deltaY34 = yOfShellCornersB[2] - yOfShellCornersB[3];

  double distance12 = std::sqrt(deltaX12 * deltaX12 + deltaY12 * deltaY12);
  double distance13 = std::sqrt(deltaX13 * deltaX13 + deltaY13 * deltaY13);
  double distance14 = std::sqrt(deltaX14 * deltaX14 + deltaY14 * deltaY14);
  double distance23 = std::sqrt(deltaX23 * deltaX23 + deltaY23 * deltaY23);
  double distance34 = std::sqrt(deltaX34 * deltaX34 + deltaY34 * deltaY34);

  double side = (distance14 + distance13 + distance23) / 3.;

    // Parameters of first trapezoid of shell B:
  baseOfShellTrd1B = distance12;
  heightOfShellTrd1B = std::sqrt(side * side - distance12 * distance12 / 4.);

    // Parameters of second trapezoid of shell B:
  baseOfShellTrd2B = distance34;
  heightOfShellTrd2B = std::sqrt(side * side - distance34 * distance34 / 4.);

  double alpha1 = std::atan(heightOfShellTrd1B / (baseOfShellTrd1B / 2.));
  double beta2 = std::atan((baseOfShellTrd2B / 2.) / heightOfShellTrd2B);
  double gamma = alpha1 + beta2;

  xOfShellTrd2B = baseOfShellTrd1B / 2. - (heightOfShellTrd2B / 2.) *
    std::cos(gamma);
  zOfShellTrd2B = -heightOfShellTrd1B / 2. + (heightOfShellTrd2B / 2.) *
    std::sin(gamma);
  phiOfShellTrd2B = -(gamma + M_PI / 2.);

    // Parameters of shell B:
  double xOfMiddlePointOfSide12 = (xOfShellCornersB[0] +
    xOfShellCornersB[1]) / 2.;
  double yOfMiddlePointOfSide12 = (yOfShellCornersB[0] +
    yOfShellCornersB[1]) / 2.;
  x0OfShellB = (xOfMiddlePointOfSide12 + xOfShellCornersB[2]) / 2.;
  y0OfShellB = (yOfMiddlePointOfSide12 + yOfShellCornersB[2]) / 2.;
  double phi0 = std::atan((xOfShellCornersB[0] - xOfShellCornersB[1]) /
    (yOfShellCornersB[1] - yOfShellCornersB[0]));
  phi0OfShellB = -M_PI / 2. + phi0;

    // Parameters of modules B:
  numberOfModulesB = pParameters->GetInteger("NumberOfModulesInRing");
  lengthOfModuleB = pParameters->GetDouble("LengthOfBarrelVolume");

    // Parameters of radiator B:
  double thicknessOfModuleWalls =
    pParameters->GetDouble("ThicknessOfModuleWalls");
  baseOfRadiatorTrd1B = baseOfShellTrd1B - thicknessOfModuleWalls *
    (2. / std::tan(alpha1) +  1. /  std::sin(alpha1));
  heightOfRadiatorTrd1B = baseOfRadiatorTrd1B * std::tan(alpha1) / 2.;

  double alpha2 = std::atan(heightOfShellTrd2B / (baseOfShellTrd2B / 2.));

  baseOfRadiatorTrd2B = baseOfShellTrd2B - thicknessOfModuleWalls *
    (2. / std::tan(alpha2) +  1. /  std::sin(alpha2));
  heightOfRadiatorTrd2B = baseOfShellTrd2B * std::tan(alpha2) / 2.;

  double xOfRadiatorTrd2InShellTrd2B = -(baseOfRadiatorTrd2B -
    baseOfShellTrd2B) / 2. - thicknessOfModuleWalls / std::tan(alpha2);
  double zOfRadiatorTrd2InShellTrd2B = (heightOfRadiatorTrd2B -
    heightOfShellTrd2B) / 2. + thicknessOfModuleWalls;

  xOfRadiatorB = -(baseOfRadiatorTrd1B - baseOfShellTrd1B) / 2. -
    thicknessOfModuleWalls / std::tan(alpha1);
  zOfRadiatorB = (heightOfRadiatorTrd1B - heightOfShellTrd1B) / 2. +
    thicknessOfModuleWalls;

  double phi = -(3. * M_PI / 2. - gamma);

  xOfRadiatorTrd2B = xOfRadiatorTrd2InShellTrd2B * std::cos(phi) +
    zOfRadiatorTrd2InShellTrd2B * std::sin(phi) + xOfShellTrd2B - xOfRadiatorB;
  zOfRadiatorTrd2B = zOfRadiatorTrd2InShellTrd2B * std::cos(phi) -
    xOfRadiatorTrd2InShellTrd2B * std::sin(phi) + zOfShellTrd2B - zOfRadiatorB;
  phiOfRadiatorTrd2B = phiOfShellTrd2B;

    // Parameters of holes B:
  numberOfHolesB = pParameters->GetInteger("NumberOfStrawsB");
  double* xOfHolesB = new double[numberOfHolesB];
  double* yOfHolesB = new double[numberOfHolesB];
  pParameters->GetDoubleArray("XOfHolesB", numberOfHolesB, xOfHolesB);
  pParameters->GetDoubleArray("YOfHolesB", numberOfHolesB, yOfHolesB);
  int i;
  if (xOfHolesB[0] != 0.)
    for (i = 0; i < numberOfHolesB; ++i)
      xOfHolesB[i] -= xOfHolesB[0];
  if (yOfHolesB[0] != 0.)
    for (i = 0; i < numberOfHolesB; ++i)
      yOfHolesB[i] -= yOfHolesB[0];

  double xGlobalOfHole1B = pParameters->GetDouble("XGlobalOfHole1B");
  double yGlobalOfHole1B = pParameters->GetDouble("YGlobalOfHole1B");
  double xGlobalOfHole19B = pParameters->GetDouble("XGlobalOfHole19B");
  double yGlobalOfHole19B = pParameters->GetDouble("YGlobalOfHole19B");

  double startingPhi = CLHEP::Hep3Vector(-xOfHolesB[18],
    yOfHolesB[18], 0.).phi();
  if (startingPhi < 0.)
    startingPhi += M_PI * 2.;
  double globalPhi = CLHEP::Hep3Vector(xGlobalOfHole19B - xGlobalOfHole1B,
    yGlobalOfHole19B - yGlobalOfHole1B, 0.).phi();
  if (globalPhi < 0.)
    globalPhi += M_PI * 2.;
  double deltaPhi = startingPhi - globalPhi;

  double* xGlobalOfHolesB = new double[numberOfHolesB];
  double* yGlobalOfHolesB = new double[numberOfHolesB];

  double sinDeltaPhi = std::sin(deltaPhi);
  double cosDeltaPhi = std::cos(deltaPhi);

  for (i = 0; i < numberOfHolesB; ++i)
  {
    xGlobalOfHolesB[i] = -xOfHolesB[i] * cosDeltaPhi +
      yOfHolesB[i] * sinDeltaPhi + xGlobalOfHole1B;
    yGlobalOfHolesB[i] = yOfHolesB[i] * cosDeltaPhi +
      xOfHolesB[i] * sinDeltaPhi + yGlobalOfHole1B;
  }

  xLocalOfHolesB = new double[numberOfHolesB];
  zLocalOfHolesB = new double[numberOfHolesB];

  double sinPhi0 = std::sin(phi0);
  double cosPhi0 = std::cos(phi0);

  for (i = 0; i < numberOfHolesB; ++i)
  {
    double temporaryX = -yGlobalOfHolesB[i] + y0OfShellB;
    double temporaryZ = xGlobalOfHolesB[i] - x0OfShellB;

    xLocalOfHolesB[i] = temporaryX * cosPhi0 + temporaryZ * sinPhi0 -
      xOfRadiatorB;
    zLocalOfHolesB[i] = temporaryZ * cosPhi0 - temporaryX * sinPhi0 -
      zOfRadiatorB;
  }

    // Parameters of cooling tubes:
  numberOfCoolingTubesB = pParameters->GetInteger("NumberOfCoolingTubes");
  double distanceToCoolingTube =
    pParameters->GetDouble("DistanceToCoolingTube");

  xOfCoolingTubesB = new double[numberOfCoolingTubesB];
  zOfCoolingTubesB = new double[numberOfCoolingTubesB];

  xOfCoolingTubesB[0] = -baseOfShellTrd1B / 2. + distanceToCoolingTube *
    (1. / std::sin(alpha1) + 1. / std::tan(alpha1));
  zOfCoolingTubesB[0] = -heightOfShellTrd1B / 2. + distanceToCoolingTube;

  double xOfCoolingTubeInShellTrd2B = -baseOfShellTrd2B / 2. +
    distanceToCoolingTube * (1. / std::sin(alpha2) + 1. / std::tan(alpha2));
  double zOfCoolingTubeInShellTrd2B = -heightOfShellTrd2B / 2. +
    distanceToCoolingTube;

  xOfCoolingTubesB[1] = xOfCoolingTubeInShellTrd2B * std::cos(phi) +
    zOfCoolingTubeInShellTrd2B * std::sin(phi) + xOfShellTrd2B;
  zOfCoolingTubesB[1] = zOfCoolingTubeInShellTrd2B * std::cos(phi) -
    xOfCoolingTubeInShellTrd2B * std::sin(phi) + zOfShellTrd2B;

  if (!pParameters->GetInteger("ParameterisedStrawsInModulesBC"))
  {
    for (i = 0; i < numberOfCoolingTubesB; ++i)
    {
      xOfCoolingTubesB[i] -= xOfRadiatorB;
      zOfCoolingTubesB[i] -= zOfRadiatorB;
    }

    xOfHolesForCoolingTubesB = NULL;
    zOfHolesForCoolingTubesB = NULL;
  }
  else
  {
    radiusOfHoleForCoolingTubeB =
      pParameters->GetDouble("OuterRadiusOfCoolingTube");

    xOfHolesForCoolingTubesB = new double[numberOfCoolingTubesB];
    zOfHolesForCoolingTubesB = new double[numberOfCoolingTubesB];

    xOfHolesForCoolingTubesB[0] = xOfCoolingTubesB[0] - xOfRadiatorB;
    zOfHolesForCoolingTubesB[0] = zOfCoolingTubesB[0] - zOfRadiatorB;

    xOfHolesForCoolingTubesB[1] = baseOfRadiatorTrd2B / 2. -
      (distanceToCoolingTube - thicknessOfModuleWalls) * (1. / std::sin(alpha2) +
      1. / std::tan(alpha2));
    zOfHolesForCoolingTubesB[1] = -heightOfRadiatorTrd2B / 2. +
      distanceToCoolingTube - thicknessOfModuleWalls;
  }

  delete [] xOfShellCornersB;
  delete [] yOfShellCornersB;
  delete [] xOfHolesB;
  delete [] yOfHolesB;

  if (pParameters->GetInteger("PrintParametersOfModulesB"))
    PrintParameters(xGlobalOfHolesB, yGlobalOfHolesB);

  delete [] xGlobalOfHolesB;
  delete [] yGlobalOfHolesB;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesB::DefineParameters done" << endmsg;
}


  // Called by DefineParameters

void TRTParametersOfModulesB::PrintParameters(double* xGlobalOfHolesB,
  double* yGlobalOfHolesB) const
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesB::PrintParameters" << endmsg;

  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersOfModulesB::PrintParameters *****" << std::endl;

  output << "Parameters of first trapezoid of shell B:" << std::endl;
  output << "  baseOfShellTrd1B=" << baseOfShellTrd1B << " mm" << std::endl;
  output << "  heightOfShellTrd1B=" << heightOfShellTrd1B << " mm" << std::endl;

  output << std::endl << "Parameters of second trapezoid of shell B:"
         << std::endl;
  output << "  baseOfShellTrd2B=" << baseOfShellTrd2B << " mm" << std::endl;
  output << "  heightOfShellTrd2B=" << heightOfShellTrd2B << " mm" << std::endl;
  output << "  xOfShellTrd2B=" << xOfShellTrd2B << " mm" << std::endl;
  output << "  zOfShellTrd2B=" << zOfShellTrd2B << " mm" << std::endl;
  output << "  phiOfShellTrd2B=" << phiOfShellTrd2B << " rad" << std::endl;

  output << std::endl << "Parameters of shell B:" << std::endl;
  output << "  x0OfShellB=" << x0OfShellB << " mm" << std::endl;
  output << "  y0OfShellB=" << y0OfShellB << " mm" << std::endl;
  output << "  phi0OfShellB=" << phi0OfShellB << " rad" << std::endl;

  output << std::endl << "Parameters of modules B:" << std::endl;
  output << "  numberOfModulesB=" << numberOfModulesB << std::endl;
  output << "  lengthOfModuleB=" << lengthOfModuleB << " mm" << std::endl;

  output << std::endl << "Parameters of first trapezoid of radiator B:"
         << std::endl;
  output << "  baseOfRadiatorTrd1B=" << baseOfRadiatorTrd1B << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd1B=" << heightOfRadiatorTrd1B << " mm"
         << std::endl;

  output << std::endl << "Parameters of second trapezoid of radiator B:"
         << std::endl;
  output << "  baseOfRadiatorTrd2B=" << baseOfRadiatorTrd2B << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd2B=" << heightOfRadiatorTrd2B << " mm"
         << std::endl;
  output << "  xOfRadiatorTrd2B=" << xOfRadiatorTrd2B << " mm" << std::endl;
  output << "  zOfRadiatorTrd2B=" << zOfRadiatorTrd2B << " mm" << std::endl;
  output << "  phiOfRadiatorTrd2B=" << phiOfRadiatorTrd2B << " rad"
         << std::endl;

  output << std::endl << "Parameters of radiator B:" << std::endl;
  output << "  xOfRadiatorB=" << xOfRadiatorB << " mm" << std::endl;
  output << "  zOfRadiatorB=" << zOfRadiatorB << " mm" << std::endl;

  output << std::endl << "Parameters of straw holes B:" << std::endl;
  output << "  numberOfHolesB=" << numberOfHolesB << std::endl;
  int i;
  if (pParameters->GetInteger("PrintParametersOfModulesB") == 2)
  {
    output << std::endl;
    for (i = 0; i < numberOfHolesB; ++i)
      output << "  xLocalOfHolesB[" << i << "]=" << xLocalOfHolesB[i] << " mm"
             << std::endl;

    output << std::endl;
    for (i = 0; i < numberOfHolesB; ++i)
      output << "  zLocalOfHolesB[" << i << "]=" << zLocalOfHolesB[i] << " mm"
             << std::endl;
  }

  output << std::endl << "Parameters of corner straw holes B:" << std::endl;
  output << "  xGlobalOfHole1=" << xGlobalOfHolesB[0] << " mm" << std::endl;
  output << "  xGlobalOfHole19=" << xGlobalOfHolesB[18] << " mm" << std::endl;
  output << "  xGlobalOfHole39=" << xGlobalOfHolesB[38] << " mm" << std::endl;
  output << "  xGlobalOfHole474=" << xGlobalOfHolesB[473] << " mm" << std::endl;
  output << "  xGlobalOfHole498=" << xGlobalOfHolesB[497] << " mm" << std::endl;
  output << "  xGlobalOfHole520=" << xGlobalOfHolesB[519] << " mm" << std::endl;
  output << "  yGlobalOfHole1=" << yGlobalOfHolesB[0] << " mm" << std::endl;
  output << "  yGlobalOfHole19=" << yGlobalOfHolesB[18] << " mm" << std::endl;
  output << "  yGlobalOfHole39=" << yGlobalOfHolesB[38] << " mm" << std::endl;
  output << "  yGlobalOfHole474=" << yGlobalOfHolesB[473] << " mm" << std::endl;
  output << "  yGlobalOfHole498=" << yGlobalOfHolesB[497] << " mm" << std::endl;
  output << "  yGlobalOfHole520=" << yGlobalOfHolesB[519] << " mm" << std::endl;

  output << std::endl << "Parameters of cooling tubes B:" << std::endl;
  output << "  numberOfCoolingTubesB=" << numberOfCoolingTubesB << std::endl
         << std::endl;
  for (i = 0; i < numberOfCoolingTubesB; ++i)
    output << "  xOfCoolingTubesB[" << i << "]=" << xOfCoolingTubesB[i] << " mm"
           << std::endl;
  output << std::endl;

  for (i = 0; i < numberOfCoolingTubesB; ++i)
    output << "  zOfCoolingTubesB[" << i << "]=" << zOfCoolingTubesB[i] << " mm"
           << std::endl;

  if (pParameters->GetInteger("ParameterisedStrawsInModulesBC"))
  {
    output << std::endl << "Parameters of holes in radiator for cooling tubes"
           << " B:" << std::endl;
    output << "  radiusOfHoleForCoolingTubeB=" << radiusOfHoleForCoolingTubeB
           << " mm" << std::endl << std::endl;
    for (i = 0; i < numberOfCoolingTubesB; ++i)
      output << "  xOfHolesForCoolingTubesB[" << i << "]="
             << xOfHolesForCoolingTubesB[i] << " mm" << std::endl;
    output << std::endl;

    for (i = 0; i < numberOfCoolingTubesB; ++i)
      output << "  zOfHolesForCoolingTubesB[" << i << "]="
             << zOfHolesForCoolingTubesB[i] << " mm" << std::endl;
  }

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesB::PrintParameters done" << endmsg;
}
