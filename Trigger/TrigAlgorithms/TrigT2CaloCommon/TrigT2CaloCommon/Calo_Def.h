/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     Calo_Def.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
// 
// AUTHOR:   M.P. Casado
// 
// ********************************************************************

#ifndef TRIGT2CALOCOMMON_CALO_DEF_H
#define TRIGT2CALOCOMMON_CALO_DEF_H

class Calorimeter {
public:
  enum SubDetector {
     PSBAR = 1, 
     EMBAR = 2, 
     EMEND = 3, 
     TILE = 4, 
     TILESCIN = 5,
     HADEND = 6,
     PSEND = 11
   };

  enum Module {
    ZERO = 0, 
    FRONT = 1, 
    MIDDLE = 2, 
    BACK = 3, 
    FOUR = 4, 
    FIVE = 5, 
    SIX = 6,
    SEVEN = 7, 
    ANY_MODULE = 8
  };
};

#endif
