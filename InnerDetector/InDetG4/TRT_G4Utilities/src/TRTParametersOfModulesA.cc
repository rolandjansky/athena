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
  m_pParameters = TRTParameters::GetPointer();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesA" << endmsg;

  DefineParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesA done" << endmsg;
}


  // Called by TRTConstructionOfModulesA::~TRTConstructionOfModulesA

TRTParametersOfModulesA::~TRTParametersOfModulesA()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesA" << endmsg;

  delete [] m_xLocalOfHolesA;
  delete [] m_zLocalOfHolesA;
  delete [] m_xOfCoolingTubesA;
  delete [] m_zOfCoolingTubesA;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesA done" << endmsg;
}


  // Called by TRTParametersOfModulesA

void TRTParametersOfModulesA::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesA::DefineParameters"	<< endmsg;

    // Distances between corners of shell A:
  int numberOfShellCorners = m_pParameters->GetInteger("NumberOfShellCorners");
  double* xOfShellCornersA = new double[numberOfShellCorners];
  double* yOfShellCornersA = new double[numberOfShellCorners];
  m_pParameters->GetDoubleArray("XOfShellCornersA", numberOfShellCorners,
    xOfShellCornersA);
  m_pParameters->GetDoubleArray("YOfShellCornersA", numberOfShellCorners,
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
  m_baseOfShellTrd1A = distance12;
  m_heightOfShellTrd1A = std::sqrt(side * side - distance12 * distance12 / 4.);

    // Parameters of second trapezoid of shell A:
  m_baseOfShellTrd2A = distance34;
  m_heightOfShellTrd2A = std::sqrt(side * side - distance34 * distance34 / 4.);

  double alpha1 = std::atan(m_heightOfShellTrd1A / (m_baseOfShellTrd1A / 2.));
  double beta2 = std::atan((m_baseOfShellTrd2A / 2.) / m_heightOfShellTrd2A);
  double gamma = alpha1 + beta2;

  m_xOfShellTrd2A = -m_baseOfShellTrd1A / 2. + (m_heightOfShellTrd2A / 2.) *
    std::cos(gamma);
  m_zOfShellTrd2A = -m_heightOfShellTrd1A / 2. + (m_heightOfShellTrd2A / 2.) *
    std::sin(gamma);
  m_phiOfShellTrd2A = gamma + M_PI / 2.;

    // Parameters of shell A:
  double xOfMiddlePointOfSide12 = (xOfShellCornersA[0] +
    xOfShellCornersA[1]) / 2.;
  double yOfMiddlePointOfSide12 = (yOfShellCornersA[0] +
    yOfShellCornersA[1]) / 2.;
  m_x0OfShellA = (xOfMiddlePointOfSide12 + xOfShellCornersA[3]) / 2.;
  m_y0OfShellA = (yOfMiddlePointOfSide12 + yOfShellCornersA[3]) / 2.;
  double phi0 = std::atan((xOfShellCornersA[0] - xOfShellCornersA[1]) /
    (yOfShellCornersA[1] - yOfShellCornersA[0]));
  m_phi0OfShellA = -M_PI / 2. + phi0;

    // Parameters of modules A:
  m_numberOfModulesA = m_pParameters->GetInteger("NumberOfModulesInRing");
  m_lengthOfModuleA = m_pParameters->GetDouble("LengthOfBarrelVolume");

    // Parameters of radiator A:
  double thicknessOfModuleWalls =
    m_pParameters->GetDouble("ThicknessOfModuleWalls");
  m_baseOfRadiatorTrd1A = m_baseOfShellTrd1A - thicknessOfModuleWalls *
    (2. / std::tan(alpha1) +  1. /  std::sin(alpha1));
  m_heightOfRadiatorTrd1A = m_baseOfRadiatorTrd1A * std::tan(alpha1) / 2.;

  double alpha2 = std::atan(m_heightOfShellTrd2A / (m_baseOfShellTrd2A / 2.));

  m_baseOfRadiatorTrd2A = m_baseOfShellTrd2A - thicknessOfModuleWalls *
    (2. / std::tan(alpha2) +  1. /  std::sin(alpha2));
  m_heightOfRadiatorTrd2A = m_baseOfShellTrd2A * std::tan(alpha2) / 2.;

  double xOfRadiatorTrd2InShellTrd2A = (m_baseOfRadiatorTrd2A -
    m_baseOfShellTrd2A) / 2. + thicknessOfModuleWalls / std::tan(alpha2);
  double zOfRadiatorTrd2InShellTrd2A = (m_heightOfRadiatorTrd2A -
    m_heightOfShellTrd2A) / 2. + thicknessOfModuleWalls;

  m_xOfRadiatorA = (m_baseOfRadiatorTrd1A - m_baseOfShellTrd1A) / 2. +
    thicknessOfModuleWalls / std::tan(alpha1);
  m_zOfRadiatorA = (m_heightOfRadiatorTrd1A - m_heightOfShellTrd1A) / 2. +
    thicknessOfModuleWalls;

  double phi = 3. * M_PI / 2. - gamma;

  m_xOfRadiatorTrd2A = xOfRadiatorTrd2InShellTrd2A * std::cos(phi) +
    zOfRadiatorTrd2InShellTrd2A * std::sin(phi) + m_xOfShellTrd2A - m_xOfRadiatorA;
  m_zOfRadiatorTrd2A = zOfRadiatorTrd2InShellTrd2A * std::cos(phi) -
    xOfRadiatorTrd2InShellTrd2A * std::sin(phi) + m_zOfShellTrd2A - m_zOfRadiatorA;
  m_phiOfRadiatorTrd2A = m_phiOfShellTrd2A;

    // Parameters of holes A:
  m_numberOfHolesA = m_pParameters->GetInteger("NumberOfStrawsA");
  double* xOfHolesA = new double[m_numberOfHolesA];
  double* yOfHolesA = new double[m_numberOfHolesA];
  m_pParameters->GetDoubleArray("XOfHolesA", m_numberOfHolesA, xOfHolesA);
  m_pParameters->GetDoubleArray("YOfHolesA", m_numberOfHolesA, yOfHolesA);
  int i;
  if (xOfHolesA[0] != 0.)
    for (i = 0; i < m_numberOfHolesA; ++i)
      xOfHolesA[i] -= xOfHolesA[0];
  if (yOfHolesA[0] != 0.)
    for (i = 0; i < m_numberOfHolesA; ++i)
      yOfHolesA[i] -= yOfHolesA[0];
  for (i = 0; i < m_numberOfHolesA; ++i)
    xOfHolesA[i] = -xOfHolesA[i];

  double xGlobalOfHole1A = m_pParameters->GetDouble("XGlobalOfHole1A");
  double yGlobalOfHole1A = m_pParameters->GetDouble("YGlobalOfHole1A");
  double xGlobalOfHole15A = m_pParameters->GetDouble("XGlobalOfHole15A");
  double yGlobalOfHole15A = m_pParameters->GetDouble("YGlobalOfHole15A");

  double startingPhi = CLHEP::Hep3Vector(xOfHolesA[14], yOfHolesA[14], 0.).phi();
  if (startingPhi < 0.)
    startingPhi += M_PI * 2.;
  double globalPhi = CLHEP::Hep3Vector(xGlobalOfHole15A - xGlobalOfHole1A,
    yGlobalOfHole15A - yGlobalOfHole1A, 0.).phi();
  if (globalPhi < 0.)
    globalPhi += M_PI * 2.;
  double deltaPhi = startingPhi - globalPhi;

  double* xGlobalOfHolesA = new double[m_numberOfHolesA];
  double* yGlobalOfHolesA = new double[m_numberOfHolesA];

  double sinDeltaPhi = std::sin(deltaPhi);
  double cosDeltaPhi = std::cos(deltaPhi);

  for (i = 0; i < m_numberOfHolesA; ++i)
  {
    xGlobalOfHolesA[i] = xOfHolesA[i] * cosDeltaPhi +
      yOfHolesA[i] * sinDeltaPhi + xGlobalOfHole1A;
    yGlobalOfHolesA[i] = yOfHolesA[i] * cosDeltaPhi -
      xOfHolesA[i] * sinDeltaPhi + yGlobalOfHole1A;
  }

  m_xLocalOfHolesA = new double[m_numberOfHolesA];
  m_zLocalOfHolesA = new double[m_numberOfHolesA];

  double sinPhi0 = std::sin(phi0);
  double cosPhi0 = std::cos(phi0);

  for (i = 0; i < m_numberOfHolesA; ++i)
  {
    double temporaryX = -yGlobalOfHolesA[i] + m_y0OfShellA;
    double temporaryZ = xGlobalOfHolesA[i] - m_x0OfShellA;

    m_xLocalOfHolesA[i] = temporaryX * cosPhi0 + temporaryZ * sinPhi0 -
      m_xOfRadiatorA;
    m_zLocalOfHolesA[i] = temporaryZ * cosPhi0 - temporaryX * sinPhi0 -
      m_zOfRadiatorA;
  }

    // Parameters of cooling tubes:
  m_numberOfCoolingTubesA = m_pParameters->GetInteger("NumberOfCoolingTubes");
  double distanceToCoolingTube =
    m_pParameters->GetDouble("DistanceToCoolingTube");

  m_xOfCoolingTubesA = new double[m_numberOfCoolingTubesA];
  m_zOfCoolingTubesA = new double[m_numberOfCoolingTubesA];

  m_xOfCoolingTubesA[0] = m_baseOfShellTrd1A / 2. - distanceToCoolingTube *
    (1. / std::sin(alpha1) + 1. / std::tan(alpha1));
  m_zOfCoolingTubesA[0] = -m_heightOfShellTrd1A / 2. + distanceToCoolingTube;

  double xOfCoolingTubeInShellTrd2A = m_baseOfShellTrd2A / 2. -
    distanceToCoolingTube * (1. / std::sin(alpha2) + 1. / std::tan(alpha2));
  double zOfCoolingTubeInShellTrd2A = -m_heightOfShellTrd2A / 2. +
    distanceToCoolingTube;

  m_xOfCoolingTubesA[1] = xOfCoolingTubeInShellTrd2A * std::cos(phi) +
    zOfCoolingTubeInShellTrd2A * std::sin(phi) + m_xOfShellTrd2A;
  m_zOfCoolingTubesA[1] = zOfCoolingTubeInShellTrd2A * std::cos(phi) -
    xOfCoolingTubeInShellTrd2A * std::sin(phi) + m_zOfShellTrd2A;

  for (i = 0; i < m_numberOfCoolingTubesA; ++i)
  {
    m_xOfCoolingTubesA[i] -= m_xOfRadiatorA;
    m_zOfCoolingTubesA[i] -= m_zOfRadiatorA;
  }

    // Number of short straws A:
  int numberOfLayersWithShortStrawsA =
    m_pParameters->GetInteger("NumberOfLayersWithShortStrawsA");
  int* numberOfShortStrawsInLayersA =
    new int[numberOfLayersWithShortStrawsA];
  m_pParameters->GetPartOfIntegerArray("NumberOfStrawsInLayersA",
    numberOfLayersWithShortStrawsA, numberOfShortStrawsInLayersA);
  m_numberOfShortStrawsA = 0;
  for (i = 0; i < numberOfLayersWithShortStrawsA; ++i)
    m_numberOfShortStrawsA += numberOfShortStrawsInLayersA[i];

  delete [] xOfShellCornersA;
  delete [] yOfShellCornersA;
  delete [] xOfHolesA;
  delete [] yOfHolesA;
  delete [] numberOfShortStrawsInLayersA;

  if (m_pParameters->GetInteger("PrintParametersOfModulesA"))
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
  output << "  baseOfShellTrd1A=" << m_baseOfShellTrd1A << " mm" << std::endl;
  output << "  heightOfShellTrd1A=" << m_heightOfShellTrd1A << " mm" << std::endl;

  output << std::endl << "Parameters of second trapezoid of shell A:"
         << std::endl;
  output << "  baseOfShellTrd2A=" << m_baseOfShellTrd2A << " mm" << std::endl;
  output << "  heightOfShellTrd2A=" << m_heightOfShellTrd2A << " mm" << std::endl;
  output << "  xOfShellTrd2A=" << m_xOfShellTrd2A << " mm" << std::endl;
  output << "  zOfShellTrd2A=" << m_zOfShellTrd2A << " mm" << std::endl;
  output << "  phiOfShellTrd2A=" << m_phiOfShellTrd2A << " rad" << std::endl;

  output << std::endl << "Parameters of shell A:" << std::endl;
  output << "  x0OfShellA=" << m_x0OfShellA << " mm" << std::endl;
  output << "  y0OfShellA=" << m_y0OfShellA << " mm" << std::endl;
  output << "  phi0OfShellA=" << m_phi0OfShellA << " rad" << std::endl;

  output << std::endl << "Parameters of modules A:" << std::endl;
  output << "  numberOfModulesA=" << m_numberOfModulesA << std::endl;
  output << "  lengthOfModuleA=" << m_lengthOfModuleA << " mm" << std::endl;

  output << std::endl << "Parameters of first trapezoid of radiator A:"
         << std::endl;
  output << "  baseOfRadiatorTrd1A=" << m_baseOfRadiatorTrd1A << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd1A=" << m_heightOfRadiatorTrd1A << " mm"
         << std::endl;

  output << std::endl << "Parameters of second trapezoid of radiator A:"
         << std::endl;
  output << "  baseOfRadiatorTrd2A=" << m_baseOfRadiatorTrd2A << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd2A=" << m_heightOfRadiatorTrd2A << " mm"
         << std::endl;
  output << "  xOfRadiatorTrd2A=" << m_xOfRadiatorTrd2A << " mm" << std::endl;
  output << "  zOfRadiatorTrd2A=" << m_zOfRadiatorTrd2A << " mm" << std::endl;
  output << "  phiOfRadiatorTrd2A=" << m_phiOfRadiatorTrd2A << " rad"
         << std::endl;

  output << std::endl << "Parameters of radiator A:" << std::endl;
  output << "  xOfRadiatorA=" << m_xOfRadiatorA << " mm" << std::endl;
  output << "  zOfRadiatorA=" << m_zOfRadiatorA << " mm" << std::endl;

  output << std::endl << "Parameters of straw holes A:" << std::endl;
  output << "  numberOfHolesA=" << m_numberOfHolesA << std::endl;
  int i;
  if (m_pParameters->GetInteger("PrintParametersOfModulesA") == 2)
  {
    output << std::endl;
    for (i = 0; i < m_numberOfHolesA; ++i)
      output << "  xLocalOfHolesA[" << i << "]=" << m_xLocalOfHolesA[i] << " mm"
             << std::endl;

    output << std::endl;
    for (i = 0; i < m_numberOfHolesA; ++i)
      output << "  zLocalOfHolesA[" << i << "]=" << m_zLocalOfHolesA[i] << " mm"
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
  output << "  numberOfCoolingTubesA=" << m_numberOfCoolingTubesA << std::endl
         << std::endl;

  for (i = 0; i < m_numberOfCoolingTubesA; ++i)
    output << "  xOfCoolingTubesA[" << i << "]=" << m_xOfCoolingTubesA[i] << " mm"
           << std::endl;
  output << std::endl;

  for (i = 0; i < m_numberOfCoolingTubesA; ++i)
    output << "  zOfCoolingTubesA[" << i << "]=" << m_zOfCoolingTubesA[i] << " mm"
           << std::endl;

  output << std::endl << "Parameters of straws A:" << std::endl;
  output << "  numberOfShortStrawsA=" << m_numberOfShortStrawsA << std::endl;

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesA::PrintParameters done" << endmsg;
}
