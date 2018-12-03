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
  m_pParameters = TRTParameters::GetPointer();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesC" << endreq;

  DefineParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfModulesC done" << endreq;
}


  // Called by TRTConstructionOfModulesC::~TRTConstructionOfModulesC

TRTParametersOfModulesC::~TRTParametersOfModulesC()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesC" << endreq;

  delete [] m_xLocalOfHolesC;
  delete [] m_zLocalOfHolesC;
  delete [] m_xOfCoolingTubesC;
  delete [] m_zOfCoolingTubesC;
  delete [] m_xOfHolesForCoolingTubesC;
  delete [] m_zOfHolesForCoolingTubesC;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfModulesC done" << endreq;
}


  // Called by TRTParametersOfModulesC

void TRTParametersOfModulesC::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesC::DefineParameters" << endreq;

    // Distances between corners of shell C:
  int numberOfShellCorners = m_pParameters->GetInteger("NumberOfShellCorners");
  double* xOfShellCornersC = new double[numberOfShellCorners];
  double* yOfShellCornersC = new double[numberOfShellCorners];
  m_pParameters->GetDoubleArray("XOfShellCornersC", numberOfShellCorners,
    xOfShellCornersC);
  m_pParameters->GetDoubleArray("YOfShellCornersC", numberOfShellCorners,
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
  m_baseOfShellTrd1C = distance12;
  m_heightOfShellTrd1C = std::sqrt(side * side - distance12 * distance12 / 4.);

    // Parameters of second trapezoid of shell C:
  m_baseOfShellTrd2C = distance34;
  m_heightOfShellTrd2C = std::sqrt(side * side - distance34 * distance34 / 4.);

  double alpha1 = std::atan(m_heightOfShellTrd1C / (m_baseOfShellTrd1C / 2.));
  double beta2 = std::atan((m_baseOfShellTrd2C / 2.) / m_heightOfShellTrd2C);
  double gamma = alpha1 + beta2;

  m_xOfShellTrd2C = -m_baseOfShellTrd1C / 2. + (m_heightOfShellTrd2C / 2.) *
    std::cos(gamma);
  m_zOfShellTrd2C = -m_heightOfShellTrd1C / 2. + (m_heightOfShellTrd2C / 2.) *
    std::sin(gamma);
  m_phiOfShellTrd2C = gamma + M_PI / 2.;

    // Parameters of shell C:
  double xOfMiddlePointOfSide12 = (xOfShellCornersC[0] +
    xOfShellCornersC[1]) / 2.;
  double yOfMiddlePointOfSide12 = (yOfShellCornersC[0] +
    yOfShellCornersC[1]) / 2.;
  m_x0OfShellC = (xOfMiddlePointOfSide12 + xOfShellCornersC[3]) / 2.;
  m_y0OfShellC = (yOfMiddlePointOfSide12 + yOfShellCornersC[3]) / 2.;
  double phi0 = std::atan((xOfShellCornersC[0] - xOfShellCornersC[1]) /
    (yOfShellCornersC[1] - yOfShellCornersC[0]));
  m_phi0OfShellC = -M_PI / 2. + phi0;

    // Parameters of modules C:
  m_numberOfModulesC = m_pParameters->GetInteger("NumberOfModulesInRing");
  m_lengthOfModuleC = m_pParameters->GetDouble("LengthOfBarrelVolume");

    // Parameters of radiator C:
  double thicknessOfModuleWalls =
    m_pParameters->GetDouble("ThicknessOfModuleWalls");
  m_baseOfRadiatorTrd1C = m_baseOfShellTrd1C - thicknessOfModuleWalls *
    (2. / std::tan(alpha1) +  1. /  std::sin(alpha1));
  m_heightOfRadiatorTrd1C = m_baseOfRadiatorTrd1C * std::tan(alpha1) / 2.;

  double alpha2 = std::atan(m_heightOfShellTrd2C / (m_baseOfShellTrd2C / 2.));

  m_baseOfRadiatorTrd2C = m_baseOfShellTrd2C - thicknessOfModuleWalls *
    (2. / std::tan(alpha2) +  1. /  std::sin(alpha2));
  m_heightOfRadiatorTrd2C = m_baseOfShellTrd2C * std::tan(alpha2) / 2.;

  double xOfRadiatorTrd2InShellTrd2C = (m_baseOfRadiatorTrd2C -
    m_baseOfShellTrd2C) / 2. + thicknessOfModuleWalls / std::tan(alpha2);
  double zOfRadiatorTrd2InShellTrd2C = (m_heightOfRadiatorTrd2C -
    m_heightOfShellTrd2C) / 2. + thicknessOfModuleWalls;

  m_xOfRadiatorC = (m_baseOfRadiatorTrd1C - m_baseOfShellTrd1C) / 2. +
    thicknessOfModuleWalls / std::tan(alpha1);
  m_zOfRadiatorC = (m_heightOfRadiatorTrd1C - m_heightOfShellTrd1C) / 2. +
    thicknessOfModuleWalls;

  double phi = 3. * M_PI / 2. - gamma;

  m_xOfRadiatorTrd2C = xOfRadiatorTrd2InShellTrd2C * std::cos(phi) +
    zOfRadiatorTrd2InShellTrd2C * std::sin(phi) + m_xOfShellTrd2C - m_xOfRadiatorC;
  m_zOfRadiatorTrd2C = zOfRadiatorTrd2InShellTrd2C * std::cos(phi) -
    xOfRadiatorTrd2InShellTrd2C * std::sin(phi) + m_zOfShellTrd2C - m_zOfRadiatorC;
  m_phiOfRadiatorTrd2C = m_phiOfShellTrd2C;

    // Parameters of holes C:
  m_numberOfHolesC = m_pParameters->GetInteger("NumberOfStrawsC");
  double* xOfHolesC = new double[m_numberOfHolesC];
  double* yOfHolesC = new double[m_numberOfHolesC];
  m_pParameters->GetDoubleArray("XOfHolesC", m_numberOfHolesC, xOfHolesC);
  m_pParameters->GetDoubleArray("YOfHolesC", m_numberOfHolesC, yOfHolesC);
  int i;
  if (xOfHolesC[0] != 0.)
    for (i = 0; i < m_numberOfHolesC; ++i)
      xOfHolesC[i] -= xOfHolesC[0];
  if (yOfHolesC[0] != 0.)
    for (i = 0; i < m_numberOfHolesC; ++i)
      yOfHolesC[i] -= yOfHolesC[0];

  double xGlobalOfHole1C = m_pParameters->GetDouble("XGlobalOfHole1C");
  double yGlobalOfHole1C = m_pParameters->GetDouble("YGlobalOfHole1C");
  double xGlobalOfHole23C = m_pParameters->GetDouble("XGlobalOfHole23C");
  double yGlobalOfHole23C = m_pParameters->GetDouble("YGlobalOfHole23C");

  double startingPhi = CLHEP::Hep3Vector(-xOfHolesC[22],
    yOfHolesC[22], 0.).phi();
  if (startingPhi < 0.)
    startingPhi += M_PI * 2.;
  double globalPhi = CLHEP::Hep3Vector(xGlobalOfHole23C - xGlobalOfHole1C,
    yGlobalOfHole23C - yGlobalOfHole1C, 0.).phi();
  if (globalPhi < 0.)
    globalPhi += M_PI * 2.;
  double deltaPhi = startingPhi - globalPhi;

  double* xGlobalOfHolesC = new double[m_numberOfHolesC];
  double* yGlobalOfHolesC = new double[m_numberOfHolesC];

  double sinDeltaPhi = std::sin(deltaPhi);
  double cosDeltaPhi = std::cos(deltaPhi);

  for (i = 0; i < m_numberOfHolesC; ++i)
  {
    xGlobalOfHolesC[i] = -xOfHolesC[i] * cosDeltaPhi +
      yOfHolesC[i] * sinDeltaPhi + xGlobalOfHole1C;
    yGlobalOfHolesC[i] = yOfHolesC[i] * cosDeltaPhi +
      xOfHolesC[i] * sinDeltaPhi + yGlobalOfHole1C;
  }

  m_xLocalOfHolesC = new double[m_numberOfHolesC];
  m_zLocalOfHolesC = new double[m_numberOfHolesC];

  double sinPhi0 = std::sin(phi0);
  double cosPhi0 = std::cos(phi0);

  for (i = 0; i < m_numberOfHolesC; ++i)
  {
    double temporaryX = -yGlobalOfHolesC[i] + m_y0OfShellC;
    double temporaryZ = xGlobalOfHolesC[i] - m_x0OfShellC;

    m_xLocalOfHolesC[i] = temporaryX * cosPhi0 + temporaryZ * sinPhi0 -
      m_xOfRadiatorC;
    m_zLocalOfHolesC[i] = temporaryZ * cosPhi0 - temporaryX * sinPhi0 -
      m_zOfRadiatorC;
  }

    // Parameters of cooling tubes:
  m_numberOfCoolingTubesC = m_pParameters->GetInteger("NumberOfCoolingTubes");
  double distanceToCoolingTube =
    m_pParameters->GetDouble("DistanceToCoolingTube");

  m_xOfCoolingTubesC = new double[m_numberOfCoolingTubesC];
  m_zOfCoolingTubesC = new double[m_numberOfCoolingTubesC];

  m_xOfCoolingTubesC[0] = m_baseOfShellTrd1C / 2. - distanceToCoolingTube *
    (1. / std::sin(alpha1) + 1. / std::tan(alpha1));
  m_zOfCoolingTubesC[0] = -m_heightOfShellTrd1C / 2. + distanceToCoolingTube;

  double xOfCoolingTubeInShellTrd2C = m_baseOfShellTrd2C / 2. -
    distanceToCoolingTube * (1. / std::sin(alpha2) + 1. / std::tan(alpha2));
  double zOfCoolingTubeInShellTrd2C = -m_heightOfShellTrd2C / 2. +
    distanceToCoolingTube;

  m_xOfCoolingTubesC[1] = xOfCoolingTubeInShellTrd2C * std::cos(phi) +
    zOfCoolingTubeInShellTrd2C * std::sin(phi) + m_xOfShellTrd2C;
  m_zOfCoolingTubesC[1] = zOfCoolingTubeInShellTrd2C * std::cos(phi) -
    xOfCoolingTubeInShellTrd2C * std::sin(phi) + m_zOfShellTrd2C;

  if (!m_pParameters->GetInteger("ParameterisedStrawsInModulesBC"))
  {
    for (i = 0; i < m_numberOfCoolingTubesC; ++i)
    {
      m_xOfCoolingTubesC[i] -= m_xOfRadiatorC;
      m_zOfCoolingTubesC[i] -= m_zOfRadiatorC;
    }

    m_xOfHolesForCoolingTubesC = NULL;
    m_zOfHolesForCoolingTubesC = NULL;
  }
  else
  {
    m_radiusOfHoleForCoolingTubeC =
      m_pParameters->GetDouble("OuterRadiusOfCoolingTube");

    m_xOfHolesForCoolingTubesC = new double[m_numberOfCoolingTubesC];
    m_zOfHolesForCoolingTubesC = new double[m_numberOfCoolingTubesC];

    m_xOfHolesForCoolingTubesC[0] = m_xOfCoolingTubesC[0] - m_xOfRadiatorC;
    m_zOfHolesForCoolingTubesC[0] = m_zOfCoolingTubesC[0] - m_zOfRadiatorC;

    m_xOfHolesForCoolingTubesC[1] = m_baseOfRadiatorTrd2C / 2. -
      (distanceToCoolingTube - thicknessOfModuleWalls) * (1. / std::sin(alpha2) +
      1. / std::tan(alpha2));
    m_zOfHolesForCoolingTubesC[1] = -m_heightOfRadiatorTrd2C / 2. +
      distanceToCoolingTube - thicknessOfModuleWalls;
  }

  delete [] xOfShellCornersC;
  delete [] yOfShellCornersC;
  delete [] xOfHolesC;
  delete [] yOfHolesC;

  if (m_pParameters->GetInteger("PrintParametersOfModulesC"))
    PrintParameters(xGlobalOfHolesC, yGlobalOfHolesC);

  delete [] xGlobalOfHolesC;
  delete [] yGlobalOfHolesC;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesC::DefineParameters done" << endreq;
}


  // Called by DefineParameters

void TRTParametersOfModulesC::PrintParameters(double* xGlobalOfHolesC,
  double* yGlobalOfHolesC) const
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesC::PrintParameters" << endreq;

  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersOfModulesC::PrintParameters *****" << std::endl;

  output << "Parameters of first trapezoid of shell C:" << std::endl;
  output << "  baseOfShellTrd1C=" << m_baseOfShellTrd1C << " mm" << std::endl;
  output << "  heightOfShellTrd1C=" << m_heightOfShellTrd1C << " mm" << std::endl;

  output << std::endl << "Parameters of second trapezoid of shell C:"
         << std::endl;
  output << "  baseOfShellTrd2C=" << m_baseOfShellTrd2C << " mm" << std::endl;
  output << "  heightOfShellTrd2C=" << m_heightOfShellTrd2C << " mm" << std::endl;
  output << "  xOfShellTrd2C=" << m_xOfShellTrd2C << " mm" << std::endl;
  output << "  zOfShellTrd2C=" << m_zOfShellTrd2C << " mm" << std::endl;
  output << "  phiOfShellTrd2C=" << m_phiOfShellTrd2C << " rad" << std::endl;

  output << std::endl << "Parameters of shell C:" << std::endl;
  output << "  x0OfShellC=" << m_x0OfShellC << " mm" << std::endl;
  output << "  y0OfShellC=" << m_y0OfShellC << " mm" << std::endl;
  output << "  phi0OfShellC=" << m_phi0OfShellC << " rad" << std::endl;

  output << std::endl << "Parameters of modules C:" << std::endl;
  output << "  numberOfModulesC=" << m_numberOfModulesC << std::endl;
  output << "  lengthOfModuleC=" << m_lengthOfModuleC << " mm" << std::endl;

  output << std::endl << "Parameters of first trapezoid of radiator C:"
         << std::endl;
  output << "  baseOfRadiatorTrd1C=" << m_baseOfRadiatorTrd1C << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd1C=" << m_heightOfRadiatorTrd1C << " mm"
         << std::endl;

  output << std::endl << "Parameters of second trapezoid of radiator C:"
         << std::endl;
  output << "  baseOfRadiatorTrd2C=" << m_baseOfRadiatorTrd2C << " mm"
         << std::endl;
  output << "  heightOfRadiatorTrd2C=" << m_heightOfRadiatorTrd2C << " mm"
         << std::endl;
  output << "  xOfRadiatorTrd2C=" << m_xOfRadiatorTrd2C << " mm" << std::endl;
  output << "  zOfRadiatorTrd2C=" << m_zOfRadiatorTrd2C << " mm" << std::endl;
  output << "  phiOfRadiatorTrd2C=" << m_phiOfRadiatorTrd2C << " rad"
         << std::endl;

  output << std::endl << "Parameters of radiator C:" << std::endl;
  output << "  xOfRadiatorC=" << m_xOfRadiatorC << " mm" << std::endl;
  output << "  zOfRadiatorC=" << m_zOfRadiatorC << " mm" << std::endl;

  output << std::endl << "Parameters of straw holes C:" << std::endl;
  output << "  numberOfHolesC=" << m_numberOfHolesC << std::endl;
  int i;
  if (m_pParameters->GetInteger("PrintParametersOfModulesC") == 2)
  {
    output << std::endl;
    for (i = 0; i < m_numberOfHolesC; ++i)
      output << "  xLocalOfHolesC[" << i << "]=" << m_xLocalOfHolesC[i] << " mm"
             << std::endl;

    output << std::endl;
    for (i = 0; i < m_numberOfHolesC; ++i)
      output << "  zLocalOfHolesC[" << i << "]=" << m_zLocalOfHolesC[i] << " mm"
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
  output << "  numberOfCoolingTubesC=" << m_numberOfCoolingTubesC << std::endl
         << std::endl;

  for (i = 0; i < m_numberOfCoolingTubesC; ++i)
    output << "  xOfCoolingTubesC[" << i << "]=" << m_xOfCoolingTubesC[i] << " mm"
           << std::endl;
  output << std::endl;

  for (i = 0; i < m_numberOfCoolingTubesC; ++i)
    output << "  zOfCoolingTubesC[" << i << "]=" << m_zOfCoolingTubesC[i] << " mm"
           << std::endl;

  if (m_pParameters->GetInteger("ParameterisedStrawsInModulesBC"))
  {
    output << std::endl << "Parameters of holes in radiator for cooling tubes"
           << " C:" << std::endl;
    output << "  radiusOfHoleForCoolingTubeC=" << m_radiusOfHoleForCoolingTubeC
           << " mm" << std::endl << std::endl;
    for (i = 0; i < m_numberOfCoolingTubesC; ++i)
      output << "  xOfHolesForCoolingTubesC[" << i << "]="
             << m_xOfHolesForCoolingTubesC[i] << " mm" << std::endl;
    output << std::endl;

    for (i = 0; i < m_numberOfCoolingTubesC; ++i)
      output << "  zOfHolesForCoolingTubesC[" << i << "]="
             << m_zOfHolesForCoolingTubesC[i] << " mm" << std::endl;
  }

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfModulesC::PrintParameters done" << endreq;
}
