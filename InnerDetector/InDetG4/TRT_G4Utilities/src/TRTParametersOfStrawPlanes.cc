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
  m_pParameters = TRTParameters::GetPointer();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfStrawPlanes" << endreq;

  DefineParameters();

  if (m_pParameters->GetInteger("PrintParametersOfStrawPlanes"))
    PrintParameters();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTParametersOfStrawPlanes done" << endreq;
}


  // Called by TRTConstructionOfStrawPlanes::~TRTConstructionOfStrawPlanes

TRTParametersOfStrawPlanes::~TRTParametersOfStrawPlanes()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfStrawPlanes" << endreq;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTParametersOfStrawPlanes done" << endreq;
}


  // Called by TRTParametersOfStrawPlanes

void TRTParametersOfStrawPlanes::DefineParameters()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfStrawPlanes::DefineParameters" << endreq;

    // Parameters of straw plane:
  m_innerRadiusOfStrawPlanesAB = m_pParameters->GetDouble("InnerRadiusOfWheelsAB") +
    m_pParameters->GetDouble("ThicknessOfInnerSupportsAB");
  m_outerRadiusOfStrawPlanesAB = m_pParameters->GetDouble("OuterRadiusOfWheelsAB") -
    m_pParameters->GetDouble("ThicknessOfOuterSupportsAB");
  m_lengthOfStrawPlane = m_pParameters->GetDouble("OuterRadiusOfStraw") * 2.;
  m_numberOfStrawsInPlanesAB =
    m_pParameters->GetInteger("NumberOfStrawsInPlanesAB");

  if (m_pParameters->GetInteger("SectorsABC"))
  {
      // Parameters of sectors:
    m_numberOfSectorsAB = m_pParameters->GetInteger("NumberOfSectorsAB");

    m_innerRadiusOfSectorsAB = m_innerRadiusOfStrawPlanesAB;
    m_outerRadiusOfSectorsAB = m_outerRadiusOfStrawPlanesAB;
    m_lengthOfSector = m_lengthOfStrawPlane;

    m_numberOfStrawsInSectorsAB = m_numberOfStrawsInPlanesAB / m_numberOfSectorsAB;
  }

    // Parameters of straw:
  m_outerRadiusOfStraw = m_pParameters->GetDouble("OuterRadiusOfStraw");
  m_lengthOfStrawsAB = (m_outerRadiusOfStrawPlanesAB - m_innerRadiusOfStrawPlanesAB) *
    m_pParameters->GetDouble("MultiplierForStrawLength");
  m_positionOfStrawsAB = (m_innerRadiusOfStrawPlanesAB +
    m_outerRadiusOfStrawPlanesAB) / 2.;

    // Parameters of wire:
  m_outerRadiusOfWire = m_pParameters->GetDouble("OuterRadiusOfWire");
  m_lengthOfWiresAB = m_lengthOfStrawsAB;

    // Parameters of dead region:
  m_innerRadiusOfDeadRegion = m_outerRadiusOfWire;
  m_outerRadiusOfDeadRegion = m_pParameters->GetDouble("InnerRadiusOfStraw");
  m_lengthOfDeadRegion = m_pParameters->GetDouble("LengthOfDeadRegion");
  m_positionOfDeadRegionsAB = (m_lengthOfStrawsAB - m_lengthOfDeadRegion) / 2.;

    // Parameters of gas:
  m_innerRadiusOfGas = m_innerRadiusOfDeadRegion;
  m_outerRadiusOfGas = m_outerRadiusOfDeadRegion;
  m_lengthOfGasAB = m_lengthOfStrawsAB - m_lengthOfDeadRegion * 2.;

    // Parameters of straw plane C:
  if (m_pParameters->GetInteger("WheelsC"))
  {
      // Parameters of straw plane:
    m_innerRadiusOfStrawPlaneC = m_pParameters->GetDouble("InnerRadiusOfWheelC") +
      m_pParameters->GetDouble("ThicknessOfInnerSupportC");
    m_outerRadiusOfStrawPlaneC = m_pParameters->GetDouble("OuterRadiusOfWheelC") -
      m_pParameters->GetDouble("ThicknessOfOuterSupportC");
    m_numberOfStrawsInPlaneC = m_pParameters->GetInteger("NumberOfStrawsInPlaneC");

    if (m_pParameters->GetInteger("SectorsABC"))
    {
        // Parameters of sectors:
      m_numberOfSectorsC = m_pParameters->GetInteger("NumberOfSectorsC");

      m_innerRadiusOfSectorC = m_innerRadiusOfStrawPlaneC;
      m_outerRadiusOfSectorC = m_outerRadiusOfStrawPlaneC;

      m_numberOfStrawsInSectorC = m_numberOfStrawsInPlaneC / m_numberOfSectorsC;
    }

      // Parameters of straw:
    m_lengthOfStrawC = (m_outerRadiusOfStrawPlaneC - m_innerRadiusOfStrawPlaneC) *
      m_pParameters->GetDouble("MultiplierForStrawLength");
    m_positionOfStrawC = (m_innerRadiusOfStrawPlaneC + m_outerRadiusOfStrawPlaneC) /
      2.;

      // Parameters of wire:
    m_lengthOfWireC = m_lengthOfStrawC;

      // Parameters of dead region:
    m_positionOfDeadRegionC = (m_lengthOfStrawC - m_lengthOfDeadRegion) / 2.;

      // Parameters of gas:
    m_lengthOfGasC = m_lengthOfStrawC - m_lengthOfDeadRegion * 2.;
  }

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfStrawPlanes::DefineParameters" << " done" << endreq;
}


  // Called by TRTParametersOfStrawPlanes

void TRTParametersOfStrawPlanes::PrintParameters() const
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfStrawPlanes::PrintParameters" << endreq;

  TRTUtilities* pUtilities = TRTUtilities::GetPointer();
  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersOfStrawPlanes::PrintParameters *****"
         << std::endl;

  output << "Parameters of straw planes AB:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfStrawPlanesAB,
    m_outerRadiusOfStrawPlanesAB, m_lengthOfStrawPlane);
  output << "  numberOfStrawsInPlanesAB=" << m_numberOfStrawsInPlanesAB
         << std::endl;

  if (m_pParameters->GetInteger("SectorsABC"))
  {
    output << std::endl << "Parameters of sectors AB:" << std::endl;
    pUtilities->PrintTubeParameters(m_innerRadiusOfSectorsAB,
      m_outerRadiusOfSectorsAB, m_lengthOfSector);
    output << "  numberOfStrawsInSectorsAB=" << m_numberOfStrawsInSectorsAB
           << std::endl;
  }

  output << std::endl << "Parameters of straws AB:" << std::endl;
  pUtilities->PrintTubeParameters(m_outerRadiusOfStraw, m_lengthOfStrawsAB);
  output << "  position=" << m_positionOfStrawsAB << " mm" << std::endl;

  output << std::endl << "Parameters of gas AB:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfGas,
    m_outerRadiusOfGas, m_lengthOfGasAB);

  output << std::endl << "Parameters of dead regions AB:" << std::endl;
  pUtilities->PrintTubeParameters(m_innerRadiusOfDeadRegion,
    m_outerRadiusOfDeadRegion, m_lengthOfDeadRegion, m_positionOfDeadRegionsAB);

  output << std::endl << "Parameters of wires AB:" << std::endl;
  pUtilities->PrintTubeParameters(m_outerRadiusOfWire, m_lengthOfWiresAB);

  if (m_pParameters->GetInteger("WheelsC"))
  {
    output << std::endl << "Parameters of straw plane C:" << std::endl;
    pUtilities->PrintTubeParameters(m_innerRadiusOfStrawPlaneC,
      m_outerRadiusOfStrawPlaneC, m_lengthOfStrawPlane);
    output << "  numberOfStrawsInPlaneC=" << m_numberOfStrawsInPlaneC
           << std::endl;

    if (m_pParameters->GetInteger("SectorsABC"))
    {
      output << std::endl << "Parameters of sector C:" << std::endl;
      pUtilities->PrintTubeParameters(m_innerRadiusOfSectorC,
        m_outerRadiusOfSectorC, m_lengthOfSector);
      output << "  numberOfStrawsInSectorC=" << m_numberOfStrawsInSectorC
             << std::endl;
    }

    output << std::endl << "Parameters of straw C:" << std::endl;
    pUtilities->PrintTubeParameters(m_outerRadiusOfStraw, m_lengthOfStrawC);
    output << "  position=" << m_positionOfStrawC << " mm" << std::endl;

    output << std::endl << "Parameters of gas C:" << std::endl;
    pUtilities->PrintTubeParameters(m_innerRadiusOfGas,
      m_outerRadiusOfGas, m_lengthOfGasC);

    output << std::endl << "Parameters of dead region C:" << std::endl;
    pUtilities->PrintTubeParameters(m_innerRadiusOfDeadRegion,
      m_outerRadiusOfDeadRegion, m_lengthOfDeadRegion, m_positionOfDeadRegionC);

    output << std::endl << "Parameters of wire C:" << std::endl;
    pUtilities->PrintTubeParameters(m_outerRadiusOfWire, m_lengthOfWireC);
  }

  output << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTParametersOfStrawPlanes::PrintParameters done" << endreq;
}
