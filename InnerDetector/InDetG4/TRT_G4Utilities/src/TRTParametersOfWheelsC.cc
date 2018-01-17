/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTParametersOfWheelsC.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTUtilities.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>



  // Called by TRTConstructionOfWheelsC::TRTConstructionOfWheelsC

TRTParametersOfWheelsC::TRTParametersOfWheelsC() : m_msg("TRTParametersOfWheelsC")
{
  m_pParameters = TRTParameters::GetPointer();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsC" << endmsg;

  DefineParameters();

  if (m_pParameters->GetInteger("PrintParametersOfWheelsC"))
    PrintParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsC done" << endmsg;
}


  // Called by TRTConstructionOfWheelsC::~TRTConstructionOfWheelsC

TRTParametersOfWheelsC::~TRTParametersOfWheelsC()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Destructor TRTParametersOfWheelsC" << endmsg;

  delete [] m_positionsOfWheelsC;
  delete [] m_positionsOfStrawPlanesC;
  delete [] m_rotationAnglesOfStrawPlanesC;
  delete [] m_positionsOfMainRadiatorsC;
  delete [] m_positionsOfThinRadiatorsC;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Destructor TRTParametersOfWheelsC done" << endmsg;
}


  // Called by TRTParametersOfWheelsC

void TRTParametersOfWheelsC::DefineParameters()
{

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsC::DefineParameters" << endmsg;

    // Parameters of wheels C:
  m_numberOfWheelsC = m_pParameters->GetInteger("NumberOfWheelsC");
  m_distanceBetweenWheelsC =  m_pParameters->GetDouble("DistanceBetweenWheelsAC");
  m_positionsOfWheelsC = new double[m_numberOfWheelsC];
  m_positionsOfWheelsC[0] = m_pParameters->GetDouble("GlobalPositionOfFirstWheelC");
  int i;
  for (i = 1; i < m_numberOfWheelsC; ++i)
    m_positionsOfWheelsC[i] = m_positionsOfWheelsC[i - 1] + m_distanceBetweenWheelsC;

    // Parameters of wheel C:
  m_innerRadiusOfWheelC = m_pParameters->GetDouble("InnerRadiusOfWheelC");
  m_outerRadiusOfWheelC = m_pParameters->GetDouble("OuterRadiusOfWheelC");
  m_lengthOfWheelC = m_pParameters->GetDouble("LengthOfWheelsAC");

    // Parameters of inner support C:
  m_innerRadiusOfInnerSupportC = m_innerRadiusOfWheelC;
  m_outerRadiusOfInnerSupportC = m_innerRadiusOfWheelC +
    m_pParameters->GetDouble("ThicknessOfInnerSupportC");
  m_lengthOfInnerSupportC = m_lengthOfWheelC;

    // Parameters of outer support C:
  m_innerRadiusOfOuterSupportC = m_outerRadiusOfWheelC -
    m_pParameters->GetDouble("ThicknessOfOuterSupportC");
  m_outerRadiusOfOuterSupportC = m_outerRadiusOfWheelC;
  m_lengthOfOuterSupportC = m_lengthOfWheelC;

    // Parameters of straw planes C:
  m_numberOfStrawPlanesC = m_pParameters->GetInteger("NumberOfStrawPlanesAC");
  m_numberOfStrawsInPlaneC = m_pParameters->GetInteger("NumberOfStrawsInPlaneC");
  m_positionsOfStrawPlanesC = new double[m_numberOfStrawPlanesC];
  m_rotationAnglesOfStrawPlanesC = new double[m_numberOfStrawPlanesC];
  m_pParameters->GetDoubleArray("PositionsOfStrawPlanesAC", m_numberOfStrawPlanesC,
    m_positionsOfStrawPlanesC);
  m_pParameters->GetDoubleArray("RotationsOfStrawPlanes", m_numberOfStrawPlanesC,
    m_rotationAnglesOfStrawPlanesC);
  double deltaPhiForStraws = 360. / (double) m_numberOfStrawsInPlaneC;
  for (i = 0; i < m_numberOfStrawPlanesC; ++i)
  {
    m_positionsOfStrawPlanesC[i] = m_positionsOfStrawPlanesC[i] -
      m_lengthOfWheelC / 2.;
    m_rotationAnglesOfStrawPlanesC[i] = m_rotationAnglesOfStrawPlanesC[i] *
      deltaPhiForStraws;
  }

    // Parameters of main radiator C:
  m_innerRadiusOfMainRadiatorC = m_outerRadiusOfInnerSupportC;
  m_outerRadiusOfMainRadiatorC = m_innerRadiusOfOuterSupportC -
    m_pParameters->GetDouble("GapBetweenRadiatorAndOuterSupport");
  m_lengthOfMainRadiatorC = m_pParameters->GetDouble("LengthOfMainRadiatorsAC");

    // Parameters of thin radiator C:
  m_innerRadiusOfThinRadiatorC = m_innerRadiusOfMainRadiatorC;
  m_outerRadiusOfThinRadiatorC = m_outerRadiusOfMainRadiatorC;
  m_lengthOfThinRadiatorC = m_pParameters->GetDouble("LengthOfThinRadiatorsAC");

    // Parameters of main and thin radiators C:
  m_numberOfMainRadiatorsC = m_pParameters->GetInteger("NumberOfMainRadiatorsAC");
  m_numberOfThinRadiatorsC = m_pParameters->GetInteger("NumberOfThinRadiatorsAC");
  double outerRadiusOfStraw = m_pParameters->GetDouble("OuterRadiusOfStraw");
  m_positionsOfMainRadiatorsC = new double[m_numberOfMainRadiatorsC];
  m_positionsOfThinRadiatorsC = new double[m_numberOfThinRadiatorsC];
  int j = 0, k = 0;
  for (i = 1; i <= m_numberOfStrawPlanesC; ++i)
  {
    if (i % 4 == 1)
      m_positionsOfThinRadiatorsC[j++] = m_positionsOfStrawPlanesC[i - 1] -
        outerRadiusOfStraw - m_lengthOfThinRadiatorC / 2.;
    if (i % 4 == 0)
    {
      m_positionsOfThinRadiatorsC[j++] = m_positionsOfStrawPlanesC[i - 1] +
        outerRadiusOfStraw + m_lengthOfThinRadiatorC / 2.;
      continue;
    }
    m_positionsOfMainRadiatorsC[k++] = m_positionsOfStrawPlanesC[i - 1] +
      outerRadiusOfStraw + m_lengthOfMainRadiatorC / 2.;
  }

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsC::DefineParameters done" << endmsg;
}


  // Called by TRTParametersOfWheelsC

void TRTParametersOfWheelsC::PrintParameters() const
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsC::PrintParameters" << endmsg;

  TRTUtilities* pUtilities = TRTUtilities::GetPointer();
  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersOfWheelsC::PrintParameters *****" << std::endl;

  output << "Parameters of wheels C:" << std::endl;
  output << "  numberOfWheelsC=" << m_numberOfWheelsC << std::endl;
  output << "  distanceBetweenWheelsC=" << m_distanceBetweenWheelsC << " mm"
         << std::endl << std::endl;
  int i;
  for (i = 0; i < m_numberOfWheelsC; ++i)
    output << "  positionsOfWheelsC[" << i << "]=" << m_positionsOfWheelsC[i]
           << " mm" << std::endl;

  output << std::endl << "Parameters of wheel C:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfWheelC, m_outerRadiusOfWheelC,
    m_lengthOfWheelC);

  output << std::endl << "Parameters of inner support C:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfInnerSupportC,
    m_outerRadiusOfInnerSupportC, m_lengthOfInnerSupportC);

  output << std::endl << "Parameters of outer support C:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfOuterSupportC,
    m_outerRadiusOfOuterSupportC, m_lengthOfOuterSupportC);

  output << std::endl << "Parameters of main radiators C:" << std::endl;
  output << "  numberOfMainRadiatorsC=" << m_numberOfMainRadiatorsC << std::endl
         << std::endl;
  for (i = 0; i < m_numberOfMainRadiatorsC; ++i)
    output << "  positionsOfMainRadiatorsC[" << i << "]="
           << m_positionsOfMainRadiatorsC[i] << " mm" << std::endl;

  output << std::endl << "Parameters of main radiator C:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfMainRadiatorC,
    m_outerRadiusOfMainRadiatorC, m_lengthOfMainRadiatorC);

  output << std::endl << "Parameters of thin radiators C:" << std::endl;
  output << "  numberOfThinRadiatorsC=" << m_numberOfThinRadiatorsC << std::endl
         << std::endl;
  for (i = 0; i < m_numberOfThinRadiatorsC; ++i)
    output << "  positionsOfThinRadiatorsC[" << i << "]="
           << m_positionsOfThinRadiatorsC[i] << " mm" << std::endl;

  output << std::endl << "Parameters of thin radiator C:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfThinRadiatorC,
    m_outerRadiusOfThinRadiatorC, m_lengthOfThinRadiatorC);

  output << std::endl << "Parameters of straw planes C:" << std::endl;
  output << "  numberOfStrawPlanesC=" << m_numberOfStrawPlanesC << std::endl;
  output << "  numberOfStrawsInPlaneC=" << m_numberOfStrawsInPlaneC << std::endl
         << std::endl;
  for (i = 0; i < m_numberOfStrawPlanesC; ++i)
    output << "  positionsOfStrawPlanesC[" << i << "]="
           << m_positionsOfStrawPlanesC[i] << " mm" << std::endl;
  output << std::endl;
  for (i = 0; i < m_numberOfStrawPlanesC; ++i)
    output << "  rotationAnglesOfStrawPlanesC[" << i << "]="
           << m_rotationAnglesOfStrawPlanesC[i] << " deg" << std::endl;

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsC::PrintParameters done" << endmsg;
}
