/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFAATHENAPOOL_ALFACONTAINER_CHARGE_P1_H
#define ALFAATHENAPOOL_ALFACONTAINER_CHARGE_P1_H

#include <vector>
#include "ALFA_RawDataCollection_charge_p1.h"

class ALFA_RawDataContainer_charge_p1: public std::vector<ALFA_RawDataCollection_charge_p1> {
  
 public:
 
 ALFA_RawDataContainer_charge_p1(){};
 
 friend class ALFA_RawDataContainerCnv_charge_p1;
 
 };
 
 #endif
