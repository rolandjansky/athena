/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_SECTORLOGICCONSTANTSPHASE1_H
#define TRIGT1INTERFACES_SECTORLOGICCONSTANTSPHASE1_H

namespace LVL1MUONIF {

  //number of bits assigned per sub-system {barrel, endcap, forward}
  static const unsigned int NBITS_BCID[3]                  = {12,12,12};
  static const unsigned int NBITS_ROI[3]                   = {5,8,8};
  static const unsigned int NBITS_OVL[3]                   = {2,2,2};
  static const unsigned int NBITS_PT[3]                    = {3,4,4};
  static const unsigned int NBITS_CHARGE[3]                = {1,1,1};
  static const unsigned int NBITS_IS2CANDIDATES[3]         = {1,1,1};
  static const unsigned int NBITS_IS2CANDIDATESINSECTOR[3] = {1,1,1};
  static const unsigned int NBITS_NSWMON[3]                = {0,1,1};
  static const unsigned int NBITS_BW2OR3[3]                = {0,1,1};
  static const unsigned int NBITS_INNERCOIN[3]             = {0,1,1};
  static const unsigned int NBITS_GOODMF[3]                = {0,1,1};

  static const size_t NCAND[3]                             = {2,4,4};

}

#endif //TRIGT1INTERFACES_SECTORLOGICCONSTANTSPHASE1_H
