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
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsB" << endmsg;

  DefineParameters();

  if (pParameters->GetInteger("PrintParametersOfWheelsB"))
    PrintParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsB done" << endmsg;
}


  // Called by TRTConstructionOfWheelsB::~TRTConstructionOfWheelsB

TRTParametersOfWheelsB::~TRTParametersOfWheelsB()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfWheelsB" << endmsg;

  delete [] positionsOfWheelsB;
  delete [] positionsOfStrawPlanesB;
  delete [] rotationAnglesOfStrawPlanesB;
  delete [] positionsOfMainRadiatorsB;
  delete [] positionsOfThinRadiatorsB;
  delete [] positionsOfMiddleRadiatorsB;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfWheelsB done" << endmsg;
}


  // Called by TRTParametersOfWheelsB

void TRTParametersOfWheelsB::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsB::DefineParameters" << endmsg;

    // Parameters of wheels B:
  numberOfWheelsB = pParameters->GetInteger("NumberOfWheelsB");
  distanceBetweenWheelsB =  pParameters->GetDouble("DistanceBetweenWheelsB");
  positionsOfWheelsB = new double[numberOfWheelsB];
  positionsOfWheelsB[0] = pParameters->GetDouble("GlobalPositionOfFirstWheelB");
  int i;
  for (i = 1; i < numberOfWheelsB; ++i)
    positionsOfWheelsB[i] = positionsOfWheelsB[i - 1] + distanceBetweenWheelsB;

    // Parameters of wheel B:
  innerRadiusOfWheelB = pParameters->GetDouble("InnerRadiusOfWheelsAB");
  outerRadiusOfWheelB = pParameters->GetDouble("OuterRadiusOfWheelsAB");
  lengthOfWheelB = pParameters->GetDouble("LengthOfWheelB");

    // Parameters of inner support B:
  innerRadiusOfInnerSupportB = innerRadiusOfWheelB;
  outerRadiusOfInnerSupportB = innerRadiusOfWheelB +
    pParameters->GetDouble("ThicknessOfInnerSupportsAB");
  lengthOfInnerSupportB = lengthOfWheelB;

    // Parameters of outer support B:
  innerRadiusOfOuterSupportB = outerRadiusOfWheelB -
    pParameters->GetDouble("ThicknessOfOuterSupportsAB");
  outerRadiusOfOuterSupportB = outerRadiusOfWheelB;
  lengthOfOuterSupportB = lengthOfWheelB;

    // Parameters of straw planes B:
  numberOfStrawPlanesB = pParameters->GetInteger("NumberOfStrawPlanesB");
  numberOfStrawsInPlaneB = pParameters->GetInteger("NumberOfStrawsInPlanesAB");
  positionsOfStrawPlanesB = new double[numberOfStrawPlanesB];
  rotationAnglesOfStrawPlanesB = new double[numberOfStrawPlanesB];
  pParameters->GetDoubleArray("PositionsOfStrawPlanesB", numberOfStrawPlanesB,
    positionsOfStrawPlanesB);
  pParameters->GetPartOfDoubleArray("RotationsOfStrawPlanes",
    numberOfStrawPlanesB, rotationAnglesOfStrawPlanesB);
  double deltaPhiForStraws = 360. / (double) numberOfStrawsInPlaneB;
  for (i = 0; i < numberOfStrawPlanesB; ++i)
  {
    positionsOfStrawPlanesB[i] = positionsOfStrawPlanesB[i] -
      lengthOfWheelB / 2.;
    rotationAnglesOfStrawPlanesB[i] = rotationAnglesOfStrawPlanesB[i] *
      deltaPhiForStraws;
  }

    // Parameters of main radiator B:
  innerRadiusOfMainRadiatorB = outerRadiusOfInnerSupportB;
  outerRadiusOfMainRadiatorB = innerRadiusOfOuterSupportB -
    pParameters->GetDouble("GapBetweenRadiatorAndOuterSupport");
  lengthOfMainRadiatorB = pParameters->GetDouble("LengthOfMainRadiatorB");

    // Parameters of main radiators B:
  numberOfMainRadiatorsB = pParameters->GetInteger("NumberOfMainRadiatorsB");
  positionsOfMainRadiatorsB = new double[numberOfMainRadiatorsB];
  double outerRadiusOfStraw = pParameters->GetDouble("OuterRadiusOfStraw");
  int j = 0;
  for (i = 1; i <= numberOfStrawPlanesB; ++i)
  {
    if (i % 4 == 0)
      continue;
    positionsOfMainRadiatorsB[j++] = positionsOfStrawPlanesB[i - 1] +
      outerRadiusOfStraw + lengthOfMainRadiatorB / 2.;
  }

    // Parameters of thin radiator B:
  innerRadiusOfThinRadiatorB = innerRadiusOfMainRadiatorB;
  outerRadiusOfThinRadiatorB = outerRadiusOfMainRadiatorB;
  lengthOfThinRadiatorB = pParameters->GetDouble("LengthOfThinRadiatorB");

    // Parameters of thin radiators B:
  numberOfThinRadiatorsB = pParameters->GetInteger("NumberOfThinRadiatorsB");
  positionsOfThinRadiatorsB = new double[numberOfThinRadiatorsB];
  positionsOfThinRadiatorsB[0] = positionsOfStrawPlanesB[0] -
    outerRadiusOfStraw - lengthOfThinRadiatorB / 2.;
  positionsOfThinRadiatorsB[1] = positionsOfStrawPlanesB[7] +
    outerRadiusOfStraw + lengthOfThinRadiatorB / 2.;

    // Parameters of middle radiator B:
  innerRadiusOfMiddleRadiatorB = innerRadiusOfMainRadiatorB;
  outerRadiusOfMiddleRadiatorB = outerRadiusOfMainRadiatorB;
  lengthOfMiddleRadiatorB = pParameters->GetDouble("LengthOfMiddleRadiatorB");

    // Parameters of middle radiators B:
  numberOfMiddleRadiatorsB =
    pParameters->GetInteger("NumberOfMiddleRadiatorsB");
  positionsOfMiddleRadiatorsB = new double[numberOfMiddleRadiatorsB];
  positionsOfMiddleRadiatorsB[0] = positionsOfStrawPlanesB[3] +
    outerRadiusOfStraw + lengthOfMiddleRadiatorB / 2.;
  positionsOfMiddleRadiatorsB[1] = positionsOfStrawPlanesB[4] -
    outerRadiusOfStraw - lengthOfMiddleRadiatorB / 2.;

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
  output << "  numberOfWheelsB=" << numberOfWheelsB << std::endl;
  output << "  distanceBetweenWheelsB=" << distanceBetweenWheelsB << " mm"
         << std::endl << std::endl;
  int i;
  for (i = 0; i < numberOfWheelsB; ++i)
    output << "  positionsOfWheelsB[" << i << "]=" << positionsOfWheelsB[i]
           << " mm" << std::endl;

  output << std::endl << "Parameters of wheel B:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfWheelB, outerRadiusOfWheelB,
    lengthOfWheelB);

  output << std::endl << "Parameters of inner support B:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfInnerSupportB,
    outerRadiusOfInnerSupportB, lengthOfInnerSupportB);

  output << std::endl << "Parameters of outer support B:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfOuterSupportB,
    outerRadiusOfOuterSupportB, lengthOfOuterSupportB);

  output << std::endl << "Parameters of main radiators B:" << std::endl;
  output << "  numberOfMainRadiatorsB=" << numberOfMainRadiatorsB << std::endl
         << std::endl;
  for (i = 0; i < numberOfMainRadiatorsB; ++i)
    output << "  positionsOfMainRadiatorsB[" << i << "]="
           << positionsOfMainRadiatorsB[i] << " mm" << std::endl;

  output << std::endl << "Parameters of main radiator B:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfMainRadiatorB,
    outerRadiusOfMainRadiatorB, lengthOfMainRadiatorB);

  output << std::endl << "Parameters of thin radiators B:" << std::endl;
  output << "  numberOfThinRadiatorsB=" << numberOfThinRadiatorsB << std::endl
         << std::endl;
  for (i = 0; i < numberOfThinRadiatorsB; ++i)
    output << "  positionsOfThinRadiatorsB[" << i << "]="
           << positionsOfThinRadiatorsB[i] << " mm" << std::endl;

  output << std::endl << "Parameters of thin radiator B:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfThinRadiatorB,
    outerRadiusOfThinRadiatorB, lengthOfThinRadiatorB);

  output << std::endl << "Parameters of middle radiators B:" << std::endl;
  output << "  numberOfMiddleRadiatorsB=" << numberOfMiddleRadiatorsB
         << std::endl << std::endl;
  for (i = 0; i < numberOfMiddleRadiatorsB; ++i)
    output << "  positionsOfMiddleRadiatorsB[" << i << "]="
           << positionsOfMiddleRadiatorsB[i] << " mm" << std::endl;

  output << std::endl << "Parameters of middle radiator B:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfMiddleRadiatorB,
    outerRadiusOfMiddleRadiatorB, lengthOfMiddleRadiatorB);

  output << std::endl << "Parameters of straw planes B:" << std::endl;
  output << "  numberOfStrawPlanesB=" << numberOfStrawPlanesB << std::endl;
  output << "  numberOfStrawsInPlaneB=" << numberOfStrawsInPlaneB << std::endl
         << std::endl;
  for (i = 0; i < numberOfStrawPlanesB; ++i)
    output << "  positionsOfStrawPlanesB[" << i << "]="
           << positionsOfStrawPlanesB[i] << " mm" << std::endl;
  output << std::endl;
  for (i = 0; i < numberOfStrawPlanesB; ++i)
    output << "  rotationAnglesOfStrawPlanesB[" << i << "]="
           << rotationAnglesOfStrawPlanesB[i] << " deg" << std::endl;

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsB::PrintParameters done" << endmsg;
}
