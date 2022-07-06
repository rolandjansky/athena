/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTOutputFile_hh
#define TRTOutputFile_hh

#include "globals.hh"
#include <fstream>

class TRTOutputFile
{
public:
 
  static TRTOutputFile* GetPointer();
  
  std::ofstream& GetReference()
  {return m_output;}
 
private:
  TRTOutputFile();
  ~TRTOutputFile();
  
  std::ofstream m_output;
};

#endif
