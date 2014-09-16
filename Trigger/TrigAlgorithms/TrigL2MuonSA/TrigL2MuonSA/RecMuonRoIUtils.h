/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RECMUONROIUTILS_H
#define  TRIGL2MUONSA_RECMUONROIUTILS_H

#include "TrigT1Interfaces/RecMuonRoI.h"

namespace TrigL2MuonSA {

class RecMuonRoIUtils
{
   public:
      RecMuonRoIUtils() {};
      ~RecMuonRoIUtils() {};

   public:
      bool isBarrel(const LVL1::RecMuonRoI* p_roi);
      bool isLowPt(const LVL1::RecMuonRoI* p_roi);
      bool isHighPt(const LVL1::RecMuonRoI* p_roi);
};

}

#endif // TRIGL2MUONSA_RECMUONROIUTILS_H
