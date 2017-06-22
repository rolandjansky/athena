/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4CODE_LARG4ENUMDEFS_H
#define LARG4CODE_LARG4ENUMDEFS_H

namespace LArG4 {

  enum eCalculatorProcessing { kEnergyAndID, kOnlyEnergy, kOnlyID };

  enum EnergyCorrection_t {
                EMEC_ECOR_ROPT,
                EMEC_ECOR_OFF,
                EMEC_ECOR_GADJ,
                EMEC_ECOR_CHCL,
                EMEC_ECOR_GADJ_OLD,
                EMEC_ECOR_GADJ_E,
                EMEC_ECOR_GADJ_S,
                EMEC_ECOR_GADJ_SE,
                EMEC_ECOR_CHCL1
        };

  struct ROOT6_NamespaceAutoloadHook{}; 
}
#endif
