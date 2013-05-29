/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_EventTPCnvDict_h
#define ALFA_EventTPCnvDict_h

#include "ALFA_EventTPCnv/ALFA_HitCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_Hit_p1.h"

#include "ALFA_EventTPCnv/ALFA_DigitCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_Digit_p1.h"

#include "ALFA_EventTPCnv/ALFA_ODHitCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_ODHit_p1.h"

#include "ALFA_EventTPCnv/ALFA_ODDigitCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_ODDigit_p1.h"

#include "ALFA_EventTPCnv/ALFA_RawData_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataContainer_p1.h"

#include "ALFA_EventTPCnv/ALFA_LocRecEvCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecEvent_p1.h"

#include "ALFA_EventTPCnv/ALFA_LocRecODEvCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecODEvent_p1.h"

#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvent_p1.h"

#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEvCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEvent_p1.h"

#include "ALFA_EventTPCnv/ALFA_GloRecEvCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_GloRecEvent_p1.h"

#include "ALFA_EventTPCnv/ALFA_CLinkEvent_p1.h"

namespace ALFA_EventTPCnv_Dict {
  std::vector<ALFA_RawData_p1>                 t0;
  std::vector<ALFA_RawDataCollection_p1>       t1;
  std::vector<ALFA_RawDataContainer_p1>        t2;
 }


#endif // ALFA_EventTPCnvDict_h