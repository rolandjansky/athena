/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTOutputFile_hh
#define TRTOutputFile_hh

#include "globals.hh"
#include <fstream>
#include "AthenaKernel/MsgStreamMember.h"


class TRTOutputFile
{
public:
  ~TRTOutputFile();
  
  static TRTOutputFile* GetPointer()
  {
    if (!pOutputFile)
      pOutputFile = new TRTOutputFile;
    return pOutputFile;
    }
  
  std::ofstream& GetReference()
  {return output;}
 
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

private:
  TRTOutputFile();
  
  int ReadParameter(std::string);
  
  int printMessages;

  std::ofstream output;
  
  static TRTOutputFile* pOutputFile;

  mutable Athena::MsgStreamMember m_msg;

};

#endif
