/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
      bool isBarrel(const LVL1::RecMuonRoI* p_roi){
        return (p_roi->sysID()==0) ? true : false; 
      };
      bool isLowPt(const LVL1::RecMuonRoI* p_roi){
        return (p_roi->getThresholdNumber() <4) ? true : false;
      };
      bool isHighPt(const LVL1::RecMuonRoI* p_roi){
        return (p_roi->getThresholdNumber()>=4) ? true : false;
      };
};

}

#endif // TRIGL2MUONSA_RECMUONROIUTILS_H
