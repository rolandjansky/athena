/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>


TRTOutputFile* TRTOutputFile::pOutputFile = NULL;


  // Called by GetPointer

TRTOutputFile::TRTOutputFile() : output("TRTOutput", std::ios::out), m_msg("TRTOutputFile")
{
  printMessages = ReadParameter("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTOutputFile" << endmsg;

  output << std::endl << "                    TRT output file" << std::endl
         << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTOutputFile done" << endmsg;
}


  // It was called by TRTRunAction::EndOfRunAction ...

TRTOutputFile::~TRTOutputFile()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTOutputFile" << endmsg;

  output.close();

  pOutputFile = NULL;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTOutputFile done" << endmsg;
}


  // Called by TRTOutputFile

int TRTOutputFile::ReadParameter(std::string parameterName)
{
  std::string fileName = "management";
  std::ifstream inputFile(fileName.c_str(), std::ios::in);

  if (!inputFile)
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTOutputFile::ReadParameter *****" << std::endl;
    std::cerr << "  Cannot open input file '" << fileName << "'." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }

  std::string inputString;
  int parameterValue=0;

  int inputState = 0;
  G4bool parameterIsFound = false;

  while (inputFile >> inputString)
  {
    if (inputState == 0)
    {
      if (inputString == "<")
        inputState = 1;
    }
    else if (inputState == 1)
    {
      if (inputString == parameterName)
        inputState = 2;
      else
        inputState = 0;
    }
    else if (inputState == 2)
    {
      if (inputString == "=")
        inputState = 3;
      else
      {
        std::cerr << std::endl;
        std::cerr << "***** TRTOutputFile::ReadParameter *****" << std::endl;
        std::cerr << "  Invalid definition of parameter '" << parameterName << "'."
	       << std::endl;
	std::cerr << "  Exit!" << std::endl << std::endl;
        exit(0);
      }
    }
    else if (inputState == 3)
    {
      parameterValue = atoi(inputString.c_str());
      inputState = 4;
    }
    else if (inputState == 4)
    {
      if (inputString == ">")
      {
        parameterIsFound = true;
	break;
      }
      else
      {
        std::cerr << std::endl;
        std::cerr << "***** TRTOutputFile::ReadParameter *****" << std::endl;
        std::cerr << "  Parameter '" << parameterName << "' has more than one"
	       << " value." << std::endl;
	std::cerr << "  Exit!" << std::endl << std::endl;
        exit(0);
      }
    }
  }
  
  if (!parameterIsFound)
  {
    std::cerr << std::endl;
    std::cerr << "***** TRTOutputFile::ReadParameter *****" << std::endl;
    std::cerr << "  Cannot find parameter '" << parameterName << "'." << std::endl;
    std::cerr << "  Exit!" << std::endl << std::endl;
    exit(0);
  }

  inputFile.close();

  return parameterValue;
}
