/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTParametersOfWheelsB.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTUtilities.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>


  // Called by TRTConstructionOfWheelsB::TRTConstructionOfWheelsB

TRTParametersOfWheelsB::TRTParametersOfWheelsB() : m_msg("TRTParametersOfWheelsB")
{
  m_pParameters = TRTParameters::GetPointer();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsB" << endmsg;

  DefineParameters();

  if (m_pParameters->GetInteger("PrintParametersOfWheelsB"))
    PrintParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsB done" << endmsg;
}


  // Called by TRTConstructionOfWheelsB::~TRTConstructionOfWheelsB

TRTParametersOfWheelsB::~TRTParametersOfWheelsB()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfWheelsB" << endmsg;

  delete [] m_positionsOfWheelsB;
  delete [] m_positionsOfStrawPlanesB;
  delete [] m_rotationAnglesOfStrawPlanesB;
  delete [] m_positionsOfMainRadiatorsB;
  delete [] m_positionsOfThinRadiatorsB;
  delete [] m_positionsOfMiddleRadiatorsB;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfWheelsB done" << endmsg;
}


  // Called by TRTParametersOfWheelsB

void TRTParametersOfWheelsB::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsB::DefineParameters" << endmsg;

    // Parameters of wheels B:
  m_numberOfWheelsB = m_pParameters->GetInteger("NumberOfWheelsB");
  m_distanceBetweenWheelsB =  m_pParameters->GetDouble("DistanceBetweenWheelsB");
  m_positionsOfWheelsB = new double[m_numberOfWheelsB];
  m_positionsOfWheelsB[0] = m_pParameters->GetDouble("GlobalPositionOfFirstWheelB");
  int i;
  for (i = 1; i < m_numberOfWheelsB; ++i)
    m_positionsOfWheelsB[i] = m_positionsOfWheelsB[i - 1] + m_distanceBetweenWheelsB;

    // Parameters of wheel B:
  m_innerRadiusOfWheelB = m_pParameters->GetDouble("InnerRadiusOfWheelsAB");
  m_outerRadiusOfWheelB = m_pParameters->GetDouble("OuterRadiusOfWheelsAB");
  m_lengthOfWheelB = m_pParameters->GetDouble("LengthOfWheelB");

    // Parameters of inner support B:
  m_innerRadiusOfInnerSupportB = m_innerRadiusOfWheelB;
  m_outerRadiusOfInnerSupportB = m_innerRadiusOfWheelB +
    m_pParameters->GetDouble("ThicknessOfInnerSupportsAB");
  m_lengthOfInnerSupportB = m_lengthOfWheelB;

    // Parameters of outer support B:
  m_innerRadiusOfOuterSupportB = m_outerRadiusOfWheelB -
    m_pParameters->GetDouble("ThicknessOfOuterSupportsAB");
  m_outerRadiusOfOuterSupportB = m_outerRadiusOfWheelB;
  m_lengthOfOuterSupportB = m_lengthOfWheelB;

    // Parameters of straw planes B:
  m_numberOfStrawPlanesB = m_pParameters->GetInteger("NumberOfStrawPlanesB");
  m_numberOfStrawsInPlaneB = m_pParameters->GetInteger("NumberOfStrawsInPlanesAB");
  m_positionsOfStrawPlanesB = new double[m_numberOfStrawPlanesB];
  m_rotationAnglesOfStrawPlanesB = new double[m_numberOfStrawPlanesB];
  m_pParameters->GetDoubleArray("PositionsOfStrawPlanesB", m_numberOfStrawPlanesB,
    m_positionsOfStrawPlanesB);
  m_pParameters->GetPartOfDoubleArray("RotationsOfStrawPlanes",
    m_numberOfStrawPlanesB, m_rotationAnglesOfStrawPlanesB);
  double deltaPhiForStraws = 360. / (double) m_numberOfStrawsInPlaneB;
  for (i = 0; i < m_numberOfStrawPlanesB; ++i)
  {
    m_positionsOfStrawPlanesB[i] = m_positionsOfStrawPlanesB[i] -
      m_lengthOfWheelB / 2.;
    m_rotationAnglesOfStrawPlanesB[i] = m_rotationAnglesOfStrawPlanesB[i] *
      deltaPhiForStraws;
  }

    // Parameters of main radiator B:
  m_innerRadiusOfMainRadiatorB = m_outerRadiusOfInnerSupportB;
  m_outerRadiusOfMainRadiatorB = m_innerRadiusOfOuterSupportB -
    m_pParameters->GetDouble("GapBetweenRadiatorAndOuterSupport");
  m_lengthOfMainRadiatorB = m_pParameters->GetDouble("LengthOfMainRadiatorB");

    // Parameters of main radiators B:
  m_numberOfMainRadiatorsB = m_pParameters->GetInteger("NumberOfMainRadiatorsB");
  m_positionsOfMainRadiatorsB = new double[m_numberOfMainRadiatorsB];
  double outerRadiusOfStraw = m_pParameters->GetDouble("OuterRadiusOfStraw");
  int j = 0;
  for (i = 1; i <= m_numberOfStrawPlanesB; ++i)
  {
    if (i % 4 == 0)
      continue;
    m_positionsOfMainRadiatorsB[j++] = m_positionsOfStrawPlanesB[i - 1] +
      outerRadiusOfStraw + m_lengthOfMainRadiatorB / 2.;
  }

    // Parameters of thin radiator B:
  m_innerRadiusOfThinRadiatorB = m_innerRadiusOfMainRadiatorB;
  m_outerRadiusOfThinRadiatorB = m_outerRadiusOfMainRadiatorB;
  m_lengthOfThinRadiatorB = m_pParameters->GetDouble("LengthOfThinRadiatorB");

    // Parameters of thin radiators B:
  m_numberOfThinRadiatorsB = m_pParameters->GetInteger("NumberOfThinRadiatorsB");
  m_positionsOfThinRadiatorsB = new double[m_numberOfThinRadiatorsB];
  m_positionsOfThinRadiatorsB[0] = m_positionsOfStrawPlanesB[0] -
    outerRadiusOfStraw - m_lengthOfThinRadiatorB / 2.;
  m_positionsOfThinRadiatorsB[1] = m_positionsOfStrawPlanesB[7] +
    outerRadiusOfStraw + m_lengthOfThinRadiatorB / 2.;

    // Parameters of middle radiator B:
  m_innerRadiusOfMiddleRadiatorB = m_innerRadiusOfMainRadiatorB;
  m_outerRadiusOfMiddleRadiatorB = m_outerRadiusOfMainRadiatorB;
  m_lengthOfMiddleRadiatorB = m_pParameters->GetDouble("LengthOfMiddleRadiatorB");

    // Parameters of middle radiators B:
  m_numberOfMiddleRadiatorsB =
    m_pParameters->GetInteger("NumberOfMiddleRadiatorsB");
  m_positionsOfMiddleRadiatorsB = new double[m_numberOfMiddleRadiatorsB];
  m_positionsOfMiddleRadiatorsB[0] = m_positionsOfStrawPlanesB[3] +
    outerRadiusOfStraw + m_lengthOfMiddleRadiatorB / 2.;
  m_positionsOfMiddleRadiatorsB[1] = m_positionsOfStrawPlanesB[4] -
    outerRadiusOfStraw - m_lengthOfMiddleRadiatorB / 2.;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsB::DefineParameters done" << endmsg;
}


  // Called by TRTParametersOfWheelsB

void TRTParametersOfWheelsB::PrintParameters() const
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsB::PrintParameters" << endmsg;

  TRTUtilities* pUtilities = TRTUtilities::GetPointer();
  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersOfWheelsB::PrintParameters *****" << std::endl;

  output << "Parameters of wheels B:" << std::endl;
  output << "  numberOfWheelsB=" << m_numberOfWheelsB << std::endl;
  output << "  distanceBetweenWheelsB=" << m_distanceBetweenWheelsB << " mm"
         << std::endl << std::endl;
  int i;
  for (i = 0; i < m_numberOfWheelsB; ++i)
    output << "  positionsOfWheelsB[" << i << "]=" << m_positionsOfWheelsB[i]
           << " mm" << std::endl;

  output << std::endl << "Parameters of wheel B:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfWheelB, m_outerRadiusOfWheelB,
    m_lengthOfWheelB);

  output << std::endl << "Parameters of inner support B:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfInnerSupportB,
    m_outerRadiusOfInnerSupportB, m_lengthOfInnerSupportB);

  output << std::endl << "Parameters of outer support B:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfOuterSupportB,
    m_outerRadiusOfOuterSupportB, m_lengthOfOuterSupportB);

  output << std::endl << "Parameters of main radiators B:" << std::endl;
  output << "  numberOfMainRadiatorsB=" << m_numberOfMainRadiatorsB << std::endl
         << std::endl;
  for (i = 0; i < m_numberOfMainRadiatorsB; ++i)
    output << "  positionsOfMainRadiatorsB[" << i << "]="
           << m_positionsOfMainRadiatorsB[i] << " mm" << std::endl;

  output << std::endl << "Parameters of main radiator B:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfMainRadiatorB,
    m_outerRadiusOfMainRadiatorB, m_lengthOfMainRadiatorB);

  output << std::endl << "Parameters of thin radiators B:" << std::endl;
  output << "  numberOfThinRadiatorsB=" << m_numberOfThinRadiatorsB << std::endl
         << std::endl;
  for (i = 0; i < m_numberOfThinRadiatorsB; ++i)
    output << "  positionsOfThinRadiatorsB[" << i << "]="
           << m_positionsOfThinRadiatorsB[i] << " mm" << std::endl;

  output << std::endl << "Parameters of thin radiator B:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfThinRadiatorB,
    m_outerRadiusOfThinRadiatorB, m_lengthOfThinRadiatorB);

  output << std::endl << "Parameters of middle radiators B:" << std::endl;
  output << "  numberOfMiddleRadiatorsB=" << m_numberOfMiddleRadiatorsB
         << std::endl << std::endl;
  for (i = 0; i < m_numberOfMiddleRadiatorsB; ++i)
    output << "  positionsOfMiddleRadiatorsB[" << i << "]="
           << m_positionsOfMiddleRadiatorsB[i] << " mm" << std::endl;

  output << std::endl << "Parameters of middle radiator B:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfMiddleRadiatorB,
    m_outerRadiusOfMiddleRadiatorB, m_lengthOfMiddleRadiatorB);

  output << std::endl << "Parameters of straw planes B:" << std::endl;
  output << "  numberOfStrawPlanesB=" << m_numberOfStrawPlanesB << std::endl;
  output << "  numberOfStrawsInPlaneB=" << m_numberOfStrawsInPlaneB << std::endl
         << std::endl;
  for (i = 0; i < m_numberOfStrawPlanesB; ++i)
    output << "  positionsOfStrawPlanesB[" << i << "]="
           << m_positionsOfStrawPlanesB[i] << " mm" << std::endl;
  output << std::endl;
  for (i = 0; i < m_numberOfStrawPlanesB; ++i)
    output << "  rotationAnglesOfStrawPlanesB[" << i << "]="
           << m_rotationAnglesOfStrawPlanesB[i] << " deg" << std::endl;

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsB::PrintParameters done" << endmsg;
}
