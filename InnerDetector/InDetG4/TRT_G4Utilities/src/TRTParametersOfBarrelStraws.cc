/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_G4Utilities/TRTParametersOfBarrelStraws.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTUtilities.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>



  // Called by TRTConstructionOfBarrelStraws::TRTConstructionOfBarrelStraws

TRTParametersOfBarrelStraws::TRTParametersOfBarrelStraws(): m_msg("TRTParametersOfBarrelStraws")
{
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfBarrelStraws" << endmsg;

  DefineParameters();

  if (pParameters->GetInteger("PrintParametersOfBarrelStraws"))
    PrintParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfBarrelStraws done" << endmsg;
}


  // Called by TRTConstructionOfBarrelStraws::~TRTConstructionOfBarrelStraws

TRTParametersOfBarrelStraws::~TRTParametersOfBarrelStraws()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfBarrelStraws" << endmsg;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfBarrelStraws done" << endmsg;
}


  // Called by TRTParametersOfBarrelStraws

void TRTParametersOfBarrelStraws::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfBarrelStraws::DefineParameters" << endmsg;

    // Parameters of straw hole:
  outerRadiusOfStrawHole = pParameters->GetDouble("OuterRadiusOfStrawHole");
  lengthOfStrawHole = pParameters->GetDouble("LengthOfBarrelVolume");

    // Parameters of straw:
  outerRadiusOfStraw = pParameters->GetDouble("OuterRadiusOfStraw");
  lengthOfStraw = lengthOfStrawHole;

    // Parameters of wire:
  outerRadiusOfWire = pParameters->GetDouble("OuterRadiusOfWire");
  lengthOfWire = lengthOfStraw;

    // Parameters of twister:
  innerRadiusOfTwister = outerRadiusOfWire;
  outerRadiusOfTwister = pParameters->GetDouble("InnerRadiusOfStraw");
  lengthOfTwister = pParameters->GetDouble("LengthOfTwister");

    // Parameters of dead regions:
  innerRadiusOfDeadRegion = outerRadiusOfWire;
  outerRadiusOfDeadRegion = outerRadiusOfTwister;
  lengthOfDeadRegion = pParameters->GetDouble("LengthOfDeadRegion");
  lengthOfLongDeadRegion =
    pParameters->GetDouble("LengthOfInsensitiveRegionA") - lengthOfTwister / 2.;

  positionOfDeadRegionLA = (lengthOfStraw - lengthOfDeadRegion) / 2.;
  positionOfDeadRegionLB = (lengthOfTwister + lengthOfDeadRegion) / 2.;
  positionOfDeadRegionSA = positionOfDeadRegionLA;
  positionOfLongDeadRegionSB = (lengthOfTwister + lengthOfLongDeadRegion) / 2.;

    // Parameters of gas:
  innerRadiusOfGas = innerRadiusOfDeadRegion;
  outerRadiusOfGas = outerRadiusOfDeadRegion;
  lengthOfGasL = (lengthOfStraw - lengthOfTwister) / 2. -
    lengthOfDeadRegion * 2.;
  lengthOfGasS = (lengthOfStraw - lengthOfTwister) / 2. - lengthOfDeadRegion -
    lengthOfLongDeadRegion;

  positionOfGasL = (lengthOfGasL + lengthOfTwister) / 2. + lengthOfDeadRegion;
  positionOfGasS = (lengthOfGasS + lengthOfTwister) / 2. +
    lengthOfLongDeadRegion;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfBarrelStraws::DefineParameters"
						<< " done" << endmsg;
}


  // Called by TRTParametersOfBarrelStraws

void TRTParametersOfBarrelStraws::PrintParameters() const
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfBarrelStraws::PrintParameters"
						<< endmsg;

  TRTUtilities* pUtilities = TRTUtilities::GetPointer();
  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersOfBarrelStraws::PrintParameters *****"
         << std::endl;

  output << "Parameters of barrel straw hole:" << std::endl;
  pUtilities->PrintTubeParameters(outerRadiusOfStrawHole,
    lengthOfStrawHole);

  output << std::endl << "Parameters of barrel straw:" << std::endl;
  pUtilities->PrintTubeParameters(outerRadiusOfStraw, lengthOfStraw);

  output << std::endl << "Parameters of barrel straw wire:" << std::endl;
  pUtilities->PrintTubeParameters(outerRadiusOfWire, lengthOfWire);

  output << std::endl << "Parameters of barrel straw twister:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfTwister,
    outerRadiusOfTwister, lengthOfTwister);

  output << std::endl << "Parameters of gas S for short straw:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfGas, outerRadiusOfGas,
    lengthOfGasS, positionOfGasS);

  output << std::endl << "Parameters of dead region SA for short straw:"
         << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfDeadRegion,
    outerRadiusOfDeadRegion, lengthOfDeadRegion, positionOfDeadRegionSA);

  output << std::endl << "Parameters of long dead region SB for short straw:"
         << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfDeadRegion,
    outerRadiusOfDeadRegion, lengthOfLongDeadRegion,
    positionOfLongDeadRegionSB);

  output << std::endl << "Parameters of gas L for long straw:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfGas, outerRadiusOfGas,
    lengthOfGasL, positionOfGasL);

  output << std::endl << "Parameters of dead region LA for long straw:"
         << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfDeadRegion,
    outerRadiusOfDeadRegion, lengthOfDeadRegion, positionOfDeadRegionLA);

  output << std::endl << "Parameters of dead region LB for long straw:"
         << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfDeadRegion,
    outerRadiusOfDeadRegion, lengthOfDeadRegion, positionOfDeadRegionLB);

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfBarrelStraws::PrintParameters"
						<< " done" << endmsg;
}
