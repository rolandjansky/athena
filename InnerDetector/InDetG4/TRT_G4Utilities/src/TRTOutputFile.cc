/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>


TRTOutputFile* TRTOutputFile::m_pOutputFile = NULL;


  // Called by GetPointer

TRTOutputFile::TRTOutputFile() : m_output("TRTOutput", std::ios::out), m_msg("TRTOutputFile")
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTOutputFile" << endmsg;

  m_output << std::endl << "                    TRT output file" << std::endl
         << std::endl;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTOutputFile done" << endmsg;
}


  // It was called by TRTRunAction::EndOfRunAction ...

TRTOutputFile::~TRTOutputFile()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTOutputFile" << endmsg;

  m_output.close();

  m_pOutputFile = NULL;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTOutputFile done" << endmsg;
}
