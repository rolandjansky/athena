/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// D. Costanzo 23 Aug 2007
// SCT3 is used for M4, it needs a separate cnv as it contains a float[20]
//
#ifndef SCT3_RAWDATA_P1_H
#define SCT3_RAWDATA_P1_H

#include "Identifier/Identifier.h"

class SCT3_RawData_p1 {
 public:
  SCT3_RawData_p1(): m_rdoId{0}, m_word{0}  {};
// List of Cnv classes that convert this into Rdo objects
  friend class SCT3_RawDataCnv_p1;
 private:
  Identifier32::value_type m_rdoId; //Offline ID for readout channel
  unsigned int m_word; // raw data word 
  float m_errCH[20];
};

#endif
