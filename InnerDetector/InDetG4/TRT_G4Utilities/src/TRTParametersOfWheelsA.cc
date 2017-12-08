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
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsA" << endmsg;

  DefineParameters();

  if (pParameters->GetInteger("PrintParametersOfWheelsA"))
    PrintParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsA done" << endmsg;
}


  // Called by TRTConstructionOfWheelsA::~TRTConstructionOfWheelsA

TRTParametersOfWheelsA::~TRTParametersOfWheelsA()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfWheelsA" << endmsg;

  delete [] positionsOfWheelsA;
  delete [] positionsOfStrawPlanesA;
  delete [] rotationAnglesOfStrawPlanesA;
  delete [] positionsOfMainRadiatorsA;
  delete [] positionsOfThinRadiatorsA;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfWheelsA done" << endmsg;
}


  // Called by TRTParametersOfWheelsA

void TRTParametersOfWheelsA::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsA::DefineParameters" << endmsg;

    // Parameters of wheels A:
  numberOfWheelsA = pParameters->GetInteger("NumberOfWheelsA");
  distanceBetweenWheelsA =  pParameters->GetDouble("DistanceBetweenWheelsAC");
  positionsOfWheelsA = new double[numberOfWheelsA];
  positionsOfWheelsA[0] = pParameters->GetDouble("GlobalPositionOfFirstWheelA");
  int i;
  for (i = 1; i < numberOfWheelsA; ++i)
    positionsOfWheelsA[i] = positionsOfWheelsA[i - 1] + distanceBetweenWheelsA;

    // Parameters of wheel A:
  innerRadiusOfWheelA = pParameters->GetDouble("InnerRadiusOfWheelsAB");
  outerRadiusOfWheelA = pParameters->GetDouble("OuterRadiusOfWheelsAB");
  lengthOfWheelA = pParameters->GetDouble("LengthOfWheelsAC");

    // Parameters of inner support A:
  innerRadiusOfInnerSupportA = innerRadiusOfWheelA;
  outerRadiusOfInnerSupportA = innerRadiusOfWheelA +
    pParameters->GetDouble("ThicknessOfInnerSupportsAB");
  lengthOfInnerSupportA = lengthOfWheelA;

    // Parameters of outer support A:
  innerRadiusOfOuterSupportA = outerRadiusOfWheelA -
    pParameters->GetDouble("ThicknessOfOuterSupportsAB");
  outerRadiusOfOuterSupportA = outerRadiusOfWheelA;
  lengthOfOuterSupportA = lengthOfWheelA;

    // Parameters of straw planes A:
  numberOfStrawPlanesA =
    pParameters->GetInteger("NumberOfStrawPlanesAC");
  numberOfStrawsInPlaneA =
    pParameters->GetInteger("NumberOfStrawsInPlanesAB");
  positionsOfStrawPlanesA = new double[numberOfStrawPlanesA];
  rotationAnglesOfStrawPlanesA = new double[numberOfStrawPlanesA];
  pParameters->GetDoubleArray("PositionsOfStrawPlanesAC", numberOfStrawPlanesA,
    positionsOfStrawPlanesA);
  pParameters->GetDoubleArray("RotationsOfStrawPlanes", numberOfStrawPlanesA,
    rotationAnglesOfStrawPlanesA);
  double deltaPhiForStraws = 360. / (double) numberOfStrawsInPlaneA;
  for (i = 0; i < numberOfStrawPlanesA; ++i)
  {
    positionsOfStrawPlanesA[i] = positionsOfStrawPlanesA[i] -
      lengthOfWheelA / 2.;
    rotationAnglesOfStrawPlanesA[i] = rotationAnglesOfStrawPlanesA[i] *
      deltaPhiForStraws;
  }

    // Parameters of main radiator A:
  innerRadiusOfMainRadiatorA = outerRadiusOfInnerSupportA;
  outerRadiusOfMainRadiatorA = innerRadiusOfOuterSupportA -
    pParameters->GetDouble("GapBetweenRadiatorAndOuterSupport");
  lengthOfMainRadiatorA = pParameters->GetDouble("LengthOfMainRadiatorsAC");

    // Parameters of thin radiator A:
  innerRadiusOfThinRadiatorA = innerRadiusOfMainRadiatorA;
  outerRadiusOfThinRadiatorA = outerRadiusOfMainRadiatorA;
  lengthOfThinRadiatorA = pParameters->GetDouble("LengthOfThinRadiatorsAC");

    // Parameters of main and thin radiators A:
  numberOfMainRadiatorsA = pParameters->GetInteger("NumberOfMainRadiatorsAC");
  numberOfThinRadiatorsA = pParameters->GetInteger("NumberOfThinRadiatorsAC");
  double outerRadiusOfStraw = pParameters->GetDouble("OuterRadiusOfStraw");
  positionsOfMainRadiatorsA = new double[numberOfMainRadiatorsA];
  positionsOfThinRadiatorsA = new double[numberOfThinRadiatorsA];
  int j = 0, k = 0;
  for (i = 1; i <= numberOfStrawPlanesA; ++i)
  {
    if (i % 4 == 1)
      positionsOfThinRadiatorsA[j++] = positionsOfStrawPlanesA[i - 1] -
        outerRadiusOfStraw - lengthOfThinRadiatorA / 2.;
    if (i % 4 == 0)
    {
      positionsOfThinRadiatorsA[j++] = positionsOfStrawPlanesA[i - 1] +
        outerRadiusOfStraw + lengthOfThinRadiatorA / 2.;
      continue;
    }
    positionsOfMainRadiatorsA[k++] = positionsOfStrawPlanesA[i - 1] +
      outerRadiusOfStraw + lengthOfMainRadiatorA / 2.;
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
  output << "  numberOfWheelsA=" << numberOfWheelsA << std::endl;
  output << "  distanceBetweenWheelsA=" << distanceBetweenWheelsA << " mm"
         << std::endl << std::endl;
  int i;
  for (i = 0; i < numberOfWheelsA; ++i)
    output << "  positionsOfWheelsA[" << i << "]=" << positionsOfWheelsA[i]
           << " mm" << std::endl;

  output << std::endl << "Parameters of wheel A:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfWheelA, outerRadiusOfWheelA,
    lengthOfWheelA);

  output << std::endl << "Parameters of inner support A:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfInnerSupportA,
    outerRadiusOfInnerSupportA, lengthOfInnerSupportA);

  output << std::endl << "Parameters of outer support A:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfOuterSupportA,
    outerRadiusOfOuterSupportA, lengthOfOuterSupportA);

  output << std::endl << "Parameters of main radiators A:" << std::endl;
  output << "  numberOfMainRadiatorsA=" << numberOfMainRadiatorsA << std::endl
         << std::endl;
  for (i = 0; i < numberOfMainRadiatorsA; ++i)
    output << "  positionsOfMainRadiatorsA[" << i << "]="
           << positionsOfMainRadiatorsA[i] << " mm" << std::endl;

  output << std::endl << "Parameters of main radiator A:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfMainRadiatorA,
    outerRadiusOfMainRadiatorA, lengthOfMainRadiatorA);

  output << std::endl << "Parameters of thin radiators A:" << std::endl;
  output << "  numberOfThinRadiatorsA=" << numberOfThinRadiatorsA << std::endl
         << std::endl;
  for (i = 0; i < numberOfThinRadiatorsA; ++i)
    output << "  positionsOfThinRadiatorsA[" << i << "]="
           << positionsOfThinRadiatorsA[i] << " mm" << std::endl;

  output << std::endl << "Parameters of thin radiator A:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfThinRadiatorA,
    outerRadiusOfThinRadiatorA, lengthOfThinRadiatorA);

  output << std::endl << "Parameters of straw planes A:" << std::endl;
  output << "  numberOfStrawPlanesA=" << numberOfStrawPlanesA << std::endl;
  output << "  numberOfStrawsInPlaneA=" << numberOfStrawsInPlaneA << std::endl
         << std::endl;
  for (i = 0; i < numberOfStrawPlanesA; ++i)
    output << "  positionsOfStrawPlanesA[" << i << "]="
           << positionsOfStrawPlanesA[i] << " mm" << std::endl;
  output << std::endl;
  for (i = 0; i < numberOfStrawPlanesA; ++i)
    output << "  rotationAnglesOfStrawPlanesA[" << i << "]="
           << rotationAnglesOfStrawPlanesA[i] << " deg" << std::endl;

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsA::PrintParameters done" << endmsg;
}
