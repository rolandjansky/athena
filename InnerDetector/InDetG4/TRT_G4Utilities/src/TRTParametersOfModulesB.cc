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
  m_pParameters = TRTParameters::GetPointer();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesB" << endmsg;

  DefineParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesB done" << endmsg;
}


  // Called by TRTConstructionOfModulesB::~TRTConstructionOfModulesB

TRTParametersOfModulesB::~TRTParametersOfModulesB()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesB" << endmsg;

  delete [] m_xLocalOfHolesB;
  delete [] m_zLocalOfHolesB;
  delete [] m_xOfCoolingTubesB;
  delete [] m_zOfCoolingTubesB;
  delete [] m_xOfHolesForCoolingTubesB;
  delete [] m_zOfHolesForCoolingTubesB;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesB done" << endmsg;
}


  // Called by TRTParametersOfModulesB

void TRTParametersOfModulesB::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesB::DefineParameters" << endmsg;

    // Distances between corners of shell B:
  int numberOfShellCorners = m_pParameters->GetInteger("NumberOfShellCorners");
  double* xOfShellCornersB = new double[numberOfShellCorners];
  double* yOfShellCornersB = new double[numberOfShellCorners];
  m_pParameters->GetDoubleArray("XOfShellCornersB", numberOfShellCorners,
    xOfShellCornersB);
  m_pParameters->GetDoubleArray("YOfShellCornersB", numberOfShellCorners,
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
  m_baseOfShellTrd1B = distance12;
  m_heightOfShellTrd1B = std::sqrt(side * side - distance12 * distance12 / 4.);

    // Parameters of second trapezoid of shell B:
  m_baseOfShellTrd2B = distance34;
  m_heightOfShellTrd2B = std::sqrt(side * side - distance34 * distance34 / 4.);

  double alpha1 = std::atan(m_heightOfShellTrd1B / (m_baseOfShellTrd1B / 2.));
  double beta2 = std::atan((m_baseOfShellTrd2B / 2.) / m_heightOfShellTrd2B);
  double gamma = alpha1 + beta2;

  m_xOfShellTrd2B = m_baseOfShellTrd1B / 2. - (m_heightOfShellTrd2B / 2.) *
    std::cos(gamma);
  m_zOfShellTrd2B = -m_heightOfShellTrd1B / 2. + (m_heightOfShellTrd2B / 2.) *
    std::sin(gamma);
  m_phiOfShellTrd2B = -(gamma + M_PI / 2.);

    // Parameters of shell B:
  double xOfMiddlePointOfSide12 = (xOfShellCornersB[0] +
    xOfShellCornersB[1]) / 2.;
  double yOfMiddlePointOfSide12 = (yOfShellCornersB[0] +
    yOfShellCornersB[1]) / 2.;
  m_x0OfShellB = (xOfMiddlePointOfSide12 + xOfShellCornersB[2]) / 2.;
  m_y0OfShellB = (yOfMiddlePointOfSide12 + yOfShellCornersB[2]) / 2.;
  double phi0 = std::atan((xOfShellCornersB[0] - xOfShellCornersB[1]) /
    (yOfShellCornersB[1] - yOfShellCornersB[0]));
  m_phi0OfShellB = -M_PI / 2. + phi0;

    // Parameters of modules B:
  m_numberOfModulesB = m_pParameters->GetInteger("NumberOfModulesInRing");
  m_lengthOfModuleB = m_pParameters->GetDouble("LengthOfBarrelVolume");

    // Parameters of radiator B:
  double thicknessOfModuleWalls =
    m_pParameters->GetDouble("ThicknessOfModuleWalls");
  m_baseOfRadiatorTrd1B = m_baseOfShellTrd1B - thicknessOfModuleWalls *
    (2. / std::tan(alpha1) +  1. /  std::sin(alpha1));
  m_heightOfRadiatorTrd1B = m_baseOfRadiatorTrd1B * std::tan(alpha1) / 2.;

  double alpha2 = std::atan(m_heightOfShellTrd2B / (m_baseOfShellTrd2B / 2.));

  m_baseOfRadiatorTrd2B = m_baseOfShellTrd2B - thicknessOfModuleWalls *
    (2. / std::tan(alpha2) +  1. /  std::sin(alpha2));
  m_heightOfRadiatorTrd2B = m_baseOfShellTrd2B * std::tan(alpha2) / 2.;

  double xOfRadiatorTrd2InShellTrd2B = -(m_baseOfRadiatorTrd2B -
    m_baseOfShellTrd2B) / 2. - thicknessOfModuleWalls / std::tan(alpha2);
  double zOfRadiatorTrd2InShellTrd2B = (m_heightOfRadiatorTrd2B -
    m_heightOfShellTrd2B) / 2. + thicknessOfModuleWalls;

  m_xOfRadiatorB = -(m_baseOfRadiatorTrd1B - m_baseOfShellTrd1B) / 2. -
    thicknessOfModuleWalls / std::tan(alpha1);
  m_zOfRadiatorB = (m_heightOfRadiatorTrd1B - m_heightOfShellTrd1B) / 2. +
    thicknessOfModuleWalls;

  double phi = -(3. * M_PI / 2. - gamma);

  m_xOfRadiatorTrd2B = xOfRadiatorTrd2InShellTrd2B * std::cos(phi) +
    zOfRadiatorTrd2InShellTrd2B * std::sin(phi) + m_xOfShellTrd2B - m_xOfRadiatorB;
  m_zOfRadiatorTrd2B = zOfRadiatorTrd2InShellTrd2B * std::cos(phi) -
    xOfRadiatorTrd2InShellTrd2B * std::sin(phi) + m_zOfShellTrd2B - m_zOfRadiatorB;
  m_phiOfRadiatorTrd2B = m_phiOfShellTrd2B;

    // Parameters of holes B:
  m_numberOfHolesB = m_pParameters->GetInteger("NumberOfStrawsB");
  double* xOfHolesB = new double[m_numberOfHolesB];
  double* yOfHolesB = new double[m_numberOfHolesB];
  m_pParameters->GetDoubleArray("XOfHolesB", m_numberOfHolesB, xOfHolesB);
  m_pParameters->GetDoubleArray("YOfHolesB", m_numberOfHolesB, yOfHolesB);
  int i;
  if (xOfHolesB[0] != 0.)
    for (i = 0; i < m_numberOfHolesB; ++i)
      xOfHolesB[i] -= xOfHolesB[0];
  if (yOfHolesB[0] != 0.)
    for (i = 0; i < m_numberOfHolesB; ++i)
      yOfHolesB[i] -= yOfHolesB[0];

  double xGlobalOfHole1B = m_pParameters->GetDouble("XGlobalOfHole1B");
  double yGlobalOfHole1B = m_pParameters->GetDouble("YGlobalOfHole1B");
  double xGlobalOfHole19B = m_pParameters->GetDouble("XGlobalOfHole19B");
  double yGlobalOfHole19B = m_pParameters->GetDouble("YGlobalOfHole19B");

  double startingPhi = CLHEP::Hep3Vector(-xOfHolesB[18],
    yOfHolesB[18], 0.).phi();
  if (startingPhi < 0.)
    startingPhi += M_PI * 2.;
  double globalPhi = CLHEP::Hep3Vector(xGlobalOfHole19B - xGlobalOfHole1B,
    yGlobalOfHole19B - yGlobalOfHole1B, 0.).phi();
  if (globalPhi < 0.)
    globalPhi += M_PI * 2.;
  double deltaPhi = startingPhi - globalPhi;

  double* xGlobalOfHolesB = new double[m_numberOfHolesB];
  double* yGlobalOfHolesB = new double[m_numberOfHolesB];

  double sinDeltaPhi = std::sin(deltaPhi);
  double cosDeltaPhi = std::cos(deltaPhi);

  for (i = 0; i < m_numberOfHolesB; ++i)
  {
    xGlobalOfHolesB[i] = -xOfHolesB[i] * cosDeltaPhi +
      yOfHolesB[i] * sinDeltaPhi + xGlobalOfHole1B;
    yGlobalOfHolesB[i] = yOfHolesB[i] * cosDeltaPhi +
      xOfHolesB[i] * sinDeltaPhi + yGlobalOfHole1B;
  }

  m_xLocalOfHolesB = new double[m_numberOfHolesB];
  m_zLocalOfHolesB = new double[m_numberOfHolesB];

  double sinPhi0 = std::sin(phi0);
  double cosPhi0 = std::cos(phi0);

  for (i = 0; i < m_numberOfHolesB; ++i)
  {
    double temporaryX = -yGlobalOfHolesB[i] + m_y0OfShellB;
    double temporaryZ = xGlobalOfHolesB[i] - m_x0OfShellB;

    m_xLocalOfHolesB[i] = temporaryX * cosPhi0 + temporaryZ * sinPhi0 -
      m_xOfRadiatorB;
    m_zLocalOfHolesB[i] = temporaryZ * cosPhi0 - temporaryX * sinPhi0 -
      m_zOfRadiatorB;
  }

    // Parameters of cooling tubes:
  m_numberOfCoolingTubesB = m_pParameters->GetInteger("NumberOfCoolingTubes");
  double distanceToCoolingTube =
    m_pParameters->GetDouble("DistanceToCoolingTube");

  m_xOfCoolingTubesB = new double[m_numberOfCoolingTubesB];
  m_zOfCoolingTubesB = new double[m_numberOfCoolingTubesB];

  m_xOfCoolingTubesB[0] = -m_baseOfShellTrd1B / 2. + distanceToCoolingTube *
    (1. / std::sin(alpha1) + 1. / std::tan(alpha1));
  m_zOfCoolingTubesB[0] = -m_heightOfShellTrd1B / 2. + distanceToCoolingTube;

  double xOfCoolingTubeInShellTrd2B = -m_baseOfShellTrd2B / 2. +
    distanceToCoolingTube * (1. / std::sin(alpha2) + 1. / std::tan(alpha2));
  double zOfCoolingTubeInShellTrd2B = -m_heightOfShellTrd2B / 2. +
    distanceToCoolingTube;

  m_xOfCoolingTubesB[1] = xOfCoolingTubeInShellTrd2B * std::cos(phi) +
    zOfCoolingTubeInShellTrd2B * std::sin(phi) + m_xOfShellTrd2B;
  m_zOfCoolingTubesB[1] = zOfCoolingTubeInShellTrd2B * std::cos(phi) -
    xOfCoolingTubeInShellTrd2B * std::sin(phi) + m_zOfShellTrd2B;

  if (!m_pParameters->GetInteger("ParameterisedStrawsInModulesBC"))
  {
    for (i = 0; i < m_numberOfCoolingTubesB; ++i)
    {
      m_xOfCoolingTubesB[i] -= m_xOfRadiatorB;
      m_zOfCoolingTubesB[i] -= m_zOfRadiatorB;
    }

    m_xOfHolesForCoolingTubesB = NULL;
    m_zOfHolesForCoolingTubesB = NULL;
  }
  else
  {
    m_radiusOfHoleForCoolingTubeB =
      m_pParameters->GetDouble("OuterRadiusOfCoolingTube");

    m_xOfHolesForCoolingTubesB = new double[m_numberOfCoolingTubesB];
    m_zOfHolesForCoolingTubesB = new double[m_numberOfCoolingTubesB];

    m_xOfHolesForCoolingTubesB[0] = m_xOfCoolingTubesB[0] - m_xOfRadiatorB;
    m_zOfHolesForCoolingTubesB[0] = m_zOfCoolingTubesB[0] - m_zOfRadiatorB;

    m_xOfHolesForCoolingTubesB[1] = m_baseOfRadiatorTrd2B / 2. -
      (distanceToCoolingTube - thicknessOfModuleWalls) * (1. / std::sin(alpha2) +
      1. / std::tan(alpha2));
    m_zOfHolesForCoolingTubesB[1] = -m_heightOfRadiatorTrd2B / 2. +
      distanceToCoolingTube - thicknessOfModuleWalls;
  }

  delete [] xOfShellCornersB;
  delete [] yOfShellCornersB;
  delete [] xOfHolesB;
  delete [] yOfHolesB;

  if (m_pParameters->GetInteger("PrintParametersOfModulesB"))
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
  output << "  baseOfShellTrd1B=" << m_baseOfShellTrd1B << " mm" << std::endl;
  output << "  heightOfShellTrd1B=" << m_heightOfShellTrd1B << " mm" << std::endl;

  output << std::endl << "Parameters of second trapezoid of shell B:"
         << std::endl;
  output << "  baseOfShellTrd2B=" << m_baseOfShellTrd2B << " mm" << std::endl;
  output << "  heightOfShellTrd2B=" << m_heightOfShellTrd2B << " mm" << std::endl;
  output << "  xOfShellTrd2B=" << m_xOfShellTrd2B << " mm" << std::endl;
  output << "  zOfShellTrd2B=" << m_zOfShellTrd2B << " mm" << std::endl;
  output << "  phiOfShellTrd2B=" << m_phiOfShellTrd2B << " rad" << std::endl;

  output << std::endl << "Parameters of shell B:" << std::endl;
  output << "  x0OfShellB=" << m_x0OfShellB << " mm" << std::endl;
  output << "  y0OfShellB=" << m_y0OfShellB << " mm" << std::endl;
  output << "  phi0OfShellB=" << m_phi0OfShellB << " rad" << std::endl;

  output << std::endl << "Parameters of modules B:" << std::endl;
  output << "  numberOfModulesB=" << m_numberOfModulesB << std::endl;
  output << "  lengthOfModuleB=" << m_lengthOfModuleB << " mm" << std::endl;

  output << std::endl << "Parameters of first trapezoid of radiator B:"
         << std::endl;
  output << "  baseOfRadiatorTrd1B=" << m_baseOfRadiatorTrd1B << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd1B=" << m_heightOfRadiatorTrd1B << " mm"
         << std::endl;

  output << std::endl << "Parameters of second trapezoid of radiator B:"
         << std::endl;
  output << "  baseOfRadiatorTrd2B=" << m_baseOfRadiatorTrd2B << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd2B=" << m_heightOfRadiatorTrd2B << " mm"
         << std::endl;
  output << "  xOfRadiatorTrd2B=" << m_xOfRadiatorTrd2B << " mm" << std::endl;
  output << "  zOfRadiatorTrd2B=" << m_zOfRadiatorTrd2B << " mm" << std::endl;
  output << "  phiOfRadiatorTrd2B=" << m_phiOfRadiatorTrd2B << " rad"
         << std::endl;

  output << std::endl << "Parameters of radiator B:" << std::endl;
  output << "  xOfRadiatorB=" << m_xOfRadiatorB << " mm" << std::endl;
  output << "  zOfRadiatorB=" << m_zOfRadiatorB << " mm" << std::endl;

  output << std::endl << "Parameters of straw holes B:" << std::endl;
  output << "  numberOfHolesB=" << m_numberOfHolesB << std::endl;
  int i;
  if (m_pParameters->GetInteger("PrintParametersOfModulesB") == 2)
  {
    output << std::endl;
    for (i = 0; i < m_numberOfHolesB; ++i)
      output << "  xLocalOfHolesB[" << i << "]=" << m_xLocalOfHolesB[i] << " mm"
             << std::endl;

    output << std::endl;
    for (i = 0; i < m_numberOfHolesB; ++i)
      output << "  zLocalOfHolesB[" << i << "]=" << m_zLocalOfHolesB[i] << " mm"
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
  output << "  numberOfCoolingTubesB=" << m_numberOfCoolingTubesB << std::endl
         << std::endl;
  for (i = 0; i < m_numberOfCoolingTubesB; ++i)
    output << "  xOfCoolingTubesB[" << i << "]=" << m_xOfCoolingTubesB[i] << " mm"
           << std::endl;
  output << std::endl;

  for (i = 0; i < m_numberOfCoolingTubesB; ++i)
    output << "  zOfCoolingTubesB[" << i << "]=" << m_zOfCoolingTubesB[i] << " mm"
           << std::endl;

  if (m_pParameters->GetInteger("ParameterisedStrawsInModulesBC"))
  {
    output << std::endl << "Parameters of holes in radiator for cooling tubes"
           << " B:" << std::endl;
    output << "  radiusOfHoleForCoolingTubeB=" << m_radiusOfHoleForCoolingTubeB
           << " mm" << std::endl << std::endl;
    for (i = 0; i < m_numberOfCoolingTubesB; ++i)
      output << "  xOfHolesForCoolingTubesB[" << i << "]="
             << m_xOfHolesForCoolingTubesB[i] << " mm" << std::endl;
    output << std::endl;

    for (i = 0; i < m_numberOfCoolingTubesB; ++i)
      output << "  zOfHolesForCoolingTubesB[" << i << "]="
             << m_zOfHolesForCoolingTubesB[i] << " mm" << std::endl;
  }

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesB::PrintParameters done" << endmsg;
}
