/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2CaloEgammaSamp1.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
//
// AUTHOR:   M.P. Casado
//
// ********************************************************************

#ifndef TRIGT2CALOEGAMMA_T2CALOEGAMMASAMP1_H
#define TRIGT2CALOEGAMMA_T2CALOEGAMMASAMP1_H

#include <string>
#include "TrigT2CaloCommon/T2CaloNewBase.h"

class StoreGateSvc;
class TriggerElement;
class IAlgToolCalo;

class T2CaloEgammaSamp1: public T2CaloNewBase {

  public:
    T2CaloEgammaSamp1(const std::string & name, ISvcLocator* pSvcLocator);
    ~T2CaloEgammaSamp1();

    //    bool execHLTAlgorithm(TriggerElement* outputTE);
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
			      HLT::TriggerElement* outputTE);

    HLT::ErrorCode hltInitialize() { return HLT::OK; }
    HLT::ErrorCode hltFinalize()   { return HLT::OK; }

};

#endif
