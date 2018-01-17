/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTParametersOfWheelsA.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTUtilities.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>


  // Called by TRTConstructionOfWheelsA::TRTConstructionOfWheelsA

TRTParametersOfWheelsA::TRTParametersOfWheelsA() : m_msg("TRTParametersOfWheelsA")
{
  m_pParameters = TRTParameters::GetPointer();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsA" << endmsg;

  DefineParameters();

  if (m_pParameters->GetInteger("PrintParametersOfWheelsA"))
    PrintParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsA done" << endmsg;
}


  // Called by TRTConstructionOfWheelsA::~TRTConstructionOfWheelsA

TRTParametersOfWheelsA::~TRTParametersOfWheelsA()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfWheelsA" << endmsg;

  delete [] m_positionsOfWheelsA;
  delete [] m_positionsOfStrawPlanesA;
  delete [] m_rotationAnglesOfStrawPlanesA;
  delete [] m_positionsOfMainRadiatorsA;
  delete [] m_positionsOfThinRadiatorsA;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfWheelsA done" << endmsg;
}


  // Called by TRTParametersOfWheelsA

void TRTParametersOfWheelsA::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsA::DefineParameters" << endmsg;

    // Parameters of wheels A:
  m_numberOfWheelsA = m_pParameters->GetInteger("NumberOfWheelsA");
  m_distanceBetweenWheelsA =  m_pParameters->GetDouble("DistanceBetweenWheelsAC");
  m_positionsOfWheelsA = new double[m_numberOfWheelsA];
  m_positionsOfWheelsA[0] = m_pParameters->GetDouble("GlobalPositionOfFirstWheelA");
  int i;
  for (i = 1; i < m_numberOfWheelsA; ++i)
    m_positionsOfWheelsA[i] = m_positionsOfWheelsA[i - 1] + m_distanceBetweenWheelsA;

    // Parameters of wheel A:
  m_innerRadiusOfWheelA = m_pParameters->GetDouble("InnerRadiusOfWheelsAB");
  m_outerRadiusOfWheelA = m_pParameters->GetDouble("OuterRadiusOfWheelsAB");
  m_lengthOfWheelA = m_pParameters->GetDouble("LengthOfWheelsAC");

    // Parameters of inner support A:
  m_innerRadiusOfInnerSupportA = m_innerRadiusOfWheelA;
  m_outerRadiusOfInnerSupportA = m_innerRadiusOfWheelA +
    m_pParameters->GetDouble("ThicknessOfInnerSupportsAB");
  m_lengthOfInnerSupportA = m_lengthOfWheelA;

    // Parameters of outer support A:
  m_innerRadiusOfOuterSupportA = m_outerRadiusOfWheelA -
    m_pParameters->GetDouble("ThicknessOfOuterSupportsAB");
  m_outerRadiusOfOuterSupportA = m_outerRadiusOfWheelA;
  m_lengthOfOuterSupportA = m_lengthOfWheelA;

    // Parameters of straw planes A:
  m_numberOfStrawPlanesA =
    m_pParameters->GetInteger("NumberOfStrawPlanesAC");
  m_numberOfStrawsInPlaneA =
    m_pParameters->GetInteger("NumberOfStrawsInPlanesAB");
  m_positionsOfStrawPlanesA = new double[m_numberOfStrawPlanesA];
  m_rotationAnglesOfStrawPlanesA = new double[m_numberOfStrawPlanesA];
  m_pParameters->GetDoubleArray("PositionsOfStrawPlanesAC", m_numberOfStrawPlanesA,
    m_positionsOfStrawPlanesA);
  m_pParameters->GetDoubleArray("RotationsOfStrawPlanes", m_numberOfStrawPlanesA,
    m_rotationAnglesOfStrawPlanesA);
  double deltaPhiForStraws = 360. / (double) m_numberOfStrawsInPlaneA;
  for (i = 0; i < m_numberOfStrawPlanesA; ++i)
  {
    m_positionsOfStrawPlanesA[i] = m_positionsOfStrawPlanesA[i] -
      m_lengthOfWheelA / 2.;
    m_rotationAnglesOfStrawPlanesA[i] = m_rotationAnglesOfStrawPlanesA[i] *
      deltaPhiForStraws;
  }

    // Parameters of main radiator A:
  m_innerRadiusOfMainRadiatorA = m_outerRadiusOfInnerSupportA;
  m_outerRadiusOfMainRadiatorA = m_innerRadiusOfOuterSupportA -
    m_pParameters->GetDouble("GapBetweenRadiatorAndOuterSupport");
  m_lengthOfMainRadiatorA = m_pParameters->GetDouble("LengthOfMainRadiatorsAC");

    // Parameters of thin radiator A:
  m_innerRadiusOfThinRadiatorA = m_innerRadiusOfMainRadiatorA;
  m_outerRadiusOfThinRadiatorA = m_outerRadiusOfMainRadiatorA;
  m_lengthOfThinRadiatorA = m_pParameters->GetDouble("LengthOfThinRadiatorsAC");

    // Parameters of main and thin radiators A:
  m_numberOfMainRadiatorsA = m_pParameters->GetInteger("NumberOfMainRadiatorsAC");
  m_numberOfThinRadiatorsA = m_pParameters->GetInteger("NumberOfThinRadiatorsAC");
  double outerRadiusOfStraw = m_pParameters->GetDouble("OuterRadiusOfStraw");
  m_positionsOfMainRadiatorsA = new double[m_numberOfMainRadiatorsA];
  m_positionsOfThinRadiatorsA = new double[m_numberOfThinRadiatorsA];
  int j = 0, k = 0;
  for (i = 1; i <= m_numberOfStrawPlanesA; ++i)
  {
    if (i % 4 == 1)
      m_positionsOfThinRadiatorsA[j++] = m_positionsOfStrawPlanesA[i - 1] -
        outerRadiusOfStraw - m_lengthOfThinRadiatorA / 2.;
    if (i % 4 == 0)
    {
      m_positionsOfThinRadiatorsA[j++] = m_positionsOfStrawPlanesA[i - 1] +
        outerRadiusOfStraw + m_lengthOfThinRadiatorA / 2.;
      continue;
    }
    m_positionsOfMainRadiatorsA[k++] = m_positionsOfStrawPlanesA[i - 1] +
      outerRadiusOfStraw + m_lengthOfMainRadiatorA / 2.;
  }

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsA::DefineParameters done" << endmsg;
}


  // Called by TRTParametersOfWheelsA

void TRTParametersOfWheelsA::PrintParameters() const
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsA::PrintParameters" << endmsg;

  TRTUtilities* pUtilities = TRTUtilities::GetPointer();
  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersOfWheelsA::PrintParameters *****" << std::endl;

  output << "Parameters of wheels A:" << std::endl;
  output << "  numberOfWheelsA=" << m_numberOfWheelsA << std::endl;
  output << "  distanceBetweenWheelsA=" << m_distanceBetweenWheelsA << " mm"
         << std::endl << std::endl;
  int i;
  for (i = 0; i < m_numberOfWheelsA; ++i)
    output << "  positionsOfWheelsA[" << i << "]=" << m_positionsOfWheelsA[i]
           << " mm" << std::endl;

  output << std::endl << "Parameters of wheel A:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfWheelA, m_outerRadiusOfWheelA,
    m_lengthOfWheelA);

  output << std::endl << "Parameters of inner support A:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfInnerSupportA,
    m_outerRadiusOfInnerSupportA, m_lengthOfInnerSupportA);

  output << std::endl << "Parameters of outer support A:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfOuterSupportA,
    m_outerRadiusOfOuterSupportA, m_lengthOfOuterSupportA);

  output << std::endl << "Parameters of main radiators A:" << std::endl;
  output << "  numberOfMainRadiatorsA=" << m_numberOfMainRadiatorsA << std::endl
         << std::endl;
  for (i = 0; i < m_numberOfMainRadiatorsA; ++i)
    output << "  positionsOfMainRadiatorsA[" << i << "]="
           << m_positionsOfMainRadiatorsA[i] << " mm" << std::endl;

  output << std::endl << "Parameters of main radiator A:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfMainRadiatorA,
    m_outerRadiusOfMainRadiatorA, m_lengthOfMainRadiatorA);

  output << std::endl << "Parameters of thin radiators A:" << std::endl;
  output << "  numberOfThinRadiatorsA=" << m_numberOfThinRadiatorsA << std::endl
         << std::endl;
  for (i = 0; i < m_numberOfThinRadiatorsA; ++i)
    output << "  positionsOfThinRadiatorsA[" << i << "]="
           << m_positionsOfThinRadiatorsA[i] << " mm" << std::endl;

  output << std::endl << "Parameters of thin radiator A:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfThinRadiatorA,
    m_outerRadiusOfThinRadiatorA, m_lengthOfThinRadiatorA);

  output << std::endl << "Parameters of straw planes A:" << std::endl;
  output << "  numberOfStrawPlanesA=" << m_numberOfStrawPlanesA << std::endl;
  output << "  numberOfStrawsInPlaneA=" << m_numberOfStrawsInPlaneA << std::endl
         << std::endl;
  for (i = 0; i < m_numberOfStrawPlanesA; ++i)
    output << "  positionsOfStrawPlanesA[" << i << "]="
           << m_positionsOfStrawPlanesA[i] << " mm" << std::endl;
  output << std::endl;
  for (i = 0; i < m_numberOfStrawPlanesA; ++i)
    output << "  rotationAnglesOfStrawPlanesA[" << i << "]="
           << m_rotationAnglesOfStrawPlanesA[i] << " deg" << std::endl;

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsA::PrintParameters done" << endmsg;
}
