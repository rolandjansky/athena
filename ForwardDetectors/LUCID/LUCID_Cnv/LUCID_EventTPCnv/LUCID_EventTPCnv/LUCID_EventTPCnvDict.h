/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_EVENTTPCNV_H
#define LUCID_EVENTTPCNV_H

#include "LUCID_EventTPCnv/LUCID_SimHit_p1.h"
#include "LUCID_EventTPCnv/LUCID_Digit_p1.h"
#include "LUCID_EventTPCnv/LUCID_DigitContainer_p1.h"
#include "LUCID_EventTPCnv/LUCID_RawData_p1.h"
#include "LUCID_EventTPCnv/LUCID_RawDataContainer_p1.h"

#include "LUCID_EventTPCnv/LUCID_Digit_p2.h"
#include "LUCID_EventTPCnv/LUCID_DigitContainer_p2.h"

#include "LUCID_EventTPCnv/LUCID_SimHit_p2.h"

namespace LUCID_EventTPCnv_Dict {

  std::vector<LUCID_SimHit_p1>                  t0;
  std::vector<LUCID_Digit_p1>                   t1;
  std::vector<LUCID_DigitContainer_p1>          t2;
  std::vector<LUCID_RawData_p1>                 t3;
  std::vector<LUCID_RawDataContainer_p1>        t4;

  std::vector<LUCID_Digit_p2>                   t5;
  std::vector<LUCID_DigitContainer_p2>          t6;

  std::vector<LUCID_SimHit_p2>                  t7;
}

#endif
