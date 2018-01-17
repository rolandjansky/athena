/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTUtilities.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>



TRTUtilities* TRTUtilities::s_pUtilities = NULL;


  // Called by GetPointer

TRTUtilities::TRTUtilities() : m_msg("TRTUtilities")
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTUtilities" << endmsg;

  m_pOutputFile = NULL;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTUtilities done" << endmsg;
}


  // Called by TRTRunAction::EndOfRunAction

TRTUtilities::~TRTUtilities()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTUtilities" << endmsg;

  s_pUtilities = NULL;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTUtilities done" << endmsg;
}


  // Called by PrintTubeParameters

std::ofstream& TRTUtilities::GetReference()
{
  if (!m_pOutputFile)
    m_pOutputFile = TRTOutputFile::GetPointer();

  return m_pOutputFile->GetReference();
}


  // Called on demand

std::string TRTUtilities::NumberToString(int number)
{
  std::string digitAsString[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
  std::string numberAsString = "";

  for ( ; ; )
  {
    numberAsString = digitAsString[number % 10] + numberAsString;
    number /= 10;
    if (number == 0)
      break;
  }

  return numberAsString;
}


  // Called on demand

void TRTUtilities::PrintTubeParameters(double outerRadius, double length)
{
  std::ofstream& output = GetReference();

  output << "  outerRadius=" << outerRadius << " mm";
  output << "  length=" << length << " mm" << std::endl;
}


  // Called on demand

void TRTUtilities::PrintTubeParameters(double innerRadius,
  double outerRadius, double length)
{
  std::ofstream& output = GetReference();

  output << "  innerRadius=" << innerRadius << " mm";
  output << "  outerRadius=" << outerRadius << " mm";
  output << "  length=" << length << " mm" << std::endl;
}


  // Called on demand

void TRTUtilities::PrintTubeParameters(double innerRadius,
  double outerRadius, double length, double position)
{
  std::ofstream& output = GetReference();

  output << "  innerRadius=" << innerRadius << " mm";
  output << "  outerRadius=" << outerRadius << " mm";
  output << "  length=" << length << " mm" << std::endl;
  output << "  position=" << position << " mm" << std::endl;
}
