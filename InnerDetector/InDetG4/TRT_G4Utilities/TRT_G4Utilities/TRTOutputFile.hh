/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTOutputFile_hh
#define TRTOutputFile_hh

#include "globals.hh"
#include <fstream>
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class ATLAS_NOT_THREAD_SAFE TRTOutputFile // This class needs give non-const reference for output streaming. static TRTOutputFile* m_pOutputFile cannot be static const. Thread unsafe exit is also used.
{
public:
  ~TRTOutputFile();
  
  static TRTOutputFile* GetPointer()
  {
    if (!m_pOutputFile)
      m_pOutputFile = new TRTOutputFile;
    return m_pOutputFile;
    }
  
  std::ofstream& GetReference()
  {return m_output;}
 
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

private:
  TRTOutputFile();
  
  
  
  std::ofstream m_output;
  
  static TRTOutputFile* m_pOutputFile;

  mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;

};

#endif
