/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETRAWDATA_P1_H
#define INDETRAWDATA_P1_H

#include "Identifier/Identifier.h"

class InDetRawData_p1 {
 public:
  InDetRawData_p1()  {};
// List of Cnv classes that convert this into Rdo objects
  friend class TRT_LoLumRawDataCnv_p1;
  friend class SCT1_RawDataCnv_p1;
  friend class Pixel1RawDataCnv_p1;
 private:
  Identifier32::value_type m_rdoId; //Offline ID for readout channel
  unsigned int m_word; // raw data word 
};

#endif
