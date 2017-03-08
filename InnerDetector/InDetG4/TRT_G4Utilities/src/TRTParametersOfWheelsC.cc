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
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsC" << endmsg;

  DefineParameters();

  if (pParameters->GetInteger("PrintParametersOfWheelsC"))
    PrintParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfWheelsC done" << endmsg;
}


  // Called by TRTConstructionOfWheelsC::~TRTConstructionOfWheelsC

TRTParametersOfWheelsC::~TRTParametersOfWheelsC()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Destructor TRTParametersOfWheelsC" << endmsg;

  delete [] positionsOfWheelsC;
  delete [] positionsOfStrawPlanesC;
  delete [] rotationAnglesOfStrawPlanesC;
  delete [] positionsOfMainRadiatorsC;
  delete [] positionsOfThinRadiatorsC;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Destructor TRTParametersOfWheelsC done" << endmsg;
}


  // Called by TRTParametersOfWheelsC

void TRTParametersOfWheelsC::DefineParameters()
{

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsC::DefineParameters" << endmsg;

    // Parameters of wheels C:
  numberOfWheelsC = pParameters->GetInteger("NumberOfWheelsC");
  distanceBetweenWheelsC =  pParameters->GetDouble("DistanceBetweenWheelsAC");
  positionsOfWheelsC = new double[numberOfWheelsC];
  positionsOfWheelsC[0] = pParameters->GetDouble("GlobalPositionOfFirstWheelC");
  int i;
  for (i = 1; i < numberOfWheelsC; ++i)
    positionsOfWheelsC[i] = positionsOfWheelsC[i - 1] + distanceBetweenWheelsC;

    // Parameters of wheel C:
  innerRadiusOfWheelC = pParameters->GetDouble("InnerRadiusOfWheelC");
  outerRadiusOfWheelC = pParameters->GetDouble("OuterRadiusOfWheelC");
  lengthOfWheelC = pParameters->GetDouble("LengthOfWheelsAC");

    // Parameters of inner support C:
  innerRadiusOfInnerSupportC = innerRadiusOfWheelC;
  outerRadiusOfInnerSupportC = innerRadiusOfWheelC +
    pParameters->GetDouble("ThicknessOfInnerSupportC");
  lengthOfInnerSupportC = lengthOfWheelC;

    // Parameters of outer support C:
  innerRadiusOfOuterSupportC = outerRadiusOfWheelC -
    pParameters->GetDouble("ThicknessOfOuterSupportC");
  outerRadiusOfOuterSupportC = outerRadiusOfWheelC;
  lengthOfOuterSupportC = lengthOfWheelC;

    // Parameters of straw planes C:
  numberOfStrawPlanesC = pParameters->GetInteger("NumberOfStrawPlanesAC");
  numberOfStrawsInPlaneC = pParameters->GetInteger("NumberOfStrawsInPlaneC");
  positionsOfStrawPlanesC = new double[numberOfStrawPlanesC];
  rotationAnglesOfStrawPlanesC = new double[numberOfStrawPlanesC];
  pParameters->GetDoubleArray("PositionsOfStrawPlanesAC", numberOfStrawPlanesC,
    positionsOfStrawPlanesC);
  pParameters->GetDoubleArray("RotationsOfStrawPlanes", numberOfStrawPlanesC,
    rotationAnglesOfStrawPlanesC);
  double deltaPhiForStraws = 360. / (double) numberOfStrawsInPlaneC;
  for (i = 0; i < numberOfStrawPlanesC; ++i)
  {
    positionsOfStrawPlanesC[i] = positionsOfStrawPlanesC[i] -
      lengthOfWheelC / 2.;
    rotationAnglesOfStrawPlanesC[i] = rotationAnglesOfStrawPlanesC[i] *
      deltaPhiForStraws;
  }

    // Parameters of main radiator C:
  innerRadiusOfMainRadiatorC = outerRadiusOfInnerSupportC;
  outerRadiusOfMainRadiatorC = innerRadiusOfOuterSupportC -
    pParameters->GetDouble("GapBetweenRadiatorAndOuterSupport");
  lengthOfMainRadiatorC = pParameters->GetDouble("LengthOfMainRadiatorsAC");

    // Parameters of thin radiator C:
  innerRadiusOfThinRadiatorC = innerRadiusOfMainRadiatorC;
  outerRadiusOfThinRadiatorC = outerRadiusOfMainRadiatorC;
  lengthOfThinRadiatorC = pParameters->GetDouble("LengthOfThinRadiatorsAC");

    // Parameters of main and thin radiators C:
  numberOfMainRadiatorsC = pParameters->GetInteger("NumberOfMainRadiatorsAC");
  numberOfThinRadiatorsC = pParameters->GetInteger("NumberOfThinRadiatorsAC");
  double outerRadiusOfStraw = pParameters->GetDouble("OuterRadiusOfStraw");
  positionsOfMainRadiatorsC = new double[numberOfMainRadiatorsC];
  positionsOfThinRadiatorsC = new double[numberOfThinRadiatorsC];
  int j = 0, k = 0;
  for (i = 1; i <= numberOfStrawPlanesC; ++i)
  {
    if (i % 4 == 1)
      positionsOfThinRadiatorsC[j++] = positionsOfStrawPlanesC[i - 1] -
        outerRadiusOfStraw - lengthOfThinRadiatorC / 2.;
    if (i % 4 == 0)
    {
      positionsOfThinRadiatorsC[j++] = positionsOfStrawPlanesC[i - 1] +
        outerRadiusOfStraw + lengthOfThinRadiatorC / 2.;
      continue;
    }
    positionsOfMainRadiatorsC[k++] = positionsOfStrawPlanesC[i - 1] +
      outerRadiusOfStraw + lengthOfMainRadiatorC / 2.;
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
  output << "  numberOfWheelsC=" << numberOfWheelsC << std::endl;
  output << "  distanceBetweenWheelsC=" << distanceBetweenWheelsC << " mm"
         << std::endl << std::endl;
  int i;
  for (i = 0; i < numberOfWheelsC; ++i)
    output << "  positionsOfWheelsC[" << i << "]=" << positionsOfWheelsC[i]
           << " mm" << std::endl;

  output << std::endl << "Parameters of wheel C:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfWheelC, outerRadiusOfWheelC,
    lengthOfWheelC);

  output << std::endl << "Parameters of inner support C:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfInnerSupportC,
    outerRadiusOfInnerSupportC, lengthOfInnerSupportC);

  output << std::endl << "Parameters of outer support C:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfOuterSupportC,
    outerRadiusOfOuterSupportC, lengthOfOuterSupportC);

  output << std::endl << "Parameters of main radiators C:" << std::endl;
  output << "  numberOfMainRadiatorsC=" << numberOfMainRadiatorsC << std::endl
         << std::endl;
  for (i = 0; i < numberOfMainRadiatorsC; ++i)
    output << "  positionsOfMainRadiatorsC[" << i << "]="
           << positionsOfMainRadiatorsC[i] << " mm" << std::endl;

  output << std::endl << "Parameters of main radiator C:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfMainRadiatorC,
    outerRadiusOfMainRadiatorC, lengthOfMainRadiatorC);

  output << std::endl << "Parameters of thin radiators C:" << std::endl;
  output << "  numberOfThinRadiatorsC=" << numberOfThinRadiatorsC << std::endl
         << std::endl;
  for (i = 0; i < numberOfThinRadiatorsC; ++i)
    output << "  positionsOfThinRadiatorsC[" << i << "]="
           << positionsOfThinRadiatorsC[i] << " mm" << std::endl;

  output << std::endl << "Parameters of thin radiator C:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfThinRadiatorC,
    outerRadiusOfThinRadiatorC, lengthOfThinRadiatorC);

  output << std::endl << "Parameters of straw planes C:" << std::endl;
  output << "  numberOfStrawPlanesC=" << numberOfStrawPlanesC << std::endl;
  output << "  numberOfStrawsInPlaneC=" << numberOfStrawsInPlaneC << std::endl
         << std::endl;
  for (i = 0; i < numberOfStrawPlanesC; ++i)
    output << "  positionsOfStrawPlanesC[" << i << "]="
           << positionsOfStrawPlanesC[i] << " mm" << std::endl;
  output << std::endl;
  for (i = 0; i < numberOfStrawPlanesC; ++i)
    output << "  rotationAnglesOfStrawPlanesC[" << i << "]="
           << rotationAnglesOfStrawPlanesC[i] << " deg" << std::endl;

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfWheelsC::PrintParameters done" << endmsg;
}
