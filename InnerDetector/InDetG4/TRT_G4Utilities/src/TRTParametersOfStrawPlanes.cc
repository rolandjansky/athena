/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTParametersOfStrawPlanes.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTUtilities.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>



  // Called by TRTConstructionOfStrawPlanes::TRTConstructionOfStrawPlanes

TRTParametersOfStrawPlanes::TRTParametersOfStrawPlanes() : m_msg("TRTParametersOfStrawPlanes")
{
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfStrawPlanes" << endmsg;

  DefineParameters();

  if (pParameters->GetInteger("PrintParametersOfStrawPlanes"))
    PrintParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfStrawPlanes done" << endmsg;
}


  // Called by TRTConstructionOfStrawPlanes::~TRTConstructionOfStrawPlanes

TRTParametersOfStrawPlanes::~TRTParametersOfStrawPlanes()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfStrawPlanes" << endmsg;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfStrawPlanes done" << endmsg;
}


  // Called by TRTParametersOfStrawPlanes

void TRTParametersOfStrawPlanes::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfStrawPlanes::DefineParameters" << endmsg;

    // Parameters of straw plane:
  innerRadiusOfStrawPlanesAB = pParameters->GetDouble("InnerRadiusOfWheelsAB") +
    pParameters->GetDouble("ThicknessOfInnerSupportsAB");
  outerRadiusOfStrawPlanesAB = pParameters->GetDouble("OuterRadiusOfWheelsAB") -
    pParameters->GetDouble("ThicknessOfOuterSupportsAB");
  lengthOfStrawPlane = pParameters->GetDouble("OuterRadiusOfStraw") * 2.;
  numberOfStrawsInPlanesAB =
    pParameters->GetInteger("NumberOfStrawsInPlanesAB");

  if (pParameters->GetInteger("SectorsABC"))
  {
      // Parameters of sectors:
    numberOfSectorsAB = pParameters->GetInteger("NumberOfSectorsAB");

    innerRadiusOfSectorsAB = innerRadiusOfStrawPlanesAB;
    outerRadiusOfSectorsAB = outerRadiusOfStrawPlanesAB;
    lengthOfSector = lengthOfStrawPlane;

    numberOfStrawsInSectorsAB = numberOfStrawsInPlanesAB / numberOfSectorsAB;
  }

    // Parameters of straw:
  outerRadiusOfStraw = pParameters->GetDouble("OuterRadiusOfStraw");
  lengthOfStrawsAB = (outerRadiusOfStrawPlanesAB - innerRadiusOfStrawPlanesAB) *
    pParameters->GetDouble("MultiplierForStrawLength");
  positionOfStrawsAB = (innerRadiusOfStrawPlanesAB +
    outerRadiusOfStrawPlanesAB) / 2.;

    // Parameters of wire:
  outerRadiusOfWire = pParameters->GetDouble("OuterRadiusOfWire");
  lengthOfWiresAB = lengthOfStrawsAB;

    // Parameters of dead region:
  innerRadiusOfDeadRegion = outerRadiusOfWire;
  outerRadiusOfDeadRegion = pParameters->GetDouble("InnerRadiusOfStraw");
  lengthOfDeadRegion = pParameters->GetDouble("LengthOfDeadRegion");
  positionOfDeadRegionsAB = (lengthOfStrawsAB - lengthOfDeadRegion) / 2.;

    // Parameters of gas:
  innerRadiusOfGas = innerRadiusOfDeadRegion;
  outerRadiusOfGas = outerRadiusOfDeadRegion;
  lengthOfGasAB = lengthOfStrawsAB - lengthOfDeadRegion * 2.;

    // Parameters of straw plane C:
  if (pParameters->GetInteger("WheelsC"))
  {
      // Parameters of straw plane:
    innerRadiusOfStrawPlaneC = pParameters->GetDouble("InnerRadiusOfWheelC") +
      pParameters->GetDouble("ThicknessOfInnerSupportC");
    outerRadiusOfStrawPlaneC = pParameters->GetDouble("OuterRadiusOfWheelC") -
      pParameters->GetDouble("ThicknessOfOuterSupportC");
    numberOfStrawsInPlaneC = pParameters->GetInteger("NumberOfStrawsInPlaneC");

    if (pParameters->GetInteger("SectorsABC"))
    {
        // Parameters of sectors:
      numberOfSectorsC = pParameters->GetInteger("NumberOfSectorsC");

      innerRadiusOfSectorC = innerRadiusOfStrawPlaneC;
      outerRadiusOfSectorC = outerRadiusOfStrawPlaneC;

      numberOfStrawsInSectorC = numberOfStrawsInPlaneC / numberOfSectorsC;
    }

      // Parameters of straw:
    lengthOfStrawC = (outerRadiusOfStrawPlaneC - innerRadiusOfStrawPlaneC) *
      pParameters->GetDouble("MultiplierForStrawLength");
    positionOfStrawC = (innerRadiusOfStrawPlaneC + outerRadiusOfStrawPlaneC) /
      2.;

      // Parameters of wire:
    lengthOfWireC = lengthOfStrawC;

      // Parameters of dead region:
    positionOfDeadRegionC = (lengthOfStrawC - lengthOfDeadRegion) / 2.;

      // Parameters of gas:
    lengthOfGasC = lengthOfStrawC - lengthOfDeadRegion * 2.;
  }

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfStrawPlanes::DefineParameters" << " done" << endmsg;
}


  // Called by TRTParametersOfStrawPlanes

void TRTParametersOfStrawPlanes::PrintParameters() const
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfStrawPlanes::PrintParameters" << endmsg;

  TRTUtilities* pUtilities = TRTUtilities::GetPointer();
  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersOfStrawPlanes::PrintParameters *****"
         << std::endl;

  output << "Parameters of straw planes AB:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfStrawPlanesAB,
    outerRadiusOfStrawPlanesAB, lengthOfStrawPlane);
  output << "  numberOfStrawsInPlanesAB=" << numberOfStrawsInPlanesAB
         << std::endl;

  if (pParameters->GetInteger("SectorsABC"))
  {
    output << std::endl << "Parameters of sectors AB:" << std::endl;
    pUtilities->PrintTubeParameters(innerRadiusOfSectorsAB,
      outerRadiusOfSectorsAB, lengthOfSector);
    output << "  numberOfStrawsInSectorsAB=" << numberOfStrawsInSectorsAB
           << std::endl;
  }

  output << std::endl << "Parameters of straws AB:" << std::endl;
  pUtilities->PrintTubeParameters(outerRadiusOfStraw, lengthOfStrawsAB);
  output << "  position=" << positionOfStrawsAB << " mm" << std::endl;

  output << std::endl << "Parameters of gas AB:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfGas,
    outerRadiusOfGas, lengthOfGasAB);

  output << std::endl << "Parameters of dead regions AB:" << std::endl;
  pUtilities->PrintTubeParameters(innerRadiusOfDeadRegion,
    outerRadiusOfDeadRegion, lengthOfDeadRegion, positionOfDeadRegionsAB);

  output << std::endl << "Parameters of wires AB:" << std::endl;
  pUtilities->PrintTubeParameters(outerRadiusOfWire, lengthOfWiresAB);

  if (pParameters->GetInteger("WheelsC"))
  {
    output << std::endl << "Parameters of straw plane C:" << std::endl;
    pUtilities->PrintTubeParameters(innerRadiusOfStrawPlaneC,
      outerRadiusOfStrawPlaneC, lengthOfStrawPlane);
    output << "  numberOfStrawsInPlaneC=" << numberOfStrawsInPlaneC
           << std::endl;

    if (pParameters->GetInteger("SectorsABC"))
    {
      output << std::endl << "Parameters of sector C:" << std::endl;
      pUtilities->PrintTubeParameters(innerRadiusOfSectorC,
        outerRadiusOfSectorC, lengthOfSector);
      output << "  numberOfStrawsInSectorC=" << numberOfStrawsInSectorC
             << std::endl;
    }

    output << std::endl << "Parameters of straw C:" << std::endl;
    pUtilities->PrintTubeParameters(outerRadiusOfStraw, lengthOfStrawC);
    output << "  position=" << positionOfStrawC << " mm" << std::endl;

    output << std::endl << "Parameters of gas C:" << std::endl;
    pUtilities->PrintTubeParameters(innerRadiusOfGas,
      outerRadiusOfGas, lengthOfGasC);

    output << std::endl << "Parameters of dead region C:" << std::endl;
    pUtilities->PrintTubeParameters(innerRadiusOfDeadRegion,
      outerRadiusOfDeadRegion, lengthOfDeadRegion, positionOfDeadRegionC);

    output << std::endl << "Parameters of wire C:" << std::endl;
    pUtilities->PrintTubeParameters(outerRadiusOfWire, lengthOfWireC);
  }

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfStrawPlanes::PrintParameters done" << endmsg;
}
