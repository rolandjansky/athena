/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITCONTAINER_P2_H
#define LARDIGITCONTAINER_P2_H

#include <vector>

class LArDigitContainer_p2{
public:
  unsigned char m_nSamples;           //identical for all readout channels
  std::vector<unsigned char> m_gain;  //2 bits per online-hash. 0:no data 1,2,3:gain+1
  std::vector<short> m_samples;
};


#endif
