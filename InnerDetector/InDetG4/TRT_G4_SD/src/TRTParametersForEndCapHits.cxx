/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "TRTParametersForEndCapHits.h"

// Package headers
#include "TRTProcessingOfEndCapHits.h"

// Athena headers
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include "TRT_G4Utilities/TRTParameters.hh"

// STL headers
#include <fstream>

// Called by TRTSensitiveDetector::InitializeHitProcessing

TRTParametersForEndCapHits::TRTParametersForEndCapHits
(TRTProcessingOfEndCapHits* pProcessing): m_pProcessingOfEndCapHits(pProcessing),
                                          m_verboseLevel(pProcessing->m_verboseLevel)
{
  m_pParameters = TRTParameters::GetPointer();

  m_printMessages = m_pParameters->GetInteger("PrintMessages"); //FIXME not used?


  if (m_verboseLevel>5) { G4cout << "######### Constructor TRTParametersForEndCapHits" << G4endl; }

  this->DefineParameters();

  if (m_pParameters->GetInteger("PrintParametersForEndCapHits"))
    this->PrintParameters();

  if (m_verboseLevel>5) { G4cout << "######### Constructor TRTParametersForEndCapHits done" << G4endl; }

}


// Called by TRTSensitiveDetector::InitializeHitProcessing

TRTParametersForEndCapHits::~TRTParametersForEndCapHits()
{
  if (m_verboseLevel>5) { G4cout << "######### Destructor TRTParametersForEndCapHits" << G4endl; }
  if (m_verboseLevel>5) { G4cout << "######### Destructor TRTParametersForEndCapHits done" << G4endl; }
}


// Called by TRTParametersForEndCapHits

void TRTParametersForEndCapHits::DefineParameters() const
{

  if (m_verboseLevel>5) { G4cout << "#########  TRTParametersForEndCapHits::DefineParameters" << G4endl; }

  int numberOfStrawsInPlanesAB =
    m_pParameters->GetInteger("NumberOfStrawsInPlanesAB");
  int numberOfStrawsInPlaneC =
    m_pParameters->GetInteger("NumberOfStrawsInPlaneC");

  m_pProcessingOfEndCapHits->m_numberOfStrawsInPlanesAB = numberOfStrawsInPlanesAB;
  m_pProcessingOfEndCapHits->m_numberOfStrawsInPlaneC = numberOfStrawsInPlaneC;

  int numberOfWheelsA
    = m_pParameters->GetInteger("NumberOfWheelsA");
  int numberOfWheelsB = m_pParameters->GetInteger("NumberOfWheelsB");
  //  int numberOfWheelsC = m_pParameters->GetInteger("NumberOfWheelsC"); // There were no c-wheels ever built :(

  int numberOfWheelsAB = numberOfWheelsA + numberOfWheelsB;

  m_pProcessingOfEndCapHits->m_numberOfWheelsAB = numberOfWheelsAB;

  int numberOfIdentifierSectors =
    m_pParameters->GetInteger("NumberOfIdentifierSectors");
  m_pProcessingOfEndCapHits->m_numberOfStrawsInIDSectorsAB =
    numberOfStrawsInPlanesAB / numberOfIdentifierSectors;
  m_pProcessingOfEndCapHits->m_numberOfStrawsInIDSectorC =
    numberOfStrawsInPlaneC / numberOfIdentifierSectors;

  if (m_pParameters->GetInteger("SectorsABC"))
    {
      int numberOfSectorsAB = m_pParameters->GetInteger("NumberOfSectorsAB");
      int numberOfSectorsC = m_pParameters->GetInteger("NumberOfSectorsC");
      int numberOfStrawsInSectorsAB = numberOfStrawsInPlanesAB /
        numberOfSectorsAB;
      int numberOfStrawsInSectorC = numberOfStrawsInPlaneC / numberOfSectorsC;
      m_pProcessingOfEndCapHits->m_numberOfStrawsInSectorsAB =
        numberOfStrawsInSectorsAB;
      m_pProcessingOfEndCapHits->m_numberOfStrawsInSectorC = numberOfStrawsInSectorC;
      m_pProcessingOfEndCapHits->m_initialStrawIDInSectorsAB =
        numberOfStrawsInSectorsAB / 2 - 1;
      m_pProcessingOfEndCapHits->m_initialStrawIDInSectorC =
        numberOfStrawsInSectorC / 2 - 1;
    }


  if (m_verboseLevel>5) { G4cout << "#########  TRTParametersForEndCapHits::DefineParameters done" << G4endl; }

}


// Called by TRTParametersForEndCapHits

void TRTParametersForEndCapHits::PrintParameters ATLAS_NOT_THREAD_SAFE () const // Thread unsafe TRTOutputFile class is used.
{
  if (m_verboseLevel>5) { G4cout << "######### Method TRTParametersForEndCapHits::PrintParameters" << G4endl; }

  int wheelsC = m_pParameters->GetInteger("WheelsC");

  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersForEndCapHits::PrintParameters *****"
         << std::endl;

  output << "Parameters of end-cap straws:" << std::endl;
  output << "  numberOfStrawsInPlanesAB="
         << m_pProcessingOfEndCapHits->m_numberOfStrawsInPlanesAB << std::endl;
  if (wheelsC)
    output << "  numberOfStrawsInPlaneC="
           << m_pProcessingOfEndCapHits->m_numberOfStrawsInPlaneC << std::endl;

  output << "  numberOfStrawsInIDSectorsAB="
         << m_pProcessingOfEndCapHits->m_numberOfStrawsInIDSectorsAB << std::endl;
  if (wheelsC)
    output << "  numberOfStrawsInIDSectorC="
           << m_pProcessingOfEndCapHits->m_numberOfStrawsInIDSectorC << std::endl;


  if (m_pParameters->GetInteger("SectorsABC"))
    {
      output << "  numberOfStrawsInSectorsAB="
             << m_pProcessingOfEndCapHits->m_numberOfStrawsInSectorsAB << std::endl;
      if (wheelsC)
        output << "  numberOfStrawsInSectorC="
               << m_pProcessingOfEndCapHits->m_numberOfStrawsInSectorC << std::endl;

      output << "  initialStrawIDInSectorsAB="
             << m_pProcessingOfEndCapHits->m_initialStrawIDInSectorsAB << std::endl;
      if (wheelsC)
        output << "  initialStrawIDInSectorC="
               << m_pProcessingOfEndCapHits->m_initialStrawIDInSectorC << std::endl;
    }

  output << std::endl << "Parameters of end-cap wheels:" << std::endl;

  output << "  numberOfWheelsAB=" << m_pProcessingOfEndCapHits->m_numberOfWheelsAB
         << std::endl;

  output << std::endl;

  if (m_verboseLevel>5) { G4cout << "######### Method TRTParametersForEndCapHits::PrintParameters done" << G4endl; }

}
