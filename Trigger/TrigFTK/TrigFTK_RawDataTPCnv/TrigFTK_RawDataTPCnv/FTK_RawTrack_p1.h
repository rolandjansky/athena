/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigFTK_RawDataTPCnv_FTK_RawTrack_p1_H
#define TrigFTK_RawDataTPCnv_FTK_RawTrack_p1_H

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include <stdint.h>

class FTK_RawTrack_p1
{
  
 public:

  FTK_RawTrack_p1() {}
  friend class FTK_RawTrackCnv_p1;  
  
 private:
  
  uint32_t m_pars[6];
  uint32_t m_pars_pix[8];
  uint32_t m_pars_sct[8];
  signed long m_barcodes_sct[8];
  signed long m_barcodes_pix[4];
  signed long m_barcode;

};

#endif 
