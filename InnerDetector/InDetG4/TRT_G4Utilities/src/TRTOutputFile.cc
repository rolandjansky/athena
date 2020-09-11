/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>

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

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTOutputFile done" << endmsg;
}


TRTOutputFile* TRTOutputFile::GetPointer()
{
  static TRTOutputFile f ATLAS_THREAD_SAFE;
  return &f;
}
