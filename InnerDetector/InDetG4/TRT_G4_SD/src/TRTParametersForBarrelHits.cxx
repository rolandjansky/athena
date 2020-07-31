/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "TRTParametersForBarrelHits.h"

// Package headers
#include "TRTProcessingOfBarrelHits.h"

// Athena headers
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include "TRT_G4Utilities/TRTParameters.hh"

// Geant4 headers
#include "G4ThreeVector.hh"

// STL headers
#include <fstream>

// Called by TRTSensitiveDetector::InitializeHitProcessing

TRTParametersForBarrelHits::TRTParametersForBarrelHits
(TRTProcessingOfBarrelHits* pProcessing): m_pProcessingOfBarrelHits(pProcessing),
                                          m_verboseLevel(pProcessing->m_verboseLevel)
{
  m_pParameters = TRTParameters::GetPointer();

  m_printMessages = m_pParameters->GetInteger("PrintMessages"); //FIXME not used

  if (m_verboseLevel>5) { G4cout << "##### Constructor TRTParametersForBarrelHits" << G4endl; }

  this->DefineParameters();

  if (m_pParameters->GetInteger("PrintParametersForBarrelHits"))
    this->PrintParameters();

  if (m_verboseLevel>5) { G4cout << "##### Constructor TRTParametersForBarrelHits done" << G4endl; }
}


// Called by TRTSensitiveDetector::InitializeHitProcessing

TRTParametersForBarrelHits::~TRTParametersForBarrelHits()
{
  if (m_verboseLevel>5) { G4cout << "##### Destructor TRTParametersForBarrelHits" << G4endl; }
  if (m_verboseLevel>5) { G4cout << "##### Destructor TRTParametersForBarrelHits done" << G4endl; }
}


// Called by TRTParametersForBarrelHits

void TRTParametersForBarrelHits::DefineParameters() const
{
  if (m_verboseLevel>5) { G4cout << "######### Method TRTParametersForBarrelHits::DefineParameters" << G4endl; }

  // Number of straw layers A, B, C:
  int numberOfStrawLayersA =
    m_pParameters->GetInteger("NumberOfStrawLayersA");
  int numberOfStrawLayersB =
    m_pParameters->GetInteger("NumberOfStrawLayersB");
  int numberOfStrawLayersC =
    m_pParameters->GetInteger("NumberOfStrawLayersC");

  // Integral distribution of straws A:
  int* numberOfStrawsInLayersA = new int[numberOfStrawLayersA];
  m_pParameters->GetIntegerArray("NumberOfStrawsInLayersA", numberOfStrawLayersA,
                               numberOfStrawsInLayersA);

  int* integralDistributionOfStrawsA = new int[numberOfStrawLayersA];
  integralDistributionOfStrawsA[0] = numberOfStrawsInLayersA[0];
  int i;
  for (i = 1; i < numberOfStrawLayersA; ++i)
    integralDistributionOfStrawsA[i] = integralDistributionOfStrawsA[i - 1] +
      numberOfStrawsInLayersA[i];

  for (i = 0; i < numberOfStrawLayersA; ++i)
    m_pProcessingOfBarrelHits->m_integralDistributionOfStrawsA[i] =
      integralDistributionOfStrawsA[i];

  delete [] numberOfStrawsInLayersA;

  // Integral distribution of straws B:
  int* numberOfStrawsInLayersB = new int[numberOfStrawLayersB];
  m_pParameters->GetIntegerArray("NumberOfStrawsInLayersB", numberOfStrawLayersB,
                               numberOfStrawsInLayersB);

  int* integralDistributionOfStrawsB = new int[numberOfStrawLayersB];
  integralDistributionOfStrawsB[0] = numberOfStrawsInLayersB[0];
  for (i = 1; i < numberOfStrawLayersB; ++i)
    integralDistributionOfStrawsB[i] = integralDistributionOfStrawsB[i - 1] +
      numberOfStrawsInLayersB[i];

  for (i = 0; i < numberOfStrawLayersB; ++i)
    m_pProcessingOfBarrelHits->m_integralDistributionOfStrawsB[i] =
      integralDistributionOfStrawsB[i];

  delete [] numberOfStrawsInLayersB;

  // Integral distribution of straws C:
  int* numberOfStrawsInLayersC = new int[numberOfStrawLayersC];
  m_pParameters->GetIntegerArray("NumberOfStrawsInLayersC", numberOfStrawLayersC,
                               numberOfStrawsInLayersC);

  int* integralDistributionOfStrawsC = new int[numberOfStrawLayersC];
  integralDistributionOfStrawsC[0] = numberOfStrawsInLayersC[0];
  for (i = 1; i < numberOfStrawLayersC; ++i)
    integralDistributionOfStrawsC[i] = integralDistributionOfStrawsC[i - 1] +
      numberOfStrawsInLayersC[i];

  for (i = 0; i < numberOfStrawLayersC; ++i)
    m_pProcessingOfBarrelHits->m_integralDistributionOfStrawsC[i] =
      integralDistributionOfStrawsC[i];

  delete [] numberOfStrawsInLayersC;

  if (m_pParameters->GetInteger("BarrelIdentifier") == 2)
    {
      // Arrays strawIDToLayerID for modules A, B, C:
      int layerID = 0;
      for (i = 0; i < m_pProcessingOfBarrelHits->m_numberOfStrawsA; ++i)
        {
          if (i + 1 > integralDistributionOfStrawsA[layerID])
            ++layerID;
          m_pProcessingOfBarrelHits->m_strawIDToLayerIDA[i] = layerID;
        }

      layerID = 0;
      for (i = 0; i < m_pProcessingOfBarrelHits->m_numberOfStrawsB; ++i)
        {
          if (i + 1 > integralDistributionOfStrawsB[layerID])
            ++layerID;
          m_pProcessingOfBarrelHits->m_strawIDToLayerIDB[i] = layerID;
        }

      layerID = 0;
      for (i = 0; i < m_pProcessingOfBarrelHits->m_numberOfStrawsC; ++i)
        {
          if (i + 1 > integralDistributionOfStrawsC[layerID])
            ++layerID;
          m_pProcessingOfBarrelHits->m_strawIDToLayerIDC[i] = layerID;
        }
    }

  delete [] integralDistributionOfStrawsA;
  delete [] integralDistributionOfStrawsB;
  delete [] integralDistributionOfStrawsC;


  if (m_verboseLevel>5) { G4cout << "######### Method TRTParametersForBarrelHits::DefineParameters done" << G4endl; }

}


// Called by TRTParametersForBarrelHits

void TRTParametersForBarrelHits::PrintParameters ATLAS_NOT_THREAD_SAFE () const // Thread unsafe TRTOutputFile is used.
{

  if (m_verboseLevel>5) { G4cout << "######### Method TRTParametersForBarrelHits::PrintParameters" << G4endl; }

  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersForBarrelHits::PrintParameters *****"
         << std::endl;

  output << "Parameters of barrel straws:" << std::endl;

  if (m_pParameters->GetInteger("BarrelIdentifier") == 2)
    {
      int numberOfStrawsA = m_pProcessingOfBarrelHits->m_numberOfStrawsA;
      int numberOfStrawsB = m_pProcessingOfBarrelHits->m_numberOfStrawsB;
      int numberOfStrawsC = m_pProcessingOfBarrelHits->m_numberOfStrawsC;

      output << "  numberOfStrawsA=" << numberOfStrawsA << std::endl;
      output << "  numberOfStrawsB=" << numberOfStrawsB << std::endl;
      output << "  numberOfStrawsC=" << numberOfStrawsC << std::endl;
    }


  int numberOfStrawLayersA = m_pProcessingOfBarrelHits->m_numberOfStrawLayersA;
  int numberOfStrawLayersB = m_pProcessingOfBarrelHits->m_numberOfStrawLayersB;
  int numberOfStrawLayersC = m_pProcessingOfBarrelHits->m_numberOfStrawLayersC;

  output << "  numberOfStrawLayersA=" << numberOfStrawLayersA << std::endl;
  output << "  numberOfStrawLayersB=" << numberOfStrawLayersB << std::endl;
  output << "  numberOfStrawLayersC=" << numberOfStrawLayersC << std::endl;

  output << std::endl << "Integral distributions of straws in layers:"
         << std::endl;

  int i;
  for (i = 0; i < numberOfStrawLayersA; ++i)
    output << "  integralDistributionOfStrawsA[" << i << "]="
           << m_pProcessingOfBarrelHits->m_integralDistributionOfStrawsA[i]
           << std::endl;
  output << std::endl;

  for (i = 0; i < numberOfStrawLayersB; ++i)
    output << "  integralDistributionOfStrawsB[" << i << "]="
           << m_pProcessingOfBarrelHits->m_integralDistributionOfStrawsB[i]
           << std::endl;
  output << std::endl;

  for (i = 0; i < numberOfStrawLayersC; ++i)
    output << "  integralDistributionOfStrawsC[" << i << "]="
           << m_pProcessingOfBarrelHits->m_integralDistributionOfStrawsC[i]
           << std::endl;

  if ((m_pParameters->GetInteger("BarrelIdentifier") == 2) &&
      m_pParameters->GetInteger("PrintParametersForBarrelHits") == 2)
    {
      output << std::endl << "Arrays barrel straw ID to layer ID:" << std::endl;

      int numberOfStrawsA = m_pProcessingOfBarrelHits->m_numberOfStrawsA;
      int numberOfStrawsB = m_pProcessingOfBarrelHits->m_numberOfStrawsB;
      int numberOfStrawsC = m_pProcessingOfBarrelHits->m_numberOfStrawsC;

      int i;
      for (i = 0; i < numberOfStrawsA; ++i)
        output << "  strawIDToLayerIDA[" << i << "]="
               << m_pProcessingOfBarrelHits->m_strawIDToLayerIDA[i] << std::endl;
      output << std::endl;

      for (i = 0; i < numberOfStrawsB; ++i)
        output << "  strawIDToLayerIDB[" << i << "]="
               << m_pProcessingOfBarrelHits->m_strawIDToLayerIDB[i] << std::endl;
      output << std::endl;

      for (i = 0; i < numberOfStrawsC; ++i)
        output << "  strawIDToLayerIDC[" << i << "]="
               << m_pProcessingOfBarrelHits->m_strawIDToLayerIDC[i] << std::endl;
    }

  output << std::endl;

  if (m_verboseLevel>5) { G4cout << "######### Method TRTParametersForBarrelHits::PrintParameters done" << G4endl; }
}
