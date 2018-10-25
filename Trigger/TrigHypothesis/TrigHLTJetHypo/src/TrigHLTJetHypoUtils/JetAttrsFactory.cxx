/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     JetAttrsFactory.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   A. Steinhebel
//
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsWidthCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsKtDRCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FalseCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsFactory.h"


Conditions JetAttrsSort(std::string var, double limitMin, double limitMax){
        if(var == "width"){
          auto conditions = conditionsFactoryJetAttrsWidth(limitMin,limitMax);
          return conditions;
          } 
        else if(var =="ktdr"){
          auto conditions = conditionsFactoryJetAttrsKtDR(limitMin,limitMax);
          return conditions;
          }
        else{
          auto conditions = conditionsFactoryFalse(0.0,1.0);
          return conditions;
          }
}

