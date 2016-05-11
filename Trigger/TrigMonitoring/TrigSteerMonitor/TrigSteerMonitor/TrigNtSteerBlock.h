/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigNtSteerBlock_H
#define TrigNtSteerBlock_H

#include <utility>
#include <vector>
#include <string>

#include "GaudiKernel/NTuple.h"



class TrigNtSteerBlock {

public:
  StatusCode book(NTuple::Tuple* nt1);
  StatusCode fill(unsigned int TEinput, unsigned int TEoutput);
  void reset();

private:
  unsigned int m_maxClus;

  NTuple::Item<long> m_nSeq;  
  NTuple::Array<float> m_TEinput;      
  NTuple::Array<float> m_TEoutput;      
  
  
};
#endif // TrigNtSteerBlock_H







