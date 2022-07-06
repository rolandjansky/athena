/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include <fstream>

  // Called by GetPointer

TRTOutputFile::TRTOutputFile() : m_output("TRTOutput", std::ios::out)
{
  m_output << std::endl << "                    TRT output file" << std::endl
         << std::endl;
}


  // It was called by TRTRunAction::EndOfRunAction ...

TRTOutputFile::~TRTOutputFile()
{
  m_output.close();
}


TRTOutputFile* TRTOutputFile::GetPointer()
{
  static TRTOutputFile f ATLAS_THREAD_SAFE;
  return &f;
}
