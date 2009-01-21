/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Persistent representation of BCM_RawData
// Author: Dominique Tardif <Dominique.Tardif@cern.ch>

#ifndef INDETBCM_EVENTATHENAPOOL_BCM_RAWDATA_P0_H
#define INDETBCM_EVENTATHENAPOOL_BCM_RAWDATA_P0_H

class BCM_RawData_p0 {

 public:
  BCM_RawData_p0() : m_word1(0), m_word2(0) {}
  friend class BCM_RawDataCnv_p0;

 private:
  unsigned int m_word1;
  unsigned int m_word2;

};

#endif
