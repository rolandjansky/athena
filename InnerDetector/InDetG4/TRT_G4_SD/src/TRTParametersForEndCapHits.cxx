/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4_SD/TRTParametersForEndCapHits.h"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4_SD/TRTProcessingOfEndCapHits.h"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>


  // Called by TRTSensitiveDetector::InitializeHitProcessing

TRTParametersForEndCapHits::TRTParametersForEndCapHits
(TRTProcessingOfEndCapHits* pProcessing): pProcessingOfEndCapHits(pProcessing),m_msg("TRTParametersForEndCapHits")
{
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");


  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Constructor TRTParametersForEndCapHits" << endreq;

  DefineParameters();

  if (pParameters->GetInteger("PrintParametersForEndCapHits"))
    PrintParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Constructor TRTParametersForEndCapHits done" << endreq;

}


  // Called by TRTSensitiveDetector::InitializeHitProcessing

TRTParametersForEndCapHits::~TRTParametersForEndCapHits()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Destructor TRTParametersForEndCapHits" << endreq;
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Destructor TRTParametersForEndCapHits done" << endreq;
}


  // Called by TRTParametersForEndCapHits

void TRTParametersForEndCapHits::DefineParameters() const
{

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "#########  TRTParametersForEndCapHits::DefineParameters" << endreq;
  
  int numberOfStrawsInPlanesAB =
    pParameters->GetInteger("NumberOfStrawsInPlanesAB");
  int numberOfStrawsInPlaneC =
    pParameters->GetInteger("NumberOfStrawsInPlaneC");

  pProcessingOfEndCapHits->numberOfStrawsInPlanesAB = numberOfStrawsInPlanesAB;
  pProcessingOfEndCapHits->numberOfStrawsInPlaneC = numberOfStrawsInPlaneC;

  int numberOfWheelsA
 = pParameters->GetInteger("NumberOfWheelsA");
  int numberOfWheelsB = pParameters->GetInteger("NumberOfWheelsB");
//  int numberOfWheelsC = pParameters->GetInteger("NumberOfWheelsC"); // There were no c-wheels ever built :(

  int numberOfWheelsAB = numberOfWheelsA + numberOfWheelsB;

  pProcessingOfEndCapHits->numberOfWheelsAB = numberOfWheelsAB;

  int numberOfIdentifierSectors =
    pParameters->GetInteger("NumberOfIdentifierSectors");
  pProcessingOfEndCapHits->numberOfStrawsInIDSectorsAB =
    numberOfStrawsInPlanesAB / numberOfIdentifierSectors;
  pProcessingOfEndCapHits->numberOfStrawsInIDSectorC =
    numberOfStrawsInPlaneC / numberOfIdentifierSectors;

  if (pParameters->GetInteger("SectorsABC"))
  {
    int numberOfSectorsAB = pParameters->GetInteger("NumberOfSectorsAB");
    int numberOfSectorsC = pParameters->GetInteger("NumberOfSectorsC");
    int numberOfStrawsInSectorsAB = numberOfStrawsInPlanesAB /
      numberOfSectorsAB;
    int numberOfStrawsInSectorC = numberOfStrawsInPlaneC / numberOfSectorsC;
    pProcessingOfEndCapHits->numberOfStrawsInSectorsAB =
      numberOfStrawsInSectorsAB;
    pProcessingOfEndCapHits->numberOfStrawsInSectorC = numberOfStrawsInSectorC;
    pProcessingOfEndCapHits->initialStrawIDInSectorsAB =
      numberOfStrawsInSectorsAB / 2 - 1;
    pProcessingOfEndCapHits->initialStrawIDInSectorC =
      numberOfStrawsInSectorC / 2 - 1;
  }


  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "#########  TRTParametersForEndCapHits::DefineParameters done" << endreq;

}


  // Called by TRTParametersForEndCapHits

void TRTParametersForEndCapHits::PrintParameters() const
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersForEndCapHits::PrintParameters" << endreq;

  int wheelsC = pParameters->GetInteger("WheelsC");

  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersForEndCapHits::PrintParameters *****"
         << std::endl;

  output << "Parameters of end-cap straws:" << std::endl;
  output << "  numberOfStrawsInPlanesAB="
         << pProcessingOfEndCapHits->numberOfStrawsInPlanesAB << std::endl;
  if (wheelsC)
    output << "  numberOfStrawsInPlaneC="
           << pProcessingOfEndCapHits->numberOfStrawsInPlaneC << std::endl;

  output << "  numberOfStrawsInIDSectorsAB="
         << pProcessingOfEndCapHits->numberOfStrawsInIDSectorsAB << std::endl;
  if (wheelsC)
    output << "  numberOfStrawsInIDSectorC="
           << pProcessingOfEndCapHits->numberOfStrawsInIDSectorC << std::endl;


  if (pParameters->GetInteger("SectorsABC"))
  {
    output << "  numberOfStrawsInSectorsAB="
           << pProcessingOfEndCapHits->numberOfStrawsInSectorsAB << std::endl;
    if (wheelsC)
      output << "  numberOfStrawsInSectorC="
             << pProcessingOfEndCapHits->numberOfStrawsInSectorC << std::endl;

    output << "  initialStrawIDInSectorsAB="
           << pProcessingOfEndCapHits->initialStrawIDInSectorsAB << std::endl;
    if (wheelsC)
      output << "  initialStrawIDInSectorC="
             << pProcessingOfEndCapHits->initialStrawIDInSectorC << std::endl;
  }

  output << std::endl << "Parameters of end-cap wheels:" << std::endl;

  output << "  numberOfWheelsAB=" << pProcessingOfEndCapHits->numberOfWheelsAB
         << std::endl;

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersForEndCapHits::PrintParameters done" << endreq;

}
