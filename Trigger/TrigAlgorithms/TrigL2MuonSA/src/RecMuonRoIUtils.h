/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RECMUONROIUTILS_H
#define  TRIGL2MUONSA_RECMUONROIUTILS_H

#include "xAODTrigger/MuonRoI.h"

namespace TrigL2MuonSA {

class RecMuonRoIUtils
{
   public:
      RecMuonRoIUtils() {};
      ~RecMuonRoIUtils() {};

   public:
      bool isBarrel(const xAOD::MuonRoI* p_roi) const {
        return (p_roi->getSource()==0) ? true : false;
      };
      bool isLowPt(const xAOD::MuonRoI* p_roi) const {
        return (p_roi->getThrNumber() <4) ? true : false;
      };
      bool isHighPt(const xAOD::MuonRoI* p_roi) const {
        return (p_roi->getThrNumber()>=4) ? true : false;
      };
};

}

#endif // TRIGL2MUONSA_RECMUONROIUTILS_H
